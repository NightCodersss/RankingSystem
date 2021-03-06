# Инструкция по использованию

## Работа с деплоями
Есть такое понятие "деплой" — это некоторая зафиксированая сборка бинарников и конфигов. (Сейчас версия библиотек не разделяется, в том числе для common)

Пусть проект находится в состоянии, когда все парвки внесены. Тогда можно сделать новый деплой:
`./new_deploy.sh my_new_deploy`
После чего нужно поправить стандартные конфиги (зачастую достаточно поставить ссылки на индексы)
Когда деплой будет сконфигурирован, можно запускать:
`./run_deploy.sh my_new_deploy`
А потом останавливать: 
`./stop_deploy.sh my_new_deploy`

Остановка нужна для высвобождния ресурсов, в частности освобождения занятых портов.

## Индексы
Также есть понятие индекса. Индекс есть пара: прямой индекс и обратный. По сути это набор папок с файлами вида `./indexes/the_best_index/forward/` и `./indexes/the_best_index/index`. Эти две папки можно автоматизировано создавать при поможщи:
`./new_index.sh the_best_index`

### Добавление документа в индекс
Рассмотрим добавленик документа не задаваясь вопросом, что есть доумент.
Добавление документа связано с деплоем: нужен бинарник bigtest для упаковки обратного индекса. К тому же нужно знать в какие индексы заливать результат — для этого используются индексы выбранного деплоя.
`./index_one_doc.sh my_deploy /home/docs/mydoc.txt`
Следует использовать абсолютные пути к файлам.

### Что есть документ
Документ (сейчас) может быть двух типов: 
 * документ, парсящийся стандартным способом (описано в ./indexer/index.py get_texts; разбивая на первое предложение и остальное)
 * докмент-хеадер в формате:
```
# doc header
<url_to_document>
<text_id> <path_to_text_file (plain text>
<text_id> <path_to_text_file (plain text>
<text_id> <path_to_text_file (plain text>
...
<text_id> <path_to_text_file (plain text>
```
Пример: ./indexer/header.txt

## Просмотр результатов поиска
`./show_doc.sh <doc_id>` сделает `cat` пути с которым был заиндексирован документ. Таблица хранится в ./indexer/hashes
