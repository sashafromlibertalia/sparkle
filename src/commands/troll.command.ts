import { Context } from 'vk-io';
import { throwException, uglifyMessage } from '../utils';
import { hearManager } from '../config';

export const trollCommandData: ICommand = {
    name: '/тролль',
    description: 'Отправляет забавно исковерканное сообщение, на которое ответил человек',
};

export const trollCommand = async () => {
    hearManager.hear(trollCommandData.name, async (context: Context) => {
        try {
            if (context.hasReplyMessage) {
                await context.send(uglifyMessage(context.message.reply_message.text));
            } else if (context.hasForwards) {
                let text = [];
                for (let i = 0; i < context.forwards.length; i++) {
                    text[i] = uglifyMessage(context.forwards[i].text);
                }

                await context.send(`${text.join('\n')}`);
            } else {
                await context.send('А каво трйолить то буим?');
            }
        }
        catch (err: any) {
            return context.send(throwException(err));
        }
    });
};

trollCommand();
