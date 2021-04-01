const moment = require("moment");
const DATA = require("./links");
const BOT = require("./vk");

function sendDayMessage() {
	setInterval(() => {
		switch (moment().day()) {
			case 1:
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
				break;
			case 2:
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
			case 3:
				// Надо четность проверять
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
				break;
			case 4:
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
				} else if (
					moment().hour() == 11 &&
					moment().minute() == 38 &&
					moment().second() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Physics.practice.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM()
					}).catch((err) => {
						console.log(err)
					})
				}else if (
					moment().hour() == 14 &&
					moment().minute() == 23
				) {
					console.log("жопа")
					BOT.API.messages.send({
						message: `не обращайте внимание`,
						chat_id: 14,
						random_id: BOT.RANDOM()
					}).catch((err) => {
						console.log(err)
					})
				}
				break;
			case 5:
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
				break;
			case 6:
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
				break;
		}
	}, 1000)
}

module.exports = {
	run() {
		sendDayMessage();
	},
};
