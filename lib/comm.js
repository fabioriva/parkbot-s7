const util = require('util')

// exports.s7Error = client => !client.Disconnect()

// exports.connect = client => client.Connect()

exports.connectTo = util.promisify((client, PLC, callback) => {
  client.ConnectTo(PLC.ip, PLC.rack, PLC.slot, function (err) {
    if (err) return callback(err)
    callback(err, true)
  })
})

// exports.disconnect = client => client.Disconnect()

exports.readArea = util.promisify(
  (client, area, dbNumber, start, amount, wordLen, callback) => {
    client.ReadArea(area, dbNumber, start, amount, wordLen, function (
      err,
      s7data
    ) {
      if (err) return callback(err)
      callback(err, s7data)
    })
  }
)

exports.writeArea = util.promisify(
  (client, area, dbNumber, start, amount, wordLen, buffer, callback) => {
    client.WriteArea(area, dbNumber, start, amount, wordLen, buffer, function (
      err
    ) {
      if (err) return callback(err)
      callback(err, true)
    })
  }
)
