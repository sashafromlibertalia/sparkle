import { Keyboard } from 'vk-io';
import { CommandsEnum } from '../commands.enum';

export const dateKeyboard = Keyboard.builder()
    .textButton({
        label: 'Понедельник',
        payload: {
            command: CommandsEnum.MONDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Вторник',
        payload: {
            command: CommandsEnum.TUESDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Среда',
        payload: {
            command: CommandsEnum.WEDNESDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .row()
    .textButton({
        label: 'Четверг',
        payload: {
            command: CommandsEnum.THURSDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Пятница',
        payload: {
            command: CommandsEnum.FRIDAY,
        },
        color: Keyboard.POSITIVE_COLOR,
    })
    .textButton({
        label: 'Суббота',
        payload: {
            command: CommandsEnum.SATURDAY,
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
