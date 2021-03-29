const BOT = require('./vk')

const gulag = BOT.MESSAGES.hear(/^\/вгулаг (.+)/i, async (context) => {
  const victim = context.$match[1]
  if (context.senderId === config.adminID) {
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
