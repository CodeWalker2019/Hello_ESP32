import type { IpcMainEvent } from 'electron'
import { SerialPort } from 'serialport'
import { ReadlineParser } from '@serialport/parser-readline'
import { serialState } from './state'
import { BAUD_RATE, START_PACKET } from './constants'
import { disconnect } from './disconnect'
import { startHeartbeat } from './startHeartbeat'

/**
 * Tears down any prior connection, opens `portPath`, sends the start
 * packet, starts the heartbeat, and streams each incoming line back to
 * `event`'s sender as an `esp32-data` message.
 */
export async function connect(event: IpcMainEvent, portPath: string): Promise<void> {
  await disconnect()

  const port = new SerialPort({ path: portPath, baudRate: BAUD_RATE })
  const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))
  serialState.port = port
  serialState.parser = parser

  port.on('open', () => {
    console.log(`Connected to target ESP32 on ${portPath}`)

    port.write(START_PACKET, (err) => {
      if (err) console.error('Failed to write start command:', err)
    })

    startHeartbeat()
  })

  parser.on('data', (data: string) => {
    event.sender.send('esp32-data', data)
  })

  port.on('error', (err) => {
    console.error('Serial Port Error:', err.message)
  })
}
