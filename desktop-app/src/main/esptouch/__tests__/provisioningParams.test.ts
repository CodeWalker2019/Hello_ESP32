import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'
import { buildEsptouchV2Packets } from '../provisioningParams'

describe('buildEsptouchV2Packets', () => {
  beforeEach(() => {
    vi.spyOn(Math, 'random').mockReturnValue(0)
  })

  afterEach(() => {
    vi.restoreAllMocks()
  })

  it('matches the reference sequence for an encrypted request', () => {
    const packets = buildEsptouchV2Packets(
      {
        ssid: Buffer.from('HomeNetwork_5G', 'utf-8'),
        password: Buffer.from('correcthorsebattery', 'utf-8'),
        bssid: Buffer.from('AABBCCDDEEFF', 'hex'),
        aesKey: Buffer.from('HelloESP32Key16!', 'utf-8')
      },
      0
    )

    expect(packets.map((p) => p.length)).toEqual([
      1048, 1082, 1048, 1082, 83, 246, 353, 481, 596, 704, 832, 961, 128, 128, 128, 115, 228, 326,
      488, 624, 723, 849, 976, 129, 129, 129, 114, 200, 357, 450, 631, 709, 877, 981, 130, 130, 130,
      92, 242, 373, 463, 613, 763, 892, 1023, 131, 131, 131, 100, 205, 365, 495, 615, 722, 879, 995,
      132, 132, 132, 64, 214, 357, 501, 576, 763, 876, 1006, 133, 133, 133, 85, 241, 343, 493, 618,
      765, 874, 1015, 134, 134, 134, 112, 225, 336, 481, 593, 753, 864, 1009, 135, 135, 135, 93,
      210, 351, 506, 576, 733, 895, 1002, 136, 136, 136, 91, 223, 377, 459, 629, 766, 895, 1009,
      137, 137, 137, 112, 208, 368, 448, 608, 736, 848, 1022
    ])
  })

  it('matches the reference sequence for an unencrypted request', () => {
    const packets = buildEsptouchV2Packets(
      {
        ssid: Buffer.from('PlainNet', 'utf-8'),
        password: Buffer.from('plainpass123', 'utf-8'),
        bssid: Buffer.from('DEADBEEF0001', 'hex')
      },
      2
    )

    expect(packets.map((p) => p.length)).toEqual([
      1048, 1076, 1048, 1076, 67, 197, 336, 500, 579, 704, 836, 961, 128, 128, 128, 76, 194, 338,
      470, 609, 767, 895, 994, 129, 129, 129, 125, 219, 320, 448, 607, 767, 888, 994, 130, 130, 130,
      76, 195, 339, 471, 608, 734, 895, 1013, 131, 131, 131, 96, 192, 368, 448, 592, 752, 880, 986
    ])
  })

  it('always opens with two guide-code sync packets (length 1048) and their size packets', () => {
    const packets = buildEsptouchV2Packets(
      {
        ssid: Buffer.from('X'),
        password: Buffer.from('y'),
        aesKey: Buffer.from('HelloESP32Key16!')
      },
      0
    )
    expect(packets[0].length).toBe(1048)
    expect(packets[2].length).toBe(1048)
    expect(packets[1].length).toBe(packets[3].length)
  })

  it('embeds the app-port-mark in the header without otherwise changing the packet count', () => {
    const base = { ssid: Buffer.from('Net'), password: Buffer.from('secret123') }
    const counts = [0, 1, 2, 3].map((mark) => buildEsptouchV2Packets(base, mark).length)
    expect(new Set(counts).size).toBe(1)
  })

  it('produces more packets for a longer password', () => {
    const short = buildEsptouchV2Packets({ ssid: Buffer.from('N'), password: Buffer.from('a') }, 0)
    const long = buildEsptouchV2Packets(
      { ssid: Buffer.from('N'), password: Buffer.from('a'.repeat(63)) },
      0
    )
    expect(long.length).toBeGreaterThan(short.length)
  })
})
