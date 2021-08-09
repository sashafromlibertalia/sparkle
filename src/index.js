const BOT = require('./vk')
const Games = require('./commands/games')
const Kicker = require('./commands/kicker')
const SavedData = require('./commands/savedData')
const DateC = require('./commands/date')
const Citgen = require('./commands/citgen')
const Troll = require('./commands/troll')
const Autosend = require('./automaticSender')
const Trollgen = require('./commands/trollgen')

require('http').createServer().listen(process.env.PORT || 8000).on('request', function (res) {
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
/шпора - добавляй важные фото/документы/шпоры, чтобы не искать потом по всей беседе
———————————
/команды - список всех моих команд
———————————
/help - моя документация`)
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
/дз завтра - отправляет домашние задания на завтра
———————————
/дз все - отправляет домашние задания на всю неделю
———————————
/игры - отправляет клавиатуру с выбором игр (да-да, не удивляйтесь)
———————————
/отзыв - напиши отзыв, и Саша его увидит. ВАЖНО: отзыв анонимен 
———————————
/шпора - добавляй важные фото/документы/шпоры, чтобы не искать потом по всей беседе
———————————
/шпора список - список шпор
———————————
/шпора ? - инструкция
———————————
/тролль - перешлите чье-то сообщение и пишите эту команду
———————————
/citgen - перешлите чье-то сообщение и пишите эту команду
———————————
/trollgen - перешлите чье-то сообщение и пишите эту команду
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
    random_id: BOT.RANDOM()
  })
})

Kicker.run()
Citgen.run()
DateC.run()
SavedData.run()
Games.run()
Troll.run
Autosend.run()
Trollgen.run()

BOT.VK.updates.start().catch(console.error)
