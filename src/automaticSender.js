const DATA = require('./links')
const BOT = require('./vk')
/**
*@note
В данном коде вводите свое расписание, и бот отправит вам рассылку с Zoom
*/
function sendDayMessage (context) {
  const moment = new Date()
  setInterval(() => {
    switch (moment.getDay()) {
      case 1:
        if (moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0) {
          context.send(`${DATA.Programming.lecture.text} ${DATA.Programming.lecture.link}`)
        }
        if (moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0) {
          context.send(`${DATA.Programming.practice.text} ${DATA.Programming.practice.link}`)
        }
        break
      case 2:
        if (
          moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.English.text}`)
        }
        if (
          moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.Math.lecture.text} ${DATA.Math.lecture.link}`)
        }
        if (
          moment.getHours() == 13 &&
					moment.getMinutes() == 28 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.DiscreteMath.practice.text} ${DATA.DiscreteMath.practice.link}`)
        }
        break
      case 3:
        // Надо четность проверять
        if (
          moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.Math.lecture.text} ${DATA.Math.lecture.link}`)
        }
        break
      case 4:
        if (
          moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.Physics.lecture.text} ${DATA.Physics.lecture.link}`)
        } else if (
          moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.Physics.practice.text} ${DATA.Physics.practice.link}`)
        }
        break
      case 5:
        if (
          moment.getHours() == 9 &&
					moment.getMinutes() == 58 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.English.text}`)
        }

        if (
          moment.getHours() == 11 &&
					moment.getMinutes() == 38 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.LinearAlgebra.lecture.text} ${DATA.LinearAlgebra.lecture.link}`)
        }

        if (
          moment.getHours() == 13 &&
					moment.getMinutes() == 28 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.LinearAlgebra.lecture.text} ${DATA.LinearAlgebra.lecture.link}`)
        }
        break
      case 6:
        if (
          moment.getHours() == 13 &&
					moment.getMinutes() == 28 &&
					moment.getSeconds() == 0
        ) {
          context.send(`${DATA.Algorithms.practice.text} ${DATA.Algorithms.practice.link}`)
        }
        break
    }
  }, 1000)
}

module.exports = {
  run () {
    /**
    * @important Бот должен быть администратором, чтобы этот код работал
	  */
    BOT.VK.updates.on('message_new', (context) => {
      BOT.API.messages.getConversationsById({ peer_ids: context.peerId }).then((res) => {
        BOT.CONFIG.ADMIN_ID.map(admin => {
          if (res.items[0].chat_settings.owner_id === admin) {
            sendDayMessage(context)
          }
        })
      })
    })
  }
}
