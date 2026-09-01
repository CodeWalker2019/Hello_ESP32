interface ReadoutProps {
  label: string
  value: string
  unit: string
  variant: 'orientation' | 'accel'
}

function Readout({ label, value, unit, variant }: ReadoutProps): React.JSX.Element {
  if (variant === 'orientation') {
    return (
      <div className="border-t border-fg pt-3">
        <div className="font-mono text-[10.5px] font-medium tracking-[0.14em] text-fg-4 uppercase">
          {label}
        </div>
        <div className="mt-2 text-[40px] font-semibold tracking-[-0.03em] text-fg tabular-nums">
          {value}
          <span className="text-[19px] font-normal text-fg-6">{unit}</span>
        </div>
      </div>
    )
  }

  return (
    <div className="flex items-baseline justify-between border-t border-rule-2 pt-3">
      <span className="font-mono text-[10.5px] tracking-[0.14em] text-fg-5 uppercase">{label}</span>
      <span className="font-mono text-[15px] text-fg tabular-nums">
        {value}
        <span className="text-fg-6">{unit}</span>
      </span>
    </div>
  )
}

export default Readout
