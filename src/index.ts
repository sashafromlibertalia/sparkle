import 'reflect-metadata';
import { Bot } from './core';

const bot = new Bot();

(async () => {
    await bot.startPolling();
})();
