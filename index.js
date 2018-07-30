const {VK} = require('vk-io');
const {Keyboard} = require('vk-io');
const vk = new VK();
const {updates} = vk;
const {api} = vk;
const cheerio = require('cheerio')
const request = require('request')
const Intl = require('intl')
const google = require('google')
const webshot = require('webshot')

//Не трогать
const TOKEN = "c6bacea9fa33ad3ba684c4ac9380cb70e650133088eb97919619ee977ae59489b5d142928b837e450cd30"

vk.setOptions({
	token: TOKEN,
	pollingGroupId: 168462227,
	peer_id: 2000000001
})

//Святыня
require('https').createServer().listen(process.env.PORT || 5000).on('request', function(request, res){
	res.end('')
});




api.baseUrl = 'https://api.vk.com/method/'

updates.startPolling()

//Святыня 2
updates.use(async (context, next) => {
	if (context.is('message')) {
		const { messagePayload } = context;

		context.state.command = messagePayload && messagePayload.command
			? messagePayload.command
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
vk.updates.hear('/start', async(context) => {
	context.send({
		message: `Привет! 
Я - Бот, созданный специально для 10-А класса 631 гимназии. К черту эту прелюдию, я могу еще долго распинаться, но вот мой список команд:
/дз - ДОМАШКА
/lesson - оповещает тебя, какой сейчас урок
/уроки - получи расписание на сегодняшний день
/game - не знаю зачем, но у меня есть игры (Я сам в шоке)
/гдз - гугли гдз и я постараюсь прислать его тебе
/отзыв - напиши отзыв, и Саша его увидит. ВАЖНО: отзыв анонимен, честное слово
/help - моя документация`})
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
					command: 'else'
				},
				color: Keyboard.POSITIVE_COLOR
			})],
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
	await context.send('Как играть в эту игру? Очень просто! Ты пишешь "шанc" и свое утверждение, а я отвечаю вероятностью.\nПример:\n- шанc, что Мы - дружный класс\n- Вероятность - 100%') 
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
})

hearCommand('else', async(context) => {
	await context.send('Раз эта кнопка у вас все еще есть, значит я страдаю от острой игровой недостаточности. Если у вас есть идеи, которые может реализовать этот бот в игровой форме - пишите Саше, он сможет :)')
})

hearCommand('cancel', async(context) => {
	await context.send('Хорошо, я выключу клавиатуру!')
})

	



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

if(newDay.getHours() === 10) 
{
	api.messages.send({
		message: random_greeting,
		peer_id: 2000000001
	})
}
	


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
		if(Time.getHours() === 8 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 2 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[1][6])
		}
		break
	}

	



	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 3 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[2][6])
		}
		break
	}






	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 4 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[3][6])
		}
		break
	}





	//Первый урок
	for(i = 30; i < 59; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][0])
		}
		break
	}
	for(i = 0; i < 10; i++)
	{
		if(Time.getHours() === 8 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][0])
		}
		break
	}


	//Второй урок
	for(i = 20; i < 59; i++)
	{
		if(Time.getHours() === 9 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][1])
		}
		break
	}
	while(i = 0)
	{
		if(Time.getHours() === 9 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][1])
		}
		break
	}


	//Третий урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 10 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][2])
		}
		break
	}


	//Четвертый урок
	for(i = 15; i < 55; i++)
	{
		if(Time.getHours() === 11 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][3])
		}
		break
	}


	//Пятый урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 12 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][4])
		}
		break
	}


	//Шестой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 13 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][5])
		}
		break
	}


	//Седьмой урок
	for(i = 10; i < 50; i++)
	{
		if(Time.getHours() === 14 & Time.getDay() === 5 & Time.getMinutes(i)) 
		{
			context.send('В данный момент проходит ' + Schedule[4][6])
		}
		break
	}


//Первый урок
for(i = 30; i < 59; i++)
{
	if(Time.getHours() === 8 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][0])
	}
	break
}
for(i = 0; i < 10; i++)
{
	if(Time.getHours() === 8 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][0])
	}
	break
}


//Второй урок
for(i = 20; i < 59; i++)
{
	if(Time.getHours() === 9 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][1])
	}
	break
}
while(i = 0)
{
	if(Time.getHours() === 9 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][1])
	}
	break
}


