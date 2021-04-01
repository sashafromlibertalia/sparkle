const DATA = require("./links");
const BOT = require("./vk");

function sendDayMessage(context) {
	let moment = new Date();
	setInterval(() => {
		switch (moment.getDay()) {
			case 1:
				if (
					moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Programming.lecture.text} ${DATA.Programming.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				if (
					moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
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
					moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.English.text}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				if (
					moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Math.lecture.text} ${DATA.Math.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				if (
					moment.getHours() == 13 &&
					moment.getMinutes() == 28 &&
					moment.getSeconds() == 0
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
					moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
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
					moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Physics.lecture.text} ${DATA.Physics.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				} else if (
					moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages
						.send({
							message: `${DATA.Physics.practice.text} ${DATA.Physics.practice.link}`,
							chat_id: BOT.CONFIG.CHAT_ID,
							random_id: BOT.RANDOM(),
						})
						.catch((err) => {
							console.log(err);
						});
				} else if (moment.getHours() == 18) {
					BOT.API.messages
						.getConversationsById({
							peer_ids: [context.peerId],
						})
						.then((res) => {
							if (
								res.items[0].chat_settings.owner_id == BOT.CONFIG.ADMIN_ID ||
								res.items[0].chat_settings.owner_id == 282987452
							) {
								// BOT.API.messages
								// 	.send({
								// 		message: `я тестирую бота`,
								// 		chat_id: BOT.CONFIG.CHAT_ID,
								// 		random_id: BOT.RANDOM(),
								// 	})
								// 	.catch((err) => {
								// 		console.log(err);
								// 	});
								context.send(`я тестирую бота`);
							}
						})
						.catch((err) => {
							console.log(err);
						});
				}
				break;
			case 5:
				if (
					moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.English.text}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}

				if (
					moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.LinearAlgebra.lecture.text} ${DATA.LinearAlgebra.lecture.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}

				if (
					moment.getHours() == 13 &&
					moment.getMinutes() == 28 &&
					moment.getSeconds() == 0
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
					moment.getHours() == 13 &&
					moment.getMinutes() == 28 &&
					moment.getSeconds() == 0
				) {
					BOT.API.messages.send({
						message: `${DATA.Algorithms.practice.text} ${DATA.Algorithms.practice.link}`,
						chat_id: BOT.CONFIG.CHAT_ID,
						random_id: BOT.RANDOM(),
					});
				}
				break;
		}
	}, 1000);
}

module.exports = {
	run() {
		BOT.VK.updates.on("message_new", async (context) => {
			sendDayMessage(context);
		});
	},
};
