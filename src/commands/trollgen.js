const BOT = require("../vk");
const request = require("request");
const troll = require("./troll")

const gm = require("gm").subClass({
	imageMagick: true,
});
const fs = require("fs");
let text = [];
let imagekek = [];

const trollCommand = BOT.MESSAGES.hear("/trollgen", async (context) => {
	if (context.hasReplyMessage) {
		text.length = 0;
		imagekek.length = 0;
		await context.send("Trollgen одобрен, ща буит ржяка");

		text = troll.rewriteMessage.apply(null, [context.replyMessage.text])

		imagekek = await BOT.API.users.get({
			user_ids: context.replyMessage.senderId,
			fields: "photo_200",
			name_case: "nom",
		});

		console.log(imagekek[0]);

		let download = function (uri, filename, callback) {
			request.head(uri, function (err, res, body) {
				console.log("content-type:", res.headers["content-type"]);
				console.log("content-length:", res.headers["content-length"]);
				request(uri).pipe(fs.createWriteStream(filename)).on("close", callback);
			});
		};

		download(imagekek[0].photo_200, "ava.png", function () {
			console.log("done");
			gm(640, 400, "#000000")
				.fill("#FFFFFF")
				.font("HelveticaNeue.ttf")
				.fontSize(30)
				.drawText(30, 42, "Золотые слова")
				.in("-page", "+30+85")
				.in("ava.png")
				.fontSize(20)
				.drawText(260, 110, `«${text}»`)
				.fontSize(30)
				.drawText(
					30,
					370,
					`© ${imagekek[0].first_name} ${imagekek[0].last_name}`
				)
				.mosaic()
				.write("rofl.png", async function (err) {
					if (err) {
						console.log(err);
					}
					const attach = await BOT.VK.upload.messagePhoto({
						source: {
							value: "./rofl.png",
						},
					});
					await context.send({
						attachment: attach.toString(),
					});
				});
		});
		fs.unlink("rofl.png");
		fs.unlink("ava.png");
		text = null;
		imagekek = null;
	} else if (context.hasForwards) {
		text.length = 0;
		imagekek.length = 0;
		if (context.forwards.length === 1) {
			text[0] = troll.rewriteMessage.apply(null, [context.forwards[0].text])
		}
		for (let i = 0; i < context.forwards.length; i++) {
			if (context.forwards[0].senderId === context.forwards[i].senderId) {
				text[i] =  troll.rewriteMessage.apply(null, [context.forwards[i].text])
			} else {
				text.length = 0;
				await context.send(
					"Так! Ошыбка! Рофляночька должня принядлежять одному человеку, а не ьязным"
				);
				return;
			}
			imagekek[i] = await BOT.API.users.get({
				user_ids: context.forwards[i].senderId,
				fields: "photo_200, photo_200_orig",
				name_case: "nom",
			});
		}

		await context.send("Trollgen одобрен, ща буит ржяка");
		let download = function (uri, filename, callback) {
			request.head(uri, function (err, res, body) {
				console.log("content-type:", res.headers["content-type"]);
				console.log("content-length:", res.headers["content-length"]);
				request(uri).pipe(fs.createWriteStream(filename)).on("close", callback);
			});
		};

		console.log(imagekek[0][0].photo_200);
		console.log(imagekek[0][0].photo_200_orig);

		if (imagekek[0][0].photo_200 === undefined) {
			download(imagekek[0][0].photo_200_orig, "ava.png", function () {
				console.log("done");
				gm(640, 400, "#000000")
					.fill("#FFFFFF")
					.font("HelveticaNeue.ttf")
					.fontSize(30)
					.drawText(30, 42, "Золотые слова")
					.in("-page", "+30+80")
					.in("ava.png")
					.fontSize(20)
					.drawText(260, 110, `«${text.join("\n")}»`)
					.fontSize(30)
					.drawText(
						30,
						370,
						`© ${imagekek[0][0].first_name} ${imagekek[0][0].last_name}`
					)
					.mosaic()
					.write("rofl.png", async function (err) {
						if (err) {
							console.log(err);
						}
						const attach = await BOT.VK.upload.messagePhoto({
							source: {
								value: "./rofl.png",
							},
						});
						await context.send({
							attachment: attach.toString(),
						});
					});
			});
			fs.unlink("rofl.png");
			fs.unlink("ava.png");
			text = null;
			imagekek = null;
		} else {
			download(imagekek[0][0].photo_200, "ava.png", function () {
				console.log("done");
				gm(640, 400, "#000000")
					.fill("#FFFFFF")
					.font("HelveticaNeue.ttf")
					.fontSize(30)
					.drawText(30, 42, "Золотые слова")
					.in("-page", "+30+85")
					.in("ava.png")
					.fontSize(20)
					.drawText(260, 110, `«${text.join("\n")}»`)
					.fontSize(30)
					.drawText(
						30,
						370,
						`© ${imagekek[0][0].first_name} ${imagekek[0][0].last_name}`
					)
					.mosaic()
					.write("rofl.png", async function (err) {
						if (err) {
							console.log(err);
						}
						const attach = await BOT.VK.upload.messagePhoto({
							source: {
								value: "./rofl.png",
							},
						});
						await context.send({
							attachment: attach.toString(),
						});
					});
			});
			fs.unlink("rofl.png");
			fs.unlink("ava.png");
			text = null;
			imagekek = null;
		}
	} else {
		await context.send("А че троллгенить то буим?");
	}
});

module.exports = {
	run() {
		trollCommand;
	},
};
