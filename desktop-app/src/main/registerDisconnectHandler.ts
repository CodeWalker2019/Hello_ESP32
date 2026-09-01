import { ipcMain } from 'electron'
import { disconnect } from './serial'
import { disconnectWifi } from './wifiConnection'

export function registerDisconnectHandler(): void {
  ipcMain.on('disconnect-esp32', () => {
    Promise.all([disconnect(), disconnectWifi()]).catch((error) => {
      console.error('Error disconnecting:', error)
    })
  })
}
