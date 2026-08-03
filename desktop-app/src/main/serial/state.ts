import { SerialPort } from 'serialport'
import { ReadlineParser } from '@serialport/parser-readline'

/**
 * Mutable state for the single active ESP32 connection, shared across the
 * serial connection modules. Mutate the fields in place — do not reassign
 * this object, since other modules hold a reference to it.
 */
export const serialState: {
  port: SerialPort | null
  parser: ReadlineParser | null
  heartbeatInterval: NodeJS.Timeout | null
} = {
  port: null,
  parser: null,
  heartbeatInterval: null
}
