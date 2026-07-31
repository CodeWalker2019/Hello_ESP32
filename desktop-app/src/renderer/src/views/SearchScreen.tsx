import { Radio, Usb, Wifi } from 'lucide-react'
import DeviceCard from '@renderer/components/DeviceCard'
import ScanSweep from '@renderer/components/ScanSweep'
import mockData from '@renderer/data/mockData.json'
import { useDeviceConnection } from '@renderer/helpers/useDeviceConnection'
import type { ScannedDevice } from '@renderer/types'

const devices = mockData.devices as ScannedDevice[]

function SearchScreen({ onConnected }: { onConnected: () => void }): React.JSX.Element {
  const { connectingId, connect } = useDeviceConnection(onConnected)

  return (
    <div className="flex h-full flex-col items-center px-8 py-12">
      <div className="mb-1.5 flex items-center gap-2.5 self-start">
        <Radio size={16} className="text-amber" />
        <span className="font-mono text-[11px] tracking-[0.15em] text-fg-dim uppercase">
          Telemetry Link &middot; Device Acquisition
        </span>
      </div>
      <h1 className="mt-1 mb-8 self-start font-display text-3xl font-medium tracking-[0.02em] uppercase">
        Searching for beacon
      </h1>

      <ScanSweep />

      <div className="my-7 font-mono text-xs tracking-[0.05em] text-fg-dim">
        BROADCASTING ON <span className="text-amber">DroneTelemetry-AP</span> &middot; CHANNEL 6
      </div>

      <div className="flex w-full max-w-[420px] flex-col gap-2.5">
        {devices.map((device) => (
          <DeviceCard
            key={device.id}
            {...device}
            connecting={connectingId === device.id}
            onConnect={() => connect(device.id)}
          />
        ))}
      </div>

      <div className="mt-auto flex gap-6 pt-8 font-mono text-[11px] text-fg-dim">
        <span className="flex items-center gap-1.5">
          <Wifi size={13} /> WIRELESS
        </span>
        <span className="flex items-center gap-1.5 opacity-50">
          <Usb size={13} /> USB &middot; NOT CONNECTED
        </span>
      </div>
    </div>
  )
}

export default SearchScreen
