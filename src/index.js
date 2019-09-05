const { VK } = require('vk-io')
const { Keyboard } = require('vk-io')
const config = require('./config')
const citgen = require('./citgen')
const Schedule = require('./schedule')
const gulag = require('./gulag')
const shpora = require('./shpora')
const shporaList = require('./shpora')
const vk = new VK()
const { updates } = vk
const { api } = vk
const cheerio = require('cheerio')
const request = require('request')
const Intl = require('intl')
const moment = require('moment')
const Time = new Date()
const formatter = new Intl.DateTimeFormat('ru', {
  month: 'long',
  day: 'numeric'
})

moment().format()

vk.setOptions({
  token: config.TOKEN,
  pollingGroupId: config.pollingGroupId,
  peer_id: config.peerID
})

// Cоздаем сервер
require('http').createServer().listen(process.env.PORT || 8000).on('request', function (request, res) {
  res.end('')
})

api.baseUrl = 'https://api.vk.com/method/'

// Обработчик сообщений и клавиатуры
updates.use(async (context, next) => {
  if (context.is('message')) {
    const { messagePayload } = context

    context.state.command = messagePayload && messagePayload.command
      ? messagePayload.command
      : null
  }

  return next()
})

const hearCommand = (name, conditions, handle) => {
  if (typeof handle !== 'function') {
    handle = conditions
    conditions = [`/${name}`]
  }

  if (!Array.isArray(conditions)) {
    conditions = [conditions]
  }

  vk.updates.hear(
    [
      (text, { state }) => (
        state.command === name
      ),
      ...conditions
    ],
    handle
  )
}

updates.hear('/start', async (context) => {
  context.send({
    message: `Привет! 
Я - Бот, созданный специально для ${config.className} ${config.schoolName}. К черту эту прелюдию, я могу еще долго распинаться, но вот мой список команд:
/дата - узнай дз на конкретный день
———————————
/дз - отправляет домашние задания с текущего дня
———————————
/дз завтра - отправляет домашние задания на завтра
———————————
/дз все - отправляет домашние задания на всю неделю
———————————
/добавить - добавляй в бота домашку, если ты его знаешь, а другие - нет
———————————
/добавить ? - справка по команде /добавить
———————————
/завтра - узнаешь расписание на завтрашний день
———————————
/игры - отправляет клавиатуру с выбором игр (да-да, не удивляйтесь)
———————————
/неделя - расписание на всю неделю
———————————
/отзыв - напиши отзыв, и ${config.adminName} его увидит. ВАЖНО: отзыв анонимен 
———————————
/урок - оповещает об уроке, проходящем в данный момент
———————————
/уроки - отправляет расписание на текущий день
———————————
/шпора - добавляй важные фото/документы/шпоры, чтобы не искать потом по всей беседе
———————————
/шпора список - список шпор
———————————
/шпора ? - инструкция
———————————
/citgen - перешлите чье-то сообщение и пишите эту команду
———————————
/help - моя документация`
  })
})

updates.hear('/игры', async (context) => {
  const gamesKeyboard = Keyboard.keyboard([
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
  ]).oneTime()

  await context.send({
    message: 'Вот список моих игр',
    keyboard: gamesKeyboard
  })
})

hearCommand('ball', async (context) => {
  await context.send(`Как играть в эту игру? Очень просто! Ты пишешь "шанc" и свое утверждение, а я отвечаю вероятностью.
	Пример:
	
	— Шанc, что мы - дружный класс.
	— Вероятность - 100%`)

  updates.hear(/шанс/i, (context) => {
    const chances = new Array(6)
    chances[0] = 'Вероятность близка к нулю :('
  		chances[1] = 'Я считаю, что 50 на 50'
  		chances[2] = 'Вероятность - 100%'
  		chances[3] = 'Я полагаю, что вероятность близка к 100%'
  		chances[4] = 'Маловероятно, но шанс есть'
    chances[5] = 'Вероятность нулевая, ничего не поделать'

    context.send(chances[Math.floor(Math.random() * chances.length)])
  })
})

hearCommand('else', async (context) => {
  await context.send(`Раз эта кнопка у вас все еще есть, значит я страдаю от острой игровой недостаточности. Если у вас есть идеи, которые может реализовать этот бот в игровой форме - пишите ${config.adminNameDat}, он сможет :)`)
})

