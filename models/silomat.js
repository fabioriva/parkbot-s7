// const util = require('util')
const { Actuator } = require('./motors')

class Silomat {
  constructor (id, name, inputs = [], outputs = [], ready = [], speed = 0) {
    this.id = id
    this.name = name
    this.M1 = new Traveling(
      1,
      { key: 'mot-traveling' },
      [inputs[0], inputs[1]],
      [outputs[0], outputs[3], outputs[5]]
    )
    this.M2 = new Hoisting(
      2,
      { key: 'mot-hoisting' },
      [inputs[2], inputs[3]],
      [outputs[1], outputs[2], outputs[3]]
    )
    this.M3 = new Centering(
      3,
      { key: 'mot-centering', query: { id: 1 } },
      [inputs[4], inputs[5]],
      [outputs[1], outputs[2], outputs[4]]
    )
    this.M4 = new Centering(
      4,
      { key: 'mot-centering', query: { id: 2 } },
      [inputs[7], inputs[6]],
      [outputs[1], outputs[2], outputs[5]]
    )
    // this.motors = [this.M1, this.M2, this.M3, this.M4]
  }

  get motors () {
    return [this.M1, this.M2, this.M3, this.M4]
  }

  get json () {
    return {
      id: this.id,
      name: this.name,
      motors: this.motors,
      M1: this.M1.json,
      M2: this.M2.json,
      M3: this.M3.json,
      M4: this.M4.json
    }
  }
}

class Traveling extends Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    super(id, name, inputs, outputs)
    this.motion_ = ['motion-bwd', 'motion-fwd']
    this.position_ = ['position-bwd', 'position-fwd']
  }
}

class Hoisting extends Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    super(id, name, inputs, outputs)
    this.motion_ = ['motion-up', 'motion-down']
    this.position_ = ['position-low', 'position-high']
  }
}

class Centering extends Actuator {
  constructor (id, name, inputs = [], outputs = []) {
    super(id, name, inputs, outputs)
    this.motion_ = ['motion-center', 'motion-uncenter']
    this.position_ = ['position-uncentered', 'position-centered']
  }
}

module.exports = { Silomat }

// const { generateBits } = require('./bits')

// class Silomat {
//   constructor (id, name, inputs = [], outputs = [], sensors = []) {
//     this.id = id
//     this.name = name
//     this.inputs = inputs
//     this.outputs = outputs
//     this.sensors = sensors
//     this.motor = generateBits('M', 0, 0)
//     this.flags = generateBits('M', 1, 1)
//   }

//   get json () {
//     return {
//       id: this.id,
//       name: this.name,
//       motion: this.motion(),
//       position: this.position(),
//       inputs: this.inputs,
//       outputs: this.outputs,
//       sensors: this.sensors,
//       enable: this.motor[4].status
//     }
//   }

//   motion () {
//     const T2 = this.outputs[0].status
//     const TRA = this.outputs[1].status
//     const TRB = this.outputs[2].status
//     const KCS = this.outputs[3].status
//     const KCV = this.outputs[4].status
//     const KCH = this.outputs[5].status
//     if (!T2 && !TRA && !TRB) {
//       return { id: 0, i18n: 'motion-no' }
//     } else if (T2) {
//       return { id: 1, i18n: 'motion-traveling' }
//     } else if (TRA && KCS) {
//       return { id: 2, i18n: 'motion-up' }
//     } else if (TRB && KCS) {
//       return { id: 3, i18n: 'motion-down' }
//     } else if (TRA && (KCV || KCH)) {
//       return { id: 4, i18n: 'motion-opening' }
//     } else if (TRB && (KCV || KCH)) {
//       return { id: 5, i18n: 'motion-closing' }
//     } else {
//       return { id: 6, i18n: 'motion-err' }
//     }
//   }

//   position () {
//     return { id: 0, i18n: 'position-no' }
//   }

//   update (buffer) {
//     let mask
//     mask = 1
//     for (let i = 0; i < this.motor.length; i++) {
//       this.motor[i].status = buffer[0] & mask ? 1 : 0
//       mask *= 2
//     }
//     mask = 1
//     for (let i = 0; i < this.flags.length; i++) {
//       this.flags[i].status = buffer[1] & mask ? 1 : 0
//       mask *= 2
//     }
//   }
// }

// module.exports = { Silomat }
