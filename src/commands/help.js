const BOT = require('../vk')


const help = BOT.MESSAGES.hear('/help', async (context) => {
    await context.send(`Итак, вот вам более-менее краткая документация.
Мой исходный код: https://github.com/sashafromlibertalia/SchoolBot
  
Краткая сводка по моим командам: /start
  
Ответы на те или иные сообщения вызваны регулярными выражениями. Как это работает? Просто! 
Я делаю триггер на то или иное слово, а бот на него отвечает.
  
Со временем команды будут увеличиваться, если вы об этом меня попросите и если в этом будет вообще всякий смысл`)
})

module.exports = {
    run() {
        help
    }
}