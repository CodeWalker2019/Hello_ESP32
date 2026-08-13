import type { IpcMainEvent } from 'electron'
import { SerialPort } from 'serialport'
import { serialState } from './state'
import { BAUD_RATE, START_PACKET } from './constants'
import { disconnect } from './disconnect'
import { startHeartbeat } from './startHeartbeat'
import { TelemetryParser } from './telemetryParser'

/**
 * Tears down any prior connection, opens `portPath`, sends the start
 * packet, starts the heartbeat, and streams each decoded telemetry packet
 * back to `event`'s sender as an `esp32-telemetry` message.
 */
export async function connect(event: IpcMainEvent, portPath: string): Promise<void> {
  await disconnect()

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
      event.sender.send('esp32-telemetry', reading)
    }
  })

  port.on('error', (err) => {
    console.error('Serial Port Error:', err.message)
    disconnect().catch((error) => {
      console.error('Error disconnecting after port error:', error)
    })
  })

  port.on('close', () => {
    console.log('Serial port closed.')
    disconnect().catch((error) => {
      console.error('Error disconnecting after port close:', error)
    })
  })
}
