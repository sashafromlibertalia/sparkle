const DATA = require('./links')
const BOT = require('./vk')
/**
*@note
В данном коде вводите свое расписание, и бот отправит вам рассылку с Zoom
*/
function sendDayMessage(context) {
    const moment = new Date()
    setInterval(() => {
        switch (moment.getDay()) {
            case 1:
                if (moment.getHours() == 0 &&
                    moment.getMinutes() == 0 &&
                    moment.getSeconds() == 0) {
                    context.send(`${DATA.Subject.lecture.text} ${DATA.Subject.lecture.link}`)
                }
                break
            case 2:
                // Расписание на вторник
                break
            case 3:
                // Расписание на среду
                break
            case 4:
                // Расписание на четверг
                break
            case 5:
                // Расписание на пятницу
                break
            case 6:
                // Расписание на субботу
                break
        }
    }, 1000)
}

module.exports = {
    run() {
        /**
        * @important Бот должен быть администратором, чтобы этот код работал
        */
        BOT.VK.updates.on('message_new', (context) => {
            BOT.API.messages.getConversationsById({ peer_ids: context.peerId }).then((res) => {
                BOT.CONFIG.ADMIN_ID.map(admin => {
                    if (res.items[0].chat_settings !== undefined && res.items[0].chat_settings.owner_id === admin) {
                        sendDayMessage(context)
                    }
                })
            })
        })
    }
}
