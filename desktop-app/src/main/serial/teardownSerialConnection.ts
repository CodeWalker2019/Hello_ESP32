import { disconnect } from './disconnect'

/** Closes any active ESP32 connection. Call on app shutdown. */
export function teardownSerialConnection(): void {
  disconnect()
}
