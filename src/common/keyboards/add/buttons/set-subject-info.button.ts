import { SubjectsEnum } from '../../../enums';
import { Context } from 'vk-io';
import { vk } from '../../../../config';
import { throwException } from '../../../../utils';

export const setSubjectInfoButton = async (subject: SubjectsEnum, context: Context) => {
    try {
        await vk.api.messages.send({
            message: 'Напишите, что нужно сделать, и я сохраню!',
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
