import { Context } from 'vk-io';
import { throwException } from '../utils';
import { dateKeyboard } from '../common/keyboards/date';
import { hearManager } from '../config';

export const dateCommandData: ICommand = {
    name: '/дата',
    description: 'Отправляет клавиатуру с домашними заданиями по дням',
};

export const dateCommand = async () => {
    hearManager.hear(dateCommandData.name, async (context: Context) => {
        try {
            await context.send({
                message: 'Выберите, какой день вам нужен:',
                keyboard: dateKeyboard,
            });
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

dateCommand();
