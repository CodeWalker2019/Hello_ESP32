import { ElectronAPI } from '@electron-toolkit/preload'
import type { TelemetryReading } from '../main/serial/types'

declare global {
  interface Window {
    electron: typeof ElectronAPI
    api: {
      scanPorts: () => Promise<unknown[]>
      connectESP32: (portPath: string) => void
      disconnectESP32: () => void
      onESP32Telemetry: (callback: (reading: TelemetryReading) => void) => () => void
    }
  }
}

export {}
