import { Radio, Usb, Wifi } from 'lucide-react'
import DeviceCard from '@renderer/components/DeviceCard'
import ScanSweep from '@renderer/components/ScanSweep'
import WifiProvisionPanel from '@renderer/components/WifiProvisionPanel'
import { useDeviceConnection } from '@renderer/helpers/useDeviceConnection'
import { useDeviceScan } from '@renderer/helpers/useDeviceScan'
import type { ScannedDevice, DeviceType } from '@renderer/types'

function SearchScreen({
  onConnected
}: {
  onConnected: (type: DeviceType) => void
}): React.JSX.Element {
  const { connectingId, connect } = useDeviceConnection(onConnected)
  const { devices, isScanning, setProvisioning } = useDeviceScan()

  return (
    <div className="flex h-full flex-col items-center px-8 py-12">
      <div className="mb-1.5 flex items-center gap-2.5 self-start">
        <Radio size={16} className="text-amber" />
        <span className="font-mono text-[11px] tracking-[0.15em] text-fg-dim uppercase">
          Telemetry Link &middot; Device Acquisition
        </span>
      </div>
      <h1 className="mt-1 mb-8 self-start font-display text-3xl font-medium tracking-[0.02em] uppercase">
        {isScanning
          ? 'Searching for beacon...'
          : devices.length > 0
            ? 'Devices Found'
            : 'No Devices Found'}
      </h1>

      <ScanSweep />

      <div className="my-7 font-mono text-xs tracking-wider text-fg-dim">
        {isScanning ? (
          <span className="text-amber">SCANNING SERIAL &amp; WI-FI INTERFACES...</span>
        ) : (
          <span>FOUND {devices.length} COMPATIBLE DEVICE(S)</span>
        )}
      </div>

      <div className="flex w-full max-w-105 flex-col gap-2.5">
        {devices.map((device: ScannedDevice): React.JSX.Element => (
          <DeviceCard
            key={device.id}
            codename={device.codename}
            id={device.id}
            rssi={device.rssi}
            type={device.type}
            connecting={connectingId === device.id}
            onConnect={(): void => {
              void connect(device.id, device.type)
            }}
          />
        ))}
      </div>

      <div className="mt-7 w-full max-w-105">
        <WifiProvisionPanel onProvisioningChange={setProvisioning} />
      </div>

      <div className="mt-auto flex gap-6 pt-8 font-mono text-[11px] text-fg-dim">
        <span className="flex items-center gap-1.5 opacity-50">
          <Wifi size={13} /> WIRELESS &middot; ACTIVE
        </span>
        <span className="flex items-center gap-1.5">
          <Usb size={13} /> USB &middot; ACTIVE
        </span>
      </div>
    </div>
  )
}

export default SearchScreen
