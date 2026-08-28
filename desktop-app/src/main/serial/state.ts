import { SerialPort } from 'serialport'

/**
 * Mutable state for the single active ESP32 connection, shared across the
 * serial connection modules. Mutate the fields in place — do not reassign
 * this object, since other modules hold a reference to it.
 */
export const serialState: {
  port: SerialPort | null
  heartbeatInterval: NodeJS.Timeout | null
} = {
  port: null,
  heartbeatInterval: null
}
