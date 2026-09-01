import { CUBE_ROTATION_SPEED, CUBE_SIZE } from '@renderer/helpers/constants'
import { getCubeFaceTransforms } from '@renderer/helpers/cube'
import { useCubeRotation } from '@renderer/helpers/useCubeRotation'

const FACE_TRANSFORMS = getCubeFaceTransforms(CUBE_SIZE)

interface FaceStyle {
  fill: string
  label: string
  align: 'flex-end' | 'flex-start'
  labelColor: string
}

const FACES: FaceStyle[] = [
  { fill: '#e8e6e1', label: '+X', align: 'flex-end', labelColor: 'text-fg-5' },
  { fill: '#dcdad4', label: '', align: 'flex-end', labelColor: 'text-fg-5' },
  { fill: '#d6d3cc', label: '', align: 'flex-end', labelColor: 'text-fg-5' },
  { fill: '#cdcac2', label: '-Y LEFT', align: 'flex-end', labelColor: 'text-fg-5' },
  { fill: '#f8f7f4', label: '+Z TOP', align: 'flex-start', labelColor: 'text-fg-6' },
  { fill: '#cdcac3', label: '', align: 'flex-end', labelColor: 'text-fg-5' }
]

interface Cube3DProps {
  pitch?: number
  roll?: number
  yaw?: number
}

function Cube3D({ pitch, roll, yaw = 0 }: Cube3DProps): React.JSX.Element {
  const defaultRotation = useCubeRotation({ x: -22, y: 35 }, CUBE_ROTATION_SPEED)

  const isLive = pitch !== undefined && roll !== undefined

  const baseAngleX = -22
  const baseAngleY = 35

  const rotX = isLive ? baseAngleX - pitch : defaultRotation.x
  const rotY = isLive ? baseAngleY + yaw : defaultRotation.y
  const rotZ = isLive ? -roll : 0

  return (
    <div className="relative flex h-full w-full items-center justify-center [perspective:700px] [perspective-origin:50%_40%]">
      <div
        className="pointer-events-none absolute h-62.5 w-62.5 rounded-full bg-fg/12 blur-[28px]"
        style={{ transform: 'translateY(140px) rotateX(75deg) scale(1.25)' }}
      />

      <div
        className={`relative [transform-style:preserve-3d] ${isLive ? '' : 'transition-transform duration-75 ease-out'}`}
        style={{
          width: CUBE_SIZE,
          height: CUBE_SIZE,
          transform: `rotateX(${rotX}deg) rotateY(${rotY}deg) rotateZ(${rotZ}deg)`
        }}
      >
        {FACE_TRANSFORMS.map((transform, i) => {
          const face = FACES[i]
          return (
            <div
              key={i}
              className={`absolute box-border flex border border-fg/[5.5%] p-2.5 [backface-visibility:hidden] ${
                face.align === 'flex-start' ? 'items-start' : 'items-end'
              }`}
              style={{ width: CUBE_SIZE, height: CUBE_SIZE, transform, backgroundColor: face.fill }}
            >
              {face.label && (
                <span
                  className={`font-mono text-[9px] tracking-[0.12em] select-none ${face.labelColor}`}
                >
                  {face.label}
                </span>
              )}
            </div>
          )
        })}
      </div>
    </div>
  )
}

export default Cube3D
