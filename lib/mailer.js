// const http = require('http')
const util = require('util')
const fetch = require('node-fetch')
const mongo = require('mongodb')
const { format, utcToZonedTime } = require('date-fns-tz')

class Item {
  constructor (email, name, phone, status = true) {
    this.email = email
    this.name = name
    this.phone = phone
    this.status = status
  }
}

exports.getMailingList = util.promisify(async (db, callback) => {
  db.collection('mailingList')
    .find()
    .toArray(function (err, mailingList) {
      if (err) callback(err)
      callback(null, {
        // count: docs.length,
        // mailingList: docs
        mailingList
      })
    })
})

exports.insertItem = util.promisify((db, { email, name, phone }, callback) => {
  const item = new Item(email, name, phone)
  db.collection('mailingList').insertOne(item, function (err, result) {
    if (err) callback(err)
    // console.log(result)
    callback(null, item)
  })
})

exports.deleteItem = util.promisify((db, { _id }, callback) => {
  db.collection('mailingList').deleteOne(
    { _id: new mongo.ObjectId(_id) },
    function (err, result) {
      if (err) callback(err)
      // console.log(result)
      callback(null, result)
    }
  )
})

const getRecipientList = util.promisify((db, callback) => {
  db.collection('mailingList')
    .find({ status: true }, { _id: 0, email: 1 })
    .toArray(function (err, docs) {
      if (err) callback(err)
      const recipientList = docs.map(e => e.email)
      callback(null, {
        recipientList
      })
    })
})

exports.sendMail = util.promisify(async (db, aps, doc, callback) => {
  try {
    const date = format(
      utcToZonedTime(doc.date, 'UTC'),
      'yyyy-MM-dd HH:mm:ss.SSS',
      {
        timeZone: 'UTC'
      }
    )
    const { device, alarm } = doc
    const { recipientList } = await getRecipientList(db)
    if (recipientList.length === 0) {
      return callback(null, 'Empty recipient list')
    }

    const body = {
      aps,
      alarm,
      date,
      device,
      locale: 'en',
      recipientList // : ['f.riva@sotefin.ch']
    }

    const response = await fetch('http://localhost:3004', {
      method: 'post',
      body: JSON.stringify(body),
      headers: { 'Content-Type': 'application/json' }
    })
    const data = await response.json()
    // const data = await response.text()
    // console.log(data)
    callback(null, data)
  } catch (err) {
    callback(err)
  }
  // use node-fetch ???!!!???: body.length

  // const options = {
  //   hostname: 'localhost',
  //   port: 3002,
  //   path: '/',
  //   method: 'POST',
  //   headers: {
  //     'Content-Type': 'application/json',
  //     'Content-Length'
  //   }
  // }
  // const req = http.request(options, res => {
  //   console.log(`statusCode: ${res.statusCode}`)

  //   res.on('data', d => {
  //     process.stdout.write(d)
  //   })
  // })

  // req.on('error', error => {
  //   console.error(error)
  //   callback(error)
  // })

  // req.write(body)
  // req.end()
  // callback(null, body)
})
