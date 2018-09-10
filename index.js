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
/insert - добавляй в бота домашку, если ты его знаешь, а другие - нет
/insert ? - справка по команде /insert
/отзыв - напиши отзыв, и Саша его увидит. ВАЖНО: отзыв анонимен, честное слово
/завтра - узнаешь расписание на завтрашний день
/неделя - расписание на всю неделю
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
Schedule[0] = new Array(6)
Schedule[0][0] = "1. История 🕐 8:30 - 9:10\n"
Schedule[0][1] = "2. Русский 🕐 9:20 - 10:00\n"
Schedule[0][2] = "3. Русский 🕐 10:15 - 10:55\n"
Schedule[0][3] = "4. Химия 🕐 11:15 - 11:55\n"
Schedule[0][4] = "5. Английский 🕐 12:10 - 12:50\n"
Schedule[0][5] = "6. Английский 🕐 13:10 - 13:50\n"

Schedule[1] = new Array(7)
Schedule[1][0] = "1. Литература 🕐 8:30 - 9:10\n"
Schedule[1][1] = "2. Биология 🕐 9:20 - 10:00\n"
Schedule[1][2] = "3. Русский 🕐 10:15 - 10:55\n"
Schedule[1][3] = "4. Общество 🕐 11:15 - 11:55\n"
Schedule[1][4] = "5. Алгебра 🕐 12:10 - 12:50\n"
Schedule[1][5] = "6. Физкультура 🕐 13:10 - 13:50\n"
Schedule[1][6] = "7. Физкультура 🕐 14:10 - 14:50\n"

Schedule[2] = new Array(7)
Schedule[2][0] = "1. Геометрия 🕐 8:30 - 9:10\n"
Schedule[2][1] = "2. Английский 🕐 9:20 - 10:00\n"
Schedule[2][2] = "3. Английский 🕐 10:15 - 10:55\n"
Schedule[2][3] = "4. МХК 🕐 11:15 - 11:55\n"
Schedule[2][4] = "5. Физика 🕐 12:10 - 12:50\n"
Schedule[2][5] = "6. Немецкий/Французский 🕐 13:10 - 13:50\n"
Schedule[2][6] = "7. Электив 🕐 14:10 - 14:50\n"

Schedule[3] = new Array(6)
Schedule[3][0] = "1. СОН 🕐 До 9:20\n"
Schedule[3][1] = "2. Физика 🕐 9:20 - 10:00\n"
Schedule[3][2] = "3. Инфа 🕐 10:15 - 10:55\n"
Schedule[3][3] = "4. Алгебра 🕐 11:15 - 11:55\n"
Schedule[3][4] = "5. История 🕐 12:10 - 12:50\n"
Schedule[3][5] = "6. История 🕐 13:10 - 13:50\n"


Schedule[4] = new Array(6)
Schedule[4][0] = "1. География 🕐 8:30 - 9:10\n"
Schedule[4][1] = "2. Электив 🕐 9:20 - 10:00\n"
Schedule[4][2] = "3. Литература 🕐 10:15 - 10:55\n"
Schedule[4][3] = "4. Литература 🕐 11:15 - 11:55\n"
Schedule[4][4] = "5. Английский 🕐 12:10 - 12:50\n"
Schedule[4][5] = "6. Английский 🕐 13:10 - 13:50\n"

Schedule[5] = new Array(6)
Schedule[5][0] = "1. Геометрия 🕐 8:30 - 9:10\n"
Schedule[5][1] = "2. Общество 🕐 9:20 - 10:00\n"
Schedule[5][2] = "3. ОБЖ 🕐 10:15 - 10:55\n"
Schedule[5][3] = "4. Физкультура 🕐 11:15 - 11:55\n"
Schedule[5][4] = "5. Немецкий/французский 🕐 12:10 - 12:50\n"
Schedule[5][5] = "6. Математика-электив 🕐 13:10 - 13:50\n"


updates.hear('/завтра', async(context) => {
	if(Time.getDay() === 1)
	{
		await context.send(`Расписание на завтра: \n ${Schedule[1].join(' ')}`)
	}
	if(Time.getDay() === 2)
	{
		await context.send(`Расписание на завтра: \n ${Schedule[2].join(' ')}`)
	}
	if(Time.getDay() === 3)
	{
		await context.send(`Расписание на завтра: \n ${Schedule[3].join(' ')}`)
	}
	if(Time.getDay() === 4)
	{
		await context.send(`Расписание на завтра: \n ${Schedule[4].join(' ')}`)
	}
	if(Time.getDay() === 5)
	{
		await context.send(`Расписание на завтра: \n ${Schedule[5].join(' ')}`)
	}
	if(Time.getDay() === 6)
	{
		await context.send(`Завтра неучебный день - кайфуйте`)
	}
	if(Time.getDay() === 0)
	{
		await context.send(`Расписание на завтра: \n ${Schedule[0].join(' ')}`)
	}
})


