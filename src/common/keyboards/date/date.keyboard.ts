import { Keyboard } from 'vk-io';
import { CommandsEnum } from '../../enums/commands.enum';

export const dateKeyboard = Keyboard.builder()
    .textButton({
        label: 'Понедельник',
        payload: {
            command: CommandsEnum.GET_MONDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Вторник',
        payload: {
            command: CommandsEnum.GET_TUESDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Среда',
        payload: {
            command: CommandsEnum.GET_WEDNESDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .row()
    .textButton({
        label: 'Четверг',
        payload: {
            command: CommandsEnum.GET_THURSDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Пятница',
        payload: {
            command: CommandsEnum.GET_FRIDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Суббота',
        payload: {
            command: CommandsEnum.GET_SATURDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .row()
    .textButton({
        label: 'Закрыть клавиатуру',
        payload: {
            command: CommandsEnum.CLOSE_KEYBOARD,
        },
        color: Keyboard.NEGATIVE_COLOR,
    })
    .oneTime();
