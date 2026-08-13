import { contextBridge, ipcRenderer } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'

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
  }
}

if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', customAPI)
  } catch (error) {
    console.error(error)
  }
}
