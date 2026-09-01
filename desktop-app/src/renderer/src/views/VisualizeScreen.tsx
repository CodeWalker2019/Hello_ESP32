import { ArrowLeft, Usb, Wifi } from 'lucide-react'
import Cube3D from '@renderer/components/Cube3D'
import Readout from '@renderer/components/Readout'
import { useSmoothedTelemetry } from '@renderer/helpers/useSmoothedTelemetry'
import { computeOrientation, countsToG } from '@renderer/helpers/telemetryMath'
import type { DeviceType } from '@renderer/types'

interface VisualizeScreenProps {
  codename: string
  connectionType?: DeviceType
  onBack: () => void
}

function VisualizeScreen({
  codename,
  connectionType = 'usb',
  onBack
}: VisualizeScreenProps): React.JSX.Element {
  const state = useSmoothedTelemetry()

  const accelX = state ? countsToG(state.accelX) : undefined
  const accelY = state ? countsToG(state.accelY) : undefined
  const accelZ = state ? countsToG(state.accelZ) : undefined

  const { roll, pitch, yaw } = state
    ? computeOrientation(state)
    : { roll: undefined, pitch: undefined, yaw: undefined }

  const isWifi = connectionType === 'wifi'

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
        <span className="flex items-center gap-1.5 font-mono text-[11px] text-fg-dim">
          {isWifi ? (
            <Wifi size={13} className="text-amber" />
          ) : (
            <Usb size={13} className="text-amber" />
          )}
          {isWifi ? 'WI-FI' : 'USB SERIAL'}
        </span>
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
