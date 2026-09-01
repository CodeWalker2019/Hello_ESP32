import { useState } from 'react'
import Header from '@renderer/components/Header'
import SearchScreen from '@renderer/views/SearchScreen'
import VisualizeScreen from '@renderer/views/VisualizeScreen'
import { SCREEN, type ScreenId, type DeviceType } from '@renderer/types'

const CODENAME = 'KESTREL-04'

function App(): React.JSX.Element {
  const [screen, setScreen] = useState<ScreenId>(SCREEN.SEARCH)
  const [connectionType, setConnectionType] = useState<DeviceType>('usb')

  return (
    <div className="flex h-screen min-h-160 w-full flex-col overflow-hidden bg-bg font-mono text-fg">
      <Header />
      <div className="min-h-0 flex-1">
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
    </div>
  )
}

export default App
