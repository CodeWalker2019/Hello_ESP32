import { SerialPort } from 'serialport'
import { ESP32_VENDOR_ID } from './constants'
import { checkPortForDevice } from './checkPortForDevice'
import { serialState } from './state'
import type { PortInfo } from './types'

/**
 * Lists all serial ports, narrows to the expected vendor ID, then confirms
 * each candidate with a handshake check. Ports that are busy or error out
 * during the check are silently skipped rather than failing the scan. The
 * currently connected port, if any, is trusted without a re-check — probing
 * it would always fail with EBUSY since this process already holds it open.
 */
export async function scanPorts(): Promise<PortInfo[]> {
  const availablePorts = await SerialPort.list()
  const candidatePorts = availablePorts.filter((p) => p.vendorId === ESP32_VENDOR_ID)
  const matchedPorts: PortInfo[] = []

  for (const portInfo of candidatePorts) {
    if (portInfo.path === serialState.port?.path) {
      matchedPorts.push(portInfo)
      continue
    }

    try {
      const isMatch = await checkPortForDevice(portInfo.path)
      if (isMatch) {
        matchedPorts.push(portInfo)
      }
    } catch {
      // Skip busy or problematic ports
    }
  }

  return matchedPorts
}
