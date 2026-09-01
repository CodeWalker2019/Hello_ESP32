import { ArrowRight } from 'lucide-react'
import type { ScannedDevice } from '@renderer/types'
import { formatSigned } from '@renderer/helpers/format'

interface DeviceCardProps extends ScannedDevice {
  index: number
  onConnect: () => void
  connecting: boolean
}

function DeviceCard({
  index,
  codename,
  id,
  rssi,
  type,
  onConnect,
  connecting
}: DeviceCardProps): React.JSX.Element {
  const signalText = rssi > 0 ? `${formatSigned(rssi, 0)} dBm` : 'N/A'
  const transportText = type === 'wifi' ? 'Wi-Fi' : 'USB'

  return (
    <button
      onClick={onConnect}
      className="group flex w-full items-center gap-5 border-b border-rule px-1 py-5.5 text-left transition-colors duration-120 hover:bg-bg-subtle"
    >
      <span className="w-5.5 shrink-0 font-mono text-[11px] text-fg-7">
        {String(index).padStart(2, '0')}
      </span>
      <span className="flex-1 text-[19px] font-semibold tracking-[-0.015em] text-fg">
        {codename}
      </span>
      <span className="w-37.5 shrink-0 font-mono text-xs text-fg-4">{id}</span>
      <span className="w-20 shrink-0 text-right font-mono text-xs text-fg-4">{signalText}</span>
      <span className="w-14 shrink-0 text-right text-[12.5px] text-fg-4">{transportText}</span>
      {connecting ? (
        <span className="font-mono text-[11px] tracking-[0.06em] text-accent uppercase">
          Linking&hellip;
        </span>
      ) : (
        <ArrowRight
          size={16}
          className="shrink-0 text-fg-7 transition-colors duration-120 group-hover:text-fg"
        />
      )}
    </button>
  )
}

export default DeviceCard
