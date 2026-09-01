import { useEffect, useState } from 'react'
import { Loader2 } from 'lucide-react'

interface FoundDevice {
  address: string
  mac: string
}

interface WifiProvisionPanelProps {
  onProvisioningChange?: (isProvisioning: boolean) => void
}

function WifiProvisionPanel({ onProvisioningChange }: WifiProvisionPanelProps): React.JSX.Element {
  const [ssid, setSsid] = useState('')
  const [password, setPassword] = useState('')
  const [isProvisioning, setIsProvisioning] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [foundDevices, setFoundDevices] = useState<FoundDevice[]>([])

  useEffect((): (() => void) => {
    return window.api.onEsptouchDeviceFound((device: FoundDevice): void => {
      setFoundDevices((prev): FoundDevice[] =>
        prev.some((d) => d.mac === device.mac) ? prev : [...prev, device]
      )
    })
  }, [])

  useEffect((): (() => void) => {
    return (): void => {
      if (isProvisioning) {
        void window.api.esptouchStop()
        onProvisioningChange?.(false)
      }
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [isProvisioning])

  const setProvisioning = (value: boolean): void => {
    setIsProvisioning(value)
    onProvisioningChange?.(value)
  }

  const handleSubmit = async (e: React.FormEvent): Promise<void> => {
    e.preventDefault()
    setError(null)
    setFoundDevices([])
    setProvisioning(true)

    try {
      const result = await window.api.esptouchStart({ ssid, password })
      if (!result.success) {
        setError(result.error ?? 'Failed to start provisioning')
        setProvisioning(false)
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to start provisioning')
      setProvisioning(false)
    }
  }

  const handleStop = async (): Promise<void> => {
    await window.api.esptouchStop()
    setProvisioning(false)
  }

  return (
    <div className="flex items-start gap-12">
      <div className="w-45 shrink-0">
        <div className="font-mono text-[10.5px] font-medium tracking-[0.14em] text-fg-5 uppercase">
          Provision
        </div>
        <p className="mt-2.5 text-[12.5px] leading-normal text-fg-5">
          Broadcasts credentials to a new board over the air.
        </p>
      </div>

      <div className="flex-1">
        <form onSubmit={(e) => void handleSubmit(e)} className="flex items-end gap-4">
          <div className="flex-1">
            <div className="mb-1.5 text-[11.5px] text-fg-4">Network</div>
            <input
              type="text"
              value={ssid}
              onChange={(e) => setSsid(e.target.value)}
              disabled={isProvisioning}
              maxLength={32}
              className="w-full border-0 border-b border-input-rule bg-transparent pb-2.25 text-sm text-fg placeholder:text-fg-5 focus:border-fg focus:outline-none disabled:opacity-50"
            />
          </div>
          <div className="flex-1">
            <div className="mb-1.5 text-[11.5px] text-fg-4">Password</div>
            <input
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              disabled={isProvisioning}
              maxLength={64}
              className="w-full border-0 border-b border-input-rule bg-transparent pb-2.25 text-sm tracking-[0.1em] text-fg placeholder:text-fg-5 focus:border-fg focus:outline-none disabled:opacity-50"
            />
          </div>

          {isProvisioning ? (
            <button
              type="button"
              onClick={() => void handleStop()}
              className="flex shrink-0 cursor-pointer items-center gap-2 rounded-lg border border-input-rule bg-bg px-4.5 py-2.75 text-[13px] font-semibold text-fg"
            >
              <Loader2 size={13} className="animate-spin" /> Broadcasting&hellip; stop
            </button>
          ) : (
            <button
              type="submit"
              disabled={!ssid}
              className="shrink-0 cursor-pointer rounded-lg bg-fg px-4.5 py-2.75 text-[13px] font-semibold text-bg disabled:cursor-not-allowed disabled:opacity-40"
            >
              Broadcast
            </button>
          )}
        </form>

        {error && <div className="mt-3 font-mono text-[11.5px] text-bad">{error}</div>}

        {foundDevices.length > 0 && (
          <div className="mt-3 flex flex-col gap-1">
            {foundDevices.map((device) => (
              <div key={device.mac} className="font-mono text-[11.5px] text-good">
                Device {device.mac} responded — joining &ldquo;{ssid}&rdquo;&hellip;
              </div>
            ))}
          </div>
        )}
      </div>
    </div>
  )
}

export default WifiProvisionPanel
