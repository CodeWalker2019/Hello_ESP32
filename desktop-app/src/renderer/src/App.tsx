import { useEffect, useState } from 'react'
import Header from '@renderer/components/Header'
import SearchScreen from '@renderer/views/SearchScreen'
import VisualizeScreen from '@renderer/views/VisualizeScreen'
import { SCREEN, type ScreenId } from '@renderer/types'
import { useDeviceConnection } from './helpers/useDeviceConnection'

const CODENAME = 'KESTREL-04'

function App(): React.JSX.Element {
  const [screen, setScreen] = useState<ScreenId>(SCREEN.SEARCH)
  const { connectingId, connect } = useDeviceConnection(() => setScreen(SCREEN.VISUALIZE))

  useEffect(() => {
    return window.api.onDisconnected(() => {
      setScreen(SCREEN.SEARCH)
    })
  }, [])

  return (
    <div className="flex h-screen min-h-160 w-full flex-col overflow-hidden bg-bg font-mono text-fg">
      <Header />
      <div className="min-h-0 flex-1">
        {screen === SCREEN.SEARCH ? (
          <SearchScreen connectingId={connectingId} connect={connect} />
        ) : (
          <VisualizeScreen codename={CODENAME} />
        )}
      </div>
    </div>
  )
}

export default App
