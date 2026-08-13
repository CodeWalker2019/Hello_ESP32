import { contextBridge, ipcRenderer } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'
import type { TelemetryReading } from '../main/serial/types'

const customAPI = {
  scanPorts: () => ipcRenderer.invoke('scan-esp32-ports'),
  connectESP32: (portPath: string) => ipcRenderer.send('connect-esp32', portPath),
  disconnectESP32: () => ipcRenderer.send('disconnect-esp32'),
  onDisconnected: (callback: () => void) => {
    const subscription = (): void => callback()
    ipcRenderer.on('disconnect-esp32', subscription)
    return () => {
      ipcRenderer.removeListener('disconnect-esp32', subscription)
    }
  },
  onESP32Data: (callback: (data: string) => void) => {
    ipcRenderer.on('esp32-data', (_, data) => callback(data))
  },
  onESP32Telemetry: (callback: (reading: TelemetryReading) => void) => {
    const listener = (_: unknown, reading: TelemetryReading): void => callback(reading)
    ipcRenderer.on('esp32-telemetry', listener)
    return () => {
      ipcRenderer.removeListener('esp32-telemetry', listener)
    }
  }
}

if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', customAPI)
  } catch (error) {
    console.error(error)
  }
} else {
  // @ts-ignore (fallback if context isolation is disabled)
  window.electron = electronAPI
  // @ts-ignore (fallback if context isolation is disabled)
  window.api = customAPI
}
