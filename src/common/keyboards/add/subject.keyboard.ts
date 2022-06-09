import { Keyboard } from 'vk-io';
import { CommandsEnum, SubjectsEnum } from '../../enums';

export const subjectKeyboard = Keyboard.builder()
    .textButton({
        label: 'Высшая математика',
        payload: {
            command: SubjectsEnum.MATHS,
        },
        color: Keyboard.PRIMARY_COLOR,
    })
    .textButton({
        label: 'Физика',
        payload: {
            command: SubjectsEnum.PHYSICS,
        },
        color: Keyboard.PRIMARY_COLOR,
    })
    .textButton({
        label: 'Программирование',
        payload: {
            command: SubjectsEnum.PROGRAMMING,
        },
        color: Keyboard.PRIMARY_COLOR,
    })
    .row()
    .textButton({
        label: 'Иностранный язык',
        payload: {
            command: SubjectsEnum.FOREIGN_LANGUAGE,
        },
        color: Keyboard.PRIMARY_COLOR,
    })
    .textButton({
        label: 'Операционные системы',
        payload: {
            command: SubjectsEnum.OS,
        },
        color: Keyboard.PRIMARY_COLOR,
    })
    .textButton({
        label: 'Базы данных',
        payload: {
            command: SubjectsEnum.DATABASE,
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
