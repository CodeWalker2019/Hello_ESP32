/** Protocol constants for the ESP32 serial handshake, heartbeat, and framing. */

export const ESP32_VENDOR_ID = '10c4'
export const TARGET_DEVICE_FAMILY_ID = 0x4b

export const BAUD_RATE = 115200
export const HANDSHAKE_TIMEOUT_MS = 1500
export const HEARTBEAT_INTERVAL_MS = 1000

/** Buffer after closing a port before it's safe to reopen — the OS doesn't always release the lock the instant the close callback fires. */
export const PORT_RELEASE_DELAY_MS = 100

export const START_PACKET = Buffer.from([0xaa, 0x55, 0x01])
export const HEARTBEAT_PACKET = Buffer.from([0xaa, 0x55, 0x02])

/** Magic prefix for a telemetry packet: 0xAA 0x55 <device family id>. */
export const TELEMETRY_MAGIC = Buffer.from([0xaa, 0x55, TARGET_DEVICE_FAMILY_ID])

/** Magic (3) + accel_x/accel_y/accel_z as big-endian int16 (6). */
export const TELEMETRY_PACKET_LENGTH = 9
