import { Context } from 'vk-io';
import { throwException } from '../../../../utils';
import { vk } from '../../../../config';

export const closeKeyboardButton = async (context: Context) => {
    try {
        await vk.api.messages.send({
            message: 'Хорошо, я выключу клавиатуру!',
            peer_id: context.peerId,
            group_id: context.$groupId,
            random_id: Date.now(),
        });
    } catch (err: any) {
        await vk.api.messages.send({
            message: throwException(err),
            peer_id: context.peerId,
            group_id: context.$groupId,
            random_id: Date.now(),
        });
    }
};
