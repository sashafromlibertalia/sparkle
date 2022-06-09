import { vk } from '../config';
import { Context } from 'vk-io';
import { ballGameKeyboardButton, closeKeyboardButton } from '../common/keyboards/games/buttons';
import { selectedDayButton } from '../common/keyboards/date/buttons';
import { addSubjectButton } from '../common/keyboards/add/buttons';
import { CommandsEnum, SubjectsEnum } from '../common/enums';
import { setSubjectInfoButton } from '../common/keyboards/add/buttons/set-subject-info.button';

vk.updates.on('message_new', async (context: Context, next) => {
    switch (context.messagePayload?.command) {
        case CommandsEnum.BALL_GAME:
            await ballGameKeyboardButton(context);
            break;
        case CommandsEnum.GET_MONDAY:
        case CommandsEnum.GET_TUESDAY:
        case CommandsEnum.GET_WEDNESDAY:
        case CommandsEnum.GET_THURSDAY:
        case CommandsEnum.GET_FRIDAY:
        case CommandsEnum.GET_SATURDAY:
            await selectedDayButton(context.messagePayload?.command, context);
            break;
        case CommandsEnum.SET_MONDAY:
        case CommandsEnum.SET_TUESDAY:
        case CommandsEnum.SET_WEDNESDAY:
        case CommandsEnum.SET_THURSDAY:
        case CommandsEnum.SET_FRIDAY:
        case CommandsEnum.SET_SATURDAY:
            await addSubjectButton(context.messagePayload?.command, context);
            break;
        case SubjectsEnum.MATHS:
        case SubjectsEnum.PHYSICS:
        case SubjectsEnum.PROGRAMMING:
        case SubjectsEnum.FOREIGN_LANGUAGE:
        case SubjectsEnum.OS:
        case SubjectsEnum.DATABASE:
            await setSubjectInfoButton(context.messagePayload?.command, context);
            break
        case CommandsEnum.CLOSE_KEYBOARD:
            await closeKeyboardButton(context);
            break;
        default:
            break;
    }

    return next();
});
