import { app, shell, BrowserWindow, ipcMain } from 'electron'
import { join } from 'path'
import dgram from 'dgram'
import { electronApp, optimizer, is } from '@electron-toolkit/utils'
import icon from '../../resources/icon.png?asset'
import { registerSerialConnectionHandlers, teardownSerialConnection } from './serial'
import { registerWifiConnectionHandlers, teardownWifiConnection } from './wifiConnection'
import { registerDisconnectHandler } from './registerDisconnectHandler'
import { registerEsptouchHandlers } from './esptouch'
import { APP_PORTS as ESPTOUCH_APP_PORTS } from './esptouch/constants'
import { registerCurrentNetworkHandlers } from './currentNetwork'

export interface WifiDeviceInfo {
  address: string
  ssid?: string
  rssi?: number
}

const discoveredWifiDevices: Map<string, WifiDeviceInfo> = new Map()
let udpBeaconSocket: dgram.Socket | null = null

function startWifiBeaconListener(): void {
  if (udpBeaconSocket) return

  try {
    udpBeaconSocket = dgram.createSocket({
      type: 'udp4',
      reuseAddr: true,
      reusePort: true
    })

    udpBeaconSocket.on('listening', (): void => {
      if (udpBeaconSocket) {
        try {
          udpBeaconSocket.setBroadcast(true)
          const addr = udpBeaconSocket.address()
          console.log(`[UDP Beacon] Socket listening on ${addr.address}:${addr.port}`)
        } catch (err) {
          console.error('[UDP Beacon] Error enabling broadcast:', err)
        }
      }
    })

    udpBeaconSocket.on('message', (msg: Buffer, rinfo: dgram.RemoteInfo): void => {
      if (ESPTOUCH_APP_PORTS.includes(rinfo.port)) return

      const messageStr: string = msg.toString('utf-8').trim()
      console.log(`[UDP Beacon Received] From ${rinfo.address}:${rinfo.port} -> "${messageStr}"`)

      if (messageStr.includes('HelloESP32') || messageStr.includes('Kestrel')) {
        discoveredWifiDevices.set(rinfo.address, {
          address: rinfo.address,
          ssid: messageStr,
          rssi: -50
        })
      }
    })

    udpBeaconSocket.on('error', (err: Error): void => {
      console.error('[UDP Beacon Listener Error]:', err)
      if (udpBeaconSocket) {
        try {
          udpBeaconSocket.close()
        } catch {}
        udpBeaconSocket = null
      }
    })

    udpBeaconSocket.bind(7001)
  } catch (err) {
    console.error('Failed to start UDP beacon listener:', err)
  }
}

function registerWifiHandlers(): void {
  ipcMain.handle('scan-wifi-devices', async (): Promise<WifiDeviceInfo[]> =>
    Array.from(discoveredWifiDevices.values())
  )
}

function createWindow(): void {
  const mainWindow: BrowserWindow = new BrowserWindow({
    width: 900,
    height: 670,
    show: false,
    autoHideMenuBar: true,
    ...(process.platform === 'linux' ? { icon } : {}),
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      sandbox: false
    }
  })

  mainWindow.on('ready-to-show', (): void => {
    mainWindow.show()
  })

  mainWindow.webContents.setWindowOpenHandler((details): { action: 'deny' } => {
    shell.openExternal(details.url)
    return { action: 'deny' }
  })

  if (is.dev && process.env['ELECTRON_RENDERER_URL']) {
    void mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    void mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

app.whenReady().then((): void => {
  electronApp.setAppUserModelId('com.electron')

  app.on('browser-window-created', (_, window: BrowserWindow): void => {
    optimizer.watchWindowShortcuts(window)
  })

  registerSerialConnectionHandlers()
  registerWifiConnectionHandlers()
  registerDisconnectHandler()
  registerWifiHandlers()
  registerEsptouchHandlers()
  registerCurrentNetworkHandlers()
  startWifiBeaconListener()

  createWindow()

  app.on('activate', (): void => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', (): void => {
  if (udpBeaconSocket) {
    udpBeaconSocket.close()
    udpBeaconSocket = null
  }
  teardownSerialConnection()
  teardownWifiConnection()
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
