import { Keyboard } from 'vk-io';
import { CommandsEnum } from '../commands.enum';

export const gamesKeyboard = Keyboard.builder()
    .textButton({
        label: 'Шар Вероятностей',
        payload: {
            command: CommandsEnum.BALL_GAME,
        },
        color: Keyboard.PRIMARY_COLOR,
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
