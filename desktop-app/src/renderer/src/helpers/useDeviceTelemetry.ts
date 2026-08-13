import { useEffect, useRef, useState } from 'react'
import { TelemetryReading } from 'src/main/serial/types'

export default function useDeviceTelemetry(): TelemetryReading | undefined {
  const [state, setState] = useState<TelemetryReading>()
  const targetRef = useRef<TelemetryReading | null>(null)

  useEffect(() => {
    const cleanup = window.api.onESP32Telemetry((result) => {
      targetRef.current = result
    })

    let animationFrameId: number

    const updateLoop = (): void => {
      if (targetRef.current) setState({ ...targetRef.current })
      animationFrameId = requestAnimationFrame(updateLoop)
    }

    animationFrameId = requestAnimationFrame(updateLoop)

    return () => {
      cleanup()
      cancelAnimationFrame(animationFrameId)
    }
  }, [])

  return state
}
