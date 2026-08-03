import { app, shell, BrowserWindow, ipcMain } from 'electron'
import { join } from 'path'
import { electronApp, optimizer, is } from '@electron-toolkit/utils'
import { SerialPort } from 'serialport'
import { ReadlineParser } from '@serialport/parser-readline'
import icon from '../../resources/icon.png?asset'

const TARGET_DEVICE_FAMILY_ID = 0x4B;

let activePort: SerialPort | null = null;
let activeParser: ReadlineParser | null = null;
let heartbeatInterval: NodeJS.Timeout | null = null;

function createWindow(): void {
  const mainWindow = new BrowserWindow({
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

  mainWindow.on('ready-to-show', () => {
    mainWindow.show()
  })

  mainWindow.webContents.setWindowOpenHandler((details) => {
    shell.openExternal(details.url)
    return { action: 'deny' }
  })

  if (is.dev && process.env['ELECTRON_RENDERER_URL']) {
    mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

// --- Serial Port Helper: Scan & Handshake Filter ---
function checkPortForDevice(path: string): Promise<boolean> {
  return new Promise((resolve) => {
    const tempPort = new SerialPort({ path, baudRate: 115200, autoOpen: false });
    let resolved = false;

    const cleanup = () => {
      if (tempPort.isOpen) {
        console.log(`Closing port ${path} after handshake check.`);
        tempPort.close();
      }
    };

    tempPort.open((err) => {
      console.log(`Attempting to open port ${path}...`);
      if (err) {
        console.error(`Error opening port ${path}:`, err);
        return resolve(false);
      }
    });

    tempPort.on('data', (data: Buffer) => {
      if (resolved) return;

      console.log(`Data received on ${path}:`, data);
    
      if (data.length >= 3 && data[0] === 0xAA && data[1] === 0x55 && data[2] === TARGET_DEVICE_FAMILY_ID) {
        resolved = true;
        cleanup();
        resolve(true);
      }
    });

    setTimeout(() => {
      if (!resolved) {
        resolved = true;
        cleanup();
        resolve(false);
      }
    }, 1500);
  });
}

// --- IPC Handlers for ESP32 ---

// 1. Scan and return only matching ports
ipcMain.handle('scan-esp32-ports', async () => {
  try {
    const availablePorts = await SerialPort.list();
    const matchedPorts: any[] = [];

    console.log(availablePorts)

    for (const portInfo of availablePorts) {
      try {
        const isMatch = await checkPortForDevice(portInfo.path);
        if (isMatch) {
          matchedPorts.push(portInfo);
        }
      } catch {
        // Skip busy or problematic ports
      }
    }
    return matchedPorts;
  } catch (error) {
    console.error('Error scanning ports:', error);
    return [];
  }
});

// 2. Connect, Send Start Signal, Start Heartbeat, and stream data back
ipcMain.on('connect-esp32', (event, portPath: string) => {
  // Close any active port and clear any old interval first
  if (activePort && activePort.isOpen) {
    activePort.close();
  }
  if (heartbeatInterval) {
    clearInterval(heartbeatInterval);
    heartbeatInterval = null;
  }

  activePort = new SerialPort({ path: portPath, baudRate: 115200 });
  activeParser = activePort.pipe(new ReadlineParser({ delimiter: '\r\n' }));

  activePort.on('open', () => {
    console.log(`Connected to target ESP32 on ${portPath}`);

    // Send initial start command packet
    const startPacket = Buffer.from([0xAA, 0x55, 0x01]);
    activePort?.write(startPacket, (err) => {
      if (err) console.error('Failed to write start command:', err);
    });

    // Start constant heartbeat loop every 1000ms (Command ID 0x02)
    const heartbeatPacket = Buffer.from([0xAA, 0x55, 0x02]);
    heartbeatInterval = setInterval(() => {
      if (activePort && activePort.isOpen) {
        activePort.write(heartbeatPacket, (err) => {
          if (err) console.error('Failed to write heartbeat:', err);
        });
      }
    }, 1000);
  });

  // Forward incoming data lines to renderer
  activeParser.on('data', (data: string) => {
    event.sender.send('esp32-data', data);
  });

  activePort.on('error', (err) => {
    console.error('Serial Port Error:', err.message);
  });
});

// 3. Disconnect / Deselect device
ipcMain.on('disconnect-esp32', () => {
  if (heartbeatInterval) {
    clearInterval(heartbeatInterval);
    heartbeatInterval = null;
  }

  if (activePort && activePort.isOpen) {
    activePort.close(() => {
      activePort = null;
      activeParser = null;
      console.log('ESP32 disconnected and heartbeat stopped.');
    });
  }
});

// --- App Lifecycle ---

app.whenReady().then(() => {
  electronApp.setAppUserModelId('com.electron')

  app.on('browser-window-created', (_, window) => {
    optimizer.watchWindowShortcuts(window)
  })

  createWindow()

  app.on('activate', function () {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', () => {
  if (heartbeatInterval) {
    clearInterval(heartbeatInterval);
    heartbeatInterval = null;
  }
  if (activePort && activePort.isOpen) {
    activePort.close();
  }
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
