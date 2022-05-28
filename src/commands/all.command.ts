import { hearManager } from '../config';
import { Context } from 'vk-io';
import { throwException } from '../utils';
import { commands } from '../core';

export const allCommand = async () => {
    hearManager.hear(['/all', '/start'], async (context: Context) => {
        try {
            let message = '';
            for (const command of commands) {
                message += `${command.name}: ${command.description}\n`;
            };

            await context.send(message);
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};
