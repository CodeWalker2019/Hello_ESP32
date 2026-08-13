import { ElectronAPI } from '@electron-toolkit/preload'
import type { PortInfo, TelemetryReading } from '../main/serial/types'

declare global {
  interface Window {
    electron: typeof ElectronAPI
    api: {
      scanPorts: () => Promise<PortInfo[]>
      connectESP32: (portPath: string) => void
      disconnectESP32: () => void
      onESP32Data: (callback: (data: string) => void) => void
      onDisconnected: (callback: () => void) => () => void
      onESP32Telemetry: (callback: (reading: TelemetryReading) => void) => () => void
    }
  }
}

export {}
