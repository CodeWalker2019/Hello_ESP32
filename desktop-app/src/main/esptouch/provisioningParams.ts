import { Crc8 } from './crc8'
import { touchAesEncrypt } from './touchAes'
import { SYNC_PACKET_LENGTH } from './constants'

const HEADER_VERSION = 0
const SEQUENCE_FIRST = -1
const EMPTY = Buffer.alloc(0)

export interface EsptouchV2Request {
  ssid: Buffer
  password: Buffer
  bssid?: Buffer
  aesKey?: Buffer
}

function checkNeedsEncodeFlag(data: Buffer): boolean {
  for (const byte of data) {
    if (byte >= 128) return true
  }
  return false
}

function randomPadding(length: number): Buffer {
  const buf = Buffer.alloc(length)
  for (let i = 0; i < length; i++) buf[i] = Math.floor(Math.random() * 127)
  return buf
}

function padTo(data: Buffer, factor: number): Buffer {
  const padding = factor - (data.length % factor)
  return padding < factor ? randomPadding(padding) : EMPTY
}

function buildHeader(ssid: Buffer, password: Buffer, bssid: Buffer, willEncrypt: boolean): Buffer {
  const crc = new Crc8()

  const ssidInfo = ssid.length | (checkNeedsEncodeFlag(ssid) ? 0b1000_0000 : 0)
  const pwdInfo = password.length | (checkNeedsEncodeFlag(password) ? 0b1000_0000 : 0)
  const reservedInfo = 0

  crc.update(bssid)
  const bssidCrc = crc.get()

  const securityVer = willEncrypt ? 1 : 0
  const flag = 1 | (willEncrypt ? securityVer << 1 : 0) | ((HEADER_VERSION & 0b11) << 6)

  const head = Buffer.from([ssidInfo, pwdInfo, reservedInfo, bssidCrc, flag, 0])
  crc.reset()
  crc.update(head, 0, 5)
  head[5] = crc.get()
  return head
}

function applyAppPortMark(head: Buffer, appPortMark: number): void {
  head[4] |= (appPortMark & 0b11) << 3
  const crc = new Crc8()
  crc.update(head, 0, 5)
  head[5] = crc.get()
}

export function buildEsptouchV2Packets(request: EsptouchV2Request, appPortMark: number): Buffer[] {
  const ssid = request.ssid
  const password = request.password
  const bssid = request.bssid ?? Buffer.alloc(6)
  const willEncrypt = !!request.aesKey && password.length > 0

  const head = buildHeader(ssid, password, bssid, willEncrypt)
  applyAppPortMark(head, appPortMark)

  let encodedPassword: Buffer
  let passwordEncode: boolean
  let passwordPaddingFactor: number

  if (willEncrypt) {
    encodedPassword = touchAesEncrypt(request.aesKey!, Buffer.alloc(16), password)
    passwordEncode = true
    passwordPaddingFactor = 5
  } else if (!checkNeedsEncodeFlag(password)) {
    encodedPassword = password
    passwordEncode = false
    passwordPaddingFactor = 6
  } else {
    encodedPassword = password
    passwordEncode = true
    passwordPaddingFactor = 5
  }
  const passwordPadding = padTo(encodedPassword, passwordPaddingFactor)

  const ssidEncode = checkNeedsEncodeFlag(ssid)
  const ssidPaddingFactor = ssidEncode ? 5 : 6
  const ssidPadding = padTo(ssid, ssidPaddingFactor)

  const full = Buffer.concat([head, encodedPassword, passwordPadding, ssid, ssidPadding])
  const ssidBeginPosition = head.length + encodedPassword.length + passwordPadding.length

  const packets: Buffer[] = []
  const crc = new Crc8()
  let offset = 0
  let sequence = SEQUENCE_FIRST
  const sequenceSizePacketSlots: number[] = []

  const addDataFor6Bytes = (buf: Buffer, seq: number, seqCrc: number, tailIsCrc: boolean): void => {
    if (seq === SEQUENCE_FIRST) {
      packets.push(Buffer.alloc(SYNC_PACKET_LENGTH))
      sequenceSizePacketSlots.push(packets.length)
      packets.push(EMPTY)
      packets.push(Buffer.alloc(SYNC_PACKET_LENGTH))
      sequenceSizePacketSlots.push(packets.length)
      packets.push(EMPTY)
    } else {
      const sequencePacket = Buffer.alloc(128 + seq)
      packets.push(sequencePacket, sequencePacket, sequencePacket)
    }

    const bitCount = tailIsCrc ? 7 : 8
    for (let i = 0; i < bitCount; i++) {
      const data =
        ((buf[5] >> i) & 1) |
        (((buf[4] >> i) & 1) << 1) |
        (((buf[3] >> i) & 1) << 2) |
        (((buf[2] >> i) & 1) << 3) |
        (((buf[1] >> i) & 1) << 4) |
        (((buf[0] >> i) & 1) << 5)
      packets.push(Buffer.alloc((i << 7) | (1 << 6) | data))
    }

    if (tailIsCrc) {
      packets.push(Buffer.alloc((7 << 7) | (1 << 6) | seqCrc))
    }
  }

  while (offset < full.length) {
    let expectLength: number
    let crcInPacket: boolean

    if (sequence === SEQUENCE_FIRST) {
      crcInPacket = true
      expectLength = 6
    } else if (offset < ssidBeginPosition) {
      crcInPacket = passwordEncode
      expectLength = passwordPaddingFactor
    } else {
      crcInPacket = ssidEncode
      expectLength = ssidPaddingFactor
    }

    const readLength = Math.min(expectLength, full.length - offset)
    const buf = Buffer.alloc(6)
    full.copy(buf, 0, offset, offset + readLength)
    offset += readLength

    crc.reset()
    crc.update(buf, 0, readLength)
    const seqCrc = crc.get()
    if (expectLength < buf.length) {
      buf[buf.length - 1] = seqCrc
    }

    addDataFor6Bytes(buf, sequence, seqCrc, !crcInPacket)
    sequence++
  }

  const sequenceSizePacket = Buffer.alloc(1072 + sequence)
  for (const slot of sequenceSizePacketSlots) {
    packets[slot] = sequenceSizePacket
  }

  return packets
}
