import { TelemetryReading } from 'src/main/serial/types'

const TO_DEGREES = 180 / Math.PI

export default function calculateOrientation(state?: TelemetryReading): {
  roll: number | undefined
  pitch: number | undefined
  yaw: number | undefined
} {
  if (
    !state ||
    state.accelX === undefined ||
    state.accelY === undefined ||
    state.accelZ === undefined
  ) {
    return { roll: undefined, pitch: undefined, yaw: undefined }
  }

  const { accelX, accelY, accelZ } = state

  const roll = Math.atan2(accelY, accelZ) * TO_DEGREES
  const pitch = Math.atan2(-accelX, Math.sqrt(accelY * accelY + accelZ * accelZ)) * TO_DEGREES
  const yaw = 0

  return { roll, pitch, yaw }
}
