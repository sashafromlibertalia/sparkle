import { hearManager } from '../config';
import { Context } from 'vk-io';
import { throwException } from '../utils';
import { addKeyboard } from '../common/keyboards/add';

export const addCommandData: ICommand = {
    name: '/добавить',
    description: 'Добавляет домашку в базу данных',
};

export const addCommand = async () => {
    hearManager.hear(addCommandData.name, async (context: Context) => {
        try {
            await context.send({
                message: 'Выберите, какой день вам нужен:',
                keyboard: addKeyboard,
            });
        }
        catch (err) {
            return context.send(throwException(err));
        }
    })
}

addCommand();
