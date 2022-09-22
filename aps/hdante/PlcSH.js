const logger = require('pino')()
const snap7 = require('bindings')('node_snap7_client')
const { updateBits } = require('../../models/Bit')

class PLC {
  constructor (plc) {
    this.client = new snap7.Snap7Client()
    this.online = false
    this.conn = plc
  }

  async error (e) {
    this.online = this.client.Disconnect()
    logger.error(this.client.ErrorText(e))
  }

  async main (def, obj) {
    try {
      const { area, dbNumber, start, amount, wordLen } = def.DATA_READ_SH
      const buffer = await this.client.ReadArea(area, dbNumber, start, amount, wordLen)
      /* Data */
      await Promise.all([
        updateBits(def.DB_DATA_INIT_AB_SH, buffer, obj.abSH),
        updateBits(def.DB_DATA_INIT_EB_SH, buffer, obj.ebSH)
      ])
    } catch (e) {
      this.error(e)
    }
  }

  async run (def, obj) {
    try {
      this.online = await this.client.ConnectTo(this.conn.ip, this.conn.rack, this.conn.slot)
      this.forever(def, obj)
    } catch (e) {
      this.error(e)
    }
  }

  forever (def, obj) {
    setTimeout(() => {
      if (this.online) {
        this.main(def, obj)
      } else {
        this.online = this.client.Connect()
        this.online ? logger.info('Connected to PLC %s', this.conn.ip) : logger.info('Connecting to PLC %s ...', this.conn.ip)
      }
      this.forever(def, obj)
    }, this.conn.polling_time)
  }

  publish (channel, data) {
    this.emit('pub', { channel, data: JSON.stringify(data) })
  }
}

module.exports = PLC
