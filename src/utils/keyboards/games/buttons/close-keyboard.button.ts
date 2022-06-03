import { Context } from 'vk-io';
import { hearManager } from '../../../../config';
import { throwException } from '../../../exception.util';
import { CommandsEnum } from '../commands';

const closeKeyboardButton = async () => {
    hearManager.hear({ 'messagePayload.command': CommandsEnum.CLOSE_KEYBOARD }, async (context: Context) => {        
        try {
            await context.send('Хорошо, я выключу клавиатуру!');
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

closeKeyboardButton();