const newDay = new Date()
var greeting = new Array(4)
greeting[0] = "Итак, мои дорогие, начался новый учебный день. Я желаю вам всем хороших оценок по всем предметам, удачи :)\n Расписание на сегодня:\n"
greeting[1] = "И снова всем приветик, господа. Скучали? Я знаю, что нет. Вот вам расписание на сегодня: \n"
greeting[2] = "Шалом, дамы и пацаны. Возможно, мои ежедневные напоминая о расписании вам надоели, но я ничего поделать не могу - я создан для выполнения конкретных задач. Кстати, вот сегодняшнее расписание: \n"
greeting[3] = "Привет. Без лишних слов. Расписание на сегодня:\n"
var random_greeting = greeting[Math.floor(Math.random() * greeting.length)]
if(newDay.getHours() === 8 && newDay.getMinutes() === 0) 
{
	if(newDay.getDay() === 1)
	{
		api.messages.send({
			message: random_greeting + Schedule[0],
			peer_id: 2000000001
		})
	}
	if(newDay.getDay() === 2)
	{
		api.messages.send({
			message: random_greeting + Schedule[1],
			peer_id: 2000000001
		})
	}
	if(newDay.getDay() === 3)
	{
		api.messages.send({
			message: random_greeting + Schedule[2],
			peer_id: 2000000001
		})
	}
	if(newDay.getDay() === 4)
	{
		api.messages.send({
			message: random_greeting + Schedule[3],
			peer_id: 2000000001
		})	
	}
	if(newDay.getDay() === 5)
	{
		api.messages.send({
			message: random_greeting + Schedule[4],
			peer_id: 2000000001
		})
	}
	if(newDay.getDay() === 6)
	{
		api.messages.send({
			message: random_greeting + Schedule[5],
			peer_id: 2000000001
		})
	}
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
		await context.send('Расписание на сегодня:\n' + Schedule[0].join(' '))
	}
	if(Time.getDay() === 2)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[1].join(' '))
	}
	if(Time.getDay() === 3)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[2].join(' '))
	}
	if(Time.getDay() === 4)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[3].join(' '))
	}
	if(Time.getDay() === 5)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[4].join(' '))
	}
	if(Time.getDay() === 6)
	{
		await context.send('Расписание на сегодня:\n' + Schedule[5].join(' '))
	}
})


const url = 'https://github.com/FloydReme/bot631/blob/master/domashka.txt'
request(url, async function(error, Response, body) {
	const $ = cheerio.load(body)
	const Englishdz = $('#LC2').text()
	const Russiandz = $('#LC4').text()
	const Literaturedz=  $('#LC6').text()
	const Germandz = $('#LC8').text()
	const Frenchdz = $('#LC10').text()
	const Algebradz = $('#LC12').text()
	const Geometrydz = $('#LC14').text()
	const Biologydz = $('#LC16').text()
	const Physicsdz = $('#LC18').text()
	const Chemistrydz = $('#LC20').text()
	const CompSciencedz= $('#LC22').text()
	const Geographydz = $('#LC24').text()
	const Mhkdz = $('#LC26').text()
	const History_dz = $('#LC28').text()
	const Societydz = $('#LC30').text()
	const OBJdz = $('#LC32').text()

	const English = $('#LC1').text()
	const Russian = $('#LC3').text()
	const Literature=  $('#LC5').text()
	const German = $('#LC7').text()
	const French = $('#LC9').text()
	const Algebra = $('#LC11').text()
	const Geometry = $('#LC13').text()
	const Biology = $('#LC15').text()
	const Physics = $('#LC17').text()
	const Chemistry = $('#LC19').text()
	const CompScience = $('#LC21').text()
	const Geography = $('#LC23').text()
	const Mhk = $('#LC25').text()
	const History_ = $('#LC27').text()
	const Society = $('#LC29').text()
	const OBJ = $('#LC31').text()

	const Monday = new Array(4)
	Monday[0] = History_ + History_dz
	Monday[1] = Russian + Russiandz
	Monday[2] = Chemistry + Chemistrydz
	Monday[3] = English + Englishdz

	const Tuesday = new Array(5)
	Tuesday[0] = Literature + Literaturedz
	Tuesday[1] = Biology + Biologydz
	Tuesday[2] = Russian + Russiandz
	Tuesday[3] = Society + Societydz
	Tuesday[4] = Algebra + Algebradz

	const Wednesday = new Array(5)
	Wednesday[0] = Geometry + Geometrydz
	Wednesday[1] = English + Englishdz
	Wednesday[2] = Mhk + Mhkdz
	Wednesday[3] = Physics + Physicsdz
	Wednesday[4] = French + Frenchdz

	const Thursday = new Array(4)
	Thursday[0] = Physics + Physicsdz
	Thursday[1] = CompScience + CompSciencedz
	Thursday[2] = Algebra + Algebradz
	Thursday[3] = History_ + History_dz


	const Friday = new Array(3)
	Friday[0] = Geography + Geographydz
	Friday[1] = Literature + Literaturedz
	Friday[2] = English + Englishdz

	const Saturday = new Array(5)
	Saturday[0] = Geometry + Geometrydz
	Saturday[1] = Society + Societydz
	Saturday[2] = OBJ + OBJdz
	Saturday[3] = French + Frenchdz
	Saturday[4] = Algebra + Algebradz

	const Sunday = new Array(16)
	Sunday[0] = English + Englishdz
	Sunday[1] = Russian + Russiandz
	Sunday[2] = Literature + Literaturedz
	Sunday[3] = History_ + History_dz
	Sunday[4] = CompScience + CompSciencedz
	Sunday[5] = Biology + Biologydz
	Sunday[6] = Algebra + Algebradz
	Sunday[7] = Geography + Geographydz
	Sunday[8] = Geometry + Geometrydz
	Sunday[9] = Society + Societydz
	Sunday[10] = Chemistry + Chemistrydz
	Sunday[11] = Physics + Physicsdz
	Sunday[12] = Mhk + Mhkdz
	Sunday[13] = French + Frenchdz
	Sunday[14] = German + Germandz
	Sunday[15] = OBJ + OBJdz


updates.hear(/^\/insert (.+) (.+)/i, async(context) => {
	const Subject = new RegExp(context.$match[1],'i') 
	const homeWork = context.$match[2]
	const subjects = []
	$('td').each(function(i, elem) {
		subjects[i] = $(this).text();
	});

	for(var j = 0; j < subjects.length; j++)
	{
		if(subjects[j].match(Subject))
		{
			context.send(`Ваш предмет: ${subjects[j]}
			Ваше дз: ${homeWork}`)
		}
	}
})



updates.hear('/insert ?', async(context) => {
	await context.send(`
Справка по команде /insert.
Она позволяет добавлять домашнее задание для каждого предмета моментально (На самом деле Саша не хочет все вводить вручную, процесс нужно автоматизировать)
Итак, как она работает?
Вы пишите: /insert название_предмета сама_домашка
Затем бот отправит вам обновленное дз по вашему предмету, и все будут счастливы!
Всем мир`)
})

updates.hear('/дз все', async(context) =>{
	await context.send(Sunday.join('\n'))
})

updates.hear('/дз', async(context) => {
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
	google(textUser, function (error) {
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


updates.hear('/неделя', async(context) => {
	await context.send(`РАСПИСАНИЕ НА ВСЮ НЕДЕЛЮ:
ПОНЕДЕЛЬНИК:
${Schedule[0].join(' ')}

ВТОРНИК:
${Schedule[1].join(' ')}

СРЕДА:
${Schedule[2].join(' ')}

ЧЕТВЕРГ:
${Schedule[3].join(' ')}

ПЯТНИЦА:
${Schedule[4].join(' ')}

СУББОТА:
${Schedule[5].join(' ')}`)
})

/*const reg1 = new Array(17)
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
})*/

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
const random2 = answers2[Math.floor(Math.random() * answers2.length)]
updates.hear(reg2, async(context) => {
	await context.send(random2)
})

/*updates.hear(/спасибо/i, async(context) => {
	await context.send('Не за что! Рад помочь')
})*/


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


/*const reg4 = new Array(10)
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
})*/



updates.hear(/ганж/i, async(context) => {
	context.send('Ты рамсы попутал, пес?')
})