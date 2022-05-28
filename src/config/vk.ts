import { Context, VK } from 'vk-io';
const { HearManager } = require('@vk-io/hear');
import dotenv from 'dotenv';
dotenv.config({ path: `.env.${process.env.NODE_ENV}` });

const vk = new VK({
    token: process.env.TOKEN ?? '',
});

const hearManager: Context = new HearManager();

vk.updates.on('message_new', hearManager.middleware);

export {vk, hearManager}
