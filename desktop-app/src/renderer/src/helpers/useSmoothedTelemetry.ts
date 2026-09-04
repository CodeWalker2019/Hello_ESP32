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
            roll: lerpTelemetry(currentRef.current.roll, targetRef.current.roll, LERP_FACTOR),
            pitch: lerpTelemetry(currentRef.current.pitch, targetRef.current.pitch, LERP_FACTOR)
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
