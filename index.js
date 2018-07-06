const {VK} = require('vk-io');
const {Keyboard} = require('vk-io');
const vk = new VK();
const {updates} = vk;
const {api} = vk;
const cheerio = require('cheerio')
const request = require("request")

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
	context.send('Привет! Я - Бот, созданный специально для 10-А класса 631 гимназии. К черту эту прелюдию, я могу еще долго распинаться, но вот мой список команд:\n/дз - ДОМАШКА\n/lesson - оповещает тебя, какой сейчас урок\n/уроки - получи расписание на сегодняшний день\n/game - не знаю зачем, но у меня есть игры (Я сам в шоке)')
})


hearCommand('game', async (context) => {
	await context.send({
		message: 'Вот список моих игр',
		keyboard: Keyboard.keyboard([
			[
				Keyboard.textButton({
				label: 'Шар Вероятностей',
				payload: {
					command: 'ball'
				},
				color: Keyboard.POSITIVE_COLOR
			}),
                Keyboard.textButton({
				label: 'Что-то еще...',
				payload: {
					command: 'asdasd'
				},
				color: Keyboard.POSITIVE_COLOR
			})
			],
                Keyboard.textButton({
				label: 'Закрыть клавиатуру',
				payload: {
					command: 'cancel'
				},
				color: Keyboard.NEGATIVE_COLOR
			})
		],
		{
			oneTime: true
		})
	});
})

hearCommand('ball', async(context) => {
	await context.send('Как играть в эту игру? Очень просто! Ты пишешь "шанс" и свое утверждение, а я отвечаю вероятностью.\nПример:\n-шанс, что Мы - дружный класс\n-Вероятность - 100%') 
})


hearCommand('cancel', async(context) => {
	await context.send('Хорошо, я выключу клавиатуру!')
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
Schedule[0][0] = "\n"
Schedule[0][1] = "\n"
Schedule[0][2] = "\n"
Schedule[0][2] = "\n"
Schedule[0][3] = "\n"
Schedule[0][4] = "\n"
Schedule[0][5] = "\n"
Schedule[0][6] = "\n"

Schedule[1] = new Array(7)
Schedule[1][0] = "\n"
Schedule[1][1] = "\n"
Schedule[1][2] = "\n"
Schedule[1][2] = "\n"
Schedule[1][3] = "\n"
Schedule[1][4] = "\n"
Schedule[1][5] = "\n"
Schedule[1][6] = "\n"

Schedule[2] = new Array(7)
Schedule[2][0] = "\n"
Schedule[2][1] = "\n"
Schedule[2][2] = "\n"
Schedule[2][2] = "\n"
Schedule[2][3] = "\n"
Schedule[2][4] = "\n"
Schedule[2][5] = "\n"
Schedule[2][6] = "\n"


Schedule[3] = new Array(7)
Schedule[3][0] = "\n"
Schedule[3][1] = "\n"
Schedule[3][2] = "\n"
Schedule[3][2] = "\n"
Schedule[3][3] = "\n"
Schedule[3][4] = "\n"
Schedule[3][5] = "\n"
Schedule[3][6] = "\n"

Schedule[4] = new Array(7)
Schedule[4][0] = "\n"
Schedule[4][1] = "\n"
Schedule[4][2] = "\n"
Schedule[4][2] = "\n"
Schedule[4][3] = "\n"
Schedule[4][4] = "\n"
Schedule[4][5] = "\n"
Schedule[4][6] = "\n"

Schedule[5] = new Array(7)
Schedule[5][0] = "\n"
Schedule[5][1] = "\n"
Schedule[5][2] = "\n"
Schedule[5][2] = "\n"
Schedule[5][3] = "\n"
Schedule[5][4] = "\n"
Schedule[5][5] = "\n"
Schedule[5][6] = "\n"


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
	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[0][6])
		}
		break
	}





	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][6])
		}
		break
	}

	



	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][6])
		}
		break
	}






	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][6])
		}
		break
	}





	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 1 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][6])
		}
		break
	}


//Первый урок
for(i = 30; i < 59; i++)
{
	if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][0])
	}
	break
}
for(i = 0; i < 10; i++)
{
	if(Time.getHours() === 8 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][0])
	}
	break
}


//Второй урок
for(i = 20; i < 59; i++)
{
	if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][1])
	}
	break
}
while(i = 0)
{
	if(Time.getHours() === 9 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][1])
	}
	break
}


//Третий урок
for(i = 15; i < 55; i++)
{
	if(Time.getHours() === 10 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][2])
	}
	break
}


//Четвертый урок
for(i = 15; i < 55; i++)
{
	if(Time.getHours() === 11 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][3])
	}
	break
}


//Пятый урок
for(i = 10; i < 50; i++)
{
	if(Time.getHours() === 12 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][4])
	}
	break
}


//Шестой урок
for(i = 10; i < 50; i++)
{
	if(Time.getHours() === 13 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][5])
	}
	break
}


//Седьмой урок
for(i = 10; i < 50; i++)
{
	if(Time.getHours() === 14 & Time.getDay() === 1 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][6])
	}
	break
}

})
		



updates.hear('/уроки', async(context) => {
	if(Time.getDay() === 1)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[0])
	}
	if(Time.getDay() === 2)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[1])
	}
	if(Time.getDay() === 3)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[2])
	}
	if(Time.getDay() === 4)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[3])
	}
	if(Time.getDay() === 5)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[4])
	}
	if(Time.getDay() === 6)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[5])
	}
})



//Парсинг тут
var url = 'https://github.com/FloydReme/testforme/blob/master/domashka.txt'
request(url, function(error, response, body) {
var $ = cheerio.load(body)
var Engligh = $('#LC1').html()
console.log(Engligh)
})
	


updates.hear('/дз', async(context) => {
	


	if(Time.getDay() === 1)
	{

	}

	if(Time.getDay() === 2)
	{
		
	}

	if(Time.getDay() === 3)
	{
		
	}

	if(Time.getDay() === 4)
	{
		
	}

	if(Time.getDay() === 5)
	{

	}

	if(Time.getDay() === 6)
	{
		
	}
})