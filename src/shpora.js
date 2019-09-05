const { VK } = require('vk-io')
const vk = new VK()
const config = require('./config')
const { updates } = vk
const { upload } = vk

vk.setOptions({
  token: config.TOKEN,
  pollingGroupId: config.poullingGroupID,
  peer_id: config.peerID
})

let list = []

const shpora = updates.hear(/^\/шпора добавить (.+)/i, async(context) => {
    let category = context.$match[1]
    if (context.hasAttachments('photo')) {
      const [attached] = context.getAttachments('photo')
      console.log(attached)
      list.push({
        text: category,
        attachment: attached.largePhoto
      })
      await context.send('Добавлено ✅')
      console.log(list)
    } else {
      await context.send('Ошибка! Чтобы добавить шпору, прикрепите документ или фотографию к сообщению')
    }
})



const shporaList = updates.hear('/шпора список', async(context) => {
  let message = ''
  for (let i = 0; i < list.length; i++) {
    message += `${i+1}. ${list[i].text}\n`
  }
  if (list.length === 0) {
    message = 'Список пуст'
  }
  await context.send(message)
})



const shporaGet = updates.hear(/^\/шпора (.+)/i, async(context) => {
  let item = context.$match[1]
  for (let i = 0; i < list.length; i++) {
    if (item === list[i].text) {
      context.sendPhoto(list[i].attachment)
    } else {
      context.send('Такой шпоры не существует :(')
    }
  }
})


module.exports = shpora
module.exports = shporaList
module.exports = shporaGet