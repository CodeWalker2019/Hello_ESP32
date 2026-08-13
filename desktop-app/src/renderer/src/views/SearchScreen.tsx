import DeviceCard from '@renderer/components/DeviceCard'
import ScanSweep from '@renderer/components/ScanSweep'
import { SCAN_POLL_INTERVAL_MS } from '@renderer/helpers/constants'
import { IUseDeviceConnectionReturnType } from '@renderer/helpers/useDeviceConnection'
import type { ScannedDevice } from '@renderer/types'
import { Radio, Usb, Wifi } from 'lucide-react'
import { useEffect, useState } from 'react'

function SearchScreen({
  connectingId,
  connect
}: IUseDeviceConnectionReturnType): React.JSX.Element {
  const [devices, setDevices] = useState<ScannedDevice[]>([])
  const [isScanning, setIsScanning] = useState<boolean>(true)

  useEffect(() => {
    let cancelled = false
    let timeoutId: ReturnType<typeof setTimeout> | undefined

    async function performScan(isInitialScan: boolean): Promise<void> {
      if (isInitialScan) setIsScanning(true)
      try {
        const foundPorts = await window.api.scanPorts()
        if (cancelled) return
        console.log('Found ESP32 ports:', foundPorts)

        const mappedDevices: ScannedDevice[] = foundPorts.map((port) => ({
          id: port.path,
          codename: `Kestrel-${port.path.split('/').pop()}`,
          rssi: 0,
          type: 'usb'
        }))

        setDevices(mappedDevices)
      } catch (err) {
        console.error('Error scanning for ESP32 devices:', err)
      } finally {
        if (isInitialScan) setIsScanning(false)
        if (!cancelled) {
          timeoutId = setTimeout(() => performScan(false), SCAN_POLL_INTERVAL_MS)
        }
      }
    }

    performScan(true)

    return () => {
      cancelled = true
      if (timeoutId) clearTimeout(timeoutId)
    }
  }, [])

  function getScanCurrentStateLabel(): string {
    if (isScanning) return 'Searching for beacon...'
    if (devices.length > 0) return 'Devices Found'
    return 'No Devices Found'
  }

  return (
    <div className="flex h-full flex-col items-center px-8 py-12">
      <div className="mb-1.5 flex items-center gap-2.5 self-start">
        <Radio size={16} className="text-amber" />
        <span className="font-mono text-[11px] tracking-[0.15em] text-fg-dim uppercase">
          Telemetry Link &middot; Device Acquisition
        </span>
      </div>
      <h1 className="mt-1 mb-8 self-start font-display text-3xl font-medium tracking-[0.02em] uppercase">
        {getScanCurrentStateLabel()}
      </h1>

      <ScanSweep />

      <div className="my-7 font-mono text-xs tracking-wider text-fg-dim">
        {isScanning ? (
          <span className="text-amber">SCANNING SERIAL PORTS...</span>
        ) : (
          <span>FOUND {devices.length} COMPATIBLE DEVICE(S)</span>
        )}
      </div>

      <div className="flex w-full max-w-105 flex-col gap-2.5">
        {devices.map((device) => (
          <DeviceCard
            key={device.id}
            codename={device.codename}
            id={device.id}
            rssi={device.rssi}
            connecting={connectingId === device.id}
            onConnect={() => connect(device.id)}
          />
        ))}
      </div>

      <div className="mt-auto flex gap-6 pt-8 font-mono text-[11px] text-fg-dim">
        <span className="flex items-center gap-1.5 opacity-50">
          <Wifi size={13} /> WIRELESS &middot; NOT ACTIVE
        </span>
        <span className="flex items-center gap-1.5">
          <Usb size={13} /> USB &middot; ACTIVE
        </span>
      </div>
    </div>
  )
}

export default SearchScreen
