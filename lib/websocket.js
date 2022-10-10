function websocket (app, obj, options) {
  const { prefix } = options

  // app.ws('/*', {
  //   /* Options */
  //   compression: options.compression,
  //   maxPayloadLength: options.maxPayloadLength,
  //   idleTimeout: options.idleTimeout,
  //   /* Handlers */
  //   open: ws => {
  //     console.log('A WebSocket connected!')
  //   },
  //   message: (ws, message, isBinary) => {
  //     /* Ok is false if backpressure was built up, wait for drain */
  //     let ok = ws.send(message, isBinary)
  //   },
  //   drain: ws => {
  //     console.log('WebSocket backpressure: ' + ws.getBufferedAmount())
  //   },
  //   close: (ws, code, message) => {
  //     console.log('WebSocket closed')
  //   }
  // })

  app.ws(prefix + '/cards', {
    open: ws => {
      ws.subscribe('aps/cards')
    }
  })

  app.ws(prefix + '/diagnostic', {
    open: ws => {
      ws.subscribe('aps/diagnostic')
    }
  })

  app.ws(prefix + '/info', {
    open: ws => {
      ws.subscribe('aps/info')
    }
  })

  app.ws(prefix + '/map', {
    open: ws => {
      ws.subscribe('aps/map')
    }
  })

  app.ws(prefix + '/overview', {
    open: ws => {
      ws.subscribe('aps/overview')
    }
    // message: (ws, message, isBinary) => {
    //   /* Ok is false if backpressure was built up, wait for drain */
    //   console.log(message, message.toString('utf8'))
    //   console.log(Buffer.from(message))
    //   console.log(Buffer.from(message).toString('utf8'))
    //   ws.locale = Buffer.from(message).toString('utf8')
    //   let ok = ws.send(message, isBinary)
    //   console.log(ws.isSubscribed())
    //   // ws.publish('aps/overview', JSON.stringify(obj.overview))
    // },
    // close: (ws, code, message) => {
    //   console.log('WebSocket closed', ws.locale)
    // }
  })

  app.ws(prefix + '/racks', {
    open: ws => {
      ws.subscribe('aps/racks')
    }
  })

  // obj.alarms.map((e, index) =>
  //   app.ws(prefix + '/alarms/' + index, {
  //     open: ws => {
  //       ws.subscribe('aps/alarms/' + index)
  //     }
  //   })
  // )

  obj.diagnostic.forEach((e, index) =>
    app.ws(prefix + '/diagnostic/' + index, {
      open: ws => {
        ws.subscribe('aps/diagnostic/' + index)
      }
    })
  )

  obj.racks.forEach((e, index) =>
    app.ws(prefix + '/racks/' + index, {
      open: ws => {
        ws.subscribe('aps/racks/' + index)
      }
    })
  )
}

module.exports = websocket
