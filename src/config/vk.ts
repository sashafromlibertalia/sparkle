import { MessageContext, VK } from 'vk-io';
import { HearManager } from '@vk-io/hear';
import { config } from './config';
const { token, pollingGroupId } = config;

const vk = new VK({
    token,
    pollingGroupId,
});

const hearManager = new HearManager<MessageContext>();

vk.updates.on('message_new', hearManager.middleware);

export { vk, hearManager };
