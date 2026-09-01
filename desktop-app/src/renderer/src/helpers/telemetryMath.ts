import type { TelemetryReading } from '../../../shared/types'

const LSB_PER_G = 16384.0
const DEADBAND = 35

export interface Orientation {
  roll: number
  pitch: number
  yaw: number
}

export function lerpTelemetry(current: number, target: number, factor: number): number {
  const diff = target - current
  if (Math.abs(diff) < DEADBAND) {
    return current + diff * 0.02
  }
  return current + diff * factor
}

export function countsToG(raw: number): number {
  return raw / LSB_PER_G
}

export function computeOrientation(reading: TelemetryReading): Orientation {
  const roll = Math.atan2(reading.accelY, reading.accelZ) * (180 / Math.PI)
  const pitch =
    Math.atan2(
      -reading.accelX,
      Math.sqrt(reading.accelY * reading.accelY + reading.accelZ * reading.accelZ)
    ) *
    (180 / Math.PI)
  const yaw = 0

  return { roll, pitch, yaw }
}
