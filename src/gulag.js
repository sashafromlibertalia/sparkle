const { VK } = require('vk-io')
const vk = new VK()
const config = require('./config')
const { updates } = vk
const { api } = vk

vk.setOptions({
  token: config.TOKEN,
  pollingGroupId: config.poullingGroupID,
  peer_id: config.peerID
})

const gulag = updates.hear(/^\/вгулаг (.+)/i, async (context) => {
  const victim = context.$match[1]
  if (context.senderId === config.adminID) {
    if (isNaN(victim)) {
      const [user] = await api.users.get({
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

module.exports = gulag
