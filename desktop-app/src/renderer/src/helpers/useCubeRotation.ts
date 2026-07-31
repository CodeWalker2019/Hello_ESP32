import { useEffect, useState } from 'react'

interface Rotation {
  x: number
  y: number
}

export function useCubeRotation(initial: Rotation, speedPerFrame: number): Rotation {
  const [rotation, setRotation] = useState(initial)

  useEffect(() => {
    let raf: number
    const tick = (): void => {
      setRotation((r) => ({ x: r.x, y: r.y + speedPerFrame }))
      raf = requestAnimationFrame(tick)
    }
    raf = requestAnimationFrame(tick)
    return () => cancelAnimationFrame(raf)
  }, [speedPerFrame])

  return rotation
}
