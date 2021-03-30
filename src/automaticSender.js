const moment = require("moment");
const DATA = require("./links");
const BOT = require("./vk");

function sendDayMessage() {
	switch (moment().day()) {
		case 1:
			setInterval(() => {
				if (
					moment().hour() == 9 &&
					moment().minute() == 58 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Programming.lecture.text} ${DATA.Programming.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				if (
					moment().hour() == 11 &&
					moment().minute() == 38 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Programming.practice.text} ${DATA.Programming.practice.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
			}, 1000);
			break;
		case 2:
			setInterval(() => {
				if (
					moment().hour() == 9 &&
					moment().minute() == 58 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.English.text}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				if (
					moment().hour() == 11 &&
					moment().minute() == 38 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Math.lecture.text} ${DATA.Math.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				if (
					moment().hour() == 13 &&
					moment().minute() == 28 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.DiscreteMath.practice.text} ${DATA.DiscreteMath.practice.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
			}, 1000);
		case 3:
			// Надо четность проверять
			setInterval(() => {
				if (
					moment().hour() == 9 &&
					moment().minute() == 58 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Math.lecture.text} ${DATA.Math.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
			}, 1000);
			break;
		case 4:
			setInterval(() => {
				if (
					moment().hour() == 9 &&
					moment().minute() == 58 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Physics.lecture.text} ${DATA.Physics.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}

				if (
					moment().hour() == 11 &&
					moment().minute() == 38 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Physics.practice.text} ${DATA.Physics.practice.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
			}, 1000);
			break;
		case 5:
			setInterval(() => {
				if (
					moment().hour() == 9 &&
					moment().minute() == 58 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.English.text}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}

				if (
					moment().hour() == 11 &&
					moment().minute() == 38 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.LinearAlgebra.lecture.text} ${DATA.LinearAlgebra.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}

                if (
					moment().hour() == 13 &&
					moment().minute() == 28 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.LinearAlgebra.lecture.text} ${DATA.LinearAlgebra.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
			}, 1000);
			break;
        case 6:
            setInterval(() => {
                if (
					moment().hour() == 13 &&
					moment().minute() == 28 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Algorithms.practice.text} ${DATA.Algorithms.practice.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
			}, 1000);
			break;
	}
}

module.exports = {
	run() {
		sendDayMessage();
	},
};
