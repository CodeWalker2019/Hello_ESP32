import { contextBridge, ipcRenderer } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'

// Define your custom API structure
const customAPI = {
  scanPorts: () => ipcRenderer.invoke('scan-esp32-ports'),
  connectESP32: (portPath: string) => ipcRenderer.send('connect-esp32', portPath),
  disconnectESP32: () => ipcRenderer.send('disconnect-esp32'),
  onESP32Data: (callback: (data: string) => void) => {
    ipcRenderer.on('esp32-data', (_, data) => callback(data))
  }
}

if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', customAPI) // <--- This exposes window.api
  } catch (error) {
    console.error(error)
  }
} else {
  // @ts-ignore (fallback if context isolation is disabled)
  window.electron = electronAPI
  // @ts-ignore
  window.api = customAPI
}