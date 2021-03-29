
const BOT = require('./vk')
const cheerio = require('cheerio')
const request = require('request')

const url = BOT.CONFIG.homeworkParserURL
request(url, async function(err, res, body) {
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

  if (err) throw err

  const $ = cheerio.load(body)
  const Englishdz = $('#LC2').text()
  const Russiandz = $('#LC5').text()
  const Literaturedz = $('#LC8').text()
  const Germandz = $('#LC11').text()
  const Frenchdz = $('#LC14').text()
  const Algebradz = $('#LC17').text()
  const Geometrydz = $('#LC20').text()
  const Biologydz = $('#LC23').text()
  const Chemistrydz = $('#LC26').text()
  const Physicsdz = $('#LC29').text()
  const CompSciencedz = $('#LC32').text()
  const Geographydz = $('#LC35').text()
  const Mhkdz = $('#LC38').text()
  const History_dz = $('#LC41').text()
  const Societydz = $('#LC44').text()
  const OBJdz = $('#LC47').text()
  const DPUAlgebra = $('#LC50').text()
  const AstronomyDZ = $('#LC53').text()

  const predmeti = new Array(18)
  predmeti[0] = $('#LC1').text() // Английский
  predmeti[1] = $('#LC4').text() // Русский
  predmeti[2] = $('#LC7').text() // Литература
  predmeti[3] = $('#LC10').text() // Немецкий
  predmeti[4] = $('#LC13').text() // Французский
  predmeti[5] = $('#LC16').text() // Алгебра
  predmeti[6] = $('#LC19').text() // Геометрия
  predmeti[7] = $('#LC22').text() // Биология
  predmeti[8] = $('#LC25').text() // Химия
  predmeti[9] = $('#LC28').text() // Физика
  predmeti[10] = $('#LC31').text() // Информатика
  predmeti[11] = $('#LC34').text() // География
  predmeti[12] = $('#LC37').text() // МХК
  predmeti[13] = $('#LC40').text() // История
  predmeti[14] = $('#LC43').text() // Обществознание
  predmeti[15] = $('#LC46').text() // ОБЖ
  predmeti[16] = $('#LC49').text() // ДПУ Алгебра
  predmeti[17] = $('#LC52').text() // Астрономия

  const line = $('#LC3').text()

  const Monday = new Array(3)
  Monday[0] = predmeti[2] + Literaturedz + `\n${line}`
  Monday[1] = predmeti[1] + Russiandz + `\n${line}`
  Monday[2] = predmeti[0] + Englishdz + `\n${line}`
  Monday[3] = predmeti[12] + Mhkdz + `\n${line}`

  const Tuesday = new Array(5)
  Tuesday[0] = predmeti[15] + OBJdz + `\n${line}`
  Tuesday[1] = predmeti[9] + Physicsdz + `\n${line}`
  Tuesday[2] = predmeti[14] + Societydz + `\n${line}`
  Tuesday[3] = predmeti[8] + Chemistrydz + `\n${line}`
  Tuesday[4] = predmeti[5] + Algebradz + `\n${line}`

  const Wednesday = new Array(4)
  Wednesday[0] = predmeti[0] + Englishdz + `\n${line}`
  Wednesday[1] = predmeti[5] + Algebradz + `\n${line}`
  Wednesday[2] = predmeti[17] + AstronomyDZ + `\n${line}`
  Wednesday[3] = predmeti[4] + Frenchdz + `\n${line}`

  const Thursday = new Array(6)
  Thursday[0] = predmeti[6] + Geometrydz + `\n${line}`
  Thursday[1] = predmeti[1] + Russiandz + `\n${line}`
  Thursday[2] = predmeti[10] + CompSciencedz + `\n${line}`
  Thursday[3] = predmeti[2] + Literaturedz + `\n${line}`
  Thursday[4] = predmeti[5] + Algebradz + `\n${line}`

  const Friday = new Array(4)
  Friday[0] = predmeti[0] + Englishdz + `\n${line}`
  Friday[1] = predmeti[13] + History_dz + `\n${line}`
  Friday[2] = predmeti[7] + Biologydz + `\n${line}`
  Friday[3] = predmeti[11] + Geographydz + `\n${line}`

  const Saturday = new Array(5)
  Saturday[0] = predmeti[13] + History_dz + `\n${line}`
  Saturday[1] = predmeti[9] + Physicsdz + `\n${line}`
  Saturday[2] = predmeti[6] + Geometrydz + `\n${line}`
  Saturday[3] = predmeti[4] + Frenchdz + `\n${line}`
  Saturday[4] = predmeti[14] + Societydz + `\n${line}`


  const preds = new Array(18)
  preds[0] = {
      namesubj: predmeti[0],
      dz: Englishdz
  }
  preds[1] = {
      namesubj: predmeti[1],
      dz: Russiandz
  }
  preds[2] = {
      namesubj: predmeti[2],
      dz: Literaturedz
  }
  preds[3] = {
      namesubj: predmeti[3],
      dz: Germandz
  }
  preds[4] = {
      namesubj: predmeti[4],
      dz: Frenchdz
  }
  preds[5] = {
      namesubj: predmeti[5],
      dz: Algebradz
  }
  preds[6] = {
      namesubj: predmeti[6],
      dz: Geometrydz
  }
  preds[7] = {
      namesubj: predmeti[7],
      dz: Biologydz
  }
  preds[8] = {
      namesubj: predmeti[8],
      dz: Chemistrydz
  }
  preds[9] = {
      namesubj: predmeti[9],
      dz: Physicsdz
  }
  preds[10] = {
      namesubj: predmeti[10],
      dz: CompSciencedz
  }
  preds[11] = {
      namesubj: predmeti[11],
      dz: Geographydz
  }
  preds[12] = {
      namesubj: predmeti[12],
      dz: Mhkdz
  }
  preds[13] = {
      namesubj: predmeti[13],
      dz: History_dz
  }
  preds[14] = {
      namesubj: predmeti[14],
      dz: Societydz
  }
  preds[15] = {
      namesubj: predmeti[15],
      dz: OBJdz
  }
  preds[16] = {
      namesubj: predmeti[16],
      dz: DPUAlgebra
  }
  preds[17] = {
      namesubj: predmeti[17],
      dz: AstronomyDZ
  }

  const Sunday = new Array(18)
  Sunday[0] = predmeti[0] + preds[0].dz + `\n${line}`
  Sunday[1] = predmeti[1] + preds[1].dz + `\n${line}`
  Sunday[2] = predmeti[2] + preds[2].dz + `\n${line}`
  Sunday[3] = predmeti[13] + preds[13].dz + `\n${line}`
  Sunday[4] = predmeti[10] + preds[10].dz + `\n${line}`
  Sunday[5] = predmeti[7] + preds[7].dz + `\n${line}`
  Sunday[6] = predmeti[5] + preds[5].dz + `\n${line}`
  Sunday[7] = predmeti[11] + preds[11].dz + `\n${line}`
  Sunday[8] = predmeti[6] + preds[6].dz + `\n${line}`
  Sunday[9] = predmeti[14] + preds[14].dz + `\n${line}`
  Sunday[10] = predmeti[9] + preds[9].dz + `\n${line}`
  Sunday[11] = predmeti[8] + preds[8].dz + `\n${line}`
  Sunday[12] = predmeti[12] + preds[12].dz + `\n${line}`
  Sunday[13] = predmeti[4] + preds[4].dz + `\n${line}`
  Sunday[14] = predmeti[3] + preds[3].dz + `\n${line}`
  Sunday[15] = predmeti[15] + preds[15].dz + `\n${line}`
  Sunday[16] = predmeti[16] + preds[16].dz + `\n${line}`
  Sunday[17] = predmeti[17] + preds[17].dz + `\n${line}`

  const Days = [Monday, Tuesday, Wednesday, Thursday, Friday, Saturday]

  BOT.MESSAGES.hear(/^\/добавить ([а-я.]+) (.+)/i, async (context) => {
      const Subject = new RegExp(context.$match[1], 'i')
      const homeWork = context.$match[2]
      const subjects = []
      $('td').each(function(i) {
          subjects[i] = $(this).text()
      })

      // Прохожусь по всем тегам td и нахожу, есть ли там регулярка с каким-нибудь предметом, если да, то выполняю следующее:
      // Прохожусь по массиву предметов и нахожу, есть ли там совпадение с найденным предметом среди них, то делаю следующее:
      // Нахожу нужный объект с предметом и вставляю homework в dz.
      for (let j = 0; j < subjects.length; j++) {
          if (subjects[j].match(Subject)) {
              for (let i = 0; i < predmeti.length; i++) {
                  if (predmeti[i] === subjects[j]) {
                      for (let g = 0; g < preds.length; g++) {
                          if (predmeti[i] === preds[g].namesubj) {
                              preds[g].dz = homeWork
                              await context.send(`ВАЖНО: Главный список по команде /дз останется старым.
📌 НОВОЕ ДЗ: ${preds[g].namesubj + homeWork} 📌`)
                          };
                      };
                  };
              };
          };
      };
  })

  const asks = new Array(2)
  asks[0] = new RegExp(/задано/i)
  asks[1] = new RegExp(/задали/i)

  BOT.MESSAGES.hear(asks, async (context) => {
      await context.send({
          message: 'Я тут увидел, что кто-то из вас спрашивает ДЗ. Выберите, какой день вам нужен:',
          keyboard: weekKeyboard
      })
  })

  BOT.MESSAGES.hear('/дата', async (context) => {
      await context.send({
          message: 'Выберите, какой день вам нужен:',
          keyboard: weekKeyboard
      })
  })

  hearCommand('monday', async (context) => {
      await context.send(`
  Итак, вот домашка на понедельник
${Monday.join('\n')}`)
  })

  hearCommand('tuesday', async (context) => {
      await context.send(`
  Итак, вот домашка на вторник 
${Tuesday.join('\n')}`)
  })

  hearCommand('wednesday', async (context) => {
      await context.send(`
  Итак, вот домашка на среду 
${Wednesday.join('\n')}`)
  })

  hearCommand('thursday', async (context) => {
      await context.send(`
  Итак, вот домашка на четверг
${Thursday.join('\n')}`)
  })

  hearCommand('friday', async (context) => {
      await context.send(`
  Итак, вот домашка на пятницу
${Friday.join('\n')}`)
  })

  hearCommand('saturday', async (context) => {
      await context.send(`
  Итак, вот домашка на субботу 
${Saturday.join('\n')}`)
  })

  BOT.MESSAGES.hear(/^\/понедельник/i, async (context) => {
      context.send(`Домашка на понедельник:
${Monday.join('\n')}`)
  })

  BOT.MESSAGES.hear(/^\/вторник/i, async (context) => {
      context.send(`Домашка на вторник:
${Tuesday.join('\n')}`)
  })

  BOT.MESSAGES.hear(/^\/среда/i, async (context) => {
      context.send(`Домашка на среду:
${Wednesday.join('\n')}`)
  })

  BOT.MESSAGES.hear(/^\/четверг/i, async (context) => {
      context.send(`Домашка на четверг:
${Thursday.join('\n')}`)
  })

  BOT.MESSAGES.hear(/^\/пятница/i, async (context) => {
      context.send(`Домашка на пятницу:
${Friday.join('\n')}`)
  })

  BOT.MESSAGES.hear(/^\/суббота/i, async (context) => {
      context.send(`Домашка на cубботу:
${Saturday.join('\n')}`)
  })

  BOT.MESSAGES.hear('/добавить ?', async (context) => {
      await context.send(`
Справка по команде /добавить.
Она позволяет добавлять домашнее задание для каждого предмета моментально
Итак, как она работает?
Вы пишите: /insert название_предмета сама_домашка
Затем бот отправит вам обновленное дз по вашему предмету, и все будут счастливы!
Всем мир`)
  })

  BOT.MESSAGES.hear('/дз все', async (context) => {
      await context.send(`
ПОНЕДЕЛЬНИК:
${Monday.join('\n')}

ВТОРНИК:
${Tuesday.join('\n')}

СРЕДА:
${Wednesday.join('\n')}

ЧЕТВЕРГ:
${Thursday.join('\n')}

ПЯТНИЦА: 
${Friday.join('\n')}

СУББОТА:
${Saturday.join('\n')}`)
  })

  BOT.MESSAGES.hear('/дз', async (context) => {
      for (i = 1; i < 7; i++) {
          if (moment().day() === i) {
              await context.send('Домашка с текущего дня (' + formatter.format(Time) + ') \n' + Days[i - 1].join('\n'))
          };
      };

      if (moment().day() === 0) {
          await context.send('Поздравляю с единственным выходным. Проведите его с пользой. Домашка на всю неделю: ' + formatter.format(Time) + ' \n' + Sunday.join('\n'))
      };
  })

  BOT.MESSAGES.hear('/дз завтра', async (context) => {
      for (i = 0; i < 7; i++) {
          if (moment().day() === i) {
              await context.send('Домашка на завтра. Сегодня ' + formatter.format(Time) + ' \n' + Days[i].join('\n'))
          };
      };
  })
})
