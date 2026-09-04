// Below this diff (in degrees), treat it as sensor noise and creep toward the
// target slowly instead of tracking it at the normal lerp factor.
const DEADBAND_DEG = 1.0

export function lerpTelemetry(current: number, target: number, factor: number): number {
  const diff = target - current
  if (Math.abs(diff) < DEADBAND_DEG) {
    return current + diff * 0.02
  }
  return current + diff * factor
}
