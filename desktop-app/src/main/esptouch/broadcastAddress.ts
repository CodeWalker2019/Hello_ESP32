import os from 'os'

export function computeBroadcastAddress(address: string, netmask: string): string | null {
  const ip = address.split('.').map(Number)
  const mask = netmask.split('.').map(Number)
  if (ip.length !== 4 || mask.length !== 4 || ip.some(Number.isNaN) || mask.some(Number.isNaN)) {
    return null
  }
  return ip.map((octet, i) => (octet | (~mask[i] & 0xff)) & 0xff).join('.')
}

export function getBroadcastAddresses(): string[] {
  const addresses = new Set<string>(['255.255.255.255'])

  for (const interfaces of Object.values(os.networkInterfaces())) {
    if (!interfaces) continue
    for (const iface of interfaces) {
      if (iface.family !== 'IPv4' || iface.internal) continue
      const broadcast = computeBroadcastAddress(iface.address, iface.netmask)
      if (broadcast) addresses.add(broadcast)
    }
  }

  return Array.from(addresses)
}
