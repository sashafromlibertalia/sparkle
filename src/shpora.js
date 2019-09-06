const { VK } = require('vk-io')
const vk = new VK()
const config = require('./config')
const { updates } = vk

vk.setOptions({
  token: config.TOKEN,
  pollingGroupId: config.poullingGroupID,
  peer_id: config.peerID
})

let listText = []
let listPhoto = []

const shpora = updates.hear(/^\/шпора добавить (.+)/i, async(context) => {
    let category = context.$match[1]
    if (context.hasAttachments('photo')) {
      const [attached] = context.getAttachments('photo')
      listText.push(category)
      listPhoto.push(attached.largePhoto)
      await context.send('Добавлено ✅')
    } else {
      await context.send('Ошибка! Чтобы добавить шпору, прикрепите документ или фотографию к сообщению')
    }
})



const shporaList = updates.hear('/шпора список', async(context) => {
  let message = ''
  for (let i = 0; i < listText.length; i++) {
    message += `${i+1}. ${listText[i]}\n`
  }
  if (listText.length === 0) {
    message = 'Список пуст'
  }
  await context.send(message)
})

const shporaRemove = updates.hear(/^\/шпора удалить (.+)/i, async(context) => {
  let item = context.$match[1]
  if (listText.includes(item)) {
    listText.splice(i)
      setTimeout(function() {
        context.send('Удалено')
      }, 500)
  } else {
    setTimeout(function() {
      context.send('Такой шпоры не существует :(')
    }, 800)
  }
})

const shporaGet = updates.hear(/^\/шпора (.+)/i, async(context) => {
  let item = context.$match[1]
  if (listText.includes(item)) {
    context.sendPhoto(listPhoto[listText.indexOf(item)])
  } else {
    setTimeout(function() {
      context.send('Такой шпоры не существует :(')
    }, 500)
  }
})




module.exports = shpora
module.exports = shporaList
module.exports = shporaGet
module.exports = shporaRemove