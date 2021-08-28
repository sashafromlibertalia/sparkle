const BOT = require('../vk')
const cheerio = require('cheerio')
const request = require('request')

// Массивы для каждого дня недели с домашкой
let MONDAY = []
let TUESDAY = []
let WEDNESDAY = []
let THURSDAY = []
let FRIDAY = []
let SATURDAY = []

const DAYS = [MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY]

const configureMonday = (DATA, TITLES, SEPARATOR) => {
    DAYS[0][0] = DATA.PROGRAMMING + TITLES.PROGRAMMING + `\n${SEPARATOR}`
}

const configureTuesday = (DATA, TITLES, SEPARATOR) => {
    DAYS[1][0] = DATA.DISCRETE_MATH + TITLES.DISCRETE_MATH + `\n${SEPARATOR}`
}

const configureWednesday = () => {
    DAYS[2][0] = 'Ничего, у нас лекции в этот день.'
}

const configureThursday = (DATA, TITLES, SEPARATOR) => {
    DAYS[3][0] = DATA.PHYSICS + TITLES.PHYSICS + `\n${SEPARATOR}`
    DAYS[3][1] = DATA.LINEAR_ALGEBRA + TITLES.LINEAR_ALGEBRA + `\n${SEPARATOR}`
}

const configureFriday = (DATA, TITLES, SEPARATOR) => {
    DAYS[4][0] = DATA.MATH_ANALYSYS + TITLES.MATH_ANALYSYS + `\n${SEPARATOR}`
}

const configureSaturday = (DATA, TITLES, SEPARATOR) => {
    DAYS[5][0] = DATA.ALGORITHMS + TITLES.ALGORITHMS + `\n${SEPARATOR}`
}

/**
* Мне было лень править свой код, но суть такая:
Я с Github произвожу парсинг построчно содержимого каждого предмета (чтобы еще стало понятнее, см. domashka.txt в репозитории)

ПОЧЕМУ GitHub?
Отвечаю - если зайти в браузере в режим разработчика, то можно увидеть, что разметка файла domashka.txt крайне удобна, чтобы парсить ее - каждая строчка текстового
файла помечена уникальным ID, обрабатывать который не составит большого труда. На данный момент я не знаю сторонних ресурсов, кто мог предложить нечто удобнее или похожее (ну, кроме
баз данных, но это уже совсем другая история)
*/
const parseCommand = request(BOT.CONFIG.PARSER_URL, async function (err, res, body) {
    if (err) {
        throw err
    }

    const $ = cheerio.load(body)

    const DATA = {
        MATH_ANALYSYS: $('#LC1').text(),
        DISCRETE_MATH: $('#LC4').text(),
        PHYSICS: $('#LC7').text(),
        LINEAR_ALGEBRA: $('#LC10').text(),
        ALGORITHMS: $('#LC13').text(),
        PROGRAMMING: $('#LC16').text()
    }
    const TITLES = {
        MATH_ANALYSYS: $('#LC2').text(),
        DISCRETE_MATH: $('#LC5').text(),
        PHYSICS: $('#LC8').text(),
        LINEAR_ALGEBRA: $('#LC11').text(),
        ALGORITHMS: $('#LC14').text(),
        PROGRAMMING: $('#LC17').text()
    }

    const SEPARATOR = $('#LC3').text()
    configureMonday(DATA, TITLES, SEPARATOR)
    configureTuesday(DATA, TITLES, SEPARATOR)
    configureWednesday()
    configureThursday(DATA, TITLES, SEPARATOR)
    configureFriday(DATA, TITLES, SEPARATOR)
    configureSaturday(DATA, TITLES, SEPARATOR)
    
})

module.exports = {
    run() {
        parseCommand
    }
}
module.exports.DAYS = DAYS