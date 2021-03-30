const BOT = require('./vk')
const kicker = BOT.MESSAGES.hear(/^\/вгулаг (.+)/i, async (context) => {
  const victim = context.$match[1]
  if (context.senderId === BOT.CONFIG.ADMIN_ID) {
      if (isNaN(victim)) {
          const [user] = await BOT.API.users.get({
              user_ids: victim,
              name_case: 'nom'
          })
          await context.send('ГУЛАГ тебя ждет, братишка')
          await context.kickUser(user.id)
      } else {
          await context.send('ГУЛАГ тебя ждет, братишка')
          await context.kickUser(victim)
      }
  } else {
      await context.send('Упс, ошибочка. У вас нет доступа к этой команде')
  }
})

module.exports = {
    run: function() {
       kicker
    }
}