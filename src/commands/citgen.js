const BOT = require('../vk')
const fs = require('fs')
const options = require('../image').options
const download = require('../image').downloadImage

const citgenCommand = BOT.MESSAGES.hear('/citgen', async (context) => {
  if (context.hasReplyMessage && !context.replyMessage.isGroup) {
    await context.send('Citgen одобрен, ща будет ржака')
    let text = context.replyMessage.text
    pic = await BOT.API.users.get({
      user_ids: context.replyMessage.senderId,
      fields: 'photo_200',
      name_case: 'nom'
    })
    download.apply(null, [pic[0], pic[0].photo_200, context, text])
    fs.unlink(options.FINAL_PATH)
    fs.unlink(options.USER_PIC)
    text = null
  } else if (context.hasForwards && !context.forwards[0].isGroup) {
    let text = []
    let img = []
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
    await context.send('Citgen одобрен, ща будет ржака')
    if (img[0][0].photo_200 === undefined) {
      download.apply(null, [img[0][0], img[0][0].photo_200_orig, context, text.join('\n')])
      fs.unlink(options.FINAL_PATH)
      fs.unlink(options.USER_PIC)
      text = null
      img = null
    } else {
      download.apply(null, [img[0][0], img[0][0].photo_200, context, text.join('\n')])
      fs.unlink(options.FINAL_PATH)
      fs.unlink(options.USER_PIC)
      text = null
      img = null
    }
  } else {
    await context.send('А че цитгенить то будем?')
  }
})

module.exports = {
  run () {
    citgenCommand
  }
}
