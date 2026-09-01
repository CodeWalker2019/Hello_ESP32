import { ipcMain } from 'electron'
import { getCurrentNetworkCredentials } from './getCurrentNetworkCredentials'

export interface CurrentNetworkResponse {
  success: boolean
  ssid?: string
  password?: string
  passwordError?: string
  error?: string
}

export function registerCurrentNetworkHandlers(): void {
  ipcMain.handle('get-current-network-credentials', async (): Promise<CurrentNetworkResponse> => {
    try {
      const { ssid, password, passwordError } = await getCurrentNetworkCredentials()
      return { success: true, ssid, password: password ?? undefined, passwordError }
    } catch (error) {
      const message = error instanceof Error ? error.message : String(error)
      console.error('[CurrentNetwork] Failed to read current Wi-Fi credentials:', message)
      return { success: false, error: message }
    }
  })
}
