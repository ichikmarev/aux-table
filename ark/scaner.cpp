#include "scaner.h"
#include <stdio.h>

static const char categories_table[] = {
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  7,  5,  7, 12,  7,  7,  4,  2,  3,  7,  7,  1,  7,  7,  7,
10, 11, 11, 11, 11, 11, 11, 11, 11, 11,  7,  1,  7,  7,  7,  7,
 7, 13, 13, 13, 14, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 14, 13, 13, 13, 13, 15, 13, 13,  6,  7,  6,  7,  1,
 1, 13, 13, 13, 14, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 14, 13, 13, 13, 13, 15, 13, 13,  7,  7,  7,  7,  1,
 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
 1,  1,  1,  1,  1,  1,  1,  1,  9,  1,  1,  1,  1,  1,  1,  1,
 1,  1,  1,  1,  1,  1,  1,  1,  9,  1,  1,  1,  1,  1,  1,  1,
 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
 8,  8,  8,  8,  8,  8,  9,  8,  8,  9,  8,  8,  8,  8,  8,  8,
 8,  8,  8,  9,  9,  9,  8,  9,  9,  9,  9,  9,  9,  8,  9,  9
};

Scaner::Scaner(const unsigned char* t){
	text = pcurrent_char = lexem_begin = const_cast<unsigned char*>(t);
	current_line = 1;
	return;
}

unsigned int Scaner::get_line_number(){
	return current_line;
}

lexem_info Scaner::get_current_lexem(){
	Automaton = a_START;
//	probably_code = NONE;
	lexem_begin = pcurrent_char;
	state = 0;
	lexem_info k;
	
	bool t = true;
	
	while((currrent_char = *pcurrent_char++) && t){
		char_category = static_cast<CATEGORY>(categories_table[currrent_char]);
		t = (this->*procs[Automaton])();
	}
	pcurrent_char--;
	
	return k; 
}

void Scaner::back(){
	pcurrent_char = lexem_begin; 
	return;
}
	
bool Scaner::start_proc(){
	bool t = true;
	switch(char_category){
		case SPACES:
			if(currrent_char == '\n'){
				current_line++;
			}
			break;
			
		case OTHER: case LETTER:
			Automaton = a_UNKNOWN;
			probably_code = UNKNOWN;
			printf("Нераспознаваемая лексема в строке %d.\n", current_line);
			break;
			
		case LEFT_BRACKET:
			probably_code = OPEN_ROUND_BRACKET;
			t = false;
			break;
			
		case RIGHT_BRACKET:
			probably_code = CLOSE_ROUND_BRACKET;
			t = false;
			break;

		case ONE_LETTER_DELIMITER:
			
			
		case PART_OF_DELIMITER:
			Automaton = a_MULTILETTER_DELIMITERS;
			switch(currrent_char){
				case '|':
					state = 0;
					break;
					
				case '&':
					state = 2;
					break;
					
				case '^':
					state = 4;
					break;
					
				case '!':
					state = 6;
					break;
			}			
			break;

		case DIGIT_0:
			break;

		case DIGITS_1_9:
			break;
		
		case DOLLAR:
			break;

		case QUOTE:
			break;

		case DQUOTE:
			break;
			
						
		case KEYWORD_BEGIN:
			state = (((char)currrent_char) == 'л') ? 0: 6; //FIXME
			Automaton = a_KEYWORD;
			break;
	}
	lexem_begin = pcurrent_char-1;	
	return t;
}

bool Scaner::unknown_proc(){
	return char_category == OTHER;
}

typedef struct tag_ELEM{
	unsigned short int code;
    unsigned short int first_state;
    char               symbols[4];
} ELEM;

/* Подпрограмма поиска символа в массиве из 4 символов. Если символ в массиве есть,
   то возвращается индекс соответствующего элемента. Иначе возвращается число 4.
   Искомый символ не должен быть равен нулю. */
int search_char(unsigned char c, char array[4]){
	unsigned int x = (c * 0x01010101) ^ (*((int*)array)); 
	/* Далее в полученном четырёхбайтовом целом числе вычисляем номер самого младшего
       нулевого байта и возвращаем этот номер. */
    /* Поиск этот будем осуществлять так, как предложено на стр.102 книги
     "Уоррен Г.С. Алгоритмические трюки для программистов.: Пер. с англ. --- М.: 
	  Вильямс, 2003." */
	unsigned int y = (x & 0x7f7f7f7f) + 0x7f7f7f7f;
	y = y | x;
	
	unsigned int t1, t2, t3, t4;
	t1 = (y >> 7) & 1;
	t2 = (y >> 15) & t1;
	t3 = (y >> 23) & t2;
	t4 = (y >> 31) & t3;
    return t1 + t2 + t3 + t4;
}