//Третий урок
for(i = 15; i < 55; i++)
{
	if(Time.getHours() === 10 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][2])
	}
	break
}


//Четвертый урок
for(i = 15; i < 55; i++)
{
	if(Time.getHours() === 11 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][3])
	}
	break
}


//Пятый урок
for(i = 10; i < 50; i++)
{
	if(Time.getHours() === 12 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][4])
	}
	break
}


//Шестой урок
for(i = 10; i < 50; i++)
{
	if(Time.getHours() === 13 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][5])
	}
	break
}


//Седьмой урок
for(i = 10; i < 50; i++)
{
	if(Time.getHours() === 14 & Time.getDay() === 6 & Time.getMinutes(i)) 
	{
		context.send('В данный момент проходит ' + Schedule[5][6])
	}
	break
}

await context.send('Сейчас урока нет. Ураааааа!')
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




updates.hear('/дз', async(context) => {
	const url = 'https://github.com/FloydReme/bot631/blob/master/domashka.txt'
    request(url, async function(error, Response, body) {
	const $ = cheerio.load(body)
	const English = $('#LC1').text()
	const Russian = $('#LC2').text()
	const Literature=  $('#LC3').text()
	const German = $('#LC4').text()
	const French = $('#LC5').text()
	const Algebra = $('#LC6').text()
	const Geometry = $('#LC7').text()
	const Biology = $('#LC8').text()
	const Physics = $('#LC10').text()
	const Chemistry = $('#LC9').text()
	const CompScience = $('#LC11').text()
	const Geography = $('#LC12').text()
	const Mhk = $('#LC13').text()
	const History_ = $('#LC14').text()
	const Society = $('#LC15').text()

	const Monday = new Array(6)
	Monday[0] = English
	Monday[1] = Literature
	Monday[2] = History_
	Monday[3] = CompScience
	Monday[4] = Biology
	Monday[5] = Algebra

	const Tuesday = new Array(6)
	Tuesday[0] = English
	Tuesday[1] = Literature
	Tuesday[2] = History_
	Tuesday[3] = CompScience
	Tuesday[4] = Biology
	Tuesday[5] = Algebra

	const Wednesday = new Array(6)
	Wednesday[0] = English
	Wednesday[1] = Literature
	Wednesday[2] = History_
	Wednesday[3] = CompScience
	Wednesday[4] = Biology
	Wednesday[5] = Algebra

	const Thursday = new Array(6)
	Thursday[0] = English
	Thursday[1] = Literature
	Thursday[2] = History_
	Thursday[3] = CompScience
	Thursday[4] = Biology
	Thursday[5] = Algebra


	const Friday = new Array(6)
	Friday[0] = English
	Friday[1] = Literature
	Friday[2] = History_
	Friday[3] = CompScience
	Friday[4] = Biology
	Friday[5] = Algebra


	const Saturday = new Array(6)
	Saturday[0] = English
	Saturday[1] = Literature
	Saturday[2] = History_
	Saturday[3] = CompScience
	Saturday[4] = Biology
	Saturday[5] = Algebra

	
	const Sunday = new Array(15)
	Sunday[0] = English
	Sunday[1] = Russian
	Sunday[2] = Literature
	Sunday[3] = History_
	Sunday[4] = CompScience
	Sunday[5] = Biology
	Sunday[6] = Algebra
	Sunday[7] = Geography
	Sunday[8] = Geometry
	Sunday[9] = Society
	Sunday[10] = Chemistry
	Sunday[11] = Physics
	Sunday[12] = Mhk
	Sunday[13] = French
	Sunday[14] = German

	if(Time.getDay() === 1)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Monday.join('\n')
		await context.send('Домашка с понедельника ' + formatter.format(Time) + ' \n'  + x)
	}
	if(Time.getDay() === 2)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Tuesday.join('\n')
		await context.send('Домашка со вторника '+ formatter.format(Time) + ' \n'  + x)
	}
	if(Time.getDay() === 3)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Wednesday.join('\n')
		await context.send('Домашка со среды '+ formatter.format(Time) + ' \n'  + x)
	}
	if(Time.getDay() === 4)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Thursday.join('\n')
		await context.send('Домашка с четверга '+ formatter.format(Time) + ' \n' + x)
	}
	if(Time.getDay() === 5)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Friday.join('\n')
		await context.send('Домашка с пятницы '+ formatter.format(Time) + ' \n'  + x)
	}
	if(Time.getDay() === 6)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Saturday.join('\n')
		await context.send('Домашка с субботы ' + formatter.format(Time) + ' \n' + x)
	}
	if(Time.getDay() === 0)
	{
		var formatter = new Intl.DateTimeFormat("ru", {
			month: "long",
			day: "numeric"
		  });
		const x = Sunday.join('\n')
		await context.send('Поздравляю с единственным выходным. Проведите его с пользой. Домашка на всю неделю: ' + formatter.format(Time) + ' \n'  + x)
	}})
})

