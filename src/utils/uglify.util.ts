interface ITroll {
    regex: RegExp,
    replaceTo: string,
}

const matches: ITroll[] = [
    {
        regex: /ог/gi,
        replaceTo: 'ох'
    },
    {
        regex: /рь/gi,
        replaceTo: 'йь'
    },
    {
        regex: /гов/gi,
        replaceTo: 'гяв'
    },
    {
        regex: /аж/gi,
        replaceTo: 'яз'
    },
    {
        regex: /на/gi,
        replaceTo: 'ня'
    },
    {
        regex: /ла/gi,
        replaceTo: 'ля'
    },
    {
        regex: /шу/gi,
        replaceTo: 'шю'
    },
    {
        regex: /жи/gi,
        replaceTo: 'жы'
    },
    {
        regex: /ши/gi,
        replaceTo: 'шы'
    },
    {
        regex: /лу/gi,
        replaceTo: 'лю'
    },
    {
        regex: /рок/gi,
        replaceTo: 'рйок'
    },
    {
        regex: /кря/gi,
        replaceTo: 'кья'
    },
    {
        regex: /ря/gi,
        replaceTo: 'ля'
    },
    {
        regex: /кр/gi,
        replaceTo: 'кл'
    },
    {
        regex: /ред/gi,
        replaceTo: 'ьед'
    },
    {
        regex: /как/gi,
        replaceTo: 'кяк'
    },
    {
        regex: /енных/gi,
        replaceTo: 'ених'
    },
    {
        regex: /ать/gi,
        replaceTo: 'ять'
    },
    {
        regex: /ас/gi,
        replaceTo: 'яс'
    },
    {
        regex: /ра/gi,
        replaceTo: 'ья'
    },
    {
        regex: /рав/gi,
        replaceTo: 'ьяв'
    },
    {
        regex: /чк/gi,
        replaceTo: 'чьк'
    },
    {
        regex: /чн/gi,
        replaceTo: 'чьн'
    },
    {
        regex: /ль/gi,
        replaceTo: 'й'
    },
    {
        regex: /гу/gi,
        replaceTo: 'гю'
    },
    {
        regex: /су/gi,
        replaceTo: 'сю'
    },
    {
        regex: /еб/gi,
        replaceTo: 'йоб'
    },
    {
        regex: /тся/gi,
        replaceTo: 'ться'
    },
    {
        regex: /оо/gi,
        replaceTo: 'а'
    },
    {
        regex: /ал/gi,
        replaceTo: 'ял'
    },
    {
        regex: /ром/gi,
        replaceTo: 'ьом'
    },
    {
        regex: /олб/gi,
        replaceTo: 'об'
    },
    {
        regex: /лять/gi,
        replaceTo: 'ьять'
    },
    {
        regex: /жа/gi,
        replaceTo: 'жя'
    },
    {
        regex: /удет/gi,
        replaceTo: 'уит'
    },
    {
        regex: /ет/gi,
        replaceTo: 'ит'
    },
    {
        regex: /чт/gi,
        replaceTo: 'щт'
    },
    {
        regex: /ох/gi,
        replaceTo: 'ёх'
    },
    {
        regex: /эле/gi,
        replaceTo: 'эе'
    },
    {
        regex: /вре/gi,
        replaceTo: 'вье'
    },
    {
        regex: /ых/gi,
        replaceTo: 'ьих'
    },
    {
        regex: /чис/gi,
        replaceTo: 'чьс'
    },
    {
        regex: /цит/gi,
        replaceTo: 'сит'
    },
    {
        regex: /цы/gi,
        replaceTo: 'си'
    },
    {
        regex: /го/gi,
        replaceTo: 'си'
    },
    {
        regex: /ли/gi,
        replaceTo: 'ль'
    },
    {
        regex: /гр/gi,
        replaceTo: 'гл'
    },
    {
        regex: /лн/gi,
        replaceTo: 'ьн'
    },
    {
        regex: /ту/gi,
        replaceTo: 'тю'
    },
    {
        regex: /те/gi,
        replaceTo: 'ти'
    },
    {
        regex: /ры/gi,
        replaceTo: 'ьы'
    },
    {
        regex: /за/gi,
        replaceTo: 'зя'
    },
    {
        regex: /мат/gi,
        replaceTo: 'мят'
    },
    {
        regex: /уж/gi,
        replaceTo: 'юш'
    },
    {
        regex: /ну/gi,
        replaceTo: 'ню'
    },
    {
        regex: /мам/gi,
        replaceTo: 'мямь'
    },
    {
        regex: /оп/gi,
        replaceTo: 'ёп'
    },
    {
        regex: /ут/gi,
        replaceTo: 'ють'
    },
    {
        regex: /тан/gi,
        replaceTo: 'тян'
    },
    {
        regex: /зыв/gi,
        replaceTo: 'зивъ'
    },
    {
        regex: /орош/gi,
        replaceTo: 'ороф'
    },
    {
        regex: /уст/gi,
        replaceTo: 'ьюс'
    },
    {
        regex: /уж/gi,
        replaceTo: 'юш'
    },
    {
        regex: /ерз/gi,
        replaceTo: 'ьерс'
    },
    {
        regex: /изик/gi,
        replaceTo: 'иьик'
    },
    {
        regex: /тик/gi,
        replaceTo: 'ьик'
    },
    {
        regex: /дем/gi,
        replaceTo: 'им'
    }
]

const rewriteMessage = (message) => {
    for (let i = 0; i < matches.length; i++) {
        if (message.match(matches[i].regex)) {
            message = message.replaceTo(matches[i].regex, matches[i].replaceTo)
        }
    }
    return message
}
