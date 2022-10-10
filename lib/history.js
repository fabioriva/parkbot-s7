const { parseISO, subDays } = require('date-fns')
const { format, utcToZonedTime } = require('date-fns-tz')
const util = require('util')
const { daily, weekly, monthly, yearly } = require('./statistics')

const COLLECTION = 'history'

class Log {
  constructor (s7log) {
    this.alarm = s7log.alarm
    this.card = s7log.card
    this.date = new Date(format(s7log.date, "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'"))
    this.device = s7log.device
    // this.event = s7log.event
    this.mode = s7log.mode
    this.operation = s7log.operation
    this.size = s7log.size
    this.stall = s7log.stall
  }
}

exports.getHistory = util.promisify((db, query, callback) => {
  // const { dateFrom, dateTo } = query
  // console.log(typeof dateFrom, dateFrom)
  // const parsed = parseISO(dateFrom)
  // console.log(parsed)
  // console.log(
  //   format(utcToZonedTime(parsed, 'UTC'), 'yyyy-MM-dd HH:mm', {
  //     timeZone: 'UTC'
  //   })
  // )
  const start = parseISO(query.dateFrom)
  const end = parseISO(query.dateTo)

  const pipeline = [
    {
      $match: {
        date: {
          $gte: start, // + 'Z'), // new Date(dateFrom),
          $lt: end // + 'Z') // new Date(dateTo)
        }
      }
    },
    {
      $lookup: {
        from: 'alarms',
        localField: 'alarm',
        foreignField: 'id',
        as: 'alarm'
      }
    },
    // { $unwind: '$alarm' },
    { $unwind: { path: '$alarm', preserveNullAndEmptyArrays: true } },
    {
      $lookup: {
        from: 'devices',
        localField: 'device',
        foreignField: 'id',
        as: 'device'
      }
    },
    { $unwind: '$device' },
    {
      $lookup: {
        from: 'modes',
        localField: 'mode',
        foreignField: 'id',
        as: 'mode'
      }
    },
    { $unwind: '$mode' },
    {
      $lookup: {
        from: 'operations',
        localField: 'operation',
        foreignField: 'id',
        as: 'operation'
      }
    },
    { $unwind: '$operation' },
    { $unset: ['_id', 'alarm._id', 'device._id', 'mode._id', 'operation._id'] },
    { $sort: { date: -1 } }
  ]
  db.collection(COLLECTION)
    .aggregate(pipeline, { enableUtf8Validation: false })
    // .sort({ date: -1 })
    .toArray(function (err, docs) {
      if (err) callback(err)
      callback(null, {
        count: docs.length,
        // dateFrom: format(parseISO(dateFrom), 'yyyy-MM-dd HH:mm'),
        // dateTo: format(parseISO(dateTo), 'yyyy-MM-dd HH:mm'),
        dateFrom: format(utcToZonedTime(start, 'UTC'), 'yyyy-MM-dd HH:mm', {
          timeZone: 'UTC'
        }),
        dateTo: format(utcToZonedTime(end, 'UTC'), 'yyyy-MM-dd HH:mm', {
          timeZone: 'UTC'
        }),
        query: docs
      })
    })
})

exports.getHistoryLog = util.promisify((db, id, callback) => {
  const pipeline = [
    {
      // $match: { _id: new mongo.ObjectID(id) }
      $match: { _id: id }
    },
    {
      $lookup: {
        from: 'alarms',
        localField: 'alarm',
        foreignField: 'id',
        as: 'alarm'
      }
    },
    { $unwind: { path: '$alarm', preserveNullAndEmptyArrays: true } },
    {
      $lookup: {
        from: 'devices',
        localField: 'device',
        foreignField: 'id',
        as: 'device'
      }
    },
    { $unwind: '$device' },
    {
      $lookup: {
        from: 'modes',
        localField: 'mode',
        foreignField: 'id',
        as: 'mode'
      }
    },
    { $unwind: '$mode' },
    {
      $lookup: {
        from: 'operations',
        localField: 'operation',
        foreignField: 'id',
        as: 'operation'
      }
    },
    { $unwind: '$operation' },
    { $unset: ['_id', 'alarm._id', 'device._id', 'mode._id', 'operation._id'] }
  ]
  db.collection(COLLECTION)
    .aggregate(pipeline)
    .toArray(function (err, results) {
      if (err) callback(err)
      callback(null, results[0])
    })
})

exports.getOperations = util.promisify(async (db, query, callback) => {
  // console.log(query.dateString)
  try {
    // const date_ = parseISO(query.dateString)
    const date = new Date(query.dateString)
    const history = db.collection(COLLECTION)
    const operations = await Promise.all([
      daily(date, history),
      weekly(date, history),
      monthly(date, history),
      yearly(date, history)
    ])
    callback(null, operations)
  } catch (err) {
    callback(err)
  }
})

exports.getRecentActivity = util.promisify((db, limit, callback) => {
  const pipeline = [
    {
      $match: {
        date: {
          $gte: subDays(new Date(), 1)
        }
        // _id: { $exists: true }
      }
    },
    { $sort: { date: -1 } },
    { $limit: limit },
    {
      $lookup: {
        from: 'alarms',
        localField: 'alarm',
        foreignField: 'id',
        as: 'alarm'
      }
    },
    { $unwind: { path: '$alarm', preserveNullAndEmptyArrays: true } },
    {
      $lookup: {
        from: 'devices',
        localField: 'device',
        foreignField: 'id',
        as: 'device'
      }
    },
    { $unwind: '$device' },
    {
      $lookup: {
        from: 'modes',
        localField: 'mode',
        foreignField: 'id',
        as: 'mode'
      }
    },
    { $unwind: '$mode' },
    {
      $lookup: {
        from: 'operations',
        localField: 'operation',
        foreignField: 'id',
        as: 'operation'
      }
    },
    { $unwind: '$operation' },
    { $unset: ['_id', 'alarm._id', 'device._id', 'mode._id', 'operation._id'] }
  ]

  db.collection(COLLECTION)
    .aggregate(pipeline)
    .toArray(function (err, docs) {
      if (err) callback(err)
      callback(null, {
        count: docs.length,
        documents: docs
      })
    })
})

exports.insertLog = util.promisify((db, s7log, callback) => {
  const log = new Log(s7log)
  db.collection(COLLECTION).insertOne(log, function (err, result) {
    if (err) callback(err)
    callback(null, result)
  })
})
