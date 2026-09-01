export function formatSigned(value: number, decimals: number): string {
  const fixed = value.toFixed(decimals)
  return fixed.startsWith('-') ? `−${fixed.slice(1)}` : fixed
}
