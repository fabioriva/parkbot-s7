const util = require('util')
const { generateBits } = require('./bits')

class Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    this.id = id
    this.name = name
    this.inputs = inputs
    this.outputs = outputs
    this.motor = generateBits('M', 0, 0)
    this.flags = generateBits('M', 1, 1)
  }

  get json () {
    return {
      id: this.id,
      name: this.name,
      motion: this.motion(),
      position: this.position(),
      inputs: this.inputs,
      outputs: this.outputs,
      enable: this.motor[4].status,
      error: this.motor[7].status
    }
  }

  motion () {
    const m1 = this.motor[0]
    const m2 = this.motor[1]
    if (!m1.status && !m2.status) {
      return { id: 0, i18n: 'motion-no' }
    } else if (m1.status && !m2.status) {
      return { id: 1, i18n: this.motion_[0] }
    } else if (m2.status && !m1.status) {
      return { id: 2, i18n: this.motion_[1] }
    } else {
      return { id: 3, i18n: 'motion-err' }
    }
  }

  position () {
    const p1 = this.motor[2]
    const p2 = this.motor[3]
    if (!p1.status && !p2.status) {
      return { id: 0, i18n: 'position-no' }
    } else if (p1.status && !p2.status) {
      return { id: 1, i18n: this.position_[0] }
    } else if (p2.status && !p1.status) {
      return { id: 2, i18n: this.position_[1] }
    } else {
      return { id: 3, i18n: 'position-err' }
    }
  }

  update (buffer) {
    let mask
    mask = 1
    for (let i = 0; i < this.motor.length; i++) {
      this.motor[i].status = buffer[0] & mask ? 1 : 0
      mask *= 2
    }
    mask = 1
    for (let i = 0; i < this.flags.length; i++) {
      if (this.flags[i].status !== undefined) {
        this.flags[i].status = buffer[1] & mask ? 1 : 0
      }
      mask *= 2
    }
  }
}

class Door extends Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    super(id, name, inputs, outputs)
    this.motion_ = ['motion-close', 'motion-open']
    this.position_ = ['position-closed', 'position-opened']
  }
}

class Flap extends Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    super(id, name, inputs, outputs)
    this.motion_ = ['motion-down', 'motion-up']
    this.position_ = ['position-high', 'position-low']
  }
}

class Lock extends Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    super(id, name, inputs, outputs)
    this.motion_ = ['motion-lock', 'motion-unlock']
    this.position_ = ['position-locked', 'position-unlocked']
  }
}

class Motor {
  constructor (
    id,
    name,
    inputs = [],
    outputs = [],
    positions = [],
    ready = [],
    inverter
  ) {
    this.id = id
    this.name = name
    this.inputs = inputs
    this.outputs = outputs
    this.positions = positions
    this.ready = ready
    this.inverter = inverter
    this.motor = generateBits('M', 0, 0)
    this.flags = generateBits('M', 1, 1)
  }

  get json () {
    return {
      id: this.id,
      name: this.name,
      motion: this.motion(),
      position: this.position(),
      inputs: this.inputs,
      outputs: this.outputs,
      ready: this.ready,
      inverter: this.inverter,
      enable: this.motor[4].status,
      error: this.motor[7].status
    }
  }

  motion () {
    const m1 = this.motor[0]
    const m2 = this.motor[1]
    if (!m1.status && !m2.status) {
      return { id: 0, i18n: 'motion-no' }
    } else if (m1.status && !m2.status) {
      return { id: 1, i18n: this.motion_[0] }
    } else if (m2.status && !m1.status) {
      return { id: 2, i18n: this.motion_[1] }
    } else {
      return { id: 3, i18n: 'motion-err' }
    }
  }

  position () {
    return this.positions
  }

  update (buffer) {
    let mask
    mask = 1
    for (let i = 0; i < this.motor.length; i++) {
      this.motor[i].status = buffer[0] & mask ? 1 : 0
      mask *= 2
    }
    mask = 1
    for (let i = 0; i < this.flags.length; i++) {
      if (this.flags[i].status !== undefined) {
        this.flags[i].status = buffer[1] & mask ? 1 : 0
      }
      mask *= 2
    }
  }
}

class Hoisting extends Motor {
  constructor (
    id,
    name,
    inputs = [],
    outputs = [],
    positions = [],
    ready = [],
    inverter
  ) {
    super(id, name, inputs, outputs, positions, ready, inverter)
    this.motion_ = ['motion-down', 'motion-up']
  }
}

class Rotation extends Motor {
  constructor (
    id,
    name,
    inputs = [],
    outputs = [],
    positions = [],
    ready = [],
    inverter
  ) {
    super(id, name, inputs, outputs, positions, ready, inverter)
    this.motion_ = ['motion-anticlockwise', 'motion-clockwise']
  }
}

class Traveling extends Motor {
  constructor (
    id,
    name,
    inputs = [],
    outputs = [],
    positions = [],
    ready = [],
    inverter
  ) {
    super(id, name, inputs, outputs, positions, ready, inverter)
    this.motion_ = ['motion-left', 'motion-right']
  }
}

const updateMotors = util.promisify(
  (start, buffer, offset, motors, callback) => {
    let byte = start
    for (let i = 0; i < motors.length; i++) {
      motors[i].update(buffer.slice(byte, byte + offset))
      byte += offset
    }
    callback(null, motors)
  }
)

module.exports = {
  updateMotors,
  Actuator,
  Door,
  Flap,
  Lock,
  Hoisting,
  Rotation,
  Traveling
}
