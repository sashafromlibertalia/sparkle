import { Context } from 'vk-io';
import { hearManager, vk } from '../../../../config';
import { throwException } from '../../../../utils';

export const ballGameKeyboardButton = async (context: Context) => {
    try {
        await vk.api.messages.send({
            message: 'Ты пишешь "шанc" и свое утверждение, а я отвечаю вероятностью:\n\n— Шанc, что мне повезет.\n— Вероятность - 100%',
            peer_id: context.peerId,
            group_id: context.$groupId,
            random_id: Date.now(),
        });

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
        await vk.api.messages.send({
            message: throwException(err),
            peer_id: context.peerId,
            group_id: context.$groupId,
            random_id: Date.now(),
        });
    }
};
