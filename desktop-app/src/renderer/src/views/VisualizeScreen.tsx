import { ArrowLeft } from 'lucide-react'
import Cube3D from '@renderer/components/Cube3D'
import Header from '@renderer/components/Header'
import Readout from '@renderer/components/Readout'
import { useSmoothedTelemetry } from '@renderer/helpers/useSmoothedTelemetry'
import { formatSigned } from '@renderer/helpers/format'
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

  const roll = state?.roll
  const pitch = state?.pitch

  const isWifi = connectionType === 'wifi'

  return (
    <div className="flex h-full flex-col">
      <Header
        left={
          <div className="flex items-center gap-4">
            <button
              onClick={onBack}
              className="flex cursor-pointer items-center gap-1.5 text-[12.5px] text-fg-3 transition-colors duration-120 hover:text-fg"
            >
              <ArrowLeft size={14} /> Devices
            </button>
            <span className="font-mono text-[10.5px] font-medium tracking-[0.16em] text-fg-4 uppercase">
              {codename}
            </span>
          </div>
        }
        right={
          <span className="flex items-center gap-2 font-mono text-[10.5px] tracking-[0.1em] text-fg-6 uppercase">
            <span className="h-1.25 w-1.25 rounded-full bg-good" />
            Linked &middot; {isWifi ? 'Wi-Fi' : 'USB serial'}
          </span>
        }
      />

      <div className="flex min-h-0 flex-1 flex-col px-20 pt-10 pb-12">
        <div className="relative min-h-0 flex-1">
          <Cube3D pitch={pitch} roll={roll} />
        </div>

        <div className="mt-9 grid grid-cols-2 gap-x-10 gap-y-6.5">
          <Readout
            variant="orientation"
            label="Roll"
            value={roll !== undefined ? formatSigned(roll, 1) : '--'}
            unit="°"
          />
          <Readout
            variant="orientation"
            label="Pitch"
            value={pitch !== undefined ? formatSigned(pitch, 1) : '--'}
            unit="°"
          />
        </div>
      </div>
    </div>
  )
}

export default VisualizeScreen
