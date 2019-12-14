# Комплексный полив
Работа посвящена исследованию, разработке автоматического полива растений. 

## Цель
Разработать систему комплексного полива растений с возможностью удаленного управле-ния, используя современную 
элементную базу. Ииспользуя современную элементную базу и ее возможности добиться максимальной эффективности 
при минимальной стоимости.

## Ход работы

### Структурная схема системы
![Структурная схема](https://i.imgur.com/TogZGIw.png)
Система состоит из блока-родителя и блока-потомка. Блоки-потомки подключаются к блокам-родителям посредством 
технолгии Wi-Fi. Блок-родитель представляет из себя одноплатный компьютер OrangePi, работающий на ОС Linux Armbian,
он подключен к сети Интернет и ожидает подключений от блоков-потомков.

### Структурная схема блока-потомка
![Структурная схема блока-потомка](https://i.imgur.com/bh4s8PO.png)
Система состоит из двух микроконтроллеров. Задача микроконтроллера STM32 производить полив, опираясь на математическую
функцию-регулятор, которая устанавливает зависимость между влажность и мощностью управляющего сигнала полива. Задачей
второго микроконтроллера является обеспечение обмена данными относительно блока-родителя.

### 3D - модель блока-потомка
![3D-модель блока-потомка](https://i.imgur.com/MpgdxRD.png)

Модель прототипа блока-потомка

### Электронная ячейка блока-потомка
![Электронная ячейка блока-потомка](https://i.imgur.com/h8hUKQc.png)

Полученный прототип блока-потомка

### Снятие показаний с прототипа блока-потомка

![Электронная ячейка блока-потомка](https://i.imgur.com/8L7JpY1.png)

Окружение, в котором производилось снятие показаний при искусственно созданых различных значениях влажности 
(в данном случае - полувлажная среда).

![Электронная ячейка блока-потомка](https://i.imgur.com/4Q4Qq4A.png)

Скриншот отражает работу веб-интерфейса, а именно состояние показаний датчика и помпы. А также показан график 
формируемого электрического сигнала 

## Вывод
В итоге проделанной работы можно сделать вывод, что разработанная система полива соответствует требованиям 
технического задания.
