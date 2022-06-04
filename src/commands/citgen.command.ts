import { hearManager } from '../config';
import { Context } from 'vk-io';
import { generateCitgenImage, throwException } from '../utils';
import { ICommand, IUser } from '../types/global';
import { fetchUserPhoto } from '../api';

export const citgenCommandData: ICommand = {
    name: '/citgen',
    description: 'Отправляет картинку-цитату с сообщением',
};

export const citgenCommand = async () => {
    const MaximumSymbolsPerLine = 26;

    hearManager.hear(citgenCommandData.name, async (context: Context) => {
        try {
            if (!context.hasForwards && !context.hasReplyMessage)
                return await context.send('А че цитгенить то будем?');

            if (context.replyMessage?.isGroup || context.forwards?.filter((msg: any) => msg.isGroup).length)
                return context.send('Цитген с ботами не работает 😔');

            await context.send('Citgen одобрен, ща будет ржака');
            switch (true) {
                case context.hasReplyMessage:
                    {
                        const text = context.replyMessage.text
                            .match(new RegExp(`.{1,${MaximumSymbolsPerLine}}`, 'g'))
                            .join('\n');

                        const [pic] = await fetchUserPhoto(context.replyMessage.senderId);
                        const { first_name, last_name, photo_200 } = pic;
                        const user: IUser = {
                            first_name,
                            last_name,
                        }

                        await generateCitgenImage(context, photo_200, text, user);
                    }
                    break;
                case context.hasForwards:
                    {
                        let text: string[] = [];

                        for (const user of context.forwards) {
                            if (context.forwards[0].senderId !== user.senderId) {
                                await context.send('Так! Ошибка! Рофляночка должна принадлежать одному человеку, а не разным');
                                return;
                            }

                            text.push(user.text
                                .match(new RegExp(`.{1,${MaximumSymbolsPerLine}}`, 'g'))
                                .join('\n'));
                        }

                        const [pic] = await fetchUserPhoto(context.forwards[0].senderId);
                        const { first_name, last_name, photo_200 } = pic;
                        const user: IUser = {
                            first_name,
                            last_name,
                        }

                        await generateCitgenImage(context, photo_200, text.join('\n'), user);
                    }
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
