import { HEARTBEAT_INTERVAL_MS, HEARTBEAT_PACKET } from './constants'
import { serialState } from './state'
import { disconnect } from './disconnect'
import { stopHeartbeat } from './stopHeartbeat'

/**
 * Starts writing the heartbeat packet to the active port on a fixed
 * interval, keeping the ESP32 connection alive. Replaces any interval
 * already running.
 */
export function startHeartbeat(): void {
  stopHeartbeat()
  serialState.heartbeatInterval = setInterval(() => {
    if (!serialState.port) {
      console.error("SerialPort doesn't exist.. Disconnecting")
    }

    if (serialState.port && !serialState.port.isOpen) {
      console.error('SerialPort is closed.. Disconnecting.', serialState.port.path)
    }

    if (!serialState.port?.isOpen) {
      disconnect()
      return
    }

    serialState.port.write(HEARTBEAT_PACKET, (err) => {
      if (err) console.error('Failed to write heartbeat:', err)
      else console.log('Heartbit data is sent', HEARTBEAT_PACKET)
    })
  }, HEARTBEAT_INTERVAL_MS)
}
