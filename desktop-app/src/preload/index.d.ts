import { ElectronAPI } from '@electron-toolkit/preload'

declare global {
  interface Window {
    electron: typeof ElectronAPI
    api: {
      scanPorts: () => Promise<any[]>
      connectESP32: (portPath: string) => void
      disconnectESP32: () => void
      onESP32Data: (callback: (data: string) => void) => void
    }
  }
}

export {}
