const request = require('request')
const fs = require('fs')
const { createCanvas, loadImage } = require("canvas")

// Параметры картинки
const options = {
    WIDTH: 640,
    HEIGHT: 400,
    BACKGROUND: '#000000',
    FILL: '#FFFFFF',
    FONT: 'assets/HelveticaNeue.ttf',
    USER_PIC: 'images/ava.png',
    FINAL_PATH: 'images/res.png',
    TITLE: 'Золотые слова'
}

/**
 * Данная функция нужна для скачивания аватара пользователя
 * @param {string} uri - ссылка на аватарку
 * @param {string} filename - путь к картинке
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
 * @param {object} user объект, необходимый для получения имени и фамилии пользователя
 * @param {string} url ссылка на картинку
 * @param {*} context наш контекст, чтобы бот понимал, кому надо ответить
 * @param {string} text сообщение, которое нужно обработать
 */
const downloadImage = (user, url, context, text) => {
    const canvas = createCanvas(options.WIDTH, options.HEIGHT)
    const ctx = canvas.getContext('2d')

    // Скачиваю аватарку
    download(url, options.USER_PIC, () => {
        ctx.fillStyle = options.BACKGROUND
        ctx.fillRect(0, 0, options.WIDTH, options.HEIGHT)
        ctx.font = "bold 28px Helvetica"
        ctx.fillStyle = options.FILL

        ctx.fillText(options.TITLE, 30, 42)
        ctx.fillText(`© ${user.first_name} ${user.last_name}`, 30, 370)

        ctx.font = "bold 20px Helvetica"
        ctx.fillText(`«${text}»`, 260, 110)

        loadImage(url).then(image => {
            ctx.drawImage(image, 30, 85)
            const buffer = canvas.toBuffer("image/png")
            fs.writeFileSync(options.FINAL_PATH, buffer)
            
            context.sendPhotos({
                value: options.FINAL_PATH
            })
        })
    })
}

module.exports.options = options
module.exports.downloadImage = downloadImage
