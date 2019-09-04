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
      list.push({
        text: category,
        attachment: attached.largePhoto
      })
      console.log(list)
    } else {
      await context.send('Ошибка! Чтобы добавить шпору, прикрепите документ или фотографию к сообщению')
    }
})

const shporaGet = updates.hear(/^\/шпора (.+)/i, async(context) => {
  let item = context.$match[1]
  for (let i = 0; i < list.length; i++) {
    if (item === list[i].text) {
      upload.messagePhoto({
        uploadUrl: list[i].attachment,
        peer_id: config.peerID
      })
    }
  }
})

const shporaList = updates.hear('/шпора список', async(context) => {
  let message = ''
  for (let i = 0; i < list.length; i++) {
    message += `${i+1}. ${list[i].text}\n`
  }
  await context.send(message)
})


module.exports = shpora
module.exports = shporaList
module.exports = shporaGet