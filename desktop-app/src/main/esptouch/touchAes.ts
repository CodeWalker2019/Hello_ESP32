import { createCipheriv } from 'crypto'

export function touchAesEncrypt(key: Buffer, iv: Buffer, plaintext: Buffer): Buffer {
  const cipher = createCipheriv('aes-128-cbc', key, iv)
  return Buffer.concat([cipher.update(plaintext), cipher.final()])
}
