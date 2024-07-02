
import Console from 'bare-console'
import process from 'bare-process'

// globals

Object.defineProperty(globalThis, 'global', {
  enumerable: true,
  writable: false,
  value: globalThis
})
Object.defineProperty(globalThis, 'window', {
  enumerable: true,
  writable: false,
  value: globalThis
})
Object.defineProperty(globalThis, 'self', {
  enumerable: true,
  writable: false,
  value: globalThis
})
Object.defineProperty(globalThis, 'process', {
  enumerable: true,
  writable: false,
  value: process
})

// error handlers

process.on('uncaughtException', (ex) => {
  console.log('uncaughtException', ex)
})

process.on('unhandledRejection', (ex) => {
  console.log('unhandledRejection', ex)
})

// console redirection to Qt side

const consoleProxy = new Console({
  colors: false,
  stdout: core.log,
  stderr: core.log
})
globalThis.console = consoleProxy
