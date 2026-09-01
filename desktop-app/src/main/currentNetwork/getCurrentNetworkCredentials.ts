import { execFile } from 'child_process'
import { promisify } from 'util'

const execFileAsync = promisify(execFile)

export interface CurrentNetworkCredentials {
  ssid: string
  password: string | null
  passwordError?: string
}

export async function getCurrentNetworkCredentials(): Promise<CurrentNetworkCredentials> {
  if (process.platform !== 'darwin') {
    throw new Error('Reading the current Wi-Fi network is only supported on macOS right now')
  }

  const ssid = await getCurrentSsid()
  if (!ssid) {
    throw new Error('Not connected to a Wi-Fi network')
  }

  try {
    const password = await getKeychainPassword(ssid)
    return { ssid, password }
  } catch {
    return {
      ssid,
      password: null,
      passwordError:
        'Found the network, but Keychain didn’t have a saved password for it — enter it manually.'
    }
  }
}

async function getWifiDeviceName(): Promise<string> {
  const { stdout } = await execFileAsync('networksetup', ['-listallhardwareports'])
  const match = stdout.match(/Hardware Port: Wi-Fi\nDevice: (\w+)/)
  return match?.[1] ?? 'en0'
}

async function getCurrentSsid(): Promise<string | null> {
  const device = await getWifiDeviceName()
  const { stdout } = await execFileAsync('ipconfig', ['getsummary', device])
  const match = stdout.match(/^\s*SSID : (.+)$/m)
  return match ? match[1].trim() : null
}

async function getKeychainPassword(ssid: string): Promise<string> {
  const { stdout } = await execFileAsync('security', [
    'find-generic-password',
    '-D',
    'AirPort network password',
    '-a',
    ssid,
    '-w'
  ])
  return stdout.trim()
}
