import DeviceCard from '@renderer/components/DeviceCard'
import Header from '@renderer/components/Header'
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

  const heading = isScanning
    ? 'Searching for beacon'
    : devices.length > 0
      ? 'Devices found'
      : 'No devices found'

  return (
    <div className="flex h-full flex-col">
      <Header
        left={
          <span className="font-mono text-[10.5px] font-medium tracking-[0.16em] text-fg-4 uppercase">
            IMU Telemetry &nbsp;/&nbsp; Ground Station
          </span>
        }
        right={
          isScanning ? (
            <span className="flex items-center gap-2 font-mono text-[10.5px] tracking-[0.1em] text-fg-6 uppercase">
              <span className="h-1.25 w-1.25 animate-pulse-dot rounded-full bg-good" />
              Scanning
            </span>
          ) : undefined
        }
      />

      <div className="flex min-h-0 flex-1 flex-col px-20 pt-19 pb-10">
        <h1 className="text-[44px] leading-[1.05] font-semibold tracking-[-0.03em] text-fg">
          {heading}
        </h1>
        <p className="mt-3.5 max-w-105 text-[15px] leading-[1.55] text-fg-3">
          {devices.length} compatible device{devices.length === 1 ? '' : 's'} on serial and Wi-Fi.
          Pick one to stream live orientation.
        </p>

        <div className="mt-13 border-t border-rule">
          {devices.map((device: ScannedDevice, i: number): React.JSX.Element => (
            <DeviceCard
              key={device.id}
              index={i + 1}
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

        <div className="mt-14">
          <WifiProvisionPanel onProvisioningChange={setProvisioning} />
        </div>

        <div className="mt-auto flex gap-6.5 pt-8 font-mono text-[10.5px] tracking-[0.1em] text-fg-7 uppercase">
          <span>Wireless &middot; active</span>
          <span>USB &middot; active</span>
        </div>
      </div>
    </div>
  )
}

export default SearchScreen
