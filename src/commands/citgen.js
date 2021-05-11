const BOT = require("../vk");
const request = require("request");
const gm = require("gm").subClass({
	imageMagick: true,
});
const fs = require("fs");
let text = [],
	imagekek = [],
	FONT = "assets/HelveticaNeue.ttf"

let download = (uri, filename, callback) => {
	request.head(uri, (err, res, body) => {
		console.log("content-type:", res.headers["content-type"]);
		console.log("content-length:", res.headers["content-length"]);
		request(uri).pipe(fs.createWriteStream(filename)).on("close", callback);
	});
};

function downloadImage(imagekek, context) {
	download(imagekek.photo_200, "images/ava.png", () => {
		console.log("done");
		gm(640, 400, "#000000")
			.fill("#FFFFFF")
			.font(FONT)
			.fontSize(30)
			.drawText(30, 42, "Золотые слова")
			.in("-page", "+30+85")
			.in("images/ava.png")
			.fontSize(20)
			.drawText(260, 110, `«${text}»`)
			.fontSize(30)
			.drawText(
				30,
				370,
				`© ${imagekek.first_name} ${imagekek.last_name}`
			)
			.mosaic()
			.write("images/rofl.png", async (err) => {
				if (err) console.error(err);
				
				const attach = await BOT.VK.upload.messagePhoto({
					source: {
						value: "./images/rofl.png",
					},
				});
				await context.send({
					attachment: attach.toString(),
				});
			});
	});
}

const citgenCommand = BOT.MESSAGES.hear("/citgen", async (context) => {
	if (context.hasReplyMessage) {
		text.length = 0;
		imagekek.length = 0;
		await context.send("Citgen одобрен, ща будет ржака");

		text = context.replyMessage.text;

		imagekek = await BOT.API.users.get({
			user_ids: context.replyMessage.senderId,
			fields: "photo_200",
			name_case: "nom",
		});

		console.log(imagekek[0]);
		downloadImage(imagekek[0], context)

		fs.unlink("images/rofl.png");
		fs.unlink("images/ava.png");
		text = null;
		imagekek = null;
	} else if (context.hasForwards) {
		text.length = 0;
		imagekek.length = 0;
		if (context.forwards.length === 1) {
			text[0] = context.forwards[0].text;
		}
		for (let i = 0; i < context.forwards.length; i++) {
			if (context.forwards[0].senderId === context.forwards[i].senderId) {
				text[i] = context.forwards[i].text;
			} else {
				text = "";
				await context.send(
					"Так! Ошибка! Рофляночка должна принадлежать одному человеку, а не разным"
				);
				return;
			}
			imagekek[i] = await BOT.API.users.get({
				user_ids: context.forwards[i].senderId,
				fields: "photo_200, photo_200_orig",
				name_case: "nom",
			});
		}

		await context.send("Citgen одобрен, ща будет ржака");
		
		console.log(imagekek[0][0].photo_200);
		console.log(imagekek[0][0].photo_200_orig);

		if (imagekek[0][0].photo_200 === undefined) {
			download(imagekek[0][0].photo_200_orig, "images/ava.png", () => {
				console.log("done");
				gm(640, 400, "#000000")
					.fill("#FFFFFF")
					.font(FONT)
					.fontSize(30)
					.drawText(30, 42, "Золотые слова")
					.in("-page", "+30+80")
					.in("images/ava.png")
					.fontSize(20)
					.drawText(260, 110, `«${text.join("\n")}»`)
					.fontSize(30)
					.drawText(
						30,
						370,
						`© ${imagekek[0][0].first_name} ${imagekek[0][0].last_name}`
					)
					.mosaic()
					.write("images/rofl.png", async (err) => {
						if (err) console.error(err);
						
						const attach = await BOT.VK.upload.messagePhoto({
							source: {
								value: "./images/rofl.png",
							},
						});
						await context.send({
							attachment: attach.toString(),
						});
					});
			});
			fs.unlink("images/rofl.png");
			fs.unlink("images/ava.png");
			text = null;
			imagekek = null;
		} else {
			download(imagekek[0][0].photo_200, "images/ava.png", () => {
				console.log("done");
				gm(640, 400, "#000000")
					.fill("#FFFFFF")
					.font(FONT)
					.fontSize(30)
					.drawText(30, 42, "Золотые слова")
					.in("-page", "+30+85")
					.in("images/ava.png")
					.fontSize(20)
					.drawText(260, 110, `«${text.join("\n")}»`)
					.fontSize(30)
					.drawText(
						30,
						370,
						`© ${imagekek[0][0].first_name} ${imagekek[0][0].last_name}`
					)
					.mosaic()
					.write("images/rofl.png", async (err) => {
						if (err) console.error(err);
						
						const attach = await BOT.VK.upload.messagePhoto({
							source: {
								value: "./images/rofl.png",
							},
						});
						await context.send({
							attachment: attach.toString(),
						});
					});
			});
			fs.unlink("images/rofl.png");
			fs.unlink("images/ava.png");
			text = null;
			imagekek = null;
		}
	} else {
		await context.send("А че цитгенить то будем?");
	}
});

module.exports = {
	run() {
		citgenCommand;
	},
};
