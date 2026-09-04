import { TELEMETRY_MAGIC, TELEMETRY_PACKET_LENGTH, TELEMETRY_DEGREE_SCALE } from './constants'
import type { TelemetryReading } from '../../shared/types'

export class TelemetryParser {
  private buffer: Buffer = Buffer.alloc(0)

  push(chunk: Buffer): TelemetryReading[] {
    this.buffer = this.buffer.length > 0 ? Buffer.concat([this.buffer, chunk]) : chunk
    const readings: TelemetryReading[] = []

    while (true) {
      const magicIndex = this.buffer.indexOf(TELEMETRY_MAGIC)

      if (magicIndex === -1) {
        const keep = Math.min(this.buffer.length, TELEMETRY_MAGIC.length - 1)
        this.buffer = this.buffer.subarray(this.buffer.length - keep)
        break
      }

      if (magicIndex + TELEMETRY_PACKET_LENGTH > this.buffer.length) {
        this.buffer = this.buffer.subarray(magicIndex)
        break
      }

      const packet = this.buffer.subarray(magicIndex, magicIndex + TELEMETRY_PACKET_LENGTH)
      readings.push({
        roll: packet.readInt16BE(3) / TELEMETRY_DEGREE_SCALE,
        pitch: packet.readInt16BE(5) / TELEMETRY_DEGREE_SCALE
      })

      this.buffer = this.buffer.subarray(magicIndex + TELEMETRY_PACKET_LENGTH)
    }

    return readings
  }
}
