const mongo = require('mongodb')

class ListItem {
  constructor (email, name, phone, status = true) {
    this.email = email
    this.name = name
    this.phone = phone
    this.status = status
  }
}

class MailingList {
  constructor (db) {
    this.collection = db.collection('mailingList')
  }

  async deleteListItem ({ _id }) {
    const result = await this.collection.deleteOne({ _id: new mongo.ObjectId(_id) })
    return result
  }

  async get () {
    const docs = await this.collection.find().toArray()
    return docs
  }

  async getRecipientList () {
    const docs = await this.collection.find({ status: true }, { _id: 0, email: 1 }).toArray()
    const recipientList = docs.map(e => e.email)
    return recipientList
  }

  async insertListItem ({ email, name, phone }) {
    const item = new ListItem(email, name, phone)
    const result = await this.collection.insertOne(item)
    return result
  }
}

module.exports = MailingList
