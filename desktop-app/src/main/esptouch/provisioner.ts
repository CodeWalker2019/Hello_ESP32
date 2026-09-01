import dgram from 'dgram'
import {
  APP_PORTS,
  DEVICE_ACK_PORT,
  DEVICE_PORT,
  DATA_PKG_INTERVAL_MS,
  DATA_PKG_INTERVAL_SLOW_MS,
  DATA_PKG_TIMEOUT_SEND_MS
} from './constants'
import { buildEsptouchV2Packets, EsptouchV2Request } from './provisioningParams'
import { getBroadcastAddresses } from './broadcastAddress'

export interface ProvisionedDevice {
  address: string
  mac: string
}

export class EsptouchV2Provisioner {
  private socket: dgram.Socket | null = null
  private sendTimeout: NodeJS.Timeout | null = null
  private readonly seenDevices = new Set<string>()

  async start(
    request: EsptouchV2Request,
    onDeviceFound: (device: ProvisionedDevice) => void
  ): Promise<void> {
    this.stop()
    this.seenDevices.clear()

    const { socket, portMark } = await this.bindAppSocket()
    this.socket = socket

    const packets = buildEsptouchV2Packets(request, portMark)
    const broadcastAddresses = getBroadcastAddresses()

    socket.on('message', (message, remoteInfo) => {
      this.handleDeviceMessage(message, remoteInfo, onDeviceFound)
    })
    socket.on('error', (err) => {
      console.error('[ESPTouch V2] Socket error:', err)
    })

    const startedAt = Date.now()
    let index = 0

    const sendNext = (): void => {
      if (!this.socket) return

      const elapsed = Date.now() - startedAt
      if (elapsed > DATA_PKG_TIMEOUT_SEND_MS) {
        console.log('[ESPTouch V2] Provisioning window elapsed, stopping broadcast')
        this.stop()
        return
      }

      const packet = packets[index % packets.length]
      for (const address of broadcastAddresses) {
        this.socket.send(packet, DEVICE_PORT, address, (err) => {
          if (err) console.error(`[ESPTouch V2] Send error to ${address}:`, err)
        })
      }
      index++

      const interval =
        elapsed > DATA_PKG_TIMEOUT_SEND_MS / 2 ? DATA_PKG_INTERVAL_SLOW_MS : DATA_PKG_INTERVAL_MS
      this.sendTimeout = setTimeout(sendNext, interval)
    }

    console.log(
      `[ESPTouch V2] Broadcasting ${packets.length} packets/cycle on app port ${APP_PORTS[portMark]} (mark ${portMark}) to`,
      broadcastAddresses
    )
    sendNext()
  }

  stop(): void {
    if (this.sendTimeout) {
      clearTimeout(this.sendTimeout)
      this.sendTimeout = null
    }
    if (this.socket) {
      try {
        this.socket.close()
      } catch {}
      this.socket = null
    }
  }

  private handleDeviceMessage(
    message: Buffer,
    remoteInfo: dgram.RemoteInfo,
    onDeviceFound: (device: ProvisionedDevice) => void
  ): void {
    if (message.length < 7) return

    const mac = Array.from(message.subarray(1, 7))
      .map((byte) => byte.toString(16).padStart(2, '0'))
      .join(':')

    this.sendDeviceAck(remoteInfo.address)

    if (this.seenDevices.has(mac)) return
    this.seenDevices.add(mac)
    onDeviceFound({ address: remoteInfo.address, mac })
  }

  private sendDeviceAck(deviceAddress: string): void {
    if (!this.socket) return
    const ackPacket = Buffer.from([1])
    this.socket.send(ackPacket, DEVICE_ACK_PORT, deviceAddress)
    this.socket.send(ackPacket, DEVICE_ACK_PORT, deviceAddress)
  }

  private bindAppSocket(): Promise<{ socket: dgram.Socket; portMark: number }> {
    return new Promise((resolve, reject) => {
      const tryBind = (index: number): void => {
        if (index >= APP_PORTS.length) {
          reject(new Error('Could not bind any ESPTouch app port'))
          return
        }

        const socket = dgram.createSocket({ type: 'udp4', reuseAddr: true })
        const onError = (): void => {
          socket.close()
          tryBind(index + 1)
        }
        socket.once('error', onError)
        socket.bind(APP_PORTS[index], () => {
          socket.removeListener('error', onError)
          socket.setBroadcast(true)
          resolve({ socket, portMark: index })
        })
      }

      tryBind(0)
    })
  }
}
