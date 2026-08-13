import Cube3D from '@renderer/components/Cube3D'
import calculateOrientation from '@renderer/helpers/calculateOrientation'
import useDeviceTelemetry from '@renderer/helpers/useDeviceTelemetry'
import TelemetrySidebar from './TelemetrySidebar'
import VisualizationHeader from './VisualizationHeader'

function VisualizeScreen({ codename }: { codename: string }): React.JSX.Element {
  const state = useDeviceTelemetry()
  const { roll, pitch, yaw } = calculateOrientation(state)
  const { accelX, accelY, accelZ } = state || {}

  return (
    <div className="flex h-full flex-col">
      <VisualizationHeader codename={codename} />

      <div className="flex flex-1">
        <div className="relative flex-1 bg-grid-lines">
          <Cube3D pitch={pitch} roll={roll} yaw={yaw} />
        </div>

        <TelemetrySidebar
          roll={roll}
          pitch={pitch}
          yaw={yaw}
          accelX={accelX}
          accelY={accelY}
          accelZ={accelZ}
        />
      </div>
    </div>
  )
}

export default VisualizeScreen
