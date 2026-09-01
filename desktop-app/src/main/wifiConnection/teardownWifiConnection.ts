import { disconnectWifi } from './disconnect'

export function teardownWifiConnection(): void {
  void disconnectWifi()
}