ELEM a_KEYWORD_jump_table[] = {
     {{Scaner::ID     , 0                    },   1, { 'л', 'с',   0,   0}},     //   0 а...
     {{Scaner::ID     , 0                    },   3, { 'г',   0,   0,   0}},     //   1 ал...
     {{Scaner::ID     , 0                    },   4, { 'м',   0,   0,   0}},     //   2 ас...
     {{Scaner::KEYWORD, Scaner::ALG          },   0, {   0,   0,   0,   0}},     //   3 алг
     {{Scaner::KEYWORD, Scaner::ASM          },   0, {   0,   0,   0,   0}},     //   4 асм
     {{Scaner::ID     , 0                    },   6, { 'е', 'о',   0,   0}},     //   5 б...
     {{Scaner::ID     , 0                    },   8, { 'з',   0,   0,   0}},     //   6 бе...
     {{Scaner::ID     , 0                    },   9, { 'л',   0,   0,   0}},     //   7 бо...
     {{Scaner::ID     , 0                    },  10, { 'з',   0,   0,   0}},     //   8 без...
     {{Scaner::ID     , 0                    },  11, { 'ь',   0,   0,   0}},     //   9 бол...
     {{Scaner::ID     , 0                    },  12, { 'н',   0,   0,   0}},     //  10 безз...
     {{Scaner::ID     , 0                    },  13, { 'ш',   0,   0,   0}},     //  11 боль...
     {{Scaner::KEYWORD, Scaner::BEZZN        },   0, {   0,   0,   0,   0}},     //  12 беззн
     {{Scaner::ID     , 0                    },  14, { 'о',   0,   0,   0}},     //  13 больш...
     {{Scaner::ID     , 0                    },  15, { 'е',   0,   0,   0}},     //  14 большо...
     {{Scaner::KEYWORD, Scaner::BOLSHOE      },   0, {   0,   0,   0,   0}},     //  15 большое
     {{Scaner::ID     , 0                    },  17, { 'е', 'о', 'ы',   0}},     //  16 в...
     {{Scaner::ID     , 0                    },  20, { 'ч', 'щ',   0,   0}},     //  17 ве...
     {{Scaner::ID     , 0                    },  22, { 'з',   0,   0,   0}},     //  18 во...
     {{Scaner::ID     , 0                    },  23, { 'б', 'й',   0,   0}},     //  19 вы...
     {{Scaner::ID     , 0                    },  25, { 'н',   0,   0,   0}},     //  20 веч...
     {{Scaner::KEYWORD, Scaner::VESH         },   0, {   0,   0,   0,   0}},     //  21 вещ
     {{Scaner::ID     , 0                    },  26, { 'в',   0,   0,   0}},     //  22 воз...
     {{Scaner::ID     , 0                    },  27, { 'е',   0,   0,   0}},     //  23 выб...
     {{Scaner::ID     , 0                    },  28, { 'д',   0,   0,   0}},     //  24 вый...
     {{Scaner::ID     , 0                    },  29, { 'о',   0,   0,   0}},     //  25 вечн...
     {{Scaner::ID     , 0                    },  30, { 'р',   0,   0,   0}},     //  26 возв...
     {{Scaner::ID     , 0                    },  31, { 'р',   0,   0,   0}},     //  27 выбе...
     {{Scaner::ID     , 0                    },  32, { 'и',   0,   0,   0}},     //  28 выйд...
     {{Scaner::KEYWORD, Scaner::VECHNO       },   0, {   0,   0,   0,   0}},     //  29 вечно
     {{Scaner::ID     , 0                    },  33, { 'а',   0,   0,   0}},     //  30 возвр...
     {{Scaner::ID     , 0                    },  34, { 'и',   0,   0,   0}},     //  31 выбер...
     {{Scaner::KEYWORD, Scaner::VIIDI        },   0, {   0,   0,   0,   0}},     //  32 выйди
     {{Scaner::ID     , 0                    },  35, { 'т',   0,   0,   0}},     //  33 возвра...
     {{Scaner::KEYWORD, Scaner::VIBERI       },   0, {   0,   0,   0,   0}},     //  34 выбери
     {{Scaner::KEYWORD, Scaner::VOZVRAT      },   0, {   0,   0,   0,   0}},     //  35 возврат
     {{Scaner::ID     , 0                    },  37, { 'о', 'р',   0,   0}},     //  36 г...
     {{Scaner::ID     , 0                    },  39, { 'л',   0,   0,   0}},     //  37 го...
     {{Scaner::ID     , 0                    },  40, { 'я',   0,   0,   0}},     //  38 гр...
     {{Scaner::ID     , 0                    },  41, { 'о',   0,   0,   0}},     //  39 гол...
     {{Scaner::ID     , 0                    },  42, { 'з',   0,   0,   0}},     //  40 гря...
     {{Scaner::ID     , 0                    },  43, { 'в',   0,   0,   0}},     //  41 голо...
     {{Scaner::ID     , 0                    },  44, { 'н',   0,   0,   0}},     //  42 гряз...
     {{Scaner::ID     , 0                    },  45, { 'н',   0,   0,   0}},     //  43 голов...
     {{Scaner::KEYWORD, Scaner::GRYAZN       },   0, {   0,   0,   0,   0}},     //  44 грязн
     {{Scaner::ID     , 0                    },  46, { 'о',   0,   0,   0}},     //  45 головн...
     {{Scaner::ID     , 0                    },  47, { 'й',   0,   0,   0}},     //  46 головно...
     {{Scaner::KEYWORD, Scaner::GOLOVNOI     },   0, {   0,   0,   0,   0}},     //  47 головной
     {{Scaner::ID     , 0                    },  49, { 'л',   0,   0,   0}},     //  48 д...
     {{Scaner::ID     , 0                    },  50, { 'я',   0,   0,   0}},     //  49 дл...
     {{Scaner::KEYWORD, Scaner::DLYA         },   0, {   0,   0,   0,   0}},     //  50 для
     {{Scaner::ID     , 0                    },  52, { 'с',   0,   0,   0}},     //  51 е...
     {{Scaner::ID     , 0                    },  53, { 'л',   0,   0,   0}},     //  52 ес...
     {{Scaner::ID     , 0                    },  54, { 'и',   0,   0,   0}},     //  53 есл...
     {{Scaner::KEYWORD, Scaner::ESLI         },   0, {   0,   0,   0,   0}},     //  54 если
     {{Scaner::ID     , 0                    },  56, { 'н',   0,   0,   0}},     //  55 з...
     {{Scaner::ID     , 0                    },  57, { 'а',   0,   0,   0}},     //  56 зн...
     {{Scaner::ID     , 0                    },  58, { 'ч',   0,   0,   0}},     //  57 зна...
     {{Scaner::ID     , 0                    },  59, { 'е',   0,   0,   0}},     //  58 знач...
     {{Scaner::ID     , 0                    },  60, { 'н',   0,   0,   0}},     //  59 значе...
     {{Scaner::ID     , 0                    },  61, { 'и',   0,   0,   0}},     //  60 значен...
     {{Scaner::ID     , 0                    },  62, { 'ю', 'я',   0,   0}},     //  61 значени...
     {{Scaner::KEYWORD, Scaner::ZNACHENIYU   },   0, {   0,   0,   0,   0}},     //  62 значению
     {{Scaner::KEYWORD, Scaner::ZNACHENIYA   },   0, {   0,   0,   0,   0}},     //  63 значения
     {{Scaner::ID     , 0                    },  65, { 'з', 'н', 'с',   0}},     //  64 и...
     {{Scaner::KEYWORD, Scaner::IZ           },   0, {   0,   0,   0,   0}},     //  65 из
     {{Scaner::ID     , 0                    },  68, { 'а', 'е',   0,   0}},     //  66 ин...
     {{Scaner::ID     , 0                    },  70, { 'п', 'т',   0,   0}},     //  67 ис...
     {{Scaner::ID     , 0                    },  72, { 'ч',   0,   0,   0}},     //  68 ина...
     {{Scaner::ID     , 0                    },  73, { 'с',   0,   0,   0}},     //  69 ине...
     {{Scaner::ID     , 0                    },  74, { 'о',   0,   0,   0}},     //  70 исп...
     {{Scaner::ID     , 0                    },  75, { 'и',   0,   0,   0}},     //  71 ист...
     {{Scaner::ID     , 0                    },  76, { 'е',   0,   0,   0}},     //  72 инач...
     {{Scaner::KEYWORD, Scaner::INES         },   0, {   0,   0,   0,   0}},     //  73 инес
     {{Scaner::ID     , 0                    },  77, { 'л',   0,   0,   0}},     //  74 испо...
     {{Scaner::ID     , 0                    },  78, { 'н',   0,   0,   0}},     //  75 исти...
     {{Scaner::KEYWORD, Scaner::INACHE       },   0, {   0,   0,   0,   0}},     //  76 иначе
     {{Scaner::ID     , 0                    },  79, { 'ь',   0,   0,   0}},     //  77 испол...
     {{Scaner::ID     , 0                    },  80, { 'а',   0,   0,   0}},     //  78 истин...
     {{Scaner::ID     , 0                    },  81, { 'з',   0,   0,   0}},     //  79 исполь...
     {{Scaner::KEYWORD, Scaner::ISTINA       },   0, {   0,   0,   0,   0}},     //  80 истина
     {{Scaner::ID     , 0                    },  82, { 'у',   0,   0,   0}},     //  81 использ...
     {{Scaner::ID     , 0                    },  83, { 'е',   0,   0,   0}},     //  82 использу...
     {{Scaner::ID     , 0                    },  84, { 'т',   0,   0,   0}},     //  83 используе...
     {{Scaner::KEYWORD, Scaner::ISPOLZUET    },   0, {   0,   0,   0,   0}},     //  84 использует
     {{Scaner::ID     , 0                    },  86, { 'о',   0,   0,   0}},     //  85 к...
     {{Scaner::ID     , 0                    },  87, { 'м',   0,   0,   0}},     //  86 ко...
     {{Scaner::ID     , 0                    },  88, { 'п',   0,   0,   0}},     //  87 ком...
     {{Scaner::ID     , 0                    },  89, { 'л',   0,   0,   0}},     //  88 комп...
     {{Scaner::KEYWORD, Scaner::KOMPL        },   0, {   0,   0,   0,   0}},     //  89 компл
     {{Scaner::ID     , 0                    },  91, { 'о',   0,   0,   0}},     //  90 л...
     {{Scaner::ID     , 0                    },  92, { 'г', 'ж',   0,   0}},     //  91 ло...
     {{Scaner::KEYWORD, Scaner::LOG          },   0, {   0,   0,   0,   0}},     //  92 лог
     {{Scaner::ID     , 0                    },  94, { 'ь',   0,   0,   0}},     //  93 лож...
     {{Scaner::KEYWORD, Scaner::LOZH         },   0, {   0,   0,   0,   0}},     //  94 ложь
     {{Scaner::ID     , 0                    },  96, { 'а', 'н', 'о',   0}},     //  95 м...
     {{Scaner::ID     , 0                    },  99, { 'л', 'с',   0,   0}},     //  96 ма...
     {{Scaner::ID     , 0                    }, 101, { 'о',   0,   0,   0}},     //  97 мн...
     {{Scaner::ID     , 0                    }, 102, { 'д',   0,   0,   0}},     //  98 мо...
     {{Scaner::ID     , 0                    }, 103, { 'е',   0,   0,   0}},     //  99 мал...
     {{Scaner::ID     , 0                    }, 104, { 'с',   0,   0,   0}},     // 100 мас...
     {{Scaner::ID     , 0                    }, 105, { 'ж',   0,   0,   0}},     // 101 мно...
     {{Scaner::ID     , 0                    }, 106, { 'у',   0,   0,   0}},     // 102 мод...
     {{Scaner::ID     , 0                    }, 107, { 'н',   0,   0,   0}},     // 103 мале...
     {{Scaner::ID     , 0                    }, 108, { 'и',   0,   0,   0}},     // 104 масс...
     {{Scaner::ID     , 0                    }, 109, { 'е',   0,   0,   0}},     // 105 множ...
     {{Scaner::ID     , 0                    }, 110, { 'л',   0,   0,   0}},     // 106 моду...
     {{Scaner::ID     , 0                    }, 111, { 'ь',   0,   0,   0}},     // 107 мален...
     {{Scaner::ID     , 0                    }, 112, { 'в',   0,   0,   0}},     // 108 масси...
     {{Scaner::ID     , 0                    }, 113, { 'с',   0,   0,   0}},     // 109 множе...
     {{Scaner::ID     , 0                    }, 114, { 'ь',   0,   0,   0}},     // 110 модул...
     {{Scaner::ID     , 0                    }, 115, { 'к',   0,   0,   0}},     // 111 малень...
     {{Scaner::KEYWORD, Scaner::MASSIV       },   0, {   0,   0,   0,   0}},     // 112 массив
     {{Scaner::ID     , 0                    }, 116, { 'т',   0,   0,   0}},     // 113 множес...
     {{Scaner::KEYWORD, Scaner::MODUL        },   0, {   0,   0,   0,   0}},     // 114 модуль
     {{Scaner::ID     , 0                    }, 117, { 'о',   0,   0,   0}},     // 115 маленьк...
     {{Scaner::ID     , 0                    }, 118, { 'в',   0,   0,   0}},     // 116 множест...
     {{Scaner::ID     , 0                    }, 119, { 'е',   0,   0,   0}},     // 117 маленько...
     {{Scaner::ID     , 0                    }, 120, { 'о',   0,   0,   0}},     // 118 множеств...
     {{Scaner::KEYWORD, Scaner::MALENKOE     },   0, {   0,   0,   0,   0}},     // 119 маленькое
     {{Scaner::KEYWORD, Scaner::MNOZHESTVO   },   0, {   0,   0,   0,   0}},     // 120 множество
     {{Scaner::ID     , 0                    }, 122, { 'и',   0,   0,   0}},     // 121 н...
     {{Scaner::ID     , 0                    }, 123, { 'ч',   0,   0,   0}},     // 122 ни...
     {{Scaner::ID     , 0                    }, 124, { 'т',   0,   0,   0}},     // 123 нич...
     {{Scaner::ID     , 0                    }, 125, { 'о',   0,   0,   0}},     // 124 ничт...
     {{Scaner::KEYWORD, Scaner::NICHTO       },   0, {   0,   0,   0,   0}},     // 125 ничто
     {{Scaner::ID     , 0                    }, 127, { 'б',   0,   0,   0}},     // 126 о...
     {{Scaner::ID     , 0                    }, 128, { 'ъ',   0,   0,   0}},     // 127 об...
     {{Scaner::ID     , 0                    }, 129, { 'е',   0,   0,   0}},     // 128 объ...
     {{Scaner::ID     , 0                    }, 130, { 'д',   0,   0,   0}},     // 129 объе...
     {{Scaner::ID     , 0                    }, 131, { 'и',   0,   0,   0}},     // 130 объед...
     {{Scaner::ID     , 0                    }, 132, { 'н',   0,   0,   0}},     // 131 объеди...
     {{Scaner::ID     , 0                    }, 133, { 'е',   0,   0,   0}},     // 132 объедин...
     {{Scaner::ID     , 0                    }, 134, { 'н',   0,   0,   0}},     // 133 объедине...
     {{Scaner::ID     , 0                    }, 135, { 'и',   0,   0,   0}},     // 134 объединен...
     {{Scaner::ID     , 0                    }, 136, { 'е',   0,   0,   0}},     // 135 объединени...
     {{Scaner::KEYWORD, Scaner::OBEDINENIE   },   0, {   0,   0,   0,   0}},     // 136 объединение
     {{Scaner::ID     , 0                    }, 138, { 'е', 'о',   0,   0}},     // 137 п...
     {{Scaner::ID     , 0                    }, 140, { 'р',   0,   0,   0}},     // 138 пе...
     {{Scaner::KEYWORD, Scaner::PO           }, 141, { 'в', 'к',   0,   0}},     // 139 по...
     {{Scaner::ID     , 0                    }, 143, { 'е',   0,   0,   0}},     // 140 пер...
     {{Scaner::ID     , 0                    }, 144, { 'т',   0,   0,   0}},     // 141 пов...
     {{Scaner::ID     , 0                    }, 145, { 'а',   0,   0,   0}},     // 142 пок...
     {{Scaner::ID     , 0                    }, 146, { 'м', 'ч',   0,   0}},     // 143 пере...
     {{Scaner::ID     , 0                    }, 148, { 'о',   0,   0,   0}},     // 144 повт...
     {{Scaner::KEYWORD, Scaner::POKA         },   0, {   0,   0,   0,   0}},     // 145 пока
     {{Scaner::KEYWORD, Scaner::PEREM        },   0, {   0,   0,   0,   0}},     // 146 перем
     {{Scaner::ID     , 0                    }, 149, { 'и',   0,   0,   0}},     // 147 переч...
     {{Scaner::ID     , 0                    }, 150, { 'р',   0,   0,   0}},     // 148 повто...
     {{Scaner::ID     , 0                    }, 151, { 'с',   0,   0,   0}},     // 149 перечи...
     {{Scaner::ID     , 0                    }, 152, { 'я',   0,   0,   0}},     // 150 повтор...
     {{Scaner::ID     , 0                    }, 153, { 'л',   0,   0,   0}},     // 151 перечис...
     {{Scaner::ID     , 0                    }, 154, { 'й',   0,   0,   0}},     // 152 повторя...
     {{Scaner::ID     , 0                    }, 155, { 'е',   0,   0,   0}},     // 153 перечисл...
     {{Scaner::KEYWORD, Scaner::POVTORYAI    },   0, {   0,   0,   0,   0}},     // 154 повторяй
     {{Scaner::ID     , 0                    }, 156, { 'н',   0,   0,   0}},     // 155 перечисле...
     {{Scaner::ID     , 0                    }, 157, { 'и',   0,   0,   0}},     // 156 перечислен...
     {{Scaner::ID     , 0                    }, 158, { 'е',   0,   0,   0}},     // 157 перечислени...
     {{Scaner::KEYWORD, Scaner::PERECHISLENIE},   0, {   0,   0,   0,   0}},     // 158 перечисление
     {{Scaner::ID     , 0                    }, 160, { 'а',   0,   0,   0}},     // 159 р...
     {{Scaner::ID     , 0                    }, 161, { 'с',   0,   0,   0}},     // 160 ра...
     {{Scaner::ID     , 0                    }, 162, { 'с',   0,   0,   0}},     // 161 рас...
     {{Scaner::ID     , 0                    }, 163, { 'м',   0,   0,   0}},     // 162 расс...
     {{Scaner::ID     , 0                    }, 164, { 'а',   0,   0,   0}},     // 163 рассм...
     {{Scaner::ID     , 0                    }, 165, { 'т',   0,   0,   0}},     // 164 рассма...
     {{Scaner::ID     , 0                    }, 166, { 'р',   0,   0,   0}},     // 165 рассмат...
     {{Scaner::ID     , 0                    }, 167, { 'и',   0,   0,   0}},     // 166 рассматр...
     {{Scaner::ID     , 0                    }, 168, { 'в',   0,   0,   0}},     // 167 рассматри...
     {{Scaner::ID     , 0                    }, 169, { 'а',   0,   0,   0}},     // 168 рассматрив...
     {{Scaner::ID     , 0                    }, 170, { 'й',   0,   0,   0}},     // 169 рассматрива...
     {{Scaner::KEYWORD, Scaner::RASSMATRIVAI },   0, {   0,   0,   0,   0}},     // 170 рассматривай
     {{Scaner::ID     , 0                    }, 172, { 'и', 'т',   0,   0}},     // 171 с...
     {{Scaner::ID     , 0                    }, 174, { 'м',   0,   0,   0}},     // 172 си...
     {{Scaner::ID     , 0                    }, 175, { 'р',   0,   0,   0}},     // 173 ст...
     {{Scaner::ID     , 0                    }, 176, { 'в',   0,   0,   0}},     // 174 сим...
     {{Scaner::ID     , 0                    }, 177, { 'у',   0,   0,   0}},     // 175 стр...
     {{Scaner::KEYWORD, Scaner::SIMV         },   0, {   0,   0,   0,   0}},     // 176 симв
     {{Scaner::ID     , 0                    }, 178, { 'к',   0,   0,   0}},     // 177 стру...
     {{Scaner::ID     , 0                    }, 179, { 'т',   0,   0,   0}},     // 178 струк...
     {{Scaner::ID     , 0                    }, 180, { 'у',   0,   0,   0}},     // 179 структ...
     {{Scaner::ID     , 0                    }, 181, { 'р',   0,   0,   0}},     // 180 структу...
     {{Scaner::ID     , 0                    }, 182, { 'а',   0,   0,   0}},     // 181 структур...
     {{Scaner::KEYWORD, Scaner::STRUKTURA    },   0, {   0,   0,   0,   0}},     // 182 структура
     {{Scaner::ID     , 0                    }, 184, { 'и', 'о',   0,   0}},     // 183 т...
     {{Scaner::ID     , 0                    }, 186, { 'п',   0,   0,   0}},     // 184 ти...
     {{Scaner::KEYWORD, Scaner::TO           },   0, {   0,   0,   0,   0}},     // 185 то
     {{Scaner::KEYWORD, Scaner::TIP          },   0, {   0,   0,   0,   0}},     // 186 тип
     {{Scaner::ID     , 0                    }, 188, { 'е',   0,   0,   0}},     // 187 ц...
     {{Scaner::ID     , 0                    }, 189, { 'л',   0,   0,   0}},     // 188 це...
     {{Scaner::KEYWORD, Scaner::CEL          },   0, {   0,   0,   0,   0}},     // 189 цел
     {{Scaner::ID     , 0                    }, 191, { 'л',   0,   0,   0}},     // 190 э...
     {{Scaner::ID     , 0                    }, 192, { 'е',   0,   0,   0}},     // 191 эл...
     {{Scaner::ID     , 0                    }, 193, { 'м',   0,   0,   0}},     // 192 эле...
     {{Scaner::ID     , 0                    }, 194, { 'е',   0,   0,   0}},     // 193 элем...
     {{Scaner::ID     , 0                    }, 195, { 'н',   0,   0,   0}},     // 194 элеме...
     {{Scaner::ID     , 0                    }, 196, { 'т',   0,   0,   0}},     // 195 элемен...
     {{Scaner::KEYWORD, Scaner::ELEMENT      },   0, {   0,   0,   0,   0}}      // 196 элемент
};

