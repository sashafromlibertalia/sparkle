let {VK} = require('vk-io'),
	vk = new VK(),
	config = require('./config'),
    {updates} = vk,
	{api} = vk;

vk.setOptions({
	token: config.TOKEN,
	pollingGroupId: config.poullingGroupID,
	peer_id: config.peerID
});

let gulag = updates.hear(/^\/вгулаг (.+)/i, async(context) => {
	let victim = context.$match[1]
	if(context.senderId === config.adminID) {
		if(isNaN(victim)) {
			let [user] = await api.users.get({
				user_ids: victim,
				name_case: 'nom'
			});
			await context.send('ГУЛАГ тебя ждет, братишка');
			await context.kickUser(user.id);
		}
		else {
			await context.send('ГУЛАГ тебя ждет, братишка');
			await context.kickUser(victim);
		}
	}
	else {
		await context.send(`Упс, ошибочка. У вас нет доступа к этой команде`);
	}
});

module.exports = gulag;