import { Context } from 'vk-io';
import { hearManager } from '../../../../config';
import { throwException } from '../../../../utils/exception.util';
import { CommandsEnum } from '../commands';

const ballGameKeyboardButton = async () => {
    hearManager.hear({ 'messagePayload.command': CommandsEnum.BALL_GAME }, async (context: Context) => {
        try {
            await context.send('Ты пишешь "шанc" и свое утверждение, а я отвечаю вероятностью:\n\n— Шанc, что мне повезет.\n— Вероятность - 100%');

            hearManager.hear(/шанс/i, async (context: Context) => {
                const chances = [
                    'Вероятность близка к нулю :(',
                    'Я считаю, что 50 на 50',
                    'Вероятность - 100%',
                    'Я полагаю, что вероятность близка к 100%',
                    'Маловероятно, но шанс есть',
                    'Вероятность нулевая, ничего не поделать',
                ];

                await context.send(chances[Math.floor(Math.random() * chances.length)]);
            });
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

ballGameKeyboardButton();
