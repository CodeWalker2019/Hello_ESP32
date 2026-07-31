import { useState } from 'react'
import Header from '@renderer/components/Header'
import SearchScreen from '@renderer/views/SearchScreen'
import VisualizeScreen from '@renderer/views/VisualizeScreen'
import type { ScreenId } from '@renderer/types'

const CODENAME = 'KESTREL-04'

function App(): React.JSX.Element {
  const [screen, setScreen] = useState<ScreenId>('search')

  return (
    <div className="flex h-screen min-h-[640px] w-full flex-col overflow-hidden bg-bg font-mono text-fg">
      <Header />
      <div className="min-h-0 flex-1">
        {screen === 'search' ? (
          <SearchScreen onConnected={() => setScreen('visualize')} />
        ) : (
          <VisualizeScreen codename={CODENAME} onBack={() => setScreen('search')} />
        )}
      </div>
    </div>
  )
}

export default App
