import { ChevronRight, Cpu } from 'lucide-react'
import type { ScannedDevice } from '@renderer/types'

interface DeviceCardProps extends ScannedDevice {
  onConnect: () => void
  connecting: boolean
}

function DeviceCard({
  codename,
  id,
  rssi,
  onConnect,
  connecting
}: DeviceCardProps): React.JSX.Element {
  return (
    <button
      onClick={onConnect}
      className="flex w-full items-center gap-4 rounded border border-amber-dim bg-panel px-[18px] py-3.5 text-left text-fg transition-colors hover:border-amber"
    >
      <Cpu size={22} strokeWidth={1.5} className="text-amber" />
      <div className="flex-1">
        <div className="font-display text-base tracking-[0.04em] uppercase">{codename}</div>
        <div className="mt-0.5 font-mono text-[11px] text-fg-dim">
          {id} &middot; SIGNAL {rssi} dBm
        </div>
      </div>
      {connecting ? (
        <span className="font-mono text-[11px] tracking-[0.08em] text-amber">LINKING&hellip;</span>
      ) : (
        <ChevronRight size={18} className="text-fg-dim" />
      )}
    </button>
  )
}

export default DeviceCard
