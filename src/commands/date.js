const BOT = require("../vk");

const Intl = require('intl')
const moment = require('moment')
const Time = new Date()
const formatter = new Intl.DateTimeFormat('ru', {
  month: 'long',
  day: 'numeric'
})

moment().format()

const weekKeyboard = BOT.KEYBOARD.keyboard([
    [
        BOT.KEYBOARD.textButton({
            label: "Понедельник",
            payload: {
                command: "monday",
            },
            color: BOT.KEYBOARD.POSITIVE_COLOR,
        }),
        BOT.KEYBOARD.textButton({
            label: "Вторник",
            payload: {
                command: "tuesday",
            },
            color: BOT.KEYBOARD.POSITIVE_COLOR,
        }),
        BOT.KEYBOARD.textButton({
            label: "Среда",
            payload: {
                command: "wednesday",
            },
            color: BOT.KEYBOARD.POSITIVE_COLOR,
        }),
    ],
    [
        BOT.KEYBOARD.textButton({
            label: "Четверг",
            payload: {
                command: "thursday",
            },
            color: BOT.KEYBOARD.POSITIVE_COLOR,
        }),
        BOT.KEYBOARD.textButton({
            label: "Пятница",
            payload: {
                command: "friday",
            },
            color: BOT.KEYBOARD.POSITIVE_COLOR,
        }),
        BOT.KEYBOARD.textButton({
            label: "Суббота",
            payload: {
                command: "saturday",
            },
            color: BOT.KEYBOARD.POSITIVE_COLOR,
        }),
    ],
    BOT.KEYBOARD.textButton({
        label: "Закрыть клавиатуру",
        payload: {
            command: "cancel",
        },
        color: BOT.KEYBOARD.NEGATIVE_COLOR,
    }),
]).oneTime();


function monday() {

}

function tuesday() {

}

function wednesday() {

}

function thursday() {

}

function friday() {

}

function saturday() {

}

function setHomeworkNext() {
   
}

const asksCommand = BOT.MESSAGES.hear([/задали/i, /задано/i], async (context) => {
	await context.send({
		message:
			"Я тут увидел, что кто-то из вас спрашивает ДЗ. Выберите, какой день вам нужен:",
		keyboard: weekKeyboard,
	});
});

const dateCommand = BOT.MESSAGES.hear("/дата", async (context) => {
	await context.send({
		message: "Выберите, какой день вам нужен:",
		keyboard: weekKeyboard
	});
});

const tomorrowCommand = BOT.MESSAGES.hear("/дз завтра", async (context) => {
    for (i = 0; i < 7; i++) {
        if (moment().day() === i) {
          await context.send('Домашка на завтра. Сегодня ' + formatter.format(Time) + ' \n' + Days[i].join('\n'))
        };
    };
});



module.exports = {
	run() {
		dateCommand
        asksCommand
        tomorrowCommand
	},
};
