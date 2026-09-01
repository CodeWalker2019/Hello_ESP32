import { useState } from 'react'
import SearchScreen from '@renderer/views/SearchScreen'
import VisualizeScreen from '@renderer/views/VisualizeScreen'
import { SCREEN, type ScreenId, type DeviceType } from '@renderer/types'

const CODENAME = 'KESTREL-04'

function App(): React.JSX.Element {
  const [screen, setScreen] = useState<ScreenId>(SCREEN.SEARCH)
  const [connectionType, setConnectionType] = useState<DeviceType>('usb')

  return (
    <div className="flex h-screen min-h-160 w-full flex-col overflow-hidden bg-bg font-sans text-fg">
      {screen === SCREEN.SEARCH ? (
        <SearchScreen
          onConnected={(type) => {
            setConnectionType(type)
            setScreen(SCREEN.VISUALIZE)
          }}
        />
      ) : (
        <VisualizeScreen
          codename={CODENAME}
          connectionType={connectionType}
          onBack={() => {
            window.api.disconnectESP32()
            setScreen(SCREEN.SEARCH)
          }}
        />
      )}
    </div>
  )
}

export default App
