import { useEffect, useRef, useState } from 'react'
import type { TelemetryReading } from '../../../shared/types'
import { lerpTelemetry } from './telemetryMath'

const LERP_FACTOR = 0.08

export function useSmoothedTelemetry(): TelemetryReading | undefined {
  const [state, setState] = useState<TelemetryReading>()
  const targetRef = useRef<TelemetryReading | null>(null)
  const currentRef = useRef<TelemetryReading | null>(null)

  useEffect((): (() => void) => {
    const cleanup = window.api.onESP32Telemetry((result: TelemetryReading): void => {
      targetRef.current = result
    })

    let animationFrameId: number

    const updateLoop = (): void => {
      if (targetRef.current) {
        if (!currentRef.current) {
          currentRef.current = { ...targetRef.current }
        } else {
          currentRef.current = {
            accelX: lerpTelemetry(currentRef.current.accelX, targetRef.current.accelX, LERP_FACTOR),
            accelY: lerpTelemetry(currentRef.current.accelY, targetRef.current.accelY, LERP_FACTOR),
            accelZ: lerpTelemetry(currentRef.current.accelZ, targetRef.current.accelZ, LERP_FACTOR)
          }
        }
        setState({ ...currentRef.current })
      }
      animationFrameId = requestAnimationFrame(updateLoop)
    }

    animationFrameId = requestAnimationFrame(updateLoop)

    return (): void => {
      cleanup()
      cancelAnimationFrame(animationFrameId)
    }
  }, [])

  return state
}
