import { ipcMain } from 'electron'
import { scanPorts } from './scanPorts'
import { connect } from './connect'

export function registerSerialConnectionHandlers(): void {
  ipcMain.handle('scan-esp32-ports', async () => {
    try {
      return await scanPorts()
    } catch (error) {
      console.error('Error scanning ports:', error)
      return []
    }
  })

  ipcMain.on('connect-esp32', (event, portPath: string) => {
    connect(event, portPath).catch((error) => {
      console.error(`Error connecting to ${portPath}:`, error)
    })
  })
}
