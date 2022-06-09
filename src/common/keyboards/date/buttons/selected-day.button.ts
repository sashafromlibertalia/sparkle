import { CommandsEnum } from '../../../enums';
import { SubjectService } from '../../../../db';
import { Context } from 'vk-io';
import { vk } from '../../../../config';
import { throwException } from '../../../../utils';

const subjectService = new SubjectService();

export const selectedDayButton = async (day: CommandsEnum, context: Context) => {
    try {
        const result = await subjectService.selectMany();
        if (!result.length)
            await vk.api.messages.send({
                message: 'Домашки нету!',
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
