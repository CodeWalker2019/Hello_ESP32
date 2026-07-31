import { CUBE_ROTATION_SPEED, CUBE_SIZE } from '@renderer/helpers/constants'
import { getCubeFaceTransforms } from '@renderer/helpers/cube'
import { useCubeRotation } from '@renderer/helpers/useCubeRotation'

const FACE_TRANSFORMS = getCubeFaceTransforms(CUBE_SIZE)

function Cube3D(): React.JSX.Element {
  const rotation = useCubeRotation({ x: -22, y: 35 }, CUBE_ROTATION_SPEED)

  return (
    <div className="flex h-full w-full items-center justify-center [perspective:900px]">
      <div
        className="relative [transform-style:preserve-3d]"
        style={{
          width: CUBE_SIZE,
          height: CUBE_SIZE,
          transform: `rotateX(${rotation.x}deg) rotateY(${rotation.y}deg)`
        }}
      >
        {FACE_TRANSFORMS.map((transform, i) => (
          <div
            key={i}
            className="absolute border border-amber bg-gradient-to-br from-amber/10 to-transparent"
            style={{ width: CUBE_SIZE, height: CUBE_SIZE, transform }}
          />
        ))}
      </div>
    </div>
  )
}

export default Cube3D
