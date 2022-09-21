const { alarms, inputsSH, outputsSH } = require('./obj')
const { Device } = require('../../models/Device')
const { Position } = require('../../models/Position')

const device = new Device(1, 'EL')

const LV1 = new Position(1, 'LV1')
const LV2 = new Position(2, 'LV2')
const LH1 = new Position(3, 'LH1')
const LH2 = new Position(4, 'LH2')
const ENC = new Position(5, 'ENC')
const positions = [LV1, LV2, LH1, LH2, ENC]

const lamps = [
  inputsSH.find(b => b.addr === 'E3.3'),
  outputsSH.find(b => b.addr === 'A0.7'),
  outputsSH.find(b => b.addr === 'A0.6')
]

const silomat_ = [
  inputsSH.find(b => b.addr === 'E2.0'),
  inputsSH.find(b => b.addr === 'E2.1'),
  inputsSH.find(b => b.addr === 'E2.2'),
  inputsSH.find(b => b.addr === 'E2.3'),
  inputsSH.find(b => b.addr === 'E2.4'),
  inputsSH.find(b => b.addr === 'E2.5'),
  inputsSH.find(b => b.addr === 'E2.6'),
  inputsSH.find(b => b.addr === 'E2.7'),
  outputsSH.find(b => b.addr === 'A2.1'),
  outputsSH.find(b => b.addr === 'A2.2'),
  outputsSH.find(b => b.addr === 'A2.3'),
  outputsSH.find(b => b.addr === 'A2.4'),
  outputsSH.find(b => b.addr === 'A2.5'),
  outputsSH.find(b => b.addr === 'A2.6')
]

const view = {
  a: device,
  b: positions,
  c: lamps,
  d: [],
  e: silomat_,
  alarms: alarms[0]._active
}

const inverters = []

const motors = []

const silomat = { motors: [] }

module.exports = { device, inverters, motors, positions, silomat, view }
