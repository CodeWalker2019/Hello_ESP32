import { useEffect, useRef, useState } from 'react'
import { CONNECT_DELAY_MS } from './constants'

interface DeviceConnection {
  connectingId: string | null
  connect: (id: string) => void
}

export function useDeviceConnection(onConnected: () => void): DeviceConnection {
  const [connectingId, setConnectingId] = useState<string | null>(null)
  const timeoutRef = useRef<ReturnType<typeof setTimeout> | undefined>(undefined)

  useEffect(() => () => clearTimeout(timeoutRef.current), [])

  const connect = (id: string): void => {
    setConnectingId(id)
    timeoutRef.current = setTimeout(onConnected, CONNECT_DELAY_MS)
  }

  return { connectingId, connect }
}
