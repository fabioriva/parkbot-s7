exports.bytesToInt = function bytesToInt (b1, b2) {
  return (b1 << 8) | b2
}

exports.bytesToLong = function bytesToLong (b1, b2, b3, b4) {
  return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4
}

exports.intToBytes = function intToBytes (i, b) {
  b[0] = i & 0xff
  b[1] = (i >> 8) & 0xff
  return i
}

exports.longToBytes = function longToBytes (i, b) {
  b[0] = i & 0xff
  b[1] = (i >> 8) & 0xff
  b[2] = (i >> 16) & 0xff
  b[3] = (i >> 24) & 0xff
  return i
}

/**
 * Parameters:
 * days - number of days since 1990-1-1
 * msec - number of milliseconds since 00:00 (midnight)
 * Return value:
 * The number of milliseconds between 1 January 1970 00:00:00 UTC and the given date
 */

exports.getPlcDateTime = function getPlcDateTime (days, msec) {
  const h = Math.floor(msec / 3600000)
  const m = Math.floor((msec % 3600000) / 60000)
  const s = Math.floor(((msec % 3600000) % 60000) / 1000)
  const ms = Math.floor(((msec % 3600000) % 60000) % 1000)
  const d = new Date(1990, 0, 1, h, m, s, ms)
  return d.setDate(d.getDate() + days)
}

// export function setPlcDate (sdate) {
//   const date = dayjs(sdate, ['YYYY-MM-DD'])
//   const init = dayjs('1990-1-1')
//   return date.diff(init, 'day') // number of days since 1990-1-1
// }

// export function setPlcTime (stime) {
//   const time = dayjs(stime, ['HH:mm:ss']) // string to moment
//   const h = dayjs(time).hour()
//   const m = dayjs(time).minute()
//   const s = dayjs(time).second()
//   const ms = dayjs(time).millisecond()
//   return h * 3600000 + m * 60000 + s * 1000 + ms // number of milliseconds
// }
