const BOT = require('../vk')
const listText = []
const listAttachment = []

function isNumber (n) {
  return !isNaN(parseFloat(n)) && !isNaN(n - 0)
}

/**
* @note
Я бы здесь добавил БД, например mongoose, чтобы шпоры не удалялись после перегрузки бота
*/
const hear = BOT.MESSAGES.hear(/^\/шпора добавить (.+)/i, async (context) => {
  const category = context.$match[1]
  if (context.hasAttachments('photo')) {
    const [attached] = context.getAttachments('photo')
    listText.push(category)
    listAttachment.push(attached.largePhoto)
    await context.send('Добавлено ✅')
  } else if (context.hasAttachments('doc')) {
    const [attached] = context.getAttachments('doc')
    listText.push(category)
    listAttachment.push(attached.url)
    await context.send('Добавлено ✅')
  } else {
    await context.send(
      'Ошибка! Чтобы добавить шпору, прикрепите документ или фотографию к сообщению'
    )
  }
})

const list = BOT.MESSAGES.hear('/шпора список', async (context) => {
  let message = ''
  for (let i = 0; i < listText.length; i++) {
    message += `${i + 1}. ${listText[i]}\n`
  }
  if (listText.length === 0) {
    message = 'Список пуст'
  }
  await context.send(message)
})

const remove = BOT.MESSAGES.hear(/^\/шпора удалить (.+)/i, async (context) => {
  const item = context.$match[1]
  if (listText.includes(item)) {
    listText.splice(listText.indexOf(item))
    listAttachment.splice(listText.indexOf(item))
    setTimeout(function () {
      context.send('Удалено')
    }, 500)
  } else {
    setTimeout(function () {
      context.send('Такой шпоры не существует :(')
    }, 800)
  }
})

const get = BOT.MESSAGES.hear(/^\/шпора (.+)/i, async (context) => {
  const item = context.$match[1]
  if (listText.includes(item)) {
    context.sendPhoto(listAttachment[listText.indexOf(item)])
  } else if (isNumber(item)) {
    context.sendPhoto(listAttachment[item - 1])
  } else {
    setTimeout(function () {
      context.send('Такой шпоры не существует :(')
    }, 500)
  }
})

module.exports = {
  run () {
    hear
    list
    remove
    get
  }
}
