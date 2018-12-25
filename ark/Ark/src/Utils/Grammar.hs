﻿module Grammar (
   НетерминалыАрконы(..),

   ТерминалыАрконы(..),

   грамматикаАрконы
) where

import Rules

data ТерминалыАрконы =
   Скобка         | Или           | НачБлока      | КонецБлока     | Ид             | Формат         |
   Секшон         | ТочкаЗапятая  | Читаемый      | Записываемый   | Исполняемый    | Вход           |
   Макро          | Епсилон       | Зап           | ОткрКругСкобка | ЗакрКругСкобка | ОткрФигСкобка  |
   ЗакрФигСкобка  | Вопрос        | МУР
 deriving(Eq,Show,Ord,Read,Enum,Bounded)

data НетерминалыАрконы =
  Программа      | ТочкаВхода  | ТелоПрограммы  | Макрос    | Секция         |
  АтрибутСекции  | ТелоСекции  | АтрибутыСекции | Команда       | Аргументы | АргументыШтрих |
  ТелоМакроса    | МашКоманда  | Аргументы'
 deriving(Eq,Show,Ord,Read,Enum,Bounded)

грамматикаАрконы :: [Правило НетерминалыАрконы ТерминалыАрконы]
грамматикаАрконы =
   [
    Программа --> [Терминал Формат, Терминал МУР, Терминал ОткрФигСкобка, Нетерминал ТочкаВхода, Нетерминал ТелоПрограммы, Терминал ЗакрФигСкобка],
    ТелоПрограммы --> [Нетерминал Макрос],
    ТелоПрограммы --> [Нетерминал Секция],
    ТелоПрограммы --> [Нетерминал ТелоПрограммы, Нетерминал Макрос],
    ТелоПрограммы --> [Нетерминал ТелоПрограммы, Нетерминал Секция],
    ТочкаВхода --> [Терминал Вход, Терминал Ид],
    Секция --> [Терминал Секшон, Нетерминал АтрибутыСекции, Терминал ОткрФигСкобка, Нетерминал ТелоСекции, Терминал ЗакрФигСкобка],
    АтрибутыСекции --> [Нетерминал АтрибутСекции],
    АтрибутыСекции --> [Нетерминал АтрибутыСекции, Нетерминал АтрибутСекции],
    АтрибутСекции --> [Терминал Читаемый],
    АтрибутСекции --> [Терминал Записываемый],
    АтрибутСекции --> [Терминал Исполняемый],
    ТелоСекции --> [Нетерминал Команда],
    ТелоСекции --> [Нетерминал Макрос],
    ТелоСекции --> [Терминал Ид, Терминал ТочкаЗапятая],
    ТелоСекции --> [Нетерминал ТелоСекции, Терминал ТочкаЗапятая, Нетерминал Команда],
    ТелоСекции --> [Нетерминал ТелоСекции, Терминал ТочкаЗапятая, Нетерминал Макрос],
    ТелоСекции --> [Нетерминал ТелоСекции, Терминал ТочкаЗапятая, Терминал Ид, Терминал ТочкаЗапятая],
    Макрос --> [Терминал Макро, Терминал Ид, Нетерминал Аргументы, Терминал ОткрФигСкобка, Нетерминал ТелоМакроса, Терминал ЗакрФигСкобка],
    Аргументы --> [Терминал Епсилон],
    Аргументы --> [Нетерминал Аргументы'],
    Аргументы' --> [Терминал Ид],
    Аргументы' --> [Нетерминал АргументыШтрих, Терминал Зап, Терминал Ид],
    ТелоМакроса --> [Терминал ОткрФигСкобка, Нетерминал Команда, Терминал ЗакрФигСкобка],
    ТелоМакроса --> [Терминал ОткрФигСкобка, Терминал Ид, Терминал ТочкаЗапятая, Терминал ЗакрФигСкобка],
    ТелоМакроса --> [Терминал ОткрФигСкобка, Нетерминал ТелоМакроса, Терминал ТочкаЗапятая, Нетерминал Команда, Терминал ЗакрФигСкобка],
    ТелоМакроса --> [Терминал ОткрФигСкобка, Нетерминал ТелоМакроса, Терминал ТочкаЗапятая, Терминал Ид, Терминал ТочкаЗапятая, Терминал ЗакрФигСкобка],
    Команда --> [Терминал ОткрКругСкобка, Терминал Ид, Терминал ТочкаЗапятая, Терминал ЗакрКругСкобка, Терминал Вопрос, Нетерминал Макрос, Нетерминал Аргументы]
   ]
