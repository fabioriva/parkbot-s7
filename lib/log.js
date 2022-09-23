const { format, utcToZonedTime } = require('date-fns-tz')
const fetch = require('node-fetch')
const logger = require('pino')()

module.exports = async function handleLog (app, aps, history, log, mailingList) {
  const doc = await history.saveLog(log)
  app.publish('aps/info', JSON.stringify({ notification: doc }))
  if (doc.operation.id === 1) {
    const recipientList = await mailingList.getRecipientList()
    console.log(recipientList)
    if (recipientList.length > 0) {
      const date = format(
        utcToZonedTime(doc.date, 'UTC'),
        'yyyy-MM-dd HH:mm:ss.SSS',
        {
          timeZone: 'UTC'
        }
      )
      const { device, alarm } = doc
      const body = {
        aps,
        alarm,
        date,
        device,
        locale: 'en',
        recipientList
      }
      // console.log(body)
      const response = await fetch(process.env.MAIL_PROVIDER, {
        method: 'post',
        body: JSON.stringify(body),
        headers: { 'Content-Type': 'application/json' }
      })
      const data = await response.json()
      logger.info(data, 'Mailer')
    }
  }
}
