const {VK} = require('vk-io');
const fs = require('fs')
const vk = new VK();
const {updates} = vk;
const {api} = vk;
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

//Святыня 2
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




//Команды
updates.hear('/start', async(context) => {
	context.send('Привет! Я - Бот, созданный специально для 10-А класса 631 гимназии. К черту эту прелюдию, я могу еще долго распинаться, но вот мой список команд:\n/homework - ДОМАШКА\n/lesson - оповещает тебя, какой сейчас урок\n/game - не знаю зачем, но у меня есть игры (Я сам в шоке)')
})

hearCommand('game', async (context) => {
	await context.send({
		message: 'Вот список моих игр',
		keyboard: Keyboard.keyboard([
			Keyboard.textButton({
				label: 'Шар Вероятностей',
				payload: {
					command: 'ball'
				},
				color: Keyboard.POSITIVE_COLOR
			}),
		])
	});
});


hearCommand('ball', async(context) => {
	await context.send('Как играть в эту игру? Очень просто! Ты пишешь "шанс" и свое утверждение, а я отвечаю вероятностью.\nПример:\n-шанс, что Мы - дружный класс\n-Вероятность - 100%')
})

updates.hear(/шанс/i, async(context) => {
	var chances = new Array(6)
  chances[0] = "Вероятность близка к нулю :("
  chances[1] = "Я считаю, что 50 на 50"
  chances[2] = "Вероятность - 100%"
  chances[3] = "Я полагаю, что вероятность близка к 100%"
  chances[4] = "Маловероятно, но шанс есть" 
  chances[5] = "Вероятность нулевая, ничего не поделать"
  var m = chances[Math.floor(Math.random() * chances.length)]
	await context.send(m)
})


updates.setHearFallbackHandler(async (context, next) => {
	await context.send('Такой команды нет. Хочешь, чтобы она появилась - пиши Саше :(');
});

const Time = new Date()
var Schedule = new Array(6)
Schedule[0] = new Array(7)
Schedule[0][0] = ""
Schedule[0][1] = ""
Schedule[0][2] = ""
Schedule[0][2] = ""
Schedule[0][3] = ""
Schedule[0][4] = ""
Schedule[0][5] = ""
Schedule[0][6] = ""

Schedule[1] = new Array(7)
Schedule[1][0] = ""
Schedule[1][1] = ""
Schedule[1][2] = ""
Schedule[1][2] = ""
Schedule[1][3] = ""
Schedule[1][4] = ""
Schedule[1][5] = ""
Schedule[1][6] = ""

Schedule[2] = new Array(7)
Schedule[2][0] = ""
Schedule[2][1] = ""
Schedule[2][2] = ""
Schedule[2][2] = ""
Schedule[2][3] = ""
Schedule[2][4] = ""
Schedule[2][5] = ""
Schedule[2][6] = ""


Schedule[3] = new Array(7)
Schedule[3][0] = ""
Schedule[3][1] = ""
Schedule[3][2] = ""
Schedule[3][2] = ""
Schedule[3][3] = ""
Schedule[3][4] = ""
Schedule[3][5] = ""
Schedule[3][6] = ""

Schedule[4] = new Array(7)
Schedule[4][0] = ""
Schedule[4][1] = ""
Schedule[4][2] = ""
Schedule[4][2] = ""
Schedule[4][3] = ""
Schedule[4][4] = ""
Schedule[4][5] = ""
Schedule[4][6] = ""

Schedule[5] = new Array(7)
Schedule[5][0] = ""
Schedule[5][1] = ""
Schedule[5][2] = ""
Schedule[5][2] = ""
Schedule[5][3] = ""
Schedule[5][4] = ""
Schedule[5][5] = ""
Schedule[5][6] = ""


const newDay = new Date()

var greeting = new Array(4)
greeting[0] = "Итак, мои дорогие, начался новый учебный день. Я желаю вам всем хороших оценок по всем предметам, удачи :)\n Расписание на сегодня:\n"
greeting[1] = "И снова всем приветик, господа. Скучали? Я знаю, что нет. Вот вам расписание на сегодня: \n"
greeting[2] = "Шалом, дамы и пацаны. Возможно, мои ежедневные напоминая о расписании вам надоели, но я ничего поделать не могу - я создан для выполнения конкретных задач. Кстати, вот сегодняшнее расписание: \n"
greeting[3] = "Привет. Без лишних слов. Расписание на сегодня:\n"
var random_greeting = greeting[Math.floor(Math.random() * greeting.length)]

var timer = setInterval(function() {
	if(newDay.getHours() === 6 ){
		api.messages.send({
			message: random_greeting,
			domain: 'egoromanov'
		}),
		console.log(random_greeting)
	}
}, 86400000)

updates.hear('/lesson', async(context) => {
	for(var i = 30; i < 60; i++)
	{
		if(Time.getHours() === 1 & Time.getDay() === 5 || Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит английский')
		}
	}
})
		
	
	
	
	