updates.hear('/help', async(context) => {
	await context.send(`Итак, вот вам более-менее краткая документация.
Мой исходный код: https://github.com/FloydReme/bot631
	
Краткая сводка по моим командам: /start

Ответы на те или иные сообщения вызваны регулярными выражениями. Как это работает? Просто! 
Я делаю триггер на то или иное слово, а бот на него отвечает.

КАК РАБОТАЕТ /гдз:
Вы пишите команду "/гдз" и следом текст задачи. Пример:
/гдз Из двух городов одновременно на встречу друг другу отправились два поезда. 

Со временем команды будут увеличиваться, если вы об этом меня попросите и если в этом будет вообще всякий смысл`)
})

updates.hear(/^\/гдз (.+)/i, async (context) => {
	const textUser = context.$match[1];
	google.resultsPerPage = 3;
	context.send('Я нашел тут пару ГДЗ по твоему запросу, глянь их:')
	google(textUser, function (error, res) {
    const settings = {
	    streamType: 'png',
		windowSize: {
			width: '1000',
			height: '1400'
		},
		shotSize: {
			width: '1000',
			height: '1400'
		},
		userAgent: 'Mozilla/5.0 (iPhone; U; CPU iPhone OS 3_2 like Mac OS X; en-us)' + ' AppleWebKit/531.21.20 (KHTML, like Gecko) Mobile/7B298g'
		}

		const link1 = res.links[0]
        const link2 = res.links[1]
        const link3 = res.links[2]
		
		Promise.all([
		webshot(link1.href, 'images/GDZ1.png', settings, function(error) 
		{
			context.send('ГДЗ номер 1:\n' + link1.href)
			context.sendPhoto('images/GDZ1.png')
			if(error)
		{
			context.send('Простите, у меня случилась ошибка :с')
		}
		}),
		webshot(link2.href, 'images/GDZ2.png', settings, function(error) 
		{
			context.send('ГДЗ номер 2:\n' + link2.href) 
			context.sendPhoto('images/GDZ2.png')
			if(error)
		{
			context.send('Простите, у меня случилась ошибка :с')
		}
		}),
		webshot(link3.href, 'images/GDZ3.png', settings, function(error) 
		{
			context.send('ГДЗ номер 3:\n' + link3.href)
			context.sendPhoto('images/GDZ3.png')
			if(error)
		{
			context.send('Простите, у меня случилась ошибка :с')
		}
		})
		])
		if(error)
		{
			context.send('Простите, у меня случилась ошибка :с')
		}
	})
})


updates.hear(/^\/отзыв (.+)/i, async(context) => {
	const feedback = context.$match[1]
	await context.send('Хорошо, твой отзыв будет отправлен Саше, спасибо :)')
	api.messages.send({
		message: 'НОВЫЙ ОТЗЫВ: ' + feedback,
		domain: 'egoromanov'
	})
})



const reg1 = new Array(17)
reg1[0] = new RegExp(/мякиш/i)
reg1[1] = new RegExp(/мякишу/i)
reg1[2] = new RegExp(/мякише/i)
reg1[3] = new RegExp(/мякиша/)
reg1[4] = new RegExp(/програмирош/i)
reg1[5] = new RegExp(/программирош/i)
reg1[6] = new RegExp(/программироша/i)
reg1[7] = new RegExp(/програмироша/i)
reg1[8] = new RegExp(/програмироша/i)
reg1[9] = new RegExp(/програмирошу/i)
reg1[10] = new RegExp(/програмироше/i)
reg1[11] = new RegExp(/программироша/i)
reg1[12] = new RegExp(/программирошу/i)
reg1[13] = new RegExp(/программироше/i)
reg1[14] = new RegExp(/мирош/i)
reg1[15] = new RegExp(/мирошу/i)
reg1[16] = new RegExp(/микромяш/i)

