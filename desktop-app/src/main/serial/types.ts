import { SerialPort } from 'serialport'

/** A single entry from `SerialPort.list()`, as returned by the platform's serial enumeration. */
export type PortInfo = Awaited<ReturnType<typeof SerialPort.list>>[number]

/** A single decoded telemetry packet. Raw accelerometer counts, not yet scaled to g. */
export interface TelemetryReading {
  accelX: number
  accelY: number
  accelZ: number
}
