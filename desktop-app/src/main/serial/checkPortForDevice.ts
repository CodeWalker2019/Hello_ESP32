import { SerialPort } from 'serialport'
import { BAUD_RATE, HANDSHAKE_TIMEOUT_MS, TARGET_DEVICE_FAMILY_ID } from './constants'

/**
 * Briefly opens the port at `path` and waits for the ESP32 handshake reply
 * (0xAA 0x55 <familyId>). Resolves `true` on a matching reply, `false` on
 * timeout, a non-matching reply, or an open error. Never rejects.
 */
export function checkPortForDevice(path: string): Promise<boolean> {
  return new Promise((resolve) => {
    const tempPort = new SerialPort({ path, baudRate: BAUD_RATE, autoOpen: false })
    let resolved = false
    let timeoutHandle: ReturnType<typeof setTimeout> | null = null

    const finish = (result: boolean): void => {
      if (resolved) return
      resolved = true

      if (timeoutHandle) {
        clearTimeout(timeoutHandle)
        timeoutHandle = null
      }

      if (tempPort.isOpen) {
        console.log(`Closing port ${path} after handshake check with result: ${result}.`)
        tempPort.close((closeErr) => {
          if (closeErr) {
            console.error(`Error closing port ${path}:`, closeErr)
          }
          // small delay so the OS fully releases the handle before
          // any immediately-following open attempt (scan -> connect race)
          setTimeout(() => resolve(result), 100)
        })
      } else {
        resolve(result)
      }
    }

    console.log(`Attempting to open port ${path}...`)

    tempPort.open((err) => {
      if (err) {
        console.error(`Error opening port ${path}:`, err)
        finish(false)
        return
      }
    })

    // Guards against errors that occur AFTER a successful open
    // (e.g. device disconnected mid-check) — without this, such
    // errors would go unhandled and could leave the promise pending.
    tempPort.on('error', (err) => {
      console.error(`Port error on ${path}:`, err)
      finish(false)
    })

    tempPort.on('data', (data: Buffer) => {
      if (resolved) return

      console.log(`Data received on ${path}:`, data, data.length)

      if (
        data.length >= 3 &&
        data[0] === 0xaa &&
        data[1] === 0x55 &&
        data[2] === TARGET_DEVICE_FAMILY_ID
      ) {
        console.log(`Data matches handshake on ${path}: ${data}`)
        finish(true)
      }
    })

    timeoutHandle = setTimeout(() => {
      finish(false)
    }, HANDSHAKE_TIMEOUT_MS)
  })
}