hearCommand('cancel', async (context) => {
  await context.send('Хорошо, я выключу клавиатуру!')
})

updates.hear('/завтра', async (context) => {
  for (i = 0; i < 7; i++) {
    if (moment().day() === i) {
      await context.send(`Расписание на завтра: \n ${Schedule[i].join(' ')}`)
    };
  }
})


let greeting = new Array(4)
greeting[0] = "Итак, мои дорогие, начался новый учебный день. Я желаю вам всем хороших оценок по всем предметам, удачи :)\n Расписание на сегодня:\n";
greeting[1] = "И снова всем приветик, господа. Скучали? Я знаю, что нет. Вот вам расписание на сегодня: \n";
greeting[2] = "Шалом, дамы и пацаны. Возможно, мои ежедневные напоминая о расписании вам надоели, но я ничего поделать не могу - я создан для выполнения конкретных задач. Кстати, вот сегодняшнее расписание: \n";
greeting[3] = "Привет. Без лишних слов. Расписание на сегодня:\n";
let random_greeting = greeting[Math.floor(Math.random() * greeting.length)];
if(moment().hour() === 7 && moment().minute() === 40) {
	for(i = 1; i < 7; i++) {
		if(moment().day() === i) {
			api.messages.send({
				message: random_greeting + Schedule[i-1],
				peer_id: config.peerID
			})
		}
	}
}

updates.hear('/урок', async (context) => {
  for (j = 1; j < 7; j++) {
    // Первый урок
    for (i = 30; i < 59; i++) {
      if (moment().hour() === 8 && moment().day() === j && moment().minute() === i && Schedule[j - 1][0] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][0])
      }
      break
    }
    for (i = 0; i < 10; i++) {
      if (moment().hour() === 8 && moment().day() === j && moment().minute() === i && Schedule[j - 1][0] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][0])
      }
      break
    }

    // Второй урок
    for (i = 20; i < 59; i++) {
      if (moment().hour() === 9 && moment().day() === j && moment().minute() === i && Schedule[j - 1][1] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][1])
      }
      break
    }
    while (i = 0) {
      if (moment().hour() === 9 && moment().day() === j && moment().minute() === i && Schedule[j - 1][1] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][1])
      }
      break
    }

    // Третий урок
    for (i = 15; i < 55; i++) {
      if (moment().hour() === 10 && moment().day() === j && moment().minute() === i && Schedule[j - 1][2] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][2])
      }
      break
    }

    // Четвертый урок
    for (i = 15; i < 55; i++) {
      if (moment().hour() === 11 && moment().day() === j && moment().minute() === i && Schedule[j - 1][3] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][3])
      }
      break
    }

    // Пятый урок
    for (i = 10; i < 50; i++) {
      if (moment().hour() === 12 && moment().day() === j && moment().minute() === i && Schedule[j - 1][4] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][4])
      }
      break
    }

    // Шестой урок
    for (i = 10; i < 50; i++) {
      if (moment().hour() === 13 && moment().day() === j && moment().minute() === i && Schedule[j - 1][5] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][5])
      }
      break
    }

    // Седьмой урок
    for (i = 10; i < 50; i++) {
      if (moment().hour() === 14 && moment().day() === j && moment().minute() === i && Schedule[j - 1][6] != undefined) {
        context.send('В данный момент проходит ' + Schedule[j - 1][6])
      }
      break
    }
  }
})

updates.hear('/уроки', async (context) => {
  for (i = 1; i < 7; i++) {
    if (moment().day() === i) {
      await context.send('Расписание на сегодня:\n' + Schedule[i - 1].join(' '))
    };
  }
})

