import { Context, MessageContext, VK } from 'vk-io';
import { HearManager } from '@vk-io/hear';
import { config } from './config';
import { CommandsEnum } from '../common/keyboards/games';
import { ballGameKeyboardButton, closeKeyboardButton } from '../common/keyboards/games/buttons';
const { token, pollingGroupId } = config;

const vk = new VK({
    token,
    pollingGroupId,
});

const hearManager = new HearManager<MessageContext>();

vk.updates.on('message_new', async (context: Context, next) => {
    switch (context.messagePayload?.command) {
        case CommandsEnum.BALL_GAME:
            await ballGameKeyboardButton(context);
            break;
        case CommandsEnum.CLOSE_KEYBOARD:
            await closeKeyboardButton(context);
            break;
        default:
            break;
    }

    return next();
});

vk.updates.on('message_new', hearManager.middleware);

export { vk, hearManager };
