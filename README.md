![alt text](/assets/Шапка.jpg)

# SchoolBot
Помощник для школьников и студентов для ВКонтакте, написанный на Node.JS
> Версия 2.0 - спустя 2 года. Все библиотеки обновились, API ВКонтакте тоже.

## Просмотр функционала и не только

Я сделал специальную [страницу](http://sashafromlibertalia.herokuapp.com/projects/schoolbot), которая поясняет все команды бота, а также демонстрирует работу с ним. (_```Содержание устарело, но суть не меняется```)_

## Установка
Чтобы установить бота, скопируйте репозиторий.
```
$ git clone https://github.com/sashafromlibertalia/SchoolBot.git
```

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
    PARSER_URL: STRING, // Ссылка на файл domashka.txt (Рекомендую хранить его на GitHub в репозитории с ботом)
    NAME_GROUP: STRING, // Ваш класс (или группа)
    NAME_PLACE: STRING, // Название вашего учебного заведения
    NAME_ADMIN: STRING, // Ваше имя
    NAME_ADMIN_DAT: STRING, // Ваше имя в дательном падеже (см. код)
    ADMIN_DOMAIN: STRING, // Короткая ссылка на вашу страницу
    ADMIN_ID: ARRAY OF INTS // Массив, состоящий из ID администраторов беседы (на тот случай, если )
}
```
После того, как все данные будут заполнены, вам нужно разобраться со структурой кода. 
```SchoolBot``` использует модульную систему: каждая важная и большая функция вынесена в отдельный файл, который далее подключается к **```index.js```**. Подробнее о файлах ниже: <br>

| Название файла | Расположение | Назначение|
|----------------|--------------|-----------|
|**```index.js```**|```./src/index.js```| Исполняемый файл программы, сюда подключаются все остальные файлы    |
|**```vk.js```**|```./src/vk.js```|В данном файле хранится конструктор __VK__, а также обработчик сообщений и клавиатуры. Этот файл подключаем ко всем побочным файлам, подразумевающим обработку разных команд.|
|**```links.js```**|```./src/links.js```| В данном файле хранится информация о предметах и ссылках на зум. 
|**```image.js```**|```./src/image.js```|Обертка для /citgen и /trollgen|
|**```automasticSender.js```**|```./src/automaticSender.js```|Этот файл нужен для отправки оповещений перед началом пар|
|**```citgen.js```**|```./src/commands/citgen.js```| Данный файл является реализацией команды _/citgen_. Юмор - наше все
|**```date.js```**|```./src/commands/date.js```| В данном файле реализована команды _/дата_ и смежные с ней
|**```games.js```**|```./src/commands/games.js```|В этом файле реализованы игры бота|
|**```kicker.js```**|```./src/commands/kicker.js```|В этом файле реализована функция _/вгулаг_, которая кикает любого пользователя. Плюс ее в том, что работать для того человека, который будет указан в ```config.js``` в поле _ADMIN_ID_|
|**```parser.js```**|```./src/commands/parser.js```|В этом файле находится парсер ```domashka.txt```|
|**```savedData```**|```./src/commands/savedData.js```|В этом файле сделана команды _/шпора_ и смежные с ней|
|**```troll.js```**|```./src/commands/troll.js```|В этом файле сделана команды _/тролль_, которая коверкает слова, которые дает пользователь. Иногда выходит очень забавно|
|**```trollgen.js```**|```./src/commands/trollgen.js```|Аналог _/citgen_|

Если вы все сделали правильно, поздравляю - бот готов :)


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
>

## Планы
- [ ] Перенос слов в цитгене
- [ ] Четность недель
- [ ] Автообновление парсера без перегрузки бота