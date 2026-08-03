import { useState } from 'react'
import Header from '@renderer/components/Header'
import SearchScreen from '@renderer/views/SearchScreen'
import VisualizeScreen from '@renderer/views/VisualizeScreen'
import { SCREEN, type ScreenId } from '@renderer/types'

const CODENAME = 'KESTREL-04'

function App(): React.JSX.Element {
  const [screen, setScreen] = useState<ScreenId>(SCREEN.SEARCH)

  return (
    <div className="flex h-screen min-h-[640px] w-full flex-col overflow-hidden bg-bg font-mono text-fg">
      <Header />
      <div className="min-h-0 flex-1">
        {screen === SCREEN.SEARCH ? (
          <SearchScreen onConnected={() => setScreen(SCREEN.VISUALIZE)} />
        ) : (
          <VisualizeScreen
            codename={CODENAME}
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
