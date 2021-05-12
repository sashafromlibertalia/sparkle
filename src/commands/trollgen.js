const BOT = require('../vk')
const troll = require('./troll')
const options = require('../image').options
const download = require('../image').downloadImage
const fs = require('fs')


const trollCommand = BOT.MESSAGES.hear('/trollgen', async (context) => {
  if (context.hasReplyMessage && !context.replyMessage.isGroup) {
    await context.send('Trollgen одобрен, ща буит ржяка')
    let text = context.replyMessage.text
    pic = await BOT.API.users.get({
      user_ids: context.replyMessage.senderId,
      fields: 'photo_200',
      name_case: 'nom'
    })
    download.apply(null, [pic[0], pic[0].photo_200, context, troll.rewriteMessage(text)])
    fs.unlink(options.FINAL_PATH)
    fs.unlink(options.USER_PIC)
    text = null
  } else if (context.hasForwards && !context.forwards[0].isGroup) {
    let text = [],
      img = []
    if (context.forwards.length === 1) {
      text[0] = context.forwards[0].text
    }
    for (let i = 0; i < context.forwards.length; i++) {
      if (context.forwards[0].senderId === context.forwards[i].senderId) {
        text[i] = context.forwards[i].text
      } else {
        text = ''
        await context.send(
          'Так! Ошибка! Рофляночка должна принадлежать одному человеку, а не разным'
        )
        return
      }
      img[i] = await BOT.API.users.get({
        user_ids: context.forwards[i].senderId,
        fields: 'photo_200, photo_200_orig',
        name_case: 'nom'
      })
    }
    await context.send('Trollgen одобрен, ща буит ржяка')
    if (img[0][0].photo_200 === undefined) {
      download.apply(null,[img[0][0], img[0][0].photo_200_orig, context, troll.rewriteMessage(text.join('\n'))])
      fs.unlink(options.FINAL_PATH)
      fs.unlink(options.USER_PIC)
      text = null
      img = null
    } else {
      download.apply(null, [img[0][0], img[0][0].photo_200, context, troll.rewriteMessage(text.join('\n'))])
      fs.unlink(options.FINAL_PATH)
      fs.unlink(options.USER_PIC)
      text = null
      img = null
    }
  } else {
    await context.send('А че троллгенить то будем?')
  }
})


module.exports = {
  run () {
    trollCommand
  }
}
