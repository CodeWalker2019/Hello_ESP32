interface HeaderProps {
  left: React.ReactNode
  right?: React.ReactNode
}

function Header({ left, right }: HeaderProps): React.JSX.Element {
  return (
    <div className="flex h-13 shrink-0 items-center justify-between border-b border-rule px-10">
      {left}
      {right}
    </div>
  )
}

export default Header
