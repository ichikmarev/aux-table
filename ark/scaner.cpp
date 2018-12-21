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
			printf("���������������� ������� � ������ %d.\n", current_line);
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
			state = (((char)currrent_char) == '�') ? 0: 6; //FIXME
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

/* ������������ ������ ������� � ������� �� 4 ��������. ���� ������ � ������� ����,
   �� ������������ ������ ���������������� ��������. ����� ������������ ����� 4.
   ������� ������ �� ������ ���� ����� ����. */
int search_char(unsigned char c, char array[4]){
	unsigned int x = (c * 0x01010101) ^ (*((int*)array)); 
	/* ����� � ���������� �������������� ����� ����� ��������� ����� ������ ��������
       �������� ����� � ���������� ���� �����. */
    /* ����� ���� ����� ������������ ���, ��� ���������� �� ���.102 �����
     "������ �.�. ��������������� ����� ��� �������������.: ���. � ����. --- �.: 
	  �������, 2003." */
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
     {{Scaner::ID     , 0                    },   1, { '�', '�',   0,   0}},     //   0 �...
     {{Scaner::ID     , 0                    },   3, { '�',   0,   0,   0}},     //   1 ��...
     {{Scaner::ID     , 0                    },   4, { '�',   0,   0,   0}},     //   2 ��...
     {{Scaner::KEYWORD, Scaner::ALG          },   0, {   0,   0,   0,   0}},     //   3 ���
     {{Scaner::KEYWORD, Scaner::ASM          },   0, {   0,   0,   0,   0}},     //   4 ���
     {{Scaner::ID     , 0                    },   6, { '�', '�',   0,   0}},     //   5 �...
     {{Scaner::ID     , 0                    },   8, { '�',   0,   0,   0}},     //   6 ��...
     {{Scaner::ID     , 0                    },   9, { '�',   0,   0,   0}},     //   7 ��...
     {{Scaner::ID     , 0                    },  10, { '�',   0,   0,   0}},     //   8 ���...
     {{Scaner::ID     , 0                    },  11, { '�',   0,   0,   0}},     //   9 ���...
     {{Scaner::ID     , 0                    },  12, { '�',   0,   0,   0}},     //  10 ����...
     {{Scaner::ID     , 0                    },  13, { '�',   0,   0,   0}},     //  11 ����...
     {{Scaner::KEYWORD, Scaner::BEZZN        },   0, {   0,   0,   0,   0}},     //  12 �����
     {{Scaner::ID     , 0                    },  14, { '�',   0,   0,   0}},     //  13 �����...
     {{Scaner::ID     , 0                    },  15, { '�',   0,   0,   0}},     //  14 ������...
     {{Scaner::KEYWORD, Scaner::BOLSHOE      },   0, {   0,   0,   0,   0}},     //  15 �������
     {{Scaner::ID     , 0                    },  17, { '�', '�', '�',   0}},     //  16 �...
     {{Scaner::ID     , 0                    },  20, { '�', '�',   0,   0}},     //  17 ��...
     {{Scaner::ID     , 0                    },  22, { '�',   0,   0,   0}},     //  18 ��...
     {{Scaner::ID     , 0                    },  23, { '�', '�',   0,   0}},     //  19 ��...
     {{Scaner::ID     , 0                    },  25, { '�',   0,   0,   0}},     //  20 ���...
     {{Scaner::KEYWORD, Scaner::VESH         },   0, {   0,   0,   0,   0}},     //  21 ���
     {{Scaner::ID     , 0                    },  26, { '�',   0,   0,   0}},     //  22 ���...
     {{Scaner::ID     , 0                    },  27, { '�',   0,   0,   0}},     //  23 ���...
     {{Scaner::ID     , 0                    },  28, { '�',   0,   0,   0}},     //  24 ���...
     {{Scaner::ID     , 0                    },  29, { '�',   0,   0,   0}},     //  25 ����...
     {{Scaner::ID     , 0                    },  30, { '�',   0,   0,   0}},     //  26 ����...
     {{Scaner::ID     , 0                    },  31, { '�',   0,   0,   0}},     //  27 ����...
     {{Scaner::ID     , 0                    },  32, { '�',   0,   0,   0}},     //  28 ����...
     {{Scaner::KEYWORD, Scaner::VECHNO       },   0, {   0,   0,   0,   0}},     //  29 �����
     {{Scaner::ID     , 0                    },  33, { '�',   0,   0,   0}},     //  30 �����...
     {{Scaner::ID     , 0                    },  34, { '�',   0,   0,   0}},     //  31 �����...
     {{Scaner::KEYWORD, Scaner::VIIDI        },   0, {   0,   0,   0,   0}},     //  32 �����
     {{Scaner::ID     , 0                    },  35, { '�',   0,   0,   0}},     //  33 ������...
     {{Scaner::KEYWORD, Scaner::VIBERI       },   0, {   0,   0,   0,   0}},     //  34 ������
     {{Scaner::KEYWORD, Scaner::VOZVRAT      },   0, {   0,   0,   0,   0}},     //  35 �������
     {{Scaner::ID     , 0                    },  37, { '�', '�',   0,   0}},     //  36 �...
     {{Scaner::ID     , 0                    },  39, { '�',   0,   0,   0}},     //  37 ��...
     {{Scaner::ID     , 0                    },  40, { '�',   0,   0,   0}},     //  38 ��...
     {{Scaner::ID     , 0                    },  41, { '�',   0,   0,   0}},     //  39 ���...
     {{Scaner::ID     , 0                    },  42, { '�',   0,   0,   0}},     //  40 ���...
     {{Scaner::ID     , 0                    },  43, { '�',   0,   0,   0}},     //  41 ����...
     {{Scaner::ID     , 0                    },  44, { '�',   0,   0,   0}},     //  42 ����...
     {{Scaner::ID     , 0                    },  45, { '�',   0,   0,   0}},     //  43 �����...
     {{Scaner::KEYWORD, Scaner::GRYAZN       },   0, {   0,   0,   0,   0}},     //  44 �����
     {{Scaner::ID     , 0                    },  46, { '�',   0,   0,   0}},     //  45 ������...
     {{Scaner::ID     , 0                    },  47, { '�',   0,   0,   0}},     //  46 �������...
     {{Scaner::KEYWORD, Scaner::GOLOVNOI     },   0, {   0,   0,   0,   0}},     //  47 ��������
     {{Scaner::ID     , 0                    },  49, { '�',   0,   0,   0}},     //  48 �...
     {{Scaner::ID     , 0                    },  50, { '�',   0,   0,   0}},     //  49 ��...
     {{Scaner::KEYWORD, Scaner::DLYA         },   0, {   0,   0,   0,   0}},     //  50 ���
     {{Scaner::ID     , 0                    },  52, { '�',   0,   0,   0}},     //  51 �...
     {{Scaner::ID     , 0                    },  53, { '�',   0,   0,   0}},     //  52 ��...
     {{Scaner::ID     , 0                    },  54, { '�',   0,   0,   0}},     //  53 ���...
     {{Scaner::KEYWORD, Scaner::ESLI         },   0, {   0,   0,   0,   0}},     //  54 ����
     {{Scaner::ID     , 0                    },  56, { '�',   0,   0,   0}},     //  55 �...
     {{Scaner::ID     , 0                    },  57, { '�',   0,   0,   0}},     //  56 ��...
     {{Scaner::ID     , 0                    },  58, { '�',   0,   0,   0}},     //  57 ���...
     {{Scaner::ID     , 0                    },  59, { '�',   0,   0,   0}},     //  58 ����...
     {{Scaner::ID     , 0                    },  60, { '�',   0,   0,   0}},     //  59 �����...
     {{Scaner::ID     , 0                    },  61, { '�',   0,   0,   0}},     //  60 ������...
     {{Scaner::ID     , 0                    },  62, { '�', '�',   0,   0}},     //  61 �������...
     {{Scaner::KEYWORD, Scaner::ZNACHENIYU   },   0, {   0,   0,   0,   0}},     //  62 ��������
     {{Scaner::KEYWORD, Scaner::ZNACHENIYA   },   0, {   0,   0,   0,   0}},     //  63 ��������
     {{Scaner::ID     , 0                    },  65, { '�', '�', '�',   0}},     //  64 �...
     {{Scaner::KEYWORD, Scaner::IZ           },   0, {   0,   0,   0,   0}},     //  65 ��
     {{Scaner::ID     , 0                    },  68, { '�', '�',   0,   0}},     //  66 ��...
     {{Scaner::ID     , 0                    },  70, { '�', '�',   0,   0}},     //  67 ��...
     {{Scaner::ID     , 0                    },  72, { '�',   0,   0,   0}},     //  68 ���...
     {{Scaner::ID     , 0                    },  73, { '�',   0,   0,   0}},     //  69 ���...
     {{Scaner::ID     , 0                    },  74, { '�',   0,   0,   0}},     //  70 ���...
     {{Scaner::ID     , 0                    },  75, { '�',   0,   0,   0}},     //  71 ���...
     {{Scaner::ID     , 0                    },  76, { '�',   0,   0,   0}},     //  72 ����...
     {{Scaner::KEYWORD, Scaner::INES         },   0, {   0,   0,   0,   0}},     //  73 ����
     {{Scaner::ID     , 0                    },  77, { '�',   0,   0,   0}},     //  74 ����...
     {{Scaner::ID     , 0                    },  78, { '�',   0,   0,   0}},     //  75 ����...
     {{Scaner::KEYWORD, Scaner::INACHE       },   0, {   0,   0,   0,   0}},     //  76 �����
     {{Scaner::ID     , 0                    },  79, { '�',   0,   0,   0}},     //  77 �����...
     {{Scaner::ID     , 0                    },  80, { '�',   0,   0,   0}},     //  78 �����...
     {{Scaner::ID     , 0                    },  81, { '�',   0,   0,   0}},     //  79 ������...
     {{Scaner::KEYWORD, Scaner::ISTINA       },   0, {   0,   0,   0,   0}},     //  80 ������
     {{Scaner::ID     , 0                    },  82, { '�',   0,   0,   0}},     //  81 �������...
     {{Scaner::ID     , 0                    },  83, { '�',   0,   0,   0}},     //  82 ��������...
     {{Scaner::ID     , 0                    },  84, { '�',   0,   0,   0}},     //  83 ���������...
     {{Scaner::KEYWORD, Scaner::ISPOLZUET    },   0, {   0,   0,   0,   0}},     //  84 ����������
     {{Scaner::ID     , 0                    },  86, { '�',   0,   0,   0}},     //  85 �...
     {{Scaner::ID     , 0                    },  87, { '�',   0,   0,   0}},     //  86 ��...
     {{Scaner::ID     , 0                    },  88, { '�',   0,   0,   0}},     //  87 ���...
     {{Scaner::ID     , 0                    },  89, { '�',   0,   0,   0}},     //  88 ����...
     {{Scaner::KEYWORD, Scaner::KOMPL        },   0, {   0,   0,   0,   0}},     //  89 �����
     {{Scaner::ID     , 0                    },  91, { '�',   0,   0,   0}},     //  90 �...
     {{Scaner::ID     , 0                    },  92, { '�', '�',   0,   0}},     //  91 ��...
     {{Scaner::KEYWORD, Scaner::LOG          },   0, {   0,   0,   0,   0}},     //  92 ���
     {{Scaner::ID     , 0                    },  94, { '�',   0,   0,   0}},     //  93 ���...
     {{Scaner::KEYWORD, Scaner::LOZH         },   0, {   0,   0,   0,   0}},     //  94 ����
     {{Scaner::ID     , 0                    },  96, { '�', '�', '�',   0}},     //  95 �...
     {{Scaner::ID     , 0                    },  99, { '�', '�',   0,   0}},     //  96 ��...
     {{Scaner::ID     , 0                    }, 101, { '�',   0,   0,   0}},     //  97 ��...
     {{Scaner::ID     , 0                    }, 102, { '�',   0,   0,   0}},     //  98 ��...
     {{Scaner::ID     , 0                    }, 103, { '�',   0,   0,   0}},     //  99 ���...
     {{Scaner::ID     , 0                    }, 104, { '�',   0,   0,   0}},     // 100 ���...
     {{Scaner::ID     , 0                    }, 105, { '�',   0,   0,   0}},     // 101 ���...
     {{Scaner::ID     , 0                    }, 106, { '�',   0,   0,   0}},     // 102 ���...
     {{Scaner::ID     , 0                    }, 107, { '�',   0,   0,   0}},     // 103 ����...
     {{Scaner::ID     , 0                    }, 108, { '�',   0,   0,   0}},     // 104 ����...
     {{Scaner::ID     , 0                    }, 109, { '�',   0,   0,   0}},     // 105 ����...
     {{Scaner::ID     , 0                    }, 110, { '�',   0,   0,   0}},     // 106 ����...
     {{Scaner::ID     , 0                    }, 111, { '�',   0,   0,   0}},     // 107 �����...
     {{Scaner::ID     , 0                    }, 112, { '�',   0,   0,   0}},     // 108 �����...
     {{Scaner::ID     , 0                    }, 113, { '�',   0,   0,   0}},     // 109 �����...
     {{Scaner::ID     , 0                    }, 114, { '�',   0,   0,   0}},     // 110 �����...
     {{Scaner::ID     , 0                    }, 115, { '�',   0,   0,   0}},     // 111 ������...
     {{Scaner::KEYWORD, Scaner::MASSIV       },   0, {   0,   0,   0,   0}},     // 112 ������
     {{Scaner::ID     , 0                    }, 116, { '�',   0,   0,   0}},     // 113 ������...
     {{Scaner::KEYWORD, Scaner::MODUL        },   0, {   0,   0,   0,   0}},     // 114 ������
     {{Scaner::ID     , 0                    }, 117, { '�',   0,   0,   0}},     // 115 �������...
     {{Scaner::ID     , 0                    }, 118, { '�',   0,   0,   0}},     // 116 �������...
     {{Scaner::ID     , 0                    }, 119, { '�',   0,   0,   0}},     // 117 ��������...
     {{Scaner::ID     , 0                    }, 120, { '�',   0,   0,   0}},     // 118 ��������...
     {{Scaner::KEYWORD, Scaner::MALENKOE     },   0, {   0,   0,   0,   0}},     // 119 ���������
     {{Scaner::KEYWORD, Scaner::MNOZHESTVO   },   0, {   0,   0,   0,   0}},     // 120 ���������
     {{Scaner::ID     , 0                    }, 122, { '�',   0,   0,   0}},     // 121 �...
     {{Scaner::ID     , 0                    }, 123, { '�',   0,   0,   0}},     // 122 ��...
     {{Scaner::ID     , 0                    }, 124, { '�',   0,   0,   0}},     // 123 ���...
     {{Scaner::ID     , 0                    }, 125, { '�',   0,   0,   0}},     // 124 ����...
     {{Scaner::KEYWORD, Scaner::NICHTO       },   0, {   0,   0,   0,   0}},     // 125 �����
     {{Scaner::ID     , 0                    }, 127, { '�',   0,   0,   0}},     // 126 �...
     {{Scaner::ID     , 0                    }, 128, { '�',   0,   0,   0}},     // 127 ��...
     {{Scaner::ID     , 0                    }, 129, { '�',   0,   0,   0}},     // 128 ���...
     {{Scaner::ID     , 0                    }, 130, { '�',   0,   0,   0}},     // 129 ����...
     {{Scaner::ID     , 0                    }, 131, { '�',   0,   0,   0}},     // 130 �����...
     {{Scaner::ID     , 0                    }, 132, { '�',   0,   0,   0}},     // 131 ������...
     {{Scaner::ID     , 0                    }, 133, { '�',   0,   0,   0}},     // 132 �������...
     {{Scaner::ID     , 0                    }, 134, { '�',   0,   0,   0}},     // 133 ��������...
     {{Scaner::ID     , 0                    }, 135, { '�',   0,   0,   0}},     // 134 ���������...
     {{Scaner::ID     , 0                    }, 136, { '�',   0,   0,   0}},     // 135 ����������...
     {{Scaner::KEYWORD, Scaner::OBEDINENIE   },   0, {   0,   0,   0,   0}},     // 136 �����������
     {{Scaner::ID     , 0                    }, 138, { '�', '�',   0,   0}},     // 137 �...
     {{Scaner::ID     , 0                    }, 140, { '�',   0,   0,   0}},     // 138 ��...
     {{Scaner::KEYWORD, Scaner::PO           }, 141, { '�', '�',   0,   0}},     // 139 ��...
     {{Scaner::ID     , 0                    }, 143, { '�',   0,   0,   0}},     // 140 ���...
     {{Scaner::ID     , 0                    }, 144, { '�',   0,   0,   0}},     // 141 ���...
     {{Scaner::ID     , 0                    }, 145, { '�',   0,   0,   0}},     // 142 ���...
     {{Scaner::ID     , 0                    }, 146, { '�', '�',   0,   0}},     // 143 ����...
     {{Scaner::ID     , 0                    }, 148, { '�',   0,   0,   0}},     // 144 ����...
     {{Scaner::KEYWORD, Scaner::POKA         },   0, {   0,   0,   0,   0}},     // 145 ����
     {{Scaner::KEYWORD, Scaner::PEREM        },   0, {   0,   0,   0,   0}},     // 146 �����
     {{Scaner::ID     , 0                    }, 149, { '�',   0,   0,   0}},     // 147 �����...
     {{Scaner::ID     , 0                    }, 150, { '�',   0,   0,   0}},     // 148 �����...
     {{Scaner::ID     , 0                    }, 151, { '�',   0,   0,   0}},     // 149 ������...
     {{Scaner::ID     , 0                    }, 152, { '�',   0,   0,   0}},     // 150 ������...
     {{Scaner::ID     , 0                    }, 153, { '�',   0,   0,   0}},     // 151 �������...
     {{Scaner::ID     , 0                    }, 154, { '�',   0,   0,   0}},     // 152 �������...
     {{Scaner::ID     , 0                    }, 155, { '�',   0,   0,   0}},     // 153 ��������...
     {{Scaner::KEYWORD, Scaner::POVTORYAI    },   0, {   0,   0,   0,   0}},     // 154 ��������
     {{Scaner::ID     , 0                    }, 156, { '�',   0,   0,   0}},     // 155 ���������...
     {{Scaner::ID     , 0                    }, 157, { '�',   0,   0,   0}},     // 156 ����������...
     {{Scaner::ID     , 0                    }, 158, { '�',   0,   0,   0}},     // 157 �����������...
     {{Scaner::KEYWORD, Scaner::PERECHISLENIE},   0, {   0,   0,   0,   0}},     // 158 ������������
     {{Scaner::ID     , 0                    }, 160, { '�',   0,   0,   0}},     // 159 �...
     {{Scaner::ID     , 0                    }, 161, { '�',   0,   0,   0}},     // 160 ��...
     {{Scaner::ID     , 0                    }, 162, { '�',   0,   0,   0}},     // 161 ���...
     {{Scaner::ID     , 0                    }, 163, { '�',   0,   0,   0}},     // 162 ����...
     {{Scaner::ID     , 0                    }, 164, { '�',   0,   0,   0}},     // 163 �����...
     {{Scaner::ID     , 0                    }, 165, { '�',   0,   0,   0}},     // 164 ������...
     {{Scaner::ID     , 0                    }, 166, { '�',   0,   0,   0}},     // 165 �������...
     {{Scaner::ID     , 0                    }, 167, { '�',   0,   0,   0}},     // 166 ��������...
     {{Scaner::ID     , 0                    }, 168, { '�',   0,   0,   0}},     // 167 ���������...
     {{Scaner::ID     , 0                    }, 169, { '�',   0,   0,   0}},     // 168 ����������...
     {{Scaner::ID     , 0                    }, 170, { '�',   0,   0,   0}},     // 169 �����������...
     {{Scaner::KEYWORD, Scaner::RASSMATRIVAI },   0, {   0,   0,   0,   0}},     // 170 ������������
     {{Scaner::ID     , 0                    }, 172, { '�', '�',   0,   0}},     // 171 �...
     {{Scaner::ID     , 0                    }, 174, { '�',   0,   0,   0}},     // 172 ��...
     {{Scaner::ID     , 0                    }, 175, { '�',   0,   0,   0}},     // 173 ��...
     {{Scaner::ID     , 0                    }, 176, { '�',   0,   0,   0}},     // 174 ���...
     {{Scaner::ID     , 0                    }, 177, { '�',   0,   0,   0}},     // 175 ���...
     {{Scaner::KEYWORD, Scaner::SIMV         },   0, {   0,   0,   0,   0}},     // 176 ����
     {{Scaner::ID     , 0                    }, 178, { '�',   0,   0,   0}},     // 177 ����...
     {{Scaner::ID     , 0                    }, 179, { '�',   0,   0,   0}},     // 178 �����...
     {{Scaner::ID     , 0                    }, 180, { '�',   0,   0,   0}},     // 179 ������...
     {{Scaner::ID     , 0                    }, 181, { '�',   0,   0,   0}},     // 180 �������...
     {{Scaner::ID     , 0                    }, 182, { '�',   0,   0,   0}},     // 181 ��������...
     {{Scaner::KEYWORD, Scaner::STRUKTURA    },   0, {   0,   0,   0,   0}},     // 182 ���������
     {{Scaner::ID     , 0                    }, 184, { '�', '�',   0,   0}},     // 183 �...
     {{Scaner::ID     , 0                    }, 186, { '�',   0,   0,   0}},     // 184 ��...
     {{Scaner::KEYWORD, Scaner::TO           },   0, {   0,   0,   0,   0}},     // 185 ��
     {{Scaner::KEYWORD, Scaner::TIP          },   0, {   0,   0,   0,   0}},     // 186 ���
     {{Scaner::ID     , 0                    }, 188, { '�',   0,   0,   0}},     // 187 �...
     {{Scaner::ID     , 0                    }, 189, { '�',   0,   0,   0}},     // 188 ��...
     {{Scaner::KEYWORD, Scaner::CEL          },   0, {   0,   0,   0,   0}},     // 189 ���
     {{Scaner::ID     , 0                    }, 191, { '�',   0,   0,   0}},     // 190 �...
     {{Scaner::ID     , 0                    }, 192, { '�',   0,   0,   0}},     // 191 ��...
     {{Scaner::ID     , 0                    }, 193, { '�',   0,   0,   0}},     // 192 ���...
     {{Scaner::ID     , 0                    }, 194, { '�',   0,   0,   0}},     // 193 ����...
     {{Scaner::ID     , 0                    }, 195, { '�',   0,   0,   0}},     // 194 �����...
     {{Scaner::ID     , 0                    }, 196, { '�',   0,   0,   0}},     // 195 ������...
     {{Scaner::KEYWORD, Scaner::ELEMENT      },   0, {   0,   0,   0,   0}}      // 196 �������
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

