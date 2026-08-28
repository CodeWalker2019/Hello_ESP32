import { serialState } from './state'
import { stopHeartbeat } from './stopHeartbeat'
import { PORT_RELEASE_DELAY_MS } from './constants'

/**
 * Stops the heartbeat and closes the active port, if any, then clears
 * connection state. Waits for the close to actually complete (plus a short
 * buffer for the OS to release the lock) before resolving — otherwise a
 * scan or connect triggered right after this resolves can race the
 * still-closing port and fail with "Cannot lock port".
 */
export function disconnect(): Promise<void> {
  stopHeartbeat()

  return new Promise((resolve) => {
    const finish = (): void => {
      serialState.port = null
      resolve()
    }

    if (serialState.port && serialState.port.isOpen) {
      serialState.port.close(() => {
        console.log('ESP32 disconnected and heartbeat stopped.')
        setTimeout(finish, PORT_RELEASE_DELAY_MS)
      })
    } else {
      finish()
    }
  })
}
