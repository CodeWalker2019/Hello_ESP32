import { ArrowLeft } from 'lucide-react'

export default function VisualizationHeader({
  codename
}: {
  codename: string
}): React.ReactElement {
  return (
    <div className="flex items-center justify-between border-b border-amber-dim px-6 py-4">
      <div className="flex items-center gap-4">
        <button
          onClick={window.api.disconnectESP32}
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
  )
}
