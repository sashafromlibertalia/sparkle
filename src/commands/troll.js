const BOT = require('../vk')

const matches = [
  {
    string: /ог/gi,
    replace: 'ох'
  },
  {
    string: /рь/gi,
    replace: 'йь'
  },
  {
    string: /гов/gi,
    replace: 'гяв'
  },
  {
    string: /аж/gi,
    replace: 'яз'
  },
  {
    string: /на/gi,
    replace: 'ня'
  },
  {
    string: /ла/gi,
    replace: 'ля'
  },
  {
    string: /шу/gi,
    replace: 'шю'
  },
  {
    string: /жи/gi,
    replace: 'жы'
  },
  {
    string: /ши/gi,
    replace: 'шы'
  },
  {
    string: /лу/gi,
    replace: 'лю'
  },
  {
    string: /рок/gi,
    replace: 'рйок'
  },
  {
    string: /кря/gi,
    replace: 'кья'
  },
  {
    string: /ря/gi,
    replace: 'ля'
  },
  {
    string: /кр/gi,
    replace: 'кл'
  },
  {
    string: /ред/gi,
    replace: 'ьед'
  },
  {
    string: /как/gi,
    replace: 'кяк'
  },
  {
    string: /енных/gi,
    replace: 'ених'
  },
  {
    string: /ать/gi,
    replace: 'ять'
  },
  {
    string: /ас/gi,
    replace: 'яс'
  },
  {
    string: /ра/gi,
    replace: 'ья'
  },
  {
    string: /рав/gi,
    replace: 'ьяв'
  },
  {
    string: /чк/gi,
    replace: 'чьк'
  },
  {
    string: /чн/gi,
    replace: 'чьн'
  },
  {
    string: /ль/gi,
    replace: 'й'
  },
  {
    string: /гу/gi,
    replace: 'гю'
  },
  {
    string: /су/gi,
    replace: 'сю'
  },
  {
    string: /еб/gi,
    replace: 'йоб'
  },
  {
    string: /тся/gi,
    replace: 'ться'
  },
  {
    string: /оо/gi,
    replace: 'а'
  },
  {
    string: /ал/gi,
    replace: 'ял'
  },
  {
    string: /ром/gi,
    replace: 'ьом'
  },
  {
    string: /олб/gi,
    replace: 'об'
  },
  {
    string: /лять/gi,
    replace: 'ьять'
  },
  {
    string: /жа/gi,
    replace: 'жя'
  },
  {
    string: /удет/gi,
    replace: 'уит'
  },
  {
    string: /ет/gi,
    replace: 'ит'
  },
  {
    string: /чт/gi,
    replace: 'щт'
  },
  {
    string: /ох/gi,
    replace: 'ёх'
  },
  {
    string: /эле/gi,
    replace: 'эе'
  },
  {
    string: /вре/gi,
    replace: 'вье'
  },
  {
    string: /ых/gi,
    replace: 'ьих'
  },
  {
    string: /чис/gi,
    replace: 'чьс'
  },
  {
    string: /цит/gi,
    replace: 'сит'
  },
  {
    string: /цы/gi,
    replace: 'си'
  },
  {
    string: /го/gi,
    replace: 'си'
  },
  {
    string: /ли/gi,
    replace: 'ль'
  },
  {
    string: /гр/gi,
    replace: 'гл'
  },
  {
    string: /лн/gi,
    replace: 'ьн'
  },
  {
    string: /ту/gi,
    replace: 'тю'
  },
  {
    string: /те/gi,
    replace: 'ти'
  },
  {
    string: /ры/gi,
    replace: 'ьы'
  },
  {
    string: /за/gi,
    replace: 'зя'
  },
  {
    string: /мат/gi,
    replace: 'мят'
  },
  {
    string: /уж/gi,
    replace: 'юш'
  },
  {
    string: /ну/gi,
    replace: 'ню'
  },
  {
    string: /мам/gi,
    replace: 'мямь'
  },
  {
    string: /оп/gi,
    replace: 'ёп'
  },
  {
    string: /ут/gi,
    replace: 'ють'
  },
  {
    string: /тан/gi,
    replace: 'тян'
  },
  {
    string: /зыв/gi,
    replace: 'зивъ'
  },
  {
    string: /орош/gi,
    replace: 'ороф'
  },
  {
    string: /уст/gi,
    replace: 'ьюс'
  },
  {
    string: /уж/gi,
    replace: 'юш'
  },
  {
    string: /ерз/gi,
    replace: 'ьерс'
  },
  {
    string: /изик/gi,
    replace: 'иьик'
  },
  {
    string: /тик/gi,
    replace: 'ьик'
  },
  {
    string: /дем/gi,
    replace: 'им'
  }
]

const rewrite = function rewriteMessage (message) {
  for (let i = 0; i < matches.length; i++) {
    if (message.match(matches[i].string)) {
      message = message.replace(matches[i].string, matches[i].replace)
    }
  }
  return message
}

const troll = BOT.MESSAGES.hear('/тролль', async (context) => {
  if (context.hasReplyMessage) {
    let msg = context.message.reply_message.text
    msg = rewriteMessage(msg)
    await context.send(msg)
  } else if (context.hasForwards) {
    const text = []
    if (context.forwards.length === 1) {
      text[0] = context.forwards[0].text
    }
    for (let i = 0; i < context.forwards.length; i++) {
      text[i] = rewriteMessage(context.forwards[i].text)
    }
    await context.send(`${text.join('\n')}`)
  } else {
    await context.send('А каво трйолить то буим?')
  }
})

module.exports = {
  run () {
    troll
  }
}
module.exports.rewriteMessage = rewrite
