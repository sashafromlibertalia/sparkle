import { hearManager } from '../config';
import { Context } from 'vk-io';
import { throwException } from '../utils';
import { ICommand } from '../types/global';
import { downloadImage, fetchUserPhoto } from '../api';

export const citgenCommandData: ICommand = {
    name: '/citgen',
    description: 'Отправляет картинку-цитату с сообщением',
};

export const citgenCommand = async () => {
    hearManager.hear(citgenCommandData.name, async (context: Context) => {
        try {
            if (!context.hasForwards && !context.hasReplyMessage)
                return await context.send('А че цитгенить то будем?');

            if (context.replyMessage.isGroup || context.forwards.filter((msg: any) => msg.isGroup).length)
                return context.send('Цитген с ботами не работает 😔');

            switch (true) {
                case context.hasReplyMessage:
                    await context.send('Citgen одобрен, ща будет ржака');
                    const text = context.replyMessage.text;

                    const [pic] = await fetchUserPhoto(context.replyMessage.senderId);
                    const { first_name, last_name, photo_200 } = pic;
                    await downloadImage(photo_200);
                    break;
                default:
                    break;
            }
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

citgenCommand();
