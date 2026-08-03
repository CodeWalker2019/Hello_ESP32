import { serialState } from './state'
import { stopHeartbeat } from './stopHeartbeat'

/** Stops the heartbeat and closes the active port, if any, and clears connection state. */
export function disconnect(): void {
  stopHeartbeat()

  if (serialState.port && serialState.port.isOpen) {
    serialState.port.close(() => {
      console.log('ESP32 disconnected and heartbeat stopped.')
    })
  }
  serialState.port = null
  serialState.parser = null
}
