import { useState } from 'react'

export interface IUseDeviceConnectionReturnType {
  connect: (portPath: string) => Promise<void>
  connectingId: string | null
}

export function useDeviceConnection(onConnected: () => void): IUseDeviceConnectionReturnType {
  const [connectingPath, setConnectingPath] = useState<string | null>(null)

  const connect = async (portPath: string): Promise<void> => {
    setConnectingPath(portPath)
    try {
      window.api.connectESP32(portPath)
      setTimeout(() => {
        setConnectingPath(null)
        onConnected()
      }, 1000)
    } catch (err) {
      console.error('Failed to connect to device:', err)
      setConnectingPath(null)
    }
  }

  return { connectingId: connectingPath, connect }
}