const url = config.homeworkParserURL
/* Парсер */
request(url, async function (err, res, body) {
  const weekKeyboard = Keyboard.keyboard([[
    Keyboard.textButton({
      label: 'Понедельник',
      payload: {
        command: 'monday'
      },
      color: Keyboard.POSITIVE_COLOR
    }),
    Keyboard.textButton({
      label: 'Вторник',
      payload: {
        command: 'tuesday'
      },
      color: Keyboard.POSITIVE_COLOR
    }),
    Keyboard.textButton({
      label: 'Среда',
      payload: {
        command: 'wednesday'
      },
      color: Keyboard.POSITIVE_COLOR
    })],
  [
    Keyboard.textButton({
      label: 'Четверг',
      payload: {
        command: 'thursday'
      },
      color: Keyboard.POSITIVE_COLOR
    }),
    Keyboard.textButton({
      label: 'Пятница',
      payload: {
        command: 'friday'
      },
      color: Keyboard.POSITIVE_COLOR
    }),
    Keyboard.textButton({
      label: 'Суббота',
      payload: {
        command: 'saturday'
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
  ]).oneTime()

  if (err) throw err

  const $ = cheerio.load(body)
  const Englishdz = $('#LC2').text()
  const Russiandz = $('#LC5').text()
 		const Literaturedz = $('#LC8').text()
 		const Germandz = $('#LC11').text()
  const Frenchdz = $('#LC14').text()
  const Algebradz = $('#LC17').text()
  const Geometrydz = $('#LC20').text()
  const Biologydz = $('#LC23').text()
  const Physicsdz = $('#LC26').text()
  const Chemistrydz = $('#LC29').text()
  const CompSciencedz = $('#LC32').text()
  const Geographydz = $('#LC35').text()
  const Mhkdz = $('#LC38').text()
  const History_dz = $('#LC41').text()
  const Societydz = $('#LC44').text()
  const OBJdz = $('#LC47').text()
  const DPUAlgebra = $('#LC50').text()
  const AstronomyDZ = $('#LC53').text()

  const predmeti = new Array(18)
  predmeti[0] = $('#LC1').text() // Английский
  predmeti[1] = $('#LC4').text() // Русский
  predmeti[2] = $('#LC7').text() // Литература
  predmeti[3] = $('#LC10').text() // Немецкий
  predmeti[4] = $('#LC13').text() // Французский
  predmeti[5] = $('#LC16').text() // Алгебра
  predmeti[6] = $('#LC19').text() // Геометрия
  predmeti[7] = $('#LC22').text() // Биология
  predmeti[8] = $('#LC25').text() // Химия
  predmeti[9] = $('#LC28').text() // Физика
  predmeti[10] = $('#LC31').text() // Информатика
  predmeti[11] = $('#LC34').text() // География
  predmeti[12] = $('#LC37').text() // МХК
  predmeti[13] = $('#LC40').text() // История
  predmeti[14] = $('#LC43').text() // Обществознание
  predmeti[15] = $('#LC46').text() // ОБЖ
  predmeti[16] = $('#LC49').text() // ДПУ Алгебра
  predmeti[17] = $('#LC52').text() // Астрономия

  const	line = $('#LC3').text()

  const Monday = new Array(3)
Monday[0] = predmeti[2] + Literaturedz + `\n${line}`
Monday[1] = predmeti[1] + Russiandz + `\n${line}`
Monday[2] = predmeti[0] + Englishdz + `\n${line}`

const Tuesday = new Array(5)
Tuesday[0] = predmeti[7] + Biologydz + `\n${line}`
Tuesday[1] = predmeti[9] + Physicsdz + `\n${line}`
Tuesday[2] = predmeti[14] + Societydz + `\n${line}`
Tuesday[3] = predmeti[8] + Chemistrydz + `\n${line}`
Tuesday[4] = predmeti[5] + Algebradz + `\n${line}`

const Wednesday = new Array(4)
Wednesday[0] = predmeti[0] + Englishdz + `\n${line}`
Wednesday[1] = predmeti[5] + Algebradz + `\n${line}`
Wednesday[2] = predmeti[17] + AstronomyDZ + `\n${line}`
Wednesday[3] = predmeti[4] + Frenchdz + `\n${line}`

const Thursday = new Array(5)
Thursday[0] = predmeti[15] + OBJdz + `\n${line}`
Thursday[1] = predmeti[6] + Geometrydz + `\n${line}`
Thursday[2] = predmeti[1] + Russiandz + `\n${line}`
Thursday[3] = predmeti[12] + Mhkdz + `\n${line}`
Thursday[4] = predmeti[2] + Literaturedz + `\n${line}`

const Friday = new Array(4)
Friday[0] = predmeti[0] + Englishdz + `\n${line}`
Friday[1] = predmeti[13] + History_dz + `\n${line}`
Friday[2] = predmeti[10] + CompSciencedz + `\n${line}`
Friday[3] = predmeti[11] + Geographydz + `\n${line}`

const Saturday = new Array(5)
Saturday[0] = predmeti[13] + History_dz + `\n${line}`
Saturday[1] = predmeti[9] + Physicsdz + `\n${line}`
Saturday[2] = predmeti[6] + Geometrydz + `\n${line}`
Saturday[3] = predmeti[4] + Frenchdz + `\n${line}`
Saturday[4] = predmeti[14] + Societydz + `\n${line}`


  const preds = new Array(18)
  preds[0] = {
    namesubj: predmeti[0],
    dz: Englishdz
  }
  preds[1] = {
    namesubj: predmeti[1],
    dz: Russiandz
  }
  preds[2] = {
    namesubj: predmeti[2],
    dz: Literaturedz
  }
  preds[3] = {
    namesubj: predmeti[3],
    dz: Germandz
  }
  preds[4] = {
    namesubj: predmeti[4],
    dz: Frenchdz
  }
  preds[5] = {
    namesubj: predmeti[5],
    dz: Algebradz
  }
  preds[6] = {
    namesubj: predmeti[6],
    dz: Geometrydz
  }
  preds[7] = {
    namesubj: predmeti[7],
    dz: Biologydz
  }
  preds[8] = {
    namesubj: predmeti[8],
    dz: Chemistrydz
  }
  preds[9] = {
    namesubj: predmeti[9],
    dz: Physicsdz
  }
  preds[10] = {
    namesubj: predmeti[10],
    dz: CompSciencedz
  }
  preds[11] = {
    namesubj: predmeti[11],
    dz: Geographydz
  }
  preds[12] = {
    namesubj: predmeti[12],
    dz: Mhkdz
  }
  preds[13] = {
    namesubj: predmeti[13],
    dz: History_dz
  }
  preds[14] = {
    namesubj: predmeti[14],
    dz: Societydz
  }
  preds[15] = {
    namesubj: predmeti[15],
    dz: OBJdz
  }
  preds[16] = {
    namesubj: predmeti[16],
    dz: DPUAlgebra
  }
  preds[17] = {
    namesubj: predmeti[17],
    dz: AstronomyDZ
  }

  const Sunday = new Array(18)
  Sunday[0] = predmeti[0] + preds[0].dz + `\n${line}`
  Sunday[1] = predmeti[1] + preds[1].dz + `\n${line}`
  Sunday[2] = predmeti[2] + preds[2].dz + `\n${line}`
  Sunday[3] = predmeti[13] + preds[13].dz + `\n${line}`
  Sunday[4] = predmeti[10] + preds[10].dz + `\n${line}`
  Sunday[5] = predmeti[7] + preds[7].dz + `\n${line}`
  Sunday[6] = predmeti[5] + preds[5].dz + `\n${line}`
  Sunday[7] = predmeti[11] + preds[11].dz + `\n${line}`
  Sunday[8] = predmeti[6] + preds[6].dz + `\n${line}`
  Sunday[9] = predmeti[14] + preds[14].dz + `\n${line}`
  Sunday[10] = predmeti[9] + preds[9].dz + `\n${line}`
  Sunday[11] = predmeti[8] + preds[8].dz + `\n${line}`
  Sunday[12] = predmeti[12] + preds[12].dz + `\n${line}`
  Sunday[13] = predmeti[4] + preds[4].dz + `\n${line}`
  Sunday[14] = predmeti[3] + preds[3].dz + `\n${line}`
  Sunday[15] = predmeti[15] + preds[15].dz + `\n${line}`
  Sunday[16] = predmeti[16] + preds[16].dz + `\n${line}`
  Sunday[17] = predmeti[17] + preds[17].dz + `\n${line}`

  const Days = [Monday, Tuesday, Wednesday, Thursday, Friday, Saturday]

  updates.hear(/^\/добавить ([а-я.]+) (.+)/i, async (context) => {
    const Subject = new RegExp(context.$match[1], 'i')
    const homeWork = context.$match[2]
    const subjects = []
    $('td').each(function (i) {
      subjects[i] = $(this).text()
    })

    // Прохожусь по всем тегам td и нахожу, есть ли там регулярка с каким-нибудь предметом, если да, то выполняю следующее:
    // Прохожусь по массиву предметов и нахожу, есть ли там совпадение с найденным предметом среди них, то делаю следующее:
    // Нахожу нужный объект с предметом и вставляю homework в dz.
    for (let j = 0; j < subjects.length; j++) {
      if (subjects[j].match(Subject)) {
        for (let i = 0; i < predmeti.length; i++) {
          if (predmeti[i] === subjects[j]) {
            for (let g = 0; g < preds.length; g++) {
              if (predmeti[i] === preds[g].namesubj) {
                preds[g].dz = homeWork
                await context.send(`ВАЖНО: Главный список по команде /дз останется старым.
📌 НОВОЕ ДЗ: ${preds[g].namesubj + homeWork} 📌`)
              };
            };
          };
        };
      };
    };
  })

  const asks = new Array(2)
  asks[0] = new RegExp(/задано/i)
  asks[1] = new RegExp(/задали/i)

  updates.hear(asks, async (context) => {
    await context.send({
      message: 'Я тут увидел, что кто-то из вас спрашивает ДЗ. Выберите, какой день вам нужен:',
      keyboard: weekKeyboard
    })
  })

  updates.hear('/дата', async (context) => {
    await context.send({
      message: 'Выберите, какой день вам нужен:',
      keyboard: weekKeyboard
    })
  })

  hearCommand('monday', async (context) => {
    await context.send(`
		Итак, вот домашка на понедельник
		${Monday.join('\n')}`)
  })

  hearCommand('tuesday', async (context) => {
    await context.send(`
		Итак, вот домашка на вторник 
		${Tuesday.join('\n')}`)
  })

  hearCommand('wednesday', async (context) => {
    await context.send(`
		Итак, вот домашка на среду 
		${Wednesday.join('\n')}`)
  })

  hearCommand('thursday', async (context) => {
    await context.send(`
		Итак, вот домашка на четверг
		${Thursday.join('\n')}`)
  })

  hearCommand('friday', async (context) => {
    await context.send(`
		Итак, вот домашка на пятницу
		${Friday.join('\n')}`)
  })

  hearCommand('saturday', async (context) => {
    await context.send(`
		Итак, вот домашка на субботу 
		${Saturday.join('\n')}`)
  })

  updates.hear(/^\/понедельник/i, async (context) => {
    context.send(`Домашка на понедельник:
		${Monday.join('\n')}`)
  })

  updates.hear(/^\/вторник/i, async (context) => {
    context.send(`Домашка на вторник:
		${Tuesday.join('\n')}`)
  })

  updates.hear(/^\/среда/i, async (context) => {
    context.send(`Домашка на среду:
		${Wednesday.join('\n')}`)
  })

  updates.hear(/^\/четверг/i, async (context) => {
    context.send(`Домашка на четверг:
		${Thursday.join('\n')}`)
  })

  updates.hear(/^\/пятница/i, async (context) => {
    context.send(`Домашка на пятницу:
		${Friday.join('\n')}`)
  })

  updates.hear(/^\/суббота/i, async (context) => {
    context.send(`Домашка на cубботу:
		${Saturday.join('\n')}`)
  })

  updates.hear('/добавить ?', async (context) => {
    await context.send(`
Справка по команде /добавить.
Она позволяет добавлять домашнее задание для каждого предмета моментально
Итак, как она работает?
Вы пишите: /insert название_предмета сама_домашка
Затем бот отправит вам обновленное дз по вашему предмету, и все будут счастливы!
Всем мир`)
  })

  updates.hear('/дз все', async (context) => {
    await context.send(Sunday.join('\n'))
  })

  updates.hear('/дз', async (context) => {
    for (i = 1; i < 7; i++) {
      if (moment().day() === i) {
        await context.send('Домашка с текущего дня (' + formatter.format(Time) + ') \n' + Days[i - 1].join('\n'))
      };
    };

    if (moment().day() === 0) {
      await context.send('Поздравляю с единственным выходным. Проведите его с пользой. Домашка на всю неделю: ' + formatter.format(Time) + ' \n' + Sunday.join('\n'))
    };
  })

  updates.hear('/дз завтра', async (context) => {
    for (i = 0; i < 7; i++) {
      if (moment().day() === i) {
        await context.send('Домашка на завтра. Сегодня ' + formatter.format(Time) + ' \n' + Days[i].join('\n'))
      };
    };
  })
})



  
for(i = 0; i < 7; i++) {
  if(moment().day() === i && moment().hour() === 15 && moment().minute() === 30) {
    context.send('Домашка на завтра. Сегодня ' + formatter.format(Time) + ' \n'  + Days[i].join('\n'))
  }
}


updates.hear('/help', async (context) => {
  await context.send(`Итак, вот вам более-менее краткая документация.
Мой исходный код: https://github.com/sashafromlibertalia/SchoolBot
	
Краткая сводка по моим командам: /start

Ответы на те или иные сообщения вызваны регулярными выражениями. Как это работает? Просто! 
Я делаю триггер на то или иное слово, а бот на него отвечает.

КАК РАБОТАЕТ /гдз:
Вы пишите команду "/гдз" и следом текст задачи. Пример:
/гдз Из двух городов одновременно на встречу друг другу отправились два поезда. 

Со временем команды будут увеличиваться, если вы об этом меня попросите и если в этом будет вообще всякий смысл`)
})

updates.hear(/^\/отзыв (.+)/i, async (context) => {
  const feedback = context.$match[1]
  await context.send(`Хорошо, твой отзыв будет отправлен ${config.adminNameDat}, спасибо :)`)
  api.messages.send({
    message: 'НОВЫЙ ОТЗЫВ: ' + feedback,
    domain: config.adminDomain
  })
})

updates.hear('/неделя', async (context) => {
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

const rozhi = new Array(4) // Любое число
rozhi[0] = 'PHOTO_ID'
rozhi[1] = 'PHOTO_ID'
rozhi[2] = 'PHOTO_ID'
rozhi[3] = 'PHOTO_ID'

updates.on('message', async (context, next) => {
  if ((context.isInbox || context.isOutbox) && context.text === '/рожа') {
    await context.send({
      message: 'Cколько лиц ты хочешь получить, мой юный извращенец?',
      keyboard: Keyboard.keyboard([
        [
          Keyboard.textButton({
            label: '1',
            payload: {
              command: 'one'
            },
            color: Keyboard.POSITIVE_COLOR
          }),
          Keyboard.textButton({
            label: '2',
            payload: {
              command: 'two'
            },
            color: Keyboard.POSITIVE_COLOR
          }),
          Keyboard.textButton({
            label: '3',
            payload: {
              command: 'three'
            },
            color: Keyboard.POSITIVE_COLOR
          })],
        [
          Keyboard.textButton({
            label: '4',
            payload: {
              command: 'four'
            },
            color: Keyboard.POSITIVE_COLOR
          }),
          Keyboard.textButton({
            label: '5',
            payload: {
              command: 'five'
            },
            color: Keyboard.POSITIVE_COLOR
          }),
          Keyboard.textButton({
            label: '10',
            payload: {
              command: 'ten'
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
      ])
    })

    hearCommand('one', async (context) => {
      await context.send({
        attachment: rozhi[Math.floor(Math.random() * rozhi.length)]
      })
    })
    hearCommand('two', async (context) => {
      await context.send({
        attachment: `${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]}`
      })
    })
    hearCommand('three', async (context) => {
      await context.send({
        attachment: `${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]}`
      })
    })
    hearCommand('four', async (context) => {
      await context.send({
        attachment: `${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]}`
      })
    })
    hearCommand('five', async (context) => {
      await context.send({
        attachment: `${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]}`
      })
    })
    hearCommand('ten', async (context) => {
      await context.send({
        attachment: `${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]},${rozhi[Math.floor(Math.random() * rozhi.length)]}`
      })
    })
  } else {
    await next()
  }
})

citgen.start()
gulag.start()
shpora.start()
shporaList.start()
updates.startPolling()

// TO-DO
/* updates.hear(/^\/гдз (.+)/i, async (context) => {
	let textUser = context.$match[1];
	context.send('Я нашел тут пару ГДЗ по твоему запросу, глянь их:')
	let link1 = cheerio.load(`https://yandex.ru/search/?text=${textUser}`)

    let settings = {
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

	console.log(link1)
	Promise.all([
		webshot(link1.href, 'images/GDZ1.png', settings, function(err)
		{
			context.send('ГДЗ номер 1:\n' + link1.href)
			context.sendPhoto('images/GDZ1.png')
		}),
		webshot(link2.href, 'images/GDZ2.png', settings, function(error)
		{
			context.send('ГДЗ номер 2:\n' + link2.href)
			context.sendPhoto('images/GDZ2.png')
		}),
		webshot(link3.href, 'images/GDZ3.png', settings, function(error)
		{
			context.send('ГДЗ номер 3:\n' + link3.href)
			context.sendPhoto('images/GDZ3.png')
		})
		])
})
*/

/* Хорошего дня */

/*
MIT License

Copyright (c) 2019 Alexander M.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/
