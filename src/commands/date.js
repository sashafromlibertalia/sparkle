const BOT = require("../vk");

const hearCommand = (name, conditions, handle) => {
	if (typeof handle !== "function") {
		handle = conditions;
		conditions = [`/${name}`];
	}

	if (!Array.isArray(conditions)) {
		conditions = [conditions];
	}

	BOT.MESSAGES.hear(
		[(text, {state}) => state.command === name, ...conditions],
		handle
	);
};

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


const asks = new Array(2);
asks[0] = new RegExp(/задано/i);
asks[1] = new RegExp(/задали/i);

const asksCommand = BOT.MESSAGES.hear(asks, async (context) => {
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




module.exports = {
	run: function () {
		dateCommand
        asksCommand
	},
};
