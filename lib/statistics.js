const util = require('util')
const {
  format,
  endOfDay,
  endOfMonth,
  endOfWeek,
  // endOfYear,
  // parseISO,
  startOfDay,
  startOfMonth,
  startOfWeek,
  // startOfYear,
  subMonths,
  subWeeks,
  subYears
} = require('date-fns')
// const { en } = require('date-fns/locale')

exports.daily = util.promisify((date, history, callback) => {
  // const date = parseISO(dateStr)
  // console.log(typeof date, date, format(date, 'yyyy-MM-dd'))
  const start = startOfDay(date)
  const end = endOfDay(date)
  const pipeline = [
    {
      $match: {
        date: { $gte: start, $lt: end },
        device: { $ne: 0 },
        $or: [{ operation: 5 }, { operation: 6 }]
        // $and: [
        //   { date: { $gte: startOfDay(date), $lt: endOfDay(date) } },
        //   { device: { $ne: 0 } },
        //   { $or: [{ operation: 5 }, { operation: 6 }] }
        // ]
      }
    },
    {
      $group: {
        _id: {
          hour: { $hour: '$date' }
        },
        total: { $sum: 1 },
        entries: { $sum: { $cond: [{ $eq: ['$operation', 5] }, 1, 0] } },
        exits: { $sum: { $cond: [{ $eq: ['$operation', 6] }, 1, 0] } }
      }
    },
    { $sort: { _id: 1 } }
  ]
  history.aggregate(pipeline).toArray(function (err, docs) {
    if (err) callback(err)
    callback(null, {
      data: docs.map(e => {
        const hour = e._id.hour % 12 || 12
        const name =
          e._id.hour < 12 || e._id.hour === 24 ? hour + ' am' : hour + ' pm'
        return {
          name,
          entries: e.entries,
          exits: e.exits,
          total: e.total
        }
      }),
      key: 'daily',
      query: { date: format(date, 'yyyy-MM-dd') }
    })
  })
})

exports.weekly = util.promisify((date, history, callback) => {
  // const date = parseISO(dateStr)
  const start = startOfWeek(subWeeks(date, 1), { weekStartsOn: 1 })
  const end = endOfWeek(subWeeks(date, 1), { weekStartsOn: 1 })
  const pipeline = [
    {
      $match: {
        date: { $gte: start, $lt: end },
        device: { $ne: 0 },
        $or: [{ operation: 5 }, { operation: 6 }]
      }
    },
    {
      $group: {
        _id: {
          year: { $year: '$date' },
          month: { $month: '$date' },
          day: { $dayOfMonth: '$date' }
        },
        total: { $sum: 1 },
        entries: { $sum: { $cond: [{ $eq: ['$operation', 5] }, 1, 0] } },
        exits: { $sum: { $cond: [{ $eq: ['$operation', 6] }, 1, 0] } }
      }
    },
    { $sort: { _id: 1 } }
  ]
  history.aggregate(pipeline).toArray(function (err, docs) {
    if (err) callback(err)
    callback(null, {
      data: docs.map(e => {
        return {
          name: e._id.month + '-' + e._id.day,
          entries: e.entries,
          exits: e.exits,
          total: e.total
        }
      }),
      key: 'weekly',
      query: {
        date: format(start, 'yyyy-MM-dd') + ' ' + format(end, 'yyyy-MM-dd')
      }
    })
  })
})

exports.monthly = util.promisify((date, history, callback) => {
  // const date = parseISO(dateStr)
  const start = startOfMonth(subMonths(date, 1))
  const end = endOfMonth(subMonths(date, 1))
  const pipeline = [
    {
      $match: {
        date: { $gte: start, $lt: end },
        device: { $ne: 0 },
        $or: [{ operation: 5 }, { operation: 6 }]
      }
    },
    {
      $group: {
        _id: {
          year: { $year: '$date' },
          month: { $month: '$date' },
          day: { $dayOfMonth: '$date' }
        },
        total: { $sum: 1 },
        entries: { $sum: { $cond: [{ $eq: ['$operation', 5] }, 1, 0] } },
        exits: { $sum: { $cond: [{ $eq: ['$operation', 6] }, 1, 0] } }
      }
    },
    { $sort: { _id: 1 } }
  ]
  history.aggregate(pipeline).toArray(function (err, docs) {
    if (err) callback(err)
    callback(null, {
      data: docs.map(e => {
        return {
          name: e._id.month + '-' + e._id.day,
          entries: e.entries,
          exits: e.exits,
          total: e.total
        }
      }),
      key: 'monthly',
      query: {
        date: format(start, 'yyyy-MM-dd') + ' ' + format(end, 'yyyy-MM-dd')
      }
    })
  })
})

exports.yearly = util.promisify((date, history, callback) => {
  // const date = parseISO(dateStr)
  const start = subYears(date, 1) // startOfYear(subYears(date, 1))
  const end = date // endOfYear(subYears(date, 1))
  const pipeline = [
    {
      $match: {
        date: { $gte: start, $lt: end },
        device: { $ne: 0 },
        $or: [{ operation: 5 }, { operation: 6 }]
      }
    },
    {
      $group: {
        _id: {
          year: { $year: '$date' },
          month: { $month: '$date' }
          // day: { $dayOfMonth: '$date' }
        },
        total: { $sum: 1 },
        entries: { $sum: { $cond: [{ $eq: ['$operation', 5] }, 1, 0] } },
        exits: { $sum: { $cond: [{ $eq: ['$operation', 6] }, 1, 0] } }
      }
    },
    { $sort: { _id: 1 } }
  ]
  history.aggregate(pipeline).toArray(function (err, docs) {
    if (err) callback(err)
    callback(null, {
      data: docs.map(e => {
        return {
          name: format(new Date().setMonth(e._id.month - 1), 'MMM'),
          entries: e.entries,
          exits: e.exits,
          total: e.total
        }
      }),
      key: 'yearly',
      query: {
        date: format(start, 'yyyy-MM-dd') + ' ' + format(end, 'yyyy-MM-dd')
      }
    })
  })
})
