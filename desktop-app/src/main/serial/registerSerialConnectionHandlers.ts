import { ipcMain } from 'electron'
import { scanPorts } from './scanPorts'
import { connect } from './connect'
import { disconnect } from './disconnect'

/** Wires the `scan-esp32-ports`, `connect-esp32`, and `disconnect-esp32` IPC channels to their handlers. */
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
    try {
      connect(event, portPath)
    } catch (error) {
      console.error(`Error connecting to ${portPath}:`, error)
    }
  })

  ipcMain.on('disconnect-esp32', () => {
    try {
      disconnect()
    } catch (error) {
      console.error('Error disconnecting:', error)
    }
  })
}
