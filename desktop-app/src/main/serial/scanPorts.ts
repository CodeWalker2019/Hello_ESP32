import { SerialPort } from 'serialport'
import { checkPortForDevice } from './checkPortForDevice'
import { serialState } from './state'
import type { PortInfo } from './types'

const ESP32_VENDOR_IDS = ['10c4', '0403', '303a', '1a86']

export async function scanPorts(): Promise<PortInfo[]> {
  const availablePorts = await SerialPort.list()

  const candidatePorts = availablePorts.filter((p) => {
    const vendorId = p.vendorId?.toLowerCase()
    const matchesVendor = vendorId ? ESP32_VENDOR_IDS.includes(vendorId) : false
    const matchesPath =
      p.path.includes('tty.usbserial') ||
      p.path.includes('tty.usbmodem') ||
      p.path.includes('cu.usbserial')

    return matchesVendor || matchesPath
  })

  const matchedPorts: PortInfo[] = []

  for (const portInfo of candidatePorts) {
    if (portInfo.path === serialState.port?.path) {
      matchedPorts.push(portInfo)
      continue
    }

    const isMatch = await checkPortForDevice(portInfo.path)
    if (isMatch) matchedPorts.push(portInfo)
  }

  return matchedPorts
}
