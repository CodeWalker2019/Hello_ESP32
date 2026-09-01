import type { IpcMainEvent } from 'electron'
import { connect as netConnect } from 'net'
import { wifiConnectionState } from './state'
import { WIFI_TELEMETRY_PORT } from './constants'
import { disconnectWifi } from './disconnect'
import { disconnect as disconnectSerial } from '../serial/disconnect'
import { TelemetryParser } from '../serial/telemetryParser'

export async function connectWifi(event: IpcMainEvent, address: string): Promise<void> {
  await Promise.all([disconnectWifi(), disconnectSerial()])

  const socket = netConnect({ host: address, port: WIFI_TELEMETRY_PORT })
  wifiConnectionState.socket = socket

  const telemetryParser = new TelemetryParser()

  socket.on('connect', () => {
    console.log(`Connected to target ESP32 over Wi-Fi at ${address}:${WIFI_TELEMETRY_PORT}`)
  })

  socket.on('data', (chunk: Buffer) => {
    for (const reading of telemetryParser.push(chunk)) {
      event.sender.send('esp32-telemetry', reading)
    }
  })

  socket.on('error', (err) => {
    console.error('Wi-Fi Telemetry Socket Error:', err.message)
  })
}
