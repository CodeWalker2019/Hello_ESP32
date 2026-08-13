import { webContents } from 'electron'
import { PORT_RELEASE_DELAY_MS } from './constants'
import { serialState } from './state'
import { stopHeartbeat } from './stopHeartbeat'

/**
 * Stops the heartbeat and closes the active port, if any, then clears
 * connection state and notifies the renderer process of the disconnection.
 * Waits for the close to actually complete (plus a short buffer for the OS to
 * release the lock) before resolving.
 */
export function disconnect(): Promise<void> {
  stopHeartbeat()

  return new Promise((resolve) => {
    const activePort = serialState.port
    serialState.port = null
    serialState.parser = null

    const finish = (): void => {
      webContents.getAllWebContents().forEach((wc) => {
        wc.send('disconnect-esp32')
      })
      resolve()
    }

    if (activePort && activePort.isOpen) {
      activePort.close(() => {
        console.log('ESP32 disconnected and heartbeat stopped.')
        setTimeout(finish, PORT_RELEASE_DELAY_MS)
      })
    } else {
      finish()
    }
  })
}
