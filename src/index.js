const BOT = require('./vk')
const Games = require('./commands/games')
const Kicker = require('./commands/kicker')
const SavedData = require('./commands/savedData')
const DateC = require('./commands/date')
const Citgen = require('./commands/citgen')

const Intl = require('intl')
const moment = require('moment')
const Time = new Date()
const formatter = new Intl.DateTimeFormat('ru', {
  month: 'long',
  day: 'numeric'
})

moment().format()

// Cоздаем сервер
require('http').createServer().listen(process.env.PORT || 8000).on('request', function(res) {
  res.end('')
})

BOT.MESSAGES.hear('/start', async (context) => {
  await context.send(`Привет! 👋
🤖 Я - Бот, созданный специально для группы ${BOT.CONFIG.NAME_GROUP} ${BOT.CONFIG.NAME_PLACE}. Я надеюсь, что буду очень полезен вам, и вы получите незабываемый опыт ☺️. Поскольку я могу многое, я привожу список основных моих команд:

/дата - узнай дз на конкретный день
———————————
/дз завтра - отправляет домашние задания на завтра
———————————
/игры - отправляет клавиатуру с выбором игр (да-да, не удивляйтесь)
———————————
/неделя - расписание на всю неделю
———————————
/шпора - добавляй важные фото/документы/шпоры, чтобы не искать потом по всей беседе
———————————
/команды - список всех моих команд
———————————
/help - моя документация`)
})

BOT.MESSAGES.hear('/завтра', async (context) => {
  for (i = 0; i < 7; i++) {
      if (moment().day() === i) {
          if (is11A === true) {
              await context.send(`Расписание на завтра: \n${Schedule[i].map(({a11}) => a11).join('')}`)
          } else {
              await context.send(`Расписание на завтра: \n${Schedule[i].map(({b11}) => b11).join('')}`)
          }
      }
  }
})

BOT.MESSAGES.hear('/help', async (context) => {
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

BOT.MESSAGES.hear('/команды', async (context) => {
  await context.send(`Список всех моих команд:
  /дата - узнай дз на конкретный день
———————————
/дз - отправляет домашние задания с текущего дня
———————————
/дз завтра - отправляет домашние задания на завтра
———————————
/дз все - отправляет домашние задания на всю неделю
———————————
/игры - отправляет клавиатуру с выбором игр (да-да, не удивляйтесь)
———————————
/неделя - расписание на всю неделю
———————————
/отзыв - напиши отзыв, и Саша его увидит. ВАЖНО: отзыв анонимен 
———————————
/шпора - добавляй важные фото/документы/шпоры, чтобы не искать потом по всей беседе
———————————
/шпора список - список шпор
———————————
/шпора ? - инструкция
———————————
/citgen - перешлите чье-то сообщение и пишите эту команду
———————————
/help - моя документация
———————————

Бот, кстати, сам умеет присылать ссылки на зум!`)
})

BOT.MESSAGES.hear(/^\/отзыв (.+)/i, async (context) => {
  const feedback = context.$match[1]
  await context.send(`Хорошо, твой отзыв будет отправлен ${BOT.CONFIG.NAME_ADMIN_DAT}, спасибо :)`)
  BOT.API.messages.send({
      message: 'НОВЫЙ ОТЗЫВ: ' + feedback,
      domain: BOT.CONFIG.ADMIN_DOMAIN,
      random_id: Math.floor(Math.random() * Math.floor(200))
  })
})

BOT.MESSAGES.hear('/неделя', async (context) => {
  if (is11A === true) {
      await context.send(`РАСПИСАНИЕ НА ВСЮ НЕДЕЛЮ:
ПОНЕДЕЛЬНИК:
${Schedule[0].map(({a11}) => a11).join('')}

ВТОРНИК:
${Schedule[1].map(({a11}) => a11).join('')}

СРЕДА:
${Schedule[2].map(({a11}) => a11).join('')}

ЧЕТВЕРГ:
${Schedule[3].map(({a11}) => a11).join('')}

ПЯТНИЦА:
${Schedule[4].map(({a11}) => a11).join('')}

СУББОТА:
${Schedule[5].map(({a11}) => a11).join('')}`)
  } else {
      await context.send(`РАСПИСАНИЕ НА ВСЮ НЕДЕЛЮ:
ПОНЕДЕЛЬНИК:
${Schedule[0].map(({b11}) => b11).join('')}

ВТОРНИК:
${Schedule[1].map(({b11}) => b11).join('')}

СРЕДА:
${Schedule[2].map(({b11}) => b11).join('')}

ЧЕТВЕРГ:
${Schedule[3].map(({b11}) => b11).join('')}

ПЯТНИЦА:
${Schedule[4].map(({b11}) => b11).join('')}

СУББОТА:
${Schedule[5].map(({b11}) => b11).join('')}`)
  }
})


Kicker.run()
Citgen.run()
DateC.run()
SavedData.run()
Games.run()

BOT.VK.updates.start().catch(console.error);