const answers1 = new Array(4)
answers1[0] = "Говнокодера вызывали? (っಠ‿ಠ)っ"
answers1[1] = "Если ты ругаешь Мироша, то ты пидор ( ͡° ͜ʖ ͡°)"
answers1[2] = "Ты что-то против меня имеешь? Го раз на раз выйдем, а не в интернете базарь (ﾒ￣▽￣)︻┳═一 "
answers1[3] = "Я хорош собой, и вы это знаете (ʘ ͜ʖ ʘ)"
const random1 = answers1[Math.floor(Math.random() * answers1.length)]
updates.hear(reg1, async(context) => {
	await context.send(random1)
})

const reg2 = new Array(10)
reg2[0] = new RegExp(/ганц/i)
reg2[1] = new RegExp(/ганца/i)
reg2[2] = new RegExp(/ганцу/i)
reg2[3] = new RegExp(/ганце/i)
reg2[4] = new RegExp(/ганцем/i)
reg2[5] = new RegExp(/богдан/i)
reg2[6] = new RegExp(/богдану/i)
reg2[7] = new RegExp(/богдана/i)
reg2[8] = new RegExp(/богданом/i)
reg2[9] = new RegExp(/богдане/i)
const answers2 = new Array(3)
answers2[0] = "Ruhm der Ukraine"
answers2[1] = "Bogdan ist der Sinn des Lebens"
answers2[2] = "der beste Mann Kappa"
const random2 = answers2[Math.floor(Math.random() * answers1.length)]
updates.hear(reg2, async(context) => {
	await context.send(random2)
})

updates.hear(/спасибо/i, async(context) => {
	await context.send('Не за что! Рад помочь')
})


const reg3 = new Array(18)
reg3[0] = new RegExp(/новосельцев/i)
reg3[1] = new RegExp(/новос/i)
reg3[2] = new RegExp(/навос/i)
reg3[3] = new RegExp(/навоз/i)
reg3[4] = new RegExp(/новосельцева/i)
reg3[5] = new RegExp(/новосельцеву/i)
reg3[6] = new RegExp(/новосельцеве/i)
reg3[7] = new RegExp(/новосельцевым/i)
reg3[8] = new RegExp(/новоса/i)
reg3[9] = new RegExp(/новоса/i)
reg3[10] = new RegExp(/новосу/i)
reg3[11] = new RegExp(/новосе/i)
reg3[12] = new RegExp(/навоса/i)
reg3[13] = new RegExp(/навосу/i)
reg3[14] = new RegExp(/навосе/i)
reg3[15] = new RegExp(/навоса/i)
reg3[16] = new RegExp(/навосу/i)
reg3[17] = new RegExp(/навозу/i)
const answers3 = new Array(3)
answers3[0] = "ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ"
answers3[1] = "ПОМАЦАЙ ПIД МАТРАЦЕМ"
answers3[2] = "Продам Брата. Дорого"
const random3 = answers3[Math.floor(Math.random() * answers3.length)]
updates.hear(reg3, async(context) => {
	await context.send(random3)
})


const reg4 = new Array(10)
reg4[0] = new RegExp(/рыжий/i)
reg4[1] = new RegExp(/рыжего/i)
reg4[2] = new RegExp(/рыжему/i)
reg4[3] = new RegExp(/рыжим/i)
reg4[4] = new RegExp(/даня/i)
reg4[5] = new RegExp(/дани/i)
reg4[6] = new RegExp(/дане/i)
reg4[7] = new RegExp(/пономарь/i)
reg4[8] = new RegExp(/пономарев/i)
reg4[9] = new RegExp(/пономарева/i)
const answers4 = new Array(3)
answers4[0] = "ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ"
answers4[1] = "ПАЦАНЫ ПТУ ВАЩЕ ЗАЕБИСЬ, ВСЕМ СОВЕТУЮ. ОТ ОДНОЙ ПАРЫ ПОУМНЕЛ НА 200 ХП"
answers4[2] = "Я НЕ ДАНИИЛ СЦУКА"
const random4 = answers4[Math.floor(Math.random() * answers3.length)]
updates.hear(reg4, async(context) => {
	await context.send(random4)
})



updates.hear(/ганж/i, async(context) => {
	context.send('Ты там нахуй охуел, пес? Сейчас гейскую порнуху скину')
})