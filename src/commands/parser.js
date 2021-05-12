const BOT = require('../vk')
const cheerio = require('cheerio')
const request = require('request')

// Массивы для каждого дня недели с домашкой
const MONDAY = []
const TUESDAY = []
const WEDNESDAY = []
const THURSDAY = []
const FRIDAY = []
const SATURDAY = []

const DAYS = [MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY]

function configureMonday () {
  DAYS[0][0] = arguments[0].PROGRAMMING + arguments[1].PROGRAMMING + `\n${arguments[2]}`
}

function configureTuesday () {
  DAYS[1][0] = arguments[0].DISCRETE_MATH + arguments[1].DISCRETE_MATH + `\n${arguments[2]}`
}

function configureWednesday () {
  DAYS[2][0] = 'Ничего, у нас лекции в этот день.'
}

function configureThursday () {
  DAYS[3][0] = arguments[0].PHYSICS + arguments[1].PHYSICS + `\n${arguments[2]}`
  DAYS[3][1] = arguments[0].LINEAR_ALGEBRA + arguments[1].LINEAR_ALGEBRA + `\n${arguments[2]}`
}

function configureFriday () {
  DAYS[4][0] = arguments[0].MATH_ANALYSYS + arguments[1].MATH_ANALYSYS + `\n${arguments[2]}`
}

function configureSaturday () {
  DAYS[5][0] = arguments[0].ALGORITHMS + arguments[1].ALGORITHMS + `\n${arguments[2]}`
}

const url = BOT.CONFIG.PARSER_URL
const parseCommand = request(url, async function (err, res, body) {
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
  run () {
    parseCommand
  }
}
module.exports.DAYS = DAYS
