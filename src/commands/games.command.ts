import { Context } from 'vk-io';
import { throwException } from '../utils';
import { gamesKeyboard } from '../common/keyboards/games';
import { hearManager } from '../config';

export const gamesCommandData: ICommand = {
    name: '/игры',
    description: 'Отправляет клавиатуру с выбором игр',
};

export const gamesCommand = async () => {
    hearManager.hear(gamesCommandData.name, async (context: Context) => {
        try {
            await context.send({
                message: 'Вот список моих игр',
                keyboard: gamesKeyboard,
            });
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

gamesCommand();
