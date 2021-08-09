/** 
@important
Советую тут ничего не трогать :)
*/

const { VK } = require('vk-io')
const config = require('./config')
const { Keyboard } = require('vk-io')
const { HearManager } = require('@vk-io/hear')
const { API } = require('vk-io')
const vk = new VK({
  token: config.TOKEN,
  pollingGroupId: config.POLLING_GROUP_ID
})
const api = new API({
  token: config.TOKEN
})

const hearManager = new HearManager()

vk.updates.on('message_new', (context, next) => {
  const { messagePayload } = context

  context.state.command =
		messagePayload && messagePayload.command ? messagePayload.command : null

  return next()
})
vk.updates.on('message_new', hearManager.middleware)

const hearCommand = (name, conditions, handle) => {
  if (typeof handle !== 'function') {
    handle = conditions
    conditions = [`/${name}`]
  }

  if (!Array.isArray(conditions)) {
    conditions = [conditions]
  }

  hearManager.hear(
    [(text, { state }) => state.command === name, ...conditions],
    handle
  )
}

const RANDOMIZE = function RANDOM () {
  return Math.round(Math.random() * Number.MAX_SAFE_INTEGER)
}

module.exports.VK = vk
module.exports.MESSAGES = hearManager
module.exports.KEYBOARD = Keyboard
module.exports.CONFIG = config
module.exports.API = api
module.exports.HEARCOMMAND = hearCommand
module.exports.RANDOM = RANDOMIZE
