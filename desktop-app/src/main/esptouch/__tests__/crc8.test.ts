import { describe, expect, it } from 'vitest'
import { Crc8 } from '../crc8'

describe('Crc8', () => {
  it('returns 0 for an empty update', () => {
    expect(new Crc8().get()).toBe(0)
  })

  it('is stable and deterministic for repeated input', () => {
    const a = new Crc8().update(Buffer.from([0x01, 0x02, 0x03])).get()
    const b = new Crc8().update(Buffer.from([0x01, 0x02, 0x03])).get()
    expect(a).toBe(b)
    expect(a).toBe(0xd8)
  })

  it('reset() returns the accumulator to its initial state', () => {
    const crc = new Crc8().update(Buffer.from([0xff, 0xee]))
    crc.reset()
    expect(crc.get()).toBe(0)
  })

  it('respects the offset/length window', () => {
    const buf = Buffer.from([0x00, 0x01, 0x02, 0x03, 0x00])
    const windowed = new Crc8().update(buf, 1, 3).get()
    const direct = new Crc8().update(Buffer.from([0x01, 0x02, 0x03])).get()
    expect(windowed).toBe(direct)
  })

  it('is sensitive to every input byte', () => {
    const base = new Crc8().update(Buffer.from([0x10, 0x20, 0x30])).get()
    const flipped = new Crc8().update(Buffer.from([0x10, 0x20, 0x31])).get()
    expect(flipped).not.toBe(base)
  })
})
