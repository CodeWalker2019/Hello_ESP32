export type DeviceType = 'usb' | 'wifi'

export interface ScannedDevice {
  codename: string
  id: string
  rssi: number
  type: DeviceType
}

export const SCREEN = {
  SEARCH: 'search',
  VISUALIZE: 'visualize'
} as const

export type ScreenId = (typeof SCREEN)[keyof typeof SCREEN]
