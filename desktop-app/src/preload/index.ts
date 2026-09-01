import { contextBridge, ipcRenderer, IpcRendererEvent } from 'electron'
import type { ProvisionParams, HandlerResponse } from '../main/esptouch'
import type { ProvisionedDevice } from '../main/esptouch/provisioner'
import type { TelemetryReading } from '../shared/types'
import type { CurrentNetworkResponse } from '../main/currentNetwork'

const api = {
  scanPorts: () => ipcRenderer.invoke('scan-esp32-ports'),

  connectESP32: (portPath: string) => ipcRenderer.send('connect-esp32', portPath),
  disconnectESP32: () => ipcRenderer.send('disconnect-esp32'),

  connectESP32Wifi: (address: string) => ipcRenderer.send('connect-esp32-wifi', address),

  scanWifiDevices: () => ipcRenderer.invoke('scan-wifi-devices'),

  onESP32Telemetry: (callback: (reading: TelemetryReading) => void): (() => void) => {
    const listener = (_event: IpcRendererEvent, reading: TelemetryReading): void => callback(reading)
    ipcRenderer.on('esp32-telemetry', listener)
    return () => ipcRenderer.removeListener('esp32-telemetry', listener)
  },

  esptouchStart: (params: ProvisionParams): Promise<HandlerResponse> =>
    ipcRenderer.invoke('esptouch:start', params),
  esptouchStop: (): Promise<HandlerResponse> => ipcRenderer.invoke('esptouch:stop'),
  onEsptouchDeviceFound: (callback: (device: ProvisionedDevice) => void): (() => void) => {
    const listener = (_event: IpcRendererEvent, device: ProvisionedDevice): void => callback(device)
    ipcRenderer.on('esptouch:on-device-found', listener)
    return () => ipcRenderer.removeListener('esptouch:on-device-found', listener)
  },

  getCurrentNetworkCredentials: (): Promise<CurrentNetworkResponse> =>
    ipcRenderer.invoke('get-current-network-credentials')
}

if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('api', api)
  } catch (error) {
    console.error('[Preload] Error exposing API in main world:', error)
  }
} else {
  // @ts-ignore (fallback when context isolation is disabled)
  window.api = api
}
