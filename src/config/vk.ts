import { Context, VK } from 'vk-io';
const { HearManager } = require('@vk-io/hear');

import { config } from './config';
const { token } = config;

const vk = new VK({
    token,
});

const hearManager: Context = new HearManager();

vk.updates.on('message_new', hearManager.middleware);

export { vk, hearManager };
