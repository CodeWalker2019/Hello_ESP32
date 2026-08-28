import { TELEMETRY_MAGIC, TELEMETRY_PACKET_LENGTH } from './constants'
import type { TelemetryReading } from './types'

/**
 * Incrementally reassembles fixed-length, magic-prefixed telemetry packets
 * out of an arbitrarily-chunked serial byte stream. Bytes preceding a
 * recognized magic sequence are discarded, so a stray identity beacon or a
 * packet torn by a connection race can't desync the framing permanently.
 */
export class TelemetryParser {
  private buffer: Buffer = Buffer.alloc(0)

  /** Feeds newly-received bytes in, returning any complete readings found. */
  push(chunk: Buffer): TelemetryReading[] {
    this.buffer = this.buffer.length > 0 ? Buffer.concat([this.buffer, chunk]) : chunk
    const readings: TelemetryReading[] = []

    while (true) {
      const magicIndex = this.buffer.indexOf(TELEMETRY_MAGIC)

      if (magicIndex === -1) {
        // Keep only a short tail in case a magic sequence is split across chunks.
        const keep = Math.min(this.buffer.length, TELEMETRY_MAGIC.length - 1)
        this.buffer = this.buffer.subarray(this.buffer.length - keep)
        break
      }

      if (magicIndex + TELEMETRY_PACKET_LENGTH > this.buffer.length) {
        // Found the header but the rest of the packet hasn't arrived yet.
        this.buffer = this.buffer.subarray(magicIndex)
        break
      }

      const packet = this.buffer.subarray(magicIndex, magicIndex + TELEMETRY_PACKET_LENGTH)
      readings.push({
        accelX: packet.readInt16BE(3),
        accelY: packet.readInt16BE(5),
        accelZ: packet.readInt16BE(7)
      })

      this.buffer = this.buffer.subarray(magicIndex + TELEMETRY_PACKET_LENGTH)
    }

    return readings
  }
}
