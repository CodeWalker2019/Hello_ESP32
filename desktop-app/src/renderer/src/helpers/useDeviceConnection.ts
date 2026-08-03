// src/renderer/helpers/useDeviceConnection.ts
import { useState } from 'react'

export function useDeviceConnection(onConnected: () => void) {
  const [connectingPath, setConnectingPath] = useState<string | null>(null)

  const connect = async (portPath: string) => {
    setConnectingPath(portPath)
    try {
      // Send connection command and start reading via Electron IPC
      window.api.connectESP32(portPath)
      
      // Simulate a brief handshake delay, then transition screen
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