![alt text](/assets/Шапка.jpg)
<div align="center">
    <img src="https://img.shields.io/npm/l/schoolbot">
    <img src="https://img.shields.io/amo/dw/schoolbot">
    <img src="https://img.shields.io/npm/v/schoolbot">
</div>


# SchoolBot
Помощник для школьников и студентов для ВКонтакте, написанный на `node.js`
## Просмотр функционала и не только

Я сделал специальную [страницу](http://sashafromlibertalia.herokuapp.com/projects/schoolbot), которая поясняет все команды бота, а также демонстрирует работу с ним. (_```Содержание устарело, но суть не меняется```)_
<br>
Альтернатива - писать напрямую [боту](https://vk.com/club168462227), которым я сам пользуюсь.

## Установка
Чтобы установить бота, скопируйте репозиторий.
```
$ git clone https://github.com/sashafromlibertalia/SchoolBot.git
```
или воспользуйтесь **npm**:
```
npm install schoolbot
```

## Важное объявление
Библиотека [canvas](https://github.com/Automattic/node-canvas) не может из коробки обрабатывать `JPEG` изображения на **ARM процессорах**. Вы будете получать следующую ошибку:

> Uncaught Error: node-canvas was built without JPEG support

Решение этой проблемы есть, но оно может сработать не для всех. Почему так? Я не знаю. Мне не помогло.
<br>
Решение есть [тут](https://github.com/Automattic/node-canvas/issues/1782), а также [тут](https://github.com/Automattic/node-canvas/issues/1733).

Я **очень** надеюсь, что автор этой библиотеки придумает решение этой проблемы.


## Дальнейшая настройка

Вам нужно создать группу в ВКонтакте, к которой будет привязан бот. Сначала разрешите сообщения сообщества, а затем вам нужно получить токен, который нужно вставить в конфиг.
Найти его можно тут: 

```Управление -> Работа с API -> Ключи доступа -> Создать ключ``` <br>
Выбираете нужные вам пункты и жмете "создать".

После того, как Вы скачали репозиторий бота, переходить в папку ```src```. В ней находится файл **```config.js```**. Он нужен для внесения параметров, необходимых для корректной работы бота.
```js
const config = {
    TOKEN: STRING,  // Сюда вы пишите токен группы, полученный с помощью LongPoll
    POLLING_GROUP_ID: INT, // ID вашей группы
    PARSER_URL: STRING, // Ссылка на файл domashka.txt (КРАЙНЕ рекомендую хранить его на GitHub в репозитории с ботом, почему - см. parser.js)
    NAME_GROUP: STRING, // Ваш класс (или группа)
    NAME_PLACE: STRING, // Название вашего учебного заведения
    NAME_ADMIN: STRING, // Ваше имя
    NAME_ADMIN_DAT: STRING, // Ваше имя в дательном падеже (см. feedback.js)
    ADMIN_DOMAIN: STRING, // Короткая ссылка на вашу страницу
    ADMIN_ID: ARRAY OF INTS // Массив, состоящий из ID администраторов беседы (на тот случай, если бот будет состоять в нескольких беседах)
}
```

После того, как все данные будут заполнены, вам нужно разобраться со структурой кода. 
```SchoolBot``` использует модульную систему: каждая функция вынесена в отдельный файл, который далее подключается к **```index.js```**. Подробнее о файлах ниже: <br>

| Название файла | Расположение | Назначение|
|----------------|--------------|-----------|
|**```index.js```**|```./src/index.js```| Исполняемый файл программы, сюда подключаются все остальные файлы    |
|**```vk.js```**|```./src/vk.js```|В данном файле хранится конструктор __VK__, а также обработчик сообщений и клавиатуры. Этот файл подключаем ко всем побочным файлам, подразумевающим обработку разных команд.|
|**```links.js```**|```./src/links.js```| В данном файле хранится информация о предметах и ссылках на Zoom. 
|**```image.js```**|```./src/image.js```|Обертка для /citgen и /trollgen|
|**```automasticSender.js```**|```./src/automaticSender.js```|Этот файл нужен для отправки оповещений перед началом пар|
|**```start.js```**|```./src/commands/start.js```|Реализация команды _/start_|
|**```commands.js```**|```./src/commands/commands.js```|Реализация команды _/команды_. Она присылает полный список команд|
|**```help.js```**|```./src/commands/commands.js```|Реализация команды _/help_|
|**```feedback.js```**|```./src/commands/feedback.js```|Реализация команды _/отзыв_|
|**```citgen.js```**|```./src/commands/citgen.js```|Реализация команды _/citgen_. Юмор - наше все
|**```date.js```**|```./src/commands/date.js```|Реализация команды _/дата_ и смежных с ней
|**```games.js```**|```./src/commands/games.js```|Реализация игр бота|
|**```kicker.js```**|```./src/commands/kicker.js```|Реализована команда _/вгулаг_, которая кикает любого пользователя. Плюс ее в том, что работать для того человека, который будет указан в ```config.js``` в поле _ADMIN_ID_|
|**```parser.js```**|```./src/commands/parser.js```|Парсер ```domashka.txt```|
|**```savedData```**|```./src/commands/savedData.js```|Реализация команды _/шпора_ и смежных с ней|
|**```troll.js```**|```./src/commands/troll.js```|Реализация команды _/тролль_, которая коверкает слова, которые дает пользователь. Иногда выходит очень забавно|
|**```trollgen.js```**|```./src/commands/trollgen.js```|Аналог _/citgen_|


## Запуск бота
Чтобы запустить бота, введите следующую команду. Она устанавливает зависимости, необходимые для работы Schoolbot:

```
npm install
```

Далее, если у вас не работают команды `/citgen` и `/trollgen`, есть два пути:
1. Если у вас macOS, введите следующую команду через [Homebrew](https://brew.sh/index_ru):
```
brew install pkg-config cairo pango libpng jpeg giflib librsvg
```
2. Если у вас Windows, то перейдите [сюда](https://github.com/Automattic/node-canvas/wiki/Installation:-Windows)


## Команды
   1. ```/дата``` - отправляет клавиатуру с выбором дня недели, по нажатии на кнопку отправляется д/з на выбранный день.
   2. ```/дз завтра``` - отправляет домашние задания на завтра
   3. ```/дз все``` - отправляет домашние задания на всю неделю
   4. ```/игры``` - отправляет клавиатуру с выбором игр
   5. ```/отзыв``` - отправляет отзыв человеку, чей ID указан в конфиге. В принципе - вещь бестолковая, но кому нужно, тот найдет ей применение.
   6. ```/шпора``` - можно добавить важные фото/документы/шпоры, чтобы не искать потом по всей беседе. (_При перегрузке/остановке бота все стирается, поэтому лучше сохранненые данные хранить в БД или еще где-нибудь_)
   7. ```/тролль``` - перешлите чье-то сообщение и пишите эту команду
   8. ```/шпора ?``` - инструкция по шпорам
   9. ```/шпора список``` - список шпор
   10. ```/citgen``` - перешлите чье-то сообщение и напишите эту команду
   11. ```/trollgen``` - перешлите чье-то сообщение и пишите эту команду
   12. ```/help``` - документация бота

> Список команд ограничивается вашей фантазией :) Дерзайте!

## Планы
- [ ] Перенос слов в цитгене
- [ ] Четность недель
- [ ] Автообновление парсера без перегрузки бота