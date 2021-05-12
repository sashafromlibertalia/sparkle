const BOT = require('../vk')
const PARSER = require('./parser')
const Intl = require('intl')
const moment = require('moment')
const Time = new Date()
const formatter = new Intl.DateTimeFormat('ru', {
  month: 'long',
  day: 'numeric'
})

moment().format()

// Клавиатура с днями недели
const weekKeyboard = BOT.KEYBOARD.keyboard([
  [
    BOT.KEYBOARD.textButton({
      label: 'Понедельник',
      payload: {
        command: 'monday'
      },
      color: BOT.KEYBOARD.POSITIVE_COLOR
    }),
    BOT.KEYBOARD.textButton({
      label: 'Вторник',
      payload: {
        command: 'tuesday'
      },
      color: BOT.KEYBOARD.POSITIVE_COLOR
    }),
    BOT.KEYBOARD.textButton({
      label: 'Среда',
      payload: {
        command: 'wednesday'
      },
      color: BOT.KEYBOARD.POSITIVE_COLOR
    })
  ],
  [
    BOT.KEYBOARD.textButton({
      label: 'Четверг',
      payload: {
        command: 'thursday'
      },
      color: BOT.KEYBOARD.POSITIVE_COLOR
    }),
    BOT.KEYBOARD.textButton({
      label: 'Пятница',
      payload: {
        command: 'friday'
      },
      color: BOT.KEYBOARD.POSITIVE_COLOR
    }),
    BOT.KEYBOARD.textButton({
      label: 'Суббота',
      payload: {
        command: 'saturday'
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

BOT.HEARCOMMAND('monday', async (context) => {
  await context.send(`Задания на понедельник:
    ${PARSER.DAYS[0].join('\n')}`)
})

BOT.HEARCOMMAND('tuesday', async (context) => {
  await context.send(`Задания на вторник:
    ${PARSER.DAYS[1].join('\n')}`)
})

BOT.HEARCOMMAND('wednesday', async (context) => {
  await context.send(`${PARSER.DAYS[2].join('\n')}`)
})

BOT.HEARCOMMAND('thursday', async (context) => {
  await context.send(`Задания на четверг:
    ${PARSER.DAYS[3].join('\n')}`)
})

BOT.HEARCOMMAND('friday', async (context) => {
  await context.send(`Задания на пятницу:
    ${PARSER.DAYS[4].join('\n')}`)
})

BOT.HEARCOMMAND('saturday', async (context) => {
  await context.send(`Задания на субботу:
    ${PARSER.DAYS[5].join('\n')}`)
})

const asksCommand = BOT.MESSAGES.hear([/задали/i, /задано/i], async (context) => {
  await context.send({
    message:
			'Я тут увидел, что кто-то из вас спрашивает ДЗ. Выберите, какой день вам нужен:',
    keyboard: weekKeyboard
  })
})

const dateCommand = BOT.MESSAGES.hear('/дата', async (context) => {
  await context.send({
    message: 'Выберите, какой день вам нужен:',
    keyboard: weekKeyboard
  })
})

const tomorrowCommand = BOT.MESSAGES.hear('/дз завтра', async (context) => {
  for (let i = 0; i < 7; i++) {
    if (moment().day() === i) {
      await context.send('Домашка на завтра. Сегодня ' + formatter.format(Time) + ' \n' + PARSER.DAYS[i].join('\n'))
    };
  };
})

const allCommand = BOT.MESSAGES.hear('/дз все', async (context) => {
  await context.send(`${PARSER.DAYS[0].join('\n')}
    ${PARSER.DAYS[1].join('\n')}
    ${PARSER.DAYS[3].join('\n')}
    ${PARSER.DAYS[4].join('\n')}
    ${PARSER.DAYS[5].join('\n')}`)
})

module.exports = {
  run () {
    PARSER.run()
    dateCommand
    asksCommand
    tomorrowCommand
    allCommand
  }
}
