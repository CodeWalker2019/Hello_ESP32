import type { IpcMainEvent } from 'electron'
import { SerialPort } from 'serialport'
import { serialState } from './state'
import { BAUD_RATE, START_PACKET } from './constants'
import { disconnect } from './disconnect'
import { startHeartbeat } from './startHeartbeat'
import { TelemetryParser } from './telemetryParser'
import { disconnectWifi } from '../wifiConnection/disconnect'

export async function connect(event: IpcMainEvent, portPath: string): Promise<void> {
  await Promise.all([disconnect(), disconnectWifi()])

  const port = new SerialPort({ path: portPath, baudRate: BAUD_RATE })
  serialState.port = port

  const telemetryParser = new TelemetryParser()

  port.on('open', () => {
    console.log(`Connected to target ESP32 on ${portPath}`)

    port.write(START_PACKET, (err) => {
      if (err) console.error('Failed to write start command:', err)
    })

    startHeartbeat()
  })

  port.on('data', (chunk: Buffer) => {
    for (const reading of telemetryParser.push(chunk)) {
      console.log(reading)
      event.sender.send('esp32-telemetry', reading)
    }
  })

  port.on('error', (err) => {
    console.error('Serial Port Error:', err.message)
  })
}
