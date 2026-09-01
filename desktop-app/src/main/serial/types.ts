import { SerialPort } from 'serialport'

export type PortInfo = Awaited<ReturnType<typeof SerialPort.list>>[number]
