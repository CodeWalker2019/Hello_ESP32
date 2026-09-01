const CRC_POLYNOM = 0x8c

function buildTable(): number[] {
  const table = new Array<number>(256)
  for (let dividend = 0; dividend < 256; dividend++) {
    let remainder = dividend
    for (let bit = 0; bit < 8; bit++) {
      remainder = remainder & 1 ? (remainder >>> 1) ^ CRC_POLYNOM : remainder >>> 1
    }
    table[dividend] = remainder & 0xff
  }
  return table
}

const CRC_TABLE = buildTable()

export class Crc8 {
  private value = 0

  update(buffer: Buffer, offset = 0, length: number = buffer.length - offset): this {
    for (let i = 0; i < length; i++) {
      const index = (buffer[offset + i] ^ this.value) & 0xff
      this.value = CRC_TABLE[index]
    }
    return this
  }

  get(): number {
    return this.value & 0xff
  }

  reset(): this {
    this.value = 0
    return this
  }
}
