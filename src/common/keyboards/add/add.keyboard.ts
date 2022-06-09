import { Keyboard } from 'vk-io';
import { CommandsEnum } from '../../enums/commands.enum';

export const addKeyboard = Keyboard.builder()
    .textButton({
        label: 'Понедельник',
        payload: {
            command: CommandsEnum.SET_MONDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Вторник',
        payload: {
            command: CommandsEnum.SET_TUESDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Среда',
        payload: {
            command: CommandsEnum.SET_WEDNESDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .row()
    .textButton({
        label: 'Четверг',
        payload: {
            command: CommandsEnum.SET_THURSDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Пятница',
        payload: {
            command: CommandsEnum.SET_FRIDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Суббота',
        payload: {
            command: CommandsEnum.SET_SATURDAY,
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
