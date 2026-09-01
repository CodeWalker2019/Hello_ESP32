import { useEffect, useState } from 'react'
import { Loader2, Wifi, KeyRound } from 'lucide-react'

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
  const [isLoadingCurrentNetwork, setIsLoadingCurrentNetwork] = useState(false)

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

  const handleUseCurrentNetwork = async (): Promise<void> => {
    setError(null)
    setIsLoadingCurrentNetwork(true)
    try {
      const result = await window.api.getCurrentNetworkCredentials()
      if (result.success && result.ssid !== undefined) {
        setSsid(result.ssid)
        if (result.password !== undefined) {
          setPassword(result.password)
        }
        if (result.passwordError) {
          setError(result.passwordError)
        }
      } else {
        setError(result.error ?? 'Failed to read current Wi-Fi network')
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to read current Wi-Fi network')
    } finally {
      setIsLoadingCurrentNetwork(false)
    }
  }

  return (
    <div className="w-full max-w-105 rounded border border-amber-dim bg-panel px-4.5 py-4">
      <div className="mb-3 flex items-center justify-between gap-2">
        <div className="flex items-center gap-2 font-display text-[13px] tracking-widest text-fg-dim uppercase">
          <Wifi size={14} className="text-amber" /> Provision New Device
        </div>
        <button
          type="button"
          onClick={() => void handleUseCurrentNetwork()}
          disabled={isProvisioning || isLoadingCurrentNetwork}
          className="flex cursor-pointer items-center gap-1.5 font-mono text-[10px] tracking-[0.06em] text-fg-dim transition-colors hover:text-amber disabled:cursor-not-allowed disabled:opacity-50"
        >
          {isLoadingCurrentNetwork ? (
            <Loader2 size={11} className="animate-spin" />
          ) : (
            <KeyRound size={11} />
          )}
          USE CURRENT NETWORK
        </button>
      </div>

      <form onSubmit={(e) => void handleSubmit(e)} className="flex flex-col gap-2.5">
        <input
          type="text"
          placeholder="Wi-Fi SSID"
          value={ssid}
          onChange={(e) => setSsid(e.target.value)}
          disabled={isProvisioning}
          maxLength={32}
          className="rounded border border-amber-dim bg-bg px-3 py-2 font-mono text-xs text-fg placeholder:text-fg-dim focus:border-amber focus:outline-none disabled:opacity-50"
        />
        <input
          type="password"
          placeholder="Wi-Fi Password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          disabled={isProvisioning}
          maxLength={64}
          className="rounded border border-amber-dim bg-bg px-3 py-2 font-mono text-xs text-fg placeholder:text-fg-dim focus:border-amber focus:outline-none disabled:opacity-50"
        />

        {isProvisioning ? (
          <button
            type="button"
            onClick={() => void handleStop()}
            className="flex cursor-pointer items-center justify-center gap-2 rounded border border-amber-dim py-2 font-mono text-[11px] tracking-[0.08em] text-fg-dim transition-colors hover:border-amber hover:text-fg"
          >
            <Loader2 size={13} className="animate-spin" /> BROADCASTING&hellip; STOP
          </button>
        ) : (
          <button
            type="submit"
            disabled={!ssid}
            className="cursor-pointer rounded border border-amber-dim py-2 font-mono text-[11px] tracking-[0.08em] text-fg-dim transition-colors hover:border-amber hover:text-fg disabled:cursor-not-allowed disabled:opacity-50"
          >
            START PROVISIONING
          </button>
        )}
      </form>

      {error && <div className="mt-3 font-mono text-[11px] text-bad">{error}</div>}

      {foundDevices.length > 0 && (
        <div className="mt-3 flex flex-col gap-1">
          {foundDevices.map((device) => (
            <div key={device.mac} className="font-mono text-[11px] text-good">
              Device {device.mac} responded — joining &ldquo;{ssid}&rdquo;&hellip;
            </div>
          ))}
          <div className="mt-1 font-mono text-[11px] text-fg-dim">
            It will appear in the device list above once it&apos;s connected.
          </div>
        </div>
      )}
    </div>
  )
}

export default WifiProvisionPanel
