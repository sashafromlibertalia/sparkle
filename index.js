const {VK} = require('vk-io');
const fs = require('fs')
const vk = new VK();
const {updates} = vk;
const {Keyboard} = require('vk-io')

//Не трогать
const TOKEN = "e74e42966fb9a1e8ab1354ab4721881369665a16367e044c005920b5220827e17ca9894b56412ea2e2891"

vk.setOptions({
    token: TOKEN
})

//Святыня
require('https').createServer().listen(process.env.PORT || 5000).on('request', function(req, res){
    res.end('')
});


updates.startPolling()

updates.use(async (context, next) => {
	if (context.is('message') && context.isOutbox()) {
		return;
	}

	try {
		await next();
	} catch (error) {
		console.error('Error:', error);
	}
});

updates.hear('/start', async(context) => {
	context.send('Привет! Я - Бот, созданный специально для 10-А класса 631 гимназии. К черту эту прелюдию, я могу еще долго распинаться, но вот мой список команд:\n/lesson - оповещает тебя, какой сейчас урок\n/game - не знаю зачем, но у меня есть игры (Я сам в шоке)')
})




updates.use(async (context, next) => {
	if (context.is('message')) {
		const payload = context.getMessagePayload();

		context.state.command = payload && payload.command
			? payload.command
			: null;
	}

	await next();
});


const hearCommand = (name, conditions, handle) => {
	if (typeof handle !== 'function') {
		handle = conditions;
		conditions = [`/${name}`];
	}

	if (!Array.isArray(conditions)) {
		conditions = [conditions];
	}

	updates.hear(
		[
			(text, { state }) => (
				state.command === name
			),
			...conditions
		],
		handle
	);
};


/*hearCommand('game', async (context, next) => {
	context.state.command = 'game';

	await next();
});



hearCommand('/game', async(context) => {
	await context.send({
		message: 'Итак...',
		keyboard: keyboard.keyboard([
			Keyboard.textButton({
				label:'Шар Вероятностей',
				payload: {
					command:'ball'
				},
				color: Keyboard.PRIMARY_COLOR
			})
		])
	})
})*/


hearCommand('game', async (context) => {
	await context.send({
		message: 'Итак...',
		keyboard: Keyboard.keyboard([
			Keyboard.textButton({
				label: 'Шар Вероятностей',
				payload: {
					command: 'ball'
				},
				color: Keyboard.PRIMARY_COLOR
			}),
			Keyboard.onTime()
		])
	});
});