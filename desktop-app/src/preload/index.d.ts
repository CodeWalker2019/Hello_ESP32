import type { ElectronAPI } from '@electron-toolkit/preload'
import type { TelemetryReading } from '../shared/types'
import type { ProvisionParams, HandlerResponse } from '../main/esptouch'
import type { ProvisionedDevice } from '../main/esptouch/provisioner'
import type { CurrentNetworkResponse } from '../main/currentNetwork'

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

declare global {
  interface Window {
    electron: ElectronAPI
    api: {
      scanPorts: () => Promise<SerialPortInfo[]>
      scanWifiDevices: () => Promise<WifiDeviceInfo[]>
      connectESP32: (portPath: string) => void
      disconnectESP32: () => void
      connectESP32Wifi: (address: string) => void
      onESP32Telemetry: (callback: (reading: TelemetryReading) => void) => () => void
      esptouchStart: (params: ProvisionParams) => Promise<HandlerResponse>
      esptouchStop: () => Promise<HandlerResponse>
      onEsptouchDeviceFound: (callback: (device: ProvisionedDevice) => void) => () => void
      getCurrentNetworkCredentials: () => Promise<CurrentNetworkResponse>
    }
  }
}

export {}
