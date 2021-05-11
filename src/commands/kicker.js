const BOT = require("../vk");
const kickerCommand = BOT.MESSAGES.hear(/^\/вгулаг (.+)/i, async (context) => {
	const victim = context.$match[1];
	if (context.senderId === BOT.CONFIG.ADMIN_ID) {
		if (isNaN(victim)) {
			const [user] = await BOT.API.users.get({
				user_ids: victim,
				name_case: "nom",
			})
			console.log(context)
			await context.send("ГУЛАГ тебя ждет, братишка");
			await BOT.API.messages.removeChatUser({
				chat_id: context.chatId,
				user_id: user.id,
			})
		} else {
			await context.send("ГУЛАГ тебя ждет, братишка")
			await BOT.API.messages.removeChatUser({
				chat_id: context.chatId,
				user_id: victim,
			})
		}
	} else {
		await context.send("Упс, ошибочка. У вас нет доступа к этой команде");
	}
});

module.exports = {
	run() {
		kickerCommand;
	},
};
