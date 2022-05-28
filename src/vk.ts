import { VK } from 'vk-io';
import dotenv from 'dotenv';
dotenv.config({ path: `.env.${process.env.NODE_ENV}` });

export const vk = new VK({
    token: process.env.TOKEN ?? '',
});
