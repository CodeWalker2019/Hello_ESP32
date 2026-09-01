import { useEffect, useRef, useState } from 'react'
import type { ScannedDevice } from '@renderer/types'
import { SCAN_POLL_INTERVAL_MS } from '@renderer/helpers/constants'
import { mapSerialPortToDevice, mapWifiDeviceToDevice } from '@renderer/helpers/deviceMapping'

export interface DeviceScanHook {
  devices: ScannedDevice[]
  isScanning: boolean
  setProvisioning: (isProvisioning: boolean) => void
}

async function scanDevices<T>(
  label: string,
  fetcher: (() => Promise<T[]>) | undefined,
  mapper: (item: T) => ScannedDevice
): Promise<ScannedDevice[]> {
  if (!fetcher) return []
  try {
    const items = await fetcher()
    return items.map(mapper)
  } catch (err) {
    console.error(`${label} scan error:`, err)
    return []
  }
}

export function useDeviceScan(): DeviceScanHook {
  const [devices, setDevices] = useState<ScannedDevice[]>([])
  const [isScanning, setIsScanning] = useState<boolean>(true)
  const isProvisioningRef = useRef<boolean>(false)

  useEffect((): (() => void) => {
    let cancelled = false
    let timeoutId: ReturnType<typeof setTimeout> | undefined

    const scheduleNextScan = (): void => {
      timeoutId = setTimeout((): void => {
        void runScanCycle(false)
      }, SCAN_POLL_INTERVAL_MS)
    }

    async function runScanCycle(isInitialScan: boolean): Promise<void> {
      if (isInitialScan) setIsScanning(true)

      const [usbDevices, wifiDevices] = await Promise.all([
        isProvisioningRef.current
          ? Promise.resolve<ScannedDevice[]>([])
          : scanDevices('USB', window.api.scanPorts, mapSerialPortToDevice),
        scanDevices('Wi-Fi', window.api.scanWifiDevices, mapWifiDeviceToDevice)
      ])

      if (cancelled) return

      try {
        setDevices([...usbDevices, ...wifiDevices])
      } finally {
        if (isInitialScan) setIsScanning(false)
        if (!cancelled) scheduleNextScan()
      }
    }

    void runScanCycle(true)

    return (): void => {
      cancelled = true
      if (timeoutId) clearTimeout(timeoutId)
    }
  }, [])

  const setProvisioning = (isProvisioning: boolean): void => {
    isProvisioningRef.current = isProvisioning
  }

  return { devices, isScanning, setProvisioning }
}
