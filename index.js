const {VK} = require('vk-io');
const {MessageContext} = require('vk-io');
const fs = require('fs')
const vk = new VK();
const {api} = vk;
const {updates} = vk;
const {upload} = vk;


//Не трогать
const TOKEN = "e74e42966fb9a1e8ab1354ab4721881369665a16367e044c005920b5220827e17ca9894b56412ea2e2891"

vk.setOptions({
    token: TOKEN
})
