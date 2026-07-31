interface ReadoutProps {
  label: string
  value: string
  unit: string
}

function Readout({ label, value, unit }: ReadoutProps): React.JSX.Element {
  return (
    <div className="flex justify-between border-b border-divider py-2.5">
      <span className="font-mono text-[11px] tracking-[0.08em] text-fg-dim">{label}</span>
      <span className="font-mono text-[13px] text-fg">
        {value}
        <span className="ml-0.5 text-fg-dim">{unit}</span>
      </span>
    </div>
  )
}

export default Readout
