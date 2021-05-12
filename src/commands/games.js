const BOT = require('../vk')

const gamesCommand = BOT.MESSAGES.hear('/игры', async (context) => {
  const gamesKeyboard = BOT.KEYBOARD.keyboard([
    [
      BOT.KEYBOARD.textButton({
        label: 'Шар Вероятностей',
        payload: {
          command: 'ball'
        },
        color: BOT.KEYBOARD.POSITIVE_COLOR
      }),
      BOT.KEYBOARD.textButton({
        label: 'Что-то еще...',
        payload: {
          command: 'else'
        },
        color: BOT.KEYBOARD.POSITIVE_COLOR
      })
    ],
    BOT.KEYBOARD.textButton({
      label: 'Закрыть клавиатуру',
      payload: {
        command: 'cancel'
      },
      color: BOT.KEYBOARD.NEGATIVE_COLOR
    })
  ]).oneTime()

  await context.send({
    message: 'Вот список моих игр',
    keyboard: gamesKeyboard
  })
})

const ballKCommand = BOT.HEARCOMMAND('ball', async (context) => {
  await context.send(`Как играть в эту игру? Очень просто! Ты пишешь "шанc" и свое утверждение, а я отвечаю вероятностью.
Пример:

— Шанc, что мы - дружный класс.
— Вероятность - 100%`)

  BOT.MESSAGES.hear(/шанс/i, async (context) => {
    const chances = new Array(6)
    chances[0] = 'Вероятность близка к нулю :('
    chances[1] = 'Я считаю, что 50 на 50'
    chances[2] = 'Вероятность - 100%'
    chances[3] = 'Я полагаю, что вероятность близка к 100%'
    chances[4] = 'Маловероятно, но шанс есть'
    chances[5] = 'Вероятность нулевая, ничего не поделать'

    await context.send(chances[Math.floor(Math.random() * chances.length)])
  })
})

const elseKCommand = BOT.HEARCOMMAND('else', async (context) => {
  await context.send(`Раз эта кнопка у вас все еще есть, значит я страдаю от острой игровой недостаточности. Если у вас есть идеи, которые может реализовать этот бот в игровой форме - пишите ${BOT.CONFIG.adminNameDat}, он сможет :)`)
})

const cancelKCommand = BOT.HEARCOMMAND('cancel', async (context) => {
  await context.send('Хорошо, я выключу клавиатуру!')
})

module.exports = {
  run () {
    gamesCommand
    ballKCommand
    elseKCommand
    cancelKCommand
  }
}
