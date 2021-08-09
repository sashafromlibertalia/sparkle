const request = require('request')
const BOT = require('./vk')
const fs = require('fs')
const gm = require('gm').subClass({
  imageMagick: true
})

/**
 * Параметры картинки
 */
const options = {
  WIDTH: 640,
  HEIGHT: 400,
  BACKGROUND: '#000000',
  FILL: '#FFFFFF',
  FONT: 'assets/HelveticaNeue.ttf',
  USER_PIC: 'images/ava.png',
  FINAL_PATH: 'images/rofl.png',
  TITLE: 'Золотые слова'
}

/**
 * Данная функция нужна для скачивания аватара пользователя
 * @param {*} uri - ссылка на аватарку
 * @param {*} filename - путь к картинке
 * @param {*} callback - колбэк
 */
const download = (uri, filename, callback) => {
  request.head(uri, (err, res, body) => {
    if (err) throw err
    request(uri).pipe(fs.createWriteStream(filename)).on('close', callback)
  })
}

/**
 * Данная функция отправляет уже готовую картинку пользователю
 * @param {*} img объект, необходимый для получения имени и фамилии пользователя
 * @param {*} url ссылка на картинку
 * @param {*} context наш контекст, чтобы бот понимал, кому надо ответить
 * @param {*} text сообщение, которое нужно обработать
 */
const downloadImage = (img, url, context, text) => {
  download(url, options.USER_PIC, () => {
    gm(options.WIDTH, options.HEIGHT, options.BACKGROUND)
      .fill(options.FILL)
      .font(options.FONT)
      .fontSize(30)
      .drawText(30, 42, options.TITLE)
      .in('-page', '+30+85')
      .in(options.USER_PIC)
      .fontSize(20)
      .drawText(260, 110, `«${text}»`)
      .fontSize(30)
      .drawText(30, 370, `© ${img.first_name} ${img.last_name}`)
      .mosaic()
      .write(options.FINAL_PATH, async (err) => {
        if (err) console.error(err)

        const attach = await BOT.VK.upload.messagePhoto({
          source: {
            value: options.FINAL_PATH
          }
        })
        await context.send({
          attachment: attach.toString()
        })
      })
  })
}

module.exports.options = options
module.exports.downloadImage = downloadImage
