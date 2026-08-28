import { CUBE_ROTATION_SPEED, CUBE_SIZE } from '@renderer/helpers/constants'
import { getCubeFaceTransforms } from '@renderer/helpers/cube'
import { useCubeRotation } from '@renderer/helpers/useCubeRotation'

const FACE_TRANSFORMS = getCubeFaceTransforms(CUBE_SIZE)

const FACE_LABELS = ['+X FRONT', '-X BACK', '+Y RIGHT', '-Y LEFT', '+Z TOP', '-Z BTM']

interface Cube3DProps {
  pitch?: number
  roll?: number
  yaw?: number
}

function Cube3D({ pitch, roll, yaw = 0 }: Cube3DProps): React.JSX.Element {
  const defaultRotation = useCubeRotation({ x: -22, y: 35 }, CUBE_ROTATION_SPEED)

  const isLive = pitch !== undefined && roll !== undefined

  // Base 3/4 elevated view (-22° pitch, 35° yaw)
  const baseAngleX = -22
  const baseAngleY = 35

  // Invert pitch so tilting the board nose-up tilts the 3D model nose-up
  const rotX = isLive ? baseAngleX - pitch : defaultRotation.x
  const rotY = isLive ? baseAngleY + yaw : defaultRotation.y
  const rotZ = isLive ? -roll : 0

  return (
    <div className="relative flex h-full w-full items-center justify-center [perspective:600px] [perspective-origin:50%_35%] overflow-hidden">
      {/* Ground Projection Shadow */}
      <div
        className="absolute h-44 w-44 rounded-full bg-amber/15 blur-2xl pointer-events-none"
        style={{
          transform: 'translateY(95px) rotateX(75deg) scale(1.3)'
        }}
      />

      {/* 3D Cube Model */}
      <div
        className={`relative [transform-style:preserve-3d] ${isLive ? '' : 'transition-transform duration-75 ease-out'}`}
        style={{
          width: CUBE_SIZE,
          height: CUBE_SIZE,
          transform: `rotateX(${rotX}deg) rotateY(${rotY}deg) rotateZ(${rotZ}deg)`
        }}
      >
        {FACE_TRANSFORMS.map((transform, i) => (
          <div
            key={i}
            className="absolute flex items-center justify-center border border-amber/70 bg-gradient-to-br from-amber/20 via-amber/5 to-transparent shadow-[inset_0_0_15px_rgba(245,158,11,0.15)]"
            style={{ width: CUBE_SIZE, height: CUBE_SIZE, transform }}
          >
            <span className="font-mono text-[9px] font-semibold tracking-widest text-amber/80 select-none">
              {FACE_LABELS[i]}
            </span>
          </div>
        ))}
      </div>
    </div>
  )
}

export default Cube3D


