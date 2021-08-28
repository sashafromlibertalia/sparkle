const BOT = require('../vk')

const feedback = BOT.MESSAGES.hear(/^\/отзыв (.+)/i, async (context) => {
    const feedback = context.$match[1]
    await context.send(`Хорошо, твой отзыв будет отправлен ${BOT.CONFIG.NAME_ADMIN_DAT}, спасибо :)`)
    BOT.API.messages.send({
        message: 'НОВЫЙ ОТЗЫВ: ' + feedback,
        domain: BOT.CONFIG.ADMIN_DOMAIN,
        random_id: BOT.RANDOM()
    })
})

module.exports = {
    run() {
        feedback
    }
}
