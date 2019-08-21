const { VK } = require('vk-io')
const vk = new VK()
const config = require('./config')
const { updates } = vk
const { api } = vk
const request = require('request')
const gm = require('gm').subClass({ imageMagick: true })
const fs = require('fs')

vk.setOptions({
  token: config.TOKEN,
  pollingGroupId: config.poullingGroupID,
  peer_id: config.peerID
})

const citgen = updates.hear('/citgen', async (context) => {
  console.log(context)
  if (context.hasReplyMessage) {
    var text = []
    var imagekek = []
    await context.send('Citgen одобрен, ща будет ржака')

    text = context.replyMessage.text

    imagekek = await api.users.get({
      user_ids: context.replyMessage.senderId,
      fields: 'photo_200',
      name_case: 'nom'
    })

    console.log(imagekek[0])

    var download = function (uri, filename, callback) {
      request.head(uri, function (err, res, body) {
			  console.log('content-type:', res.headers['content-type'])
			  console.log('content-length:', res.headers['content-length'])
			  request(uri).pipe(fs.createWriteStream(filename)).on('close', callback)
      })
    }

    download(imagekek[0].photo_200, 'ava.png', function () {
      console.log('done')
      gm(640, 400, '#000000')
        .fill('#FFFFFF')
        .font('HelveticaNeue.ttf')
        .fontSize(30)
        .drawText(30, 42, 'Золотые слова')
        .in('-page', '+30+85')
        .in('ava.png')
        .fontSize(20)
        .drawText(260, 110, `«${text}»`)
        .fontSize(30)
        .drawText(30, 370, `© ${imagekek[0].first_name} ${imagekek[0].last_name}`)
        .mosaic()
        .write('rofl.png', async function (err) {
          if (err) {
            console.log(err)
          }
          await context.sendPhoto('rofl.png')
          await fs.unlink('rofl.png')
        })
    })
  } else if (context.hasForwards) {
    var text = []
    var imagekek = []
    await context.send('Citgen одобрен, ща будет ржака')

    if (context.forwards.length === 1) {
      text[0] = context.forwards[0].text
    }
    for (var i = 0; i < context.forwards.length; i++) {
      for (var j = 1; j < context.forwards.length; j++) {
        if (context.forwards[i].from_id === context.forwards[j].from_id) {
          text[i] = context.forwards[i].text
        } else {
          text = ''
          await context.send('Так! Ошибка! Рофляночка должна принадлежать одному человеку, а не разным')
          break
        }
      }
      imagekek[i] = await api.users.get({
        user_ids: context.forwards[i].senderId,
        fields: 'photo_200, photo_200_orig',
        name_case: 'nom'
      })
    }

    var download = function (uri, filename, callback) {
      request.head(uri, function (err, res, body) {
			  console.log('content-type:', res.headers['content-type'])
			  console.log('content-length:', res.headers['content-length'])
			  request(uri).pipe(fs.createWriteStream(filename)).on('close', callback)
      })
    }

    console.log(imagekek[0][0].photo_200)
    console.log(imagekek[0][0].photo_200_orig)

    if (imagekek[0][0].photo_200 === undefined) {
      download(imagekek[0][0].photo_200_orig, 'ava.png', function () {
        console.log('done')
        gm(640, 400, '#000000')
          .fill('#FFFFFF')
          .font('HelveticaNeue.ttf')
          .fontSize(30)
          .drawText(30, 42, 'Золотые слова')
          .in('-page', '+30+80')
          .in('ava.png')
          .fontSize(20)
          .drawText(260, 110, `«${text.join('\n')}»`)
          .fontSize(30)
          .drawText(30, 370, `© ${imagekek[0][0].first_name} ${imagekek[0][0].last_name}`)
          .mosaic()
          .write('rofl.png', async function (err) {
            if (err) {
              console.log(err)
            }
            await context.sendPhoto('rofl.png')
            await fs.unlink('rofl.png')
          })
      })
    } else {
      download(imagekek[0][0].photo_200, 'ava.png', function () {
        console.log('done')
        gm(640, 400, '#000000')
          .fill('#FFFFFF')
          .font('HelveticaNeue.ttf')
          .fontSize(30)
          .drawText(30, 42, 'Золотые слова')
          .in('-page', '+30+85')
          .in('ava.png')
          .fontSize(20)
          .drawText(260, 110, `«${text.join('\n')}»`)
          .fontSize(30)
          .drawText(30, 370, `© ${imagekek[0][0].first_name} ${imagekek[0][0].last_name}`)
          .mosaic()
          .write('rofl.png', async function (err) {
            if (err) {
              console.log(err)
            }
            await context.sendPhoto('rofl.png')
            await fs.unlink('rofl.png')
          })
      })
    }
  } else {
    await context.send('А че цитгенить то будем?')
  }
})

module.exports = citgen
