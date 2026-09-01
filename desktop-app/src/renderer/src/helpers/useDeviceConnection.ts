import { useState } from 'react'
import type { DeviceType } from '@renderer/types'

export interface DeviceConnectionHook {
  connectingId: string | null
  connect: (id: string, type: DeviceType) => Promise<void>
}

export function useDeviceConnection(onConnected: (type: DeviceType) => void): DeviceConnectionHook {
  const [connectingPath, setConnectingPath] = useState<string | null>(null)

  const connect = async (id: string, type: DeviceType): Promise<void> => {
    setConnectingPath(id)
    try {
      if (type === 'wifi') {
        window.api.connectESP32Wifi(id)
      } else {
        window.api.connectESP32(id)
      }

      setTimeout((): void => {
        setConnectingPath(null)
        onConnected(type)
      }, 1000)
    } catch (err) {
      console.error('Failed to connect to device:', err)
      setConnectingPath(null)
    }
  }

  return { connectingId: connectingPath, connect }
}
