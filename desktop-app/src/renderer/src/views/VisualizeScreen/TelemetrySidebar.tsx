import Readout from '@renderer/components/Readout'

function TelemetrySidebar({
  roll,
  pitch,
  yaw,
  accelX,
  accelY,
  accelZ
}: {
  roll?: number
  pitch?: number
  yaw?: number
  accelX?: number
  accelY?: number
  accelZ?: number
}): React.ReactElement {
  const formatVal = (val: number | undefined, decimals = 1): string =>
    val !== undefined ? val.toFixed(decimals) : '--'

  return (
    <div className="w-55 border-l border-amber-dim px-5 py-5">
      <div className="mb-3 font-display text-[13px] tracking-widest text-fg-dim uppercase">
        Orientation
      </div>
      <Readout label="ROLL" value={formatVal(roll)} unit="°" />
      <Readout label="PITCH" value={formatVal(pitch)} unit="°" />
      <Readout label="YAW" value={formatVal(yaw)} unit="°" />

      <div className="mt-5 mb-3 font-display text-[13px] tracking-widest text-fg-dim uppercase">
        Raw Accel
      </div>
      <Readout label="X" value={formatVal(accelX, 2)} unit="g" />
      <Readout label="Y" value={formatVal(accelY, 2)} unit="g" />
      <Readout label="Z" value={formatVal(accelZ, 2)} unit="g" />
    </div>
  )
}

export default TelemetrySidebar
