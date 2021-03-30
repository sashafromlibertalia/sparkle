const BOT = require('./vk')


const hearCommand = (name, conditions, handle) => {
    if (typeof handle !== 'function') {
        handle = conditions;
        conditions = [`/${name}`];
    }
  
    if (!Array.isArray(conditions)) {
        conditions = [conditions];
    }
  
    BOT.MESSAGES.hear(
        [
            (text, {
                state
            }) => (
                state.command === name
            ),
            ...conditions
        ],
        handle
    );
  };
  

BOT.MESSAGES.hear('/игры', async (context) => {
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
  
hearCommand('ball', async (context) => {
    await context.send(`Как играть в эту игру? Очень просто! Ты пишешь "шанc" и свое утверждение, а я отвечаю вероятностью.
  Пример:
  
  — Шанc, что мы - дружный класс.
  — Вероятность - 100%`)
  
    BOT.MESSAGES.hear(/шанс/i, (context) => {
        const chances = new Array(6)
        chances[0] = 'Вероятность близка к нулю :('
        chances[1] = 'Я считаю, что 50 на 50'
        chances[2] = 'Вероятность - 100%'
        chances[3] = 'Я полагаю, что вероятность близка к 100%'
        chances[4] = 'Маловероятно, но шанс есть'
        chances[5] = 'Вероятность нулевая, ничего не поделать'
  
        context.send(chances[Math.floor(Math.random() * chances.length)])
    })
  })
  
hearCommand('else', async (context) => {
    await context.send(`Раз эта кнопка у вас все еще есть, значит я страдаю от острой игровой недостаточности. Если у вас есть идеи, которые может реализовать этот бот в игровой форме - пишите ${BOT.CONFIG.adminNameDat}, он сможет :)`)
})
  
hearCommand('cancel', async (context) => {
    await context.send('Хорошо, я выключу клавиатуру!')
})


