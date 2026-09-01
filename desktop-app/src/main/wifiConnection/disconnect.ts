import { wifiConnectionState } from './state'
import { SOCKET_CLOSE_DELAY_MS } from './constants'

export function disconnectWifi(): Promise<void> {
  return new Promise((resolve) => {
    const finish = (): void => {
      wifiConnectionState.socket = null
      resolve()
    }

    const socket = wifiConnectionState.socket
    if (socket && !socket.destroyed) {
      socket.once('close', () => {
        console.log('ESP32 Wi-Fi telemetry socket closed.')
        setTimeout(finish, SOCKET_CLOSE_DELAY_MS)
      })
      socket.destroy()
    } else {
      finish()
    }
  })
}
