import type { ScannedDevice } from '@renderer/types'

export interface SerialPortInfo {
  path: string
  manufacturer?: string
  serialNumber?: string
  vendorId?: string
  productId?: string
}

export interface WifiDeviceInfo {
  address: string
  ssid?: string
  rssi?: number
}

export function mapSerialPortToDevice(port: SerialPortInfo): ScannedDevice {
  return {
    id: port.path,
    codename: `Kestrel-${port.path.split('/').pop()}`,
    rssi: 0,
    type: 'usb'
  }
}

export function mapWifiDeviceToDevice(wifi: WifiDeviceInfo): ScannedDevice {
  return {
    id: wifi.address,
    codename: wifi.ssid || `Kestrel-WiFi-${wifi.address}`,
    rssi: wifi.rssi ?? -50,
    type: 'wifi'
  }
}
