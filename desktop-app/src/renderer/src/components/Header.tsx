import { Circle } from 'lucide-react'

function Header(): React.JSX.Element {
  return (
    <div className="flex items-center gap-2 border-b border-amber-dim bg-bg-header px-5 py-2.5">
      <Circle size={8} className="fill-amber text-amber" />
      <span className="text-[11px] tracking-[0.15em] text-fg-dim">
        IMU TELEMETRY &middot; GROUND STATION
      </span>
    </div>
  )
}

export default Header
