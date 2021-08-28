const BOT = require('../vk')
const kickerCommand = BOT.MESSAGES.hear(/^\/вгулаг (.+)/i, async (context) => {
    const victim = context.$match[1]
    BOT.CONFIG.ADMIN_ID.map(async (id) => {
        if (context.senderId === id) {
            /**
            @note Условие выполнится, если кикается пользователь по строковой ссылке
            */
            if (isNaN(victim)) {
                const [user] = await BOT.API.users.get({
                    user_ids: victim,
                    name_case: 'nom'
                })
                await context.send('ГУЛАГ тебя ждет, братишка')
                await BOT.API.messages.removeChatUser({
                    chat_id: context.chatId,
                    user_id: user.id
                })
            } else {
                await context.send('ГУЛАГ тебя ждет, братишка')
                await BOT.API.messages.removeChatUser({
                    chat_id: context.chatId,
                    user_id: victim
                })
            }
        }
    })
})

module.exports = {
    run() {
        kickerCommand
    }
}
