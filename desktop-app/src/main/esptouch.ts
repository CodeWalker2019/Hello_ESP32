import { ipcMain, IpcMainInvokeEvent } from 'electron'
import { EsptouchV2Provisioner, ProvisionedDevice } from './esptouch/provisioner'
import { DEFAULT_AES_KEY, PASSWORD_LENGTH_MAX, SSID_LENGTH_MAX } from './esptouch/constants'

export interface ProvisionParams {
  ssid: string
  password: string
}

export interface HandlerResponse {
  success: boolean
  error?: string
}

const provisioner = new EsptouchV2Provisioner()

function validate(params: ProvisionParams): string | null {
  if (!params?.ssid) return 'SSID is required'
  if (Buffer.byteLength(params.ssid, 'utf-8') > SSID_LENGTH_MAX) {
    return `SSID must be at most ${SSID_LENGTH_MAX} bytes`
  }
  if (Buffer.byteLength(params.password ?? '', 'utf-8') > PASSWORD_LENGTH_MAX) {
    return `Password must be at most ${PASSWORD_LENGTH_MAX} bytes`
  }
  return null
}

export function registerEsptouchHandlers(): void {
  ipcMain.handle(
    'esptouch:start',
    async (event: IpcMainInvokeEvent, params: ProvisionParams): Promise<HandlerResponse> => {
      const validationError = validate(params)
      if (validationError) {
        console.error('[ESPTouch V2] Rejected start request:', validationError)
        return { success: false, error: validationError }
      }

      try {
        await provisioner.start(
          {
            ssid: Buffer.from(params.ssid, 'utf-8'),
            password: Buffer.from(params.password ?? '', 'utf-8'),
            aesKey: Buffer.from(DEFAULT_AES_KEY, 'utf-8')
          },
          (device: ProvisionedDevice): void => {
            console.log('[ESPTouch V2] Device responded:', device)
            event.sender.send('esptouch:on-device-found', device)
          }
        )
        return { success: true }
      } catch (error) {
        const message = error instanceof Error ? error.message : String(error)
        console.error('[ESPTouch V2] Failed to start:', message)
        return { success: false, error: message }
      }
    }
  )

  ipcMain.handle('esptouch:stop', async (): Promise<HandlerResponse> => {
    provisioner.stop()
    return { success: true }
  })
}
