/** Protocol constants for the ESP32 serial handshake, heartbeat, and framing. */

export const ESP32_VENDOR_ID = '10c4'
export const TARGET_DEVICE_FAMILY_ID = 0x4b

export const BAUD_RATE = 115200
export const HANDSHAKE_TIMEOUT_MS = 1500
export const HEARTBEAT_INTERVAL_MS = 1000

export const START_PACKET = Buffer.from([0xaa, 0x55, 0x01])
export const HEARTBEAT_PACKET = Buffer.from([0xaa, 0x55, 0x02])
