import { ipcMain } from 'electron'
import { connectWifi } from './connect'

export function registerWifiConnectionHandlers(): void {
  ipcMain.on('connect-esp32-wifi', (event, address: string) => {
    connectWifi(event, address).catch((error) => {
      console.error(`Error connecting to ${address} over Wi-Fi:`, error)
    })
  })
}
