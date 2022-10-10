const EventEmitter = require('events')
const logger = require('pino')()
const snap7 = require('bindings')('node_snap7_client')
// const comm = require('./comm')
const { countAlarms, updateAlarms } = require('../models/alarms')
const { updateBits } = require('../models/bits')
const { updateCards } = require('../models/cards')
const { updateDevices } = require('../models/devices')
const { updateInverters } = require('../models/inverters')
const { updateMotors } = require('../models/motors')
const { updatePositions } = require('../models/positions')
const { updateQueue } = require('../models/queue')
const { updateExits, updateGarageScreens, updateOccupancy } = require('../models/screens')
const { updateStalls, occupancy } = require('../models/stalls')

class PLC extends EventEmitter {
  constructor (plc) {
    super()
    this.client = new snap7.Snap7Client()
    this.online = false
    this.plc = plc
  }

  error (error) {
    logger.error('[error] %s', error)
  }

  errorS7 (e) {
    // this.online = !this.client.Disconnect()
    // logger.error('[error S7] %s', this.client.ErrorText(error))
    this.online = this.client.Disconnect()
    isNaN(e) ? logger.error(e) : logger.error(this.client.ErrorText(e))
  }

  async connect () {
    this.online = await this.client.ConnectTo(this.plc.ip, this.plc.rack, this.plc.slot)
    logger.info('PLC %s is online', this.plc.ip)
  }

  async alarms (def, obj) {
    if (def.DBS_ALARM.length !== 0) {
      try {
        await Promise.all(
          obj.alarms.map(async (item, index) => {
            const buffer = await this.client.ReadArea(
              0x84,
              def.DBS_ALARM[index],
              def.DB_ALARM_INIT,
              def.DB_ALARM_LEN,
              0x02
            )
            const alarms = await updateAlarms(
              0,
              buffer,
              def.ALARM_LEN,
              item.alarms
            )
            item._active = alarms
            obj.overview.devices[index].alarms = item._active
          })
        )
      } catch (error) {
        this.error(error)
      } finally {
        this.publish('aps/alarms', obj.alarms)
      }
    }
  }

  async cards (def, obj) {
    if (def.CARD_READ !== undefined) {
      try {
        const buffer = await this.read(def.CARD_READ)
        const cards = await updateCards(0, buffer, def.CARD_LEN, obj.cards)
        this.publish('aps/cards', cards)
      } catch (error) {
        this.error(error)
      }
    }
  }

  async data (def, obj) {
    try {
      const buffer = await this.read(def.DATA_READ)
      await Promise.all([
        updateBits(def.DB_DATA_INIT_AB, buffer, obj.ab),
        updateBits(def.DB_DATA_INIT_EB, buffer, obj.eb),
        updateBits(def.DB_DATA_INIT_MB, buffer, obj.mb),
        updateDevices(
          def.DB_DATA_INIT_DEVICE,
          buffer,
          16,
          obj.devices,
          obj.modes
        ),
        updatePositions(def.DB_DATA_INIT_POS, buffer, 4, obj.positions),
        updateQueue(def.DB_DATA_INIT_QUEUE, buffer, 4, obj.queue)
      ])
      await this.dss(def, obj, buffer) // DSS (New!)
    } catch (error) {
      this.error(error)
    } finally {
      this.publish('aps/overview', obj.overview)
      // this.publish('aps/racks', obj.racks)
      obj.racks.forEach((rack, key) => this.publish('aps/racks/' + key, rack))
    }
  }

  async diag (def, obj) {
    if (def.DIAG_READ !== undefined) {
      try {
        const buffer = await this.read(def.DIAG_READ)
        await Promise.all([
          updateMotors(def.DB_DIAG_INIT_MOT, buffer, 2, obj.motors),
          updateInverters(def.DB_DIAG_INIT_VFD, buffer, 10, obj.inverters),
          updateMotors(def.DB_DIAG_INIT_SIL, buffer, 2, obj.silomats)
        ])
      } catch (error) {
        this.error(error)
      } finally {
        const diagnostic = obj.diagnostic.map((item, key) => ({
          device: obj.overview.devices[key],
          inverters: item.inverters,
          motors: item.motors.map(m => m.json),
          silomat: item.silomat.json
        }))
        diagnostic.forEach(
          (element, key) => this.publish('aps/diagnostic/' + key, element) // diagnostic[key])
        )
      }
    }
  }

  async dss (def, obj, buffer) {
    try {
      if (obj.exits !== undefined) {
        await updateExits(def.DB_DATA_INIT_EXITS, buffer, 4, obj.exits)
      }
      if (obj.screens !== undefined) {
        await updateGarageScreens(def.DB_DATA_INIT_SCREENS, buffer, 10, obj.screens, obj.occupancy)
      }
      if (obj.occupancy !== undefined) {
        await updateOccupancy(def.DB_DATA_INIT_OCCUPANCY, buffer, 6, obj.occupancy)
      }
      if (obj.exitScreen !== undefined) {
        await obj.exitScreen.update(obj.exits, obj.queue)
      }
    } catch (error) {
      this.error(error)
    }
  }

  async map (def, obj) {
    try {
      const buffer = await this.read(def.MAP_READ)
      const stalls = await updateStalls(0, buffer, def.STALL_LEN, obj.stalls)
      // const stalls = await updateStalls_(1, def.STALLS, buffer, def.STALL_LEN, obj.stalls)
      const data = occupancy(0, stalls, def.STALL_STATUS)
      obj.map.occupancy = data
      this.publish('aps/map', obj.map)
    } catch (error) {
      this.error(error)
    }
  }

  async stall (def, obj, stall) {
    try {
      if (stall < 1 || stall > def.STALLS) return
      const buffer = await this.read({
        area: 0x84,
        dbNumber: def.DB_MAP,
        start: stall === 1 ? 0 : (stall - 1) * def.STALL_LEN,
        amount: def.STALL_LEN,
        wordLen: 0x02
      })
      obj.stalls[stall - 1].update(buffer)
      const data = occupancy(0, obj.stalls, def.STALL_STATUS)
      obj.map.occupancy = data
      this.publish('aps/map', obj.map)
    } catch (error) {
      this.error(error)
    }
  }

  async main (def, obj) {
    try {
      await this.connect()
      await this.alarms(def, obj)
      await this.cards(def, obj)
      await this.map(def, obj)
      // recursive
      this.forever(def, obj)
    } catch (err) {
      this.errorS7(err)
    }
  }

  forever (def, obj) {
    // const hrstart = process.hrtime()
    setTimeout(async () => {
      if (this.online) {
        this.data(def, obj)
        this.diag(def, obj)
      } else {
        this.online = this.client.Connect()
        this.online ? logger.info('Connected to PLC %s', this.plc.ip) : logger.info('Connecting to PLC %s ...', this.plc.ip)
      }
      // const hrend = process.hrtime(hrstart)
      // console.info('Execution time (hr):', hrend)
      this.publish('aps/info', {
        comm: this.online,
        diag: countAlarms(obj.alarms),
        map: obj.map.occupancy,
        expired: obj.merkers.find(m => m.addr === 'M7.7').status
      })
      this.forever(def, obj)
    }, this.plc.polling_time)
  }

  publish (channel, data) {
    this.emit('pub', { channel, data: JSON.stringify(data) })
  }

  async read (conn) {
    try {
      const buffer = await this.client.ReadArea(
        conn.area,
        conn.dbNumber,
        conn.start,
        conn.amount,
        conn.wordLen
      )
      return buffer
    } catch (error) {
      this.errorS7(error)
    }
  }

  async write (conn, buffer) {
    try {
      const response = await this.client.WriteArea(
        conn.area,
        conn.dbNumber,
        conn.start,
        conn.amount,
        conn.wordLen,
        buffer
      )
      return response // return true on success
    } catch (error) {
      this.errorS7(error)
    }
  }
}

module.exports = PLC
