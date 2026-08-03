import { serialState } from './state'

/** Clears the heartbeat interval, if one is currently running. Safe to call when idle. */
export function stopHeartbeat(): void {
  if (serialState.heartbeatInterval) {
    clearInterval(serialState.heartbeatInterval)
    serialState.heartbeatInterval = null
  }
}
