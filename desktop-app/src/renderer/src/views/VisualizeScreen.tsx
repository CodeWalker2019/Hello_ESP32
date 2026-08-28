import { ArrowLeft } from 'lucide-react'
import Cube3D from '@renderer/components/Cube3D'
import Readout from '@renderer/components/Readout'
import { useEffect, useRef, useState } from 'react'
import type { TelemetryReading } from '../../../main/serial/types'

const LSB_PER_G = 16384.0
const LERP_FACTOR = 0.08 // Frame-by-frame interpolation factor for silky smooth motion
const DEADBAND = 35 // Raw count noise threshold to suppress sensor jitter when stationary

function lerp(current: number, target: number, factor: number): number {
  const diff = target - current
  if (Math.abs(diff) < DEADBAND) {
    return current + diff * 0.02
  }
  return current + diff * factor
}

function VisualizeScreen({
  codename,
  onBack
}: {
  codename: string
  onBack: () => void
}): React.JSX.Element {
  const [state, setState] = useState<TelemetryReading>()
  const targetRef = useRef<TelemetryReading | null>(null)
  const currentRef = useRef<TelemetryReading | null>(null)

  useEffect(() => {
    const cleanup = window.api.onESP32Telemetry((result) => {
      targetRef.current = result
    })

    let animationFrameId: number

    const updateLoop = (): void => {
      if (targetRef.current) {
        if (!currentRef.current) {
          currentRef.current = { ...targetRef.current }
        } else {
          currentRef.current = {
            accelX: lerp(currentRef.current.accelX, targetRef.current.accelX, LERP_FACTOR),
            accelY: lerp(currentRef.current.accelY, targetRef.current.accelY, LERP_FACTOR),
            accelZ: lerp(currentRef.current.accelZ, targetRef.current.accelZ, LERP_FACTOR)
          }
        }
        setState({ ...currentRef.current })
      }
      animationFrameId = requestAnimationFrame(updateLoop)
    }

    animationFrameId = requestAnimationFrame(updateLoop)

    return () => {
      cleanup()
      cancelAnimationFrame(animationFrameId)
    }
  }, [])

  const accelX = state ? state.accelX / LSB_PER_G : undefined
  const accelY = state ? state.accelY / LSB_PER_G : undefined
  const accelZ = state ? state.accelZ / LSB_PER_G : undefined

  const roll =
    state && state.accelY !== undefined && state.accelZ !== undefined
      ? Math.atan2(state.accelY, state.accelZ) * (180 / Math.PI)
      : undefined

  const pitch =
    state && state.accelX !== undefined
      ? Math.atan2(
          -state.accelX,
          Math.sqrt(state.accelY * state.accelY + state.accelZ * state.accelZ)
        ) *
        (180 / Math.PI)
      : undefined

  const yaw = state ? 0 : undefined

  return (
    <div className="flex h-full flex-col">
      <div className="flex items-center justify-between border-b border-amber-dim px-6 py-4">
        <div className="flex items-center gap-4">
          <button
            onClick={onBack}
            className="flex items-center gap-1.5 rounded border border-amber-dim px-2.5 py-1.5 font-mono text-[11px] tracking-[0.08em] text-fg-dim transition-colors hover:border-amber hover:text-fg cursor-pointer"
          >
            <ArrowLeft size={13} /> DEVICES
          </button>
          <div className="flex items-center gap-2.5">
            <div className="h-2 w-2 rounded-full bg-good shadow-[0_0_8px_#6FCB9F]" />
            <span className="font-mono text-xs tracking-widest text-fg">
              LINKED &middot; {codename}
            </span>
          </div>
        </div>
        <span className="font-mono text-[11px] text-fg-dim">100 Hz &middot; USB</span>
      </div>

      <div className="flex flex-1">
        <div className="relative flex-1 bg-grid-lines">
          <Cube3D pitch={pitch} roll={roll} yaw={yaw} />
        </div>

        <div className="w-55 border-l border-amber-dim px-5 py-5">
          <div className="mb-3 font-display text-[13px] tracking-widest text-fg-dim uppercase">
            Orientation
          </div>
          <Readout label="ROLL" value={roll !== undefined ? roll.toFixed(1) : '--'} unit="°" />
          <Readout label="PITCH" value={pitch !== undefined ? pitch.toFixed(1) : '--'} unit="°" />
          <Readout label="YAW" value={yaw !== undefined ? yaw.toFixed(1) : '--'} unit="°" />
          <div className="mt-5 mb-3 font-display text-[13px] tracking-widest text-fg-dim uppercase">
            Raw Accel
          </div>
          <Readout label="X" value={accelX !== undefined ? accelX.toFixed(2) : '--'} unit="g" />
          <Readout label="Y" value={accelY !== undefined ? accelY.toFixed(2) : '--'} unit="g" />
          <Readout label="Z" value={accelZ !== undefined ? accelZ.toFixed(2) : '--'} unit="g" />
        </div>
      </div>
    </div>
  )
}

export default VisualizeScreen
