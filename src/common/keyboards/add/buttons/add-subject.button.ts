import { Context } from 'vk-io';
import { vk } from '../../../../config';
import { throwException } from '../../../../utils';
import { subjectKeyboard } from '../subject.keyboard';
import { CommandsEnum } from '../../../enums';

export const addSubjectButton = async (day: CommandsEnum, context: Context) => {
    try {
        await vk.api.messages.send({
            message: 'Выберите предмет, куда нужно добавить домашку',
            keyboard: subjectKeyboard,
            peer_id: context.peerId,
            group_id: context.$groupId,
            random_id: Date.now(),
        });
    }
    catch (err) {
        await vk.api.messages.send({
            message: throwException(err),
            peer_id: context.peerId,
            group_id: context.$groupId,
            random_id: Date.now(),
        });
    }
};
