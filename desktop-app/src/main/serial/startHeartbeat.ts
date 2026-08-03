import { serialState } from './state'
import { HEARTBEAT_INTERVAL_MS, HEARTBEAT_PACKET } from './constants'
import { stopHeartbeat } from './stopHeartbeat'

/**
 * Starts writing the heartbeat packet to the active port on a fixed
 * interval, keeping the ESP32 connection alive. Replaces any interval
 * already running.
 */
export function startHeartbeat(): void {
  stopHeartbeat()
  serialState.heartbeatInterval = setInterval(() => {
    if (serialState.port && serialState.port.isOpen) {
      serialState.port.write(HEARTBEAT_PACKET, (err) => {
        if (err) console.error('Failed to write heartbeat:', err)
      })
    }
  }, HEARTBEAT_INTERVAL_MS)
}
