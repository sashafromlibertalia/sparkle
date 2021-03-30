const BOT = require('../vk')
const cheerio = require('cheerio')
const request = require('request')

const url = BOT.CONFIG.homeworkParserURL
const parseCommand = request(url, async function(err, res, body) {
  if (err) {
      throw err
  }

  const $ = cheerio.load(body)
  
  const HOMEWORKS = {
    MATH_ANALYSYS: $('#LC2').text(),
    DISCRETE_MATH: $('#LC5').text(),
    PHYSICS: $('#LC8').text(),
    LINEAR_ALGEBRA: $('#LC11').text(),
    ALGORITHMS: $('#LC14').text(),
    PROGRAMMING: $('#LC17').text(),
  }

  const SUBJECTS = {
      MATH_ANALYSYS: $('#LC1').text(),
      DISCRETE_MATH: $('#LC4').text(),
      PHYSICS: $('#LC7').text(),
      LINEAR_ALGEBRA: $('#LC10').text(),
      ALGORITHMS: $('#LC13').text(),
      PROGRAMMING: $('#LC16').text(),
  }
  const SEPARATOR = $('#LC3').text()

})

module.exports = {
    run: function() {
        parseCommand
    }
}