ELEM a_DELIMETERS_jump_table[] = {
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },   1, { '&', '=', '|',   0}},     //   0 !...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },   4, { '&',   0,   0,   0}},     //   1 !&...
     {{Scaner::DELIMETER, Scaner::NOT_EQUAL             },   0, {   0,   0,   0,   0}},     //   2 !=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },   5, { '|',   0,   0,   0}},     //   3 !|...
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_AND         },   6, { '.', ':',   0,   0}},     //   4 !&&...
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_OR          },   8, { '.', ':',   0,   0}},     //   5 !||...
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_AND_SOKR    },  10, { ':',   0,   0,   0}},     //   6 !&&....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  11, { '=',   0,   0,   0}},     //   7 !&&:...
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_OR_SOKR     },  12, { ':',   0,   0,   0}},     //   8 !||....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  13, { '=',   0,   0,   0}},     //   9 !||:...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  14, { '=',   0,   0,   0}},     //  10 !&&.:...
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_AND_PR      },   0, {   0,   0,   0,   0}},     //  11 !&&:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  15, { '=',   0,   0,   0}},     //  12 !||.:...
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_OR_PR       },   0, {   0,   0,   0,   0}},     //  13 !||:=
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_AND_SOKR_PR },   0, {   0,   0,   0,   0}},     //  14 !&&.:=
     {{Scaner::DELIMETER, Scaner::LOGIC_NOT_OR_SOKR_PR  },   0, {   0,   0,   0,   0}},     //  15 !||.:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  17, { '#',   0,   0,   0}},     //  16 #...
     {{Scaner::DELIMETER, Scaner::SIZEOF                },   0, {   0,   0,   0,   0}},     //  17 ##
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  19, { '.', ':',   0,   0}},     //  18 %...
     {{Scaner::DELIMETER, Scaner::MOD_SOKR              },  21, { ':',   0,   0,   0}},     //  19 %....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  22, { '=',   0,   0,   0}},     //  20 %:...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  23, { '=',   0,   0,   0}},     //  21 %.:...
     {{Scaner::DELIMETER, Scaner::MOD_PR                },   0, {   0,   0,   0,   0}},     //  22 %:=
     {{Scaner::DELIMETER, Scaner::MOD_SOKR_PR           },   0, {   0,   0,   0,   0}},     //  23 %.:=
     {{Scaner::DELIMETER, Scaner::BIT_AND               },  25, { '&', ':',   0,   0}},     //  24 &...
     {{Scaner::DELIMETER, Scaner::LOGIC_AND             },  27, { '.', ':',   0,   0}},     //  25 &&...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  29, { '=',   0,   0,   0}},     //  26 &:...
     {{Scaner::DELIMETER, Scaner::LOGIC_AND_SOKR        },  30, { ':',   0,   0,   0}},     //  27 &&....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  31, { '=',   0,   0,   0}},     //  28 &&:...
     {{Scaner::DELIMETER, Scaner::BIT_AND_PR            },   0, {   0,   0,   0,   0}},     //  29 &:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  32, { '=',   0,   0,   0}},     //  30 &&.:...
     {{Scaner::DELIMETER, Scaner::LOGIC_AND_PR          },   0, {   0,   0,   0,   0}},     //  31 &&:=
     {{Scaner::DELIMETER, Scaner::LOGIC_AND_SOKR_PR     },   0, {   0,   0,   0,   0}},     //  32 &&.:=
     {{Scaner::DELIMETER, Scaner::MULTI                 },  34, { '*', '.', ':',   0}},     //  33 *...
     {{Scaner::DELIMETER, Scaner::POWER                 },  37, { '.', ':',   0,   0}},     //  34 **...
     {{Scaner::DELIMETER, Scaner::MUL_ROUND             },  39, { ':',   0,   0,   0}},     //  35 *....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  40, { '=',   0,   0,   0}},     //  36 *:...
     {{Scaner::DELIMETER, Scaner::POWER_ROUND           },  41, { ':',   0,   0,   0}},     //  37 **....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  42, { '=',   0,   0,   0}},     //  38 **:...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  43, { '=',   0,   0,   0}},     //  39 *.:...
     {{Scaner::DELIMETER, Scaner::MUL_PR                },   0, {   0,   0,   0,   0}},     //  40 *:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  44, { '=',   0,   0,   0}},     //  41 **.:...
     {{Scaner::DELIMETER, Scaner::POWER_PR              },   0, {   0,   0,   0,   0}},     //  42 **:=
     {{Scaner::DELIMETER, Scaner::MUL_ROUND_PR          },   0, {   0,   0,   0,   0}},     //  43 *.:=
     {{Scaner::DELIMETER, Scaner::POWER_ROUND_PR        },   0, {   0,   0,   0,   0}},     //  44 **.:=
     {{Scaner::DELIMETER, Scaner::ADD                   },  46, { '+', '.', ':', '|'}},     //  45 +...
     {{Scaner::DELIMETER, Scaner::NEXT_VALUE            },  50, { '<',   0,   0,   0}},     //  46 ++...
     {{Scaner::DELIMETER, Scaner::SUM_ROUND             },  51, { ':',   0,   0,   0}},     //  47 +....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  52, { '=',   0,   0,   0}},     //  48 +:...
     {{Scaner::DELIMETER, Scaner::FOR_LABEL             },   0, {   0,   0,   0,   0}},     //  49 +|
     {{Scaner::DELIMETER, Scaner::NEXT_VALUE_WRAP       },   0, {   0,   0,   0,   0}},     //  50 ++<
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  53, { '=',   0,   0,   0}},     //  51 +.:...
     {{Scaner::DELIMETER, Scaner::SUM_PR                },   0, {   0,   0,   0,   0}},     //  52 +:=
     {{Scaner::DELIMETER, Scaner::SUM_ROUND_PR          },   0, {   0,   0,   0,   0}},     //  53 +.:=
     {{Scaner::DELIMETER, Scaner::SUB                   },  55, { '-', '.', ':',   0}},     //  54 -...
     {{Scaner::DELIMETER, Scaner::PREV_VALUE            },  58, { '<',   0,   0,   0}},     //  55 --...
     {{Scaner::DELIMETER, Scaner::SUB_ROUND             },  59, { ':',   0,   0,   0}},     //  56 -....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  60, { '=',   0,   0,   0}},     //  57 -:...
     {{Scaner::DELIMETER, Scaner::PREV_VALUE_WRAP       },   0, {   0,   0,   0,   0}},     //  58 --<
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  61, { '=',   0,   0,   0}},     //  59 -.:...
     {{Scaner::DELIMETER, Scaner::SUB_PR                },   0, {   0,   0,   0,   0}},     //  60 -:=
     {{Scaner::DELIMETER, Scaner::SUB_ROUNT_PR          },   0, {   0,   0,   0,   0}},     //  61 -.:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  63, { '.',   0,   0,   0}},     //  62 ....
     {{Scaner::DELIMETER, Scaner::RANGE                 },   0, {   0,   0,   0,   0}},     //  63 ..
     {{Scaner::DELIMETER, Scaner::DIV                   },  65, { '.', ':','\\',   0}},     //  64 /...
     {{Scaner::DELIMETER, Scaner::DIV_ROUND             },  68, { ':',   0,   0,   0}},     //  65 /....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  69, { '=',   0,   0,   0}},     //  66 /:...
     {{Scaner::DELIMETER, Scaner::SET_XOR               },  70, { ':',   0,   0,   0}},     //  67 /\...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  71, { '=',   0,   0,   0}},     //  68 /.:...
     {{Scaner::DELIMETER, Scaner::INT_DIV               },   0, {   0,   0,   0,   0}},     //  69 /:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  72, { '=',   0,   0,   0}},     //  70 /\:...
     {{Scaner::DELIMETER, Scaner::DIV_ROUND_PR          },   0, {   0,   0,   0,   0}},     //  71 /.:=
     {{Scaner::DELIMETER, Scaner::SET_XOR_PR            },   0, {   0,   0,   0,   0}},     //  72 /\:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  74, { ':', '=',   0,   0}},     //  73 :...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  76, { '=',   0,   0,   0}},     //  74 ::...
     {{Scaner::DELIMETER, Scaner::PRISV                 },   0, {   0,   0,   0,   0}},     //  75 :=
     {{Scaner::DELIMETER, Scaner::COPY                  },   0, {   0,   0,   0,   0}},     //  76 ::=
     {{Scaner::DELIMETER, Scaner::LESS                  },  78, { '<', '=',   0,   0}},     //  77 <...
     {{Scaner::DELIMETER, Scaner::SHIFT_LEFT            },  80, { ':',   0,   0,   0}},     //  78 <<...
     {{Scaner::DELIMETER, Scaner::LESS_OR_EQUAL         },   0, {   0,   0,   0,   0}},     //  79 <=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  81, { '=',   0,   0,   0}},     //  80 <<:...
     {{Scaner::DELIMETER, Scaner::SHIFT_LEFT_PR         },   0, {   0,   0,   0,   0}},     //  81 <<:=
     {{Scaner::DELIMETER, Scaner::GREAT                 },  83, { '=', '>',   0,   0}},     //  82 >...
     {{Scaner::DELIMETER, Scaner::GREAT_OR_EQUAL        },   0, {   0,   0,   0,   0}},     //  83 >=
     {{Scaner::DELIMETER, Scaner::SHIFT_RIGHT           },  85, { ':',   0,   0,   0}},     //  84 >>...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  86, { '=',   0,   0,   0}},     //  85 >>:...
     {{Scaner::DELIMETER, Scaner::SHIFT_RIGHT_PR        },   0, {   0,   0,   0,   0}},     //  86 >>:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  88, { '.',   0,   0,   0}},     //  87 ?...
     {{Scaner::DELIMETER, Scaner::LOGIC_TERN            },   0, {   0,   0,   0,   0}},     //  88 ?.
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  90, { '|',   0,   0,   0}},     //  89 @...
     {{Scaner::DELIMETER, Scaner::TYPE                  },   0, {   0,   0,   0,   0}},     //  90 @|
     {{Scaner::DELIMETER, Scaner::SET_SUB               },  92, { ':',   0,   0,   0}},     //  91 \...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  93, { '=',   0,   0,   0}},     //  92 \:...
     {{Scaner::DELIMETER, Scaner::SET_SUB_PR            },   0, {   0,   0,   0,   0}},     //  93 \:=
     {{Scaner::DELIMETER, Scaner::BIT_XOR               },  95, { ':', '^',   0,   0}},     //  94 ^...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  97, { '=',   0,   0,   0}},     //  95 ^:...
     {{Scaner::DELIMETER, Scaner::LOGIC_XOR             },  98, { ':',   0,   0,   0}},     //  96 ^^...
     {{Scaner::DELIMETER, Scaner::BIT_XOR_PR            },   0, {   0,   0,   0,   0}},     //  97 ^:=
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               },  99, { '=',   0,   0,   0}},     //  98 ^^:...
     {{Scaner::DELIMETER, Scaner::LOGIC_XOR_PR          },   0, {   0,   0,   0,   0}},     //  99 ^^:=
     {{Scaner::DELIMETER, Scaner::FIG_OPEN              }, 101, { '{',   0,   0,   0}},     // 100 {...
     {{Scaner::DELIMETER, Scaner::DOUBLE_FIG_OPEN       },   0, {   0,   0,   0,   0}},     // 101 {{
     {{Scaner::DELIMETER, Scaner::BIT_OR                }, 103, { ':', '|',   0,   0}},     // 102 |...
     {{Scaner::DELIMETER, Scaner::GLOBAL_VAR            }, 105, { '=',   0,   0,   0}},     // 103 |:...
     {{Scaner::DELIMETER, Scaner::LOGIC_OR              }, 106, { '.', ':',   0,   0}},     // 104 ||...
     {{Scaner::DELIMETER, Scaner::BIT_OR_PR             },   0, {   0,   0,   0,   0}},     // 105 |:=
     {{Scaner::DELIMETER, Scaner::LOGIC_OR_SOKR         }, 108, { ':',   0,   0,   0}},     // 106 ||....
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               }, 109, { '=',   0,   0,   0}},     // 107 ||:...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               }, 110, { '=',   0,   0,   0}},     // 108 ||.:...
     {{Scaner::DELIMETER, Scaner::LOGIC_OR_PR           },   0, {   0,   0,   0,   0}},     // 109 ||:=
     {{Scaner::DELIMETER, Scaner::LOGIC_OR_SOKR_PR      },   0, {   0,   0,   0,   0}},     // 110 ||.:=
     {{Scaner::DELIMETER, Scaner::FIG_CLOSE             }, 112, { '}',   0,   0,   0}},     // 111 }...
     {{Scaner::DELIMETER, Scaner::DOUBLE_FIG_CLOSE      },   0, {   0,   0,   0,   0}},     // 112 }}
     {{Scaner::DELIMETER, Scaner::BIT_NOT               }, 114, { '&', '|',   0,   0}},     // 113 ~...
     {{Scaner::DELIMETER, Scaner::BIT_NOT_AND           }, 116, { ':',   0,   0,   0}},     // 114 ~&...
     {{Scaner::DELIMETER, Scaner::BIT_NOT_OR            }, 117, { ':',   0,   0,   0}},     // 115 ~|...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               }, 118, { '=',   0,   0,   0}},     // 116 ~&:...
     {{Scaner::UNKNOWN  , Scaner::UNKNOWN               }, 119, { '=',   0,   0,   0}},     // 117 ~|:...
     {{Scaner::DELIMETER, Scaner::BIT_NOT_AND_PR        },   0, {   0,   0,   0,   0}},     // 118 ~&:=
     {{Scaner::DELIMETER, Scaner::BIT_NOT_OR_PR         },   0, {   0,   0,   0,   0}}      // 119 ~|:=
};

bool Scaner::delimiters_proc(){
	bool t = false;
	probably_code = static_cast<LEXEM_CODE>(a_KEYWORD_jump_table[state].code);
	if(char_category == PART_OF_DELIMITER){
		int y = search_char(currrent_char, a_KEYWORD_jump_table[state].symbols);
		if(y != 4){
			state = a_KEYWORD_jump_table[state].first_state + y;
			t = true;
		}
	}
	return t;
}
/*
bool Scaner::string_char_proc() {
	
	switch (state) {
		case 0:
			switch (char_category) {
				case QUOTE:
					state = 3;
					break;
				default:
					state = 0;
					break;
			}
			break;
		case 1:
			switch (char_category) {
				case QUOTE:
					state = 1;
					break;
				default:
					state = 4;
					break;	
			}
			break;	
		case 2:
			switch (char_category) {
				case DIGITS_1_9:
					state = 5;
					break;
				case DIGIT_0:
					state = 6;
					break;
			}
			break;
		case 3:
			switch (char_category) {
				case QUOTE:
					state = 0;
					break;
				case DQOUTE:
					state = 1;
					break;
				case DOLLAR:
					state = 2;
					break;	
			}
			break;	
		case 4:
			switch (char_category) {
				case QUOTE:
					state = 0;
					break;
				case DQUOTE:
					state = 1;
					break;
				case DOLLAR:
					state = 2;
					break;	
			}
			break;
		case 5:
			switch (char_category) {
				case QUOTE:
					state = 0;
					break;
				case DQOUTE:
					state = 1;
					break;
			}
			break;
		case 6:
			switch (char_category) {
				case QOUTE:
					state = 0;
					break;
				case DQOUTE:
					state = 1;
					break;
				case DIGIT_0: case DIGITS_1_9:
					state = 5;
					break;
				case LETTER:
					switch (current_char) {
						case 'x': case 'X':
							state = 7;
							break;
					}
					break;	
			}
			break;
		case 7:
			switch (char_category) {
				case DIGIT_0: case DIGITS_1_9:
					state = 8;
					break;
				case LETTER:
					if ((current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F')) {
						state = 8;
						break;			
					}	
			}
		case 8:
			switch (char_category) {
				case QUOTE:
					state = 0;
					break;
				case DQUOTE:
					state = 1;
					break;
				case DIGIT_0: case DIGITS_1_9:
					state = 8;
					break;
				case LETTER:
					if ((current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F')) {
						state = 8;
						break;			
					}	
			}
	}
}
*/

bool Scaner::keyword_proc(){
	bool t = false;
	if( (1<<char_category) & ( (1<<KEYWORD_BEGIN) | (1<<LETTER) ) ){
		int y = search_char(currrent_char, a_KEYWORD_jump_table[state].symbols);
		probably_code = static_cast<LEXEM_CODE>(a_KEYWORD_jump_table[state].code);
		if(y != 4){
			state = a_KEYWORD_jump_table[state].first_state + y;
			t = true;
		}		
	}
	return t;
}

bool Scaner::comment_proc(){
	switch (state) {
		case 0:
			switch (current_char) {
				case '(':
					state = 1;
					break;
				case '*':
					state = 2;
					break;
			}
			break;
		case 1:
			switch (current_char) {
				case '*':
					state = 3;
					commentary_level++;
					break;
				default:
					state = 0;
			}
			break;
		case 2:
			switch (current_char) {
				case '(':
					state = 1;
					break;
				case ')':
					state = 3;
					commentary_level--;
					break;
				default:
					state = 0;
			}
			break;
		case 3:
			state = 0;
			pcurrent_char--;
			if (!commentary_level) Automaton = a_COMMENT;
			break;
	}
}

bool Scaner::numbers_proc() {
	switch (state) {
		case 0:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 1;
					break;
				case LETTER_sSdDeE:
					state = 4;
					break;
				case LETTER_xX:
					state = 5;
					break;	
				case PART_OF_DELIMITER:
					if (currenct_char == '.') {
						state = 2;
						break;
					}
				default:	
			}
			break;
		case 1:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 1;
					break;
				case LETTER_sSdDeE:
					state = 4;
					break;
				case PART_OF_DELIMITER:
					if (currenct_char == '.') state = 2;
					break;
				default:
			}
			break;
		case 2:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 3;
					break;
				default:
			}
			break;
		case 3:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 3;
					break;
				case LETTER_sSdDeE:
					state = 4;
					break;
				default:
			}
			break;
		case 4:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 7;
					break;
				case PART_OF_DELIMITER:
					switch (current_char) {
						case '+': case '-':
							state = 6;
							break;
					}	
			}
			break;
		case 5:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 8;
					break;
				case LATIN_LETTER: case LETTER_sSdDeE: case LETTER_xX:
					state = 8;
					break;
				default:	
			}
			break;	
		case 6:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 7;
					break;
				default:
			}
			break;
		case 7:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 7;
					break;
				default:
			}
			break;				
		case 8:
			switch (char_category) {
				case DIGIT_0: case DIGIT_1_9:
					state = 8;
					break;
				case LATIN_LETTER: case LETTER_sSdDeE: case LETTER_xX:
					state = 8;
					break;
				default:	
			}
			break;	
	}	
}

Scaner::Automaton_proc Scaner::procs[] = {
     &Scaner::start_proc     , &Scaner::unknown_proc, 
	 &Scaner::delimiters_proc, &Scaner::keyword_proc
};

