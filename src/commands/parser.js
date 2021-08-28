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

const configureMonday = () => {
    DAYS[0][0] = arguments[0].PROGRAMMING + arguments[1].PROGRAMMING + `\n${arguments[2]}`
}

const configureTuesday = () => {
    DAYS[1][0] = arguments[0].DISCRETE_MATH + arguments[1].DISCRETE_MATH + `\n${arguments[2]}`
}

const configureWednesday = () => {
    DAYS[2][0] = 'Ничего, у нас лекции в этот день.'
}

const configureThursday = () => {
    DAYS[3][0] = arguments[0].PHYSICS + arguments[1].PHYSICS + `\n${arguments[2]}`
    DAYS[3][1] = arguments[0].LINEAR_ALGEBRA + arguments[1].LINEAR_ALGEBRA + `\n${arguments[2]}`
}

const configureFriday = () => {
    DAYS[4][0] = arguments[0].MATH_ANALYSYS + arguments[1].MATH_ANALYSYS + `\n${arguments[2]}`
}

const configureSaturday = () => {
    DAYS[5][0] = arguments[0].ALGORITHMS + arguments[1].ALGORITHMS + `\n${arguments[2]}`
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
