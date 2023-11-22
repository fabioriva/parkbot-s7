const logger = require('pino')()
const snap7 = require('bindings')('node_snap7_client')

async function forever (client) {
  try {
    await client.ReadArea(0x84, 505, 0, 142, 0x02)
    // const buffer = await client.ReadArea(0x84, 505, 0, 142, 0x02)
    // logger.info(buffer)
  } catch (e) {
    logger.error(e)
  }
}

async function main () {
  try {
    const client = new snap7.Snap7Client()
    await client.ConnectTo('192.168.20.55', 0, 1)
    logger.info('PLC %s is online', '192.168.20.55')
    setInterval(() => forever(client), 500)
  } catch (e) {
    logger.error(e)
  }
}

main()
