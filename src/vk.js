const { VK } = require('vk-io')
const config = require('./config')
const { Keyboard } = require('vk-io')
const { HearManager } = require('@vk-io/hear');
const vk = new VK({
  token: config.TOKEN,
  pollingGroupId: config.poullingGroupID,
  peer_id: config.peerID
})
const { api } = vk
const hearManager = new HearManager();

vk.updates.on('message_new', (context, next) => {
	const { messagePayload } = context;

	context.state.command = messagePayload && messagePayload.command
		? messagePayload.command
		: null;

	return next();
});
vk.updates.on('message_new', hearManager.middleware);

module.exports.VK = vk
module.exports.MESSAGES = hearManager
module.exports.KEYBOARD = Keyboard
module.exports.CONFIG = config
module.exports.API = api