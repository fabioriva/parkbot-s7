const def = require('./def')
const { alarms, inputs, merkers, outputs } = require('./obj')
const { Device } = require('../../models/Device')
const { Position } = require('../../models/Position')

const device = new Device(2, 'EU')

const ENR = new Position(6, 'ENR')
const positions = [ENR]

const lamps = [
  inputs.find(b => b.addr === 'E7.3'),
  outputs.find(b => b.addr === 'A6.7'),
  outputs.find(b => b.addr === 'A6.6')
  // inputs.find(b => b.addr === 'E6.3') // ???
]

const A0 = {
  conn: def.REQ_ENTRY,
  enable: merkers.find(b => b.addr === 'M3.2'),
  key: 'action-entry'
}

const view = {
  a: device,
  b: positions,
  c: lamps,
  d: [A0],
  e: [],
  alarms: alarms[1]._active
}

const inverters = []

const motors = []

const silomat = { motors: [] }

module.exports = { device, inverters, motors, positions, silomat, view }
