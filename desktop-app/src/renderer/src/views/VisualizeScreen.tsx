import { ArrowLeft } from 'lucide-react'
import Cube3D from '@renderer/components/Cube3D'
import Readout from '@renderer/components/Readout'

function VisualizeScreen({
  codename,
  onBack
}: {
  codename: string
  onBack: () => void
}): React.JSX.Element {
  return (
    <div className="flex h-full flex-col">
      <div className="flex items-center justify-between border-b border-amber-dim px-6 py-4">
        <div className="flex items-center gap-4">
          <button
            onClick={onBack}
            className="flex items-center gap-1.5 rounded border border-amber-dim px-2.5 py-1.5 font-mono text-[11px] tracking-[0.08em] text-fg-dim transition-colors hover:border-amber hover:text-fg"
          >
            <ArrowLeft size={13} /> DEVICES
          </button>
          <div className="flex items-center gap-2.5">
            <div className="h-2 w-2 rounded-full bg-good shadow-[0_0_8px_#6FCB9F]" />
            <span className="font-mono text-xs tracking-[0.1em] text-fg">
              LINKED &middot; {codename}
            </span>
          </div>
        </div>
        <span className="font-mono text-[11px] text-fg-dim">100 Hz &middot; USB</span>
      </div>

      <div className="flex flex-1">
        <div className="relative flex-1 bg-grid-lines">
          <Cube3D />
        </div>

        <div className="w-[220px] border-l border-amber-dim px-5 py-5">
          <div className="mb-3 font-display text-[13px] tracking-[0.1em] text-fg-dim uppercase">
            Orientation
          </div>
          <Readout label="ROLL" value="-4.2" unit="°" />
          <Readout label="PITCH" value="12.7" unit="°" />
          <Readout label="YAW" value="188.3" unit="°" />
          <div className="mt-5 mb-3 font-display text-[13px] tracking-[0.1em] text-fg-dim uppercase">
            Raw Accel
          </div>
          <Readout label="X" value="0.02" unit="g" />
          <Readout label="Y" value="-0.98" unit="g" />
          <Readout label="Z" value="0.11" unit="g" />
        </div>
      </div>
    </div>
  )
}

export default VisualizeScreen
