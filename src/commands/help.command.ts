import { hearManager } from '../config';
import { Context } from 'vk-io';
import { throwException } from '../utils';
import { ICommand } from '../types/global';

export const helpCommandData: ICommand = {
    name: '/help',
    description: 'Документация бота',
};

export const helpCommand = async () => {
    hearManager.hear(helpCommandData.name, async (context: Context) => {
        try {
            await context.send('📑 Документация: https://sashafromlibertalia.github.io/sparkle/\n⚙️ GitHub: https://github.com/sashafromlibertalia/sparkle');
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

helpCommand();
