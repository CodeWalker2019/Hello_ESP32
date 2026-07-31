import { SCAN_RING_SCALES, SCAN_RING_SIZE } from '@renderer/helpers/constants'

function ScanSweep(): React.JSX.Element {
  return (
    <div className="relative" style={{ width: SCAN_RING_SIZE, height: SCAN_RING_SIZE }}>
      {SCAN_RING_SCALES.map((scale, i) => (
        <div
          key={i}
          className="absolute inset-0 m-auto rounded-full border border-amber-dim"
          style={{ width: SCAN_RING_SIZE * scale, height: SCAN_RING_SIZE * scale }}
        />
      ))}
      <div className="absolute inset-0 animate-spin-slow rounded-full bg-[conic-gradient(from_0deg,transparent_0deg,#FFB00055_25deg,transparent_60deg)]" />
      <div className="absolute inset-0 m-auto h-2 w-2 rounded-full bg-amber shadow-[0_0_12px_#FFB000]" />
    </div>
  )
}

export default ScanSweep
