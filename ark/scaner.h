
struct lexem_code {
	short lexem_type;
	short lexem_subtype;
};

#define MAX_STRING_LENGTH (40*1024*sizeof(char)-sizeof(unsigned))
#define MAX_ID_LENGTH 256

struct id_string {
	unsigned pos;
	unsigned char buffer[MAX_STRING_LENGTH];
};

struct lexem_value {
	union {
		__int128 integer_value;
		__float128 float_value;
		id_string ids;
	};
};

struct lexem_info {
	lexem_code lc;
	lexem_value* value;
};

class Scaner {	
	public:

		enum LEXEM_CODE {
			NONE, UNKNOWN, KEYWORD, DELIMETER, OPEN_ROUND_BRACKET, CLOSE_ROUND_BRACKET, ID
		};

		enum KEYWORD_CODE {
			ALG, ASM, BEZZN, BOLSHOE, VECHNO, VESH, VOZVRAT, VIBERI, VIIDI, 
			GOLOVNOI, GRYAZN, DLYA, ESLI, ZNACHENIYU, ZNACHENIYA, IZ, INACHE, INES, ISPOLZUET,
			ISTINA, KOMPL, LOG, LOZH, MALENKOE, MASSIV, MNOZHESTVO, MODUL, NICHTO, OBEDINENIE,
			PEREM, PERECHISLENIE, POVTORYAI, POKA, RASSMATRIVAI, SIMV, STRUKTURA, TIP, TO, CEL, ELEMENT
		};

		void back();
		
		lexem_info get_current_lexem();
		
		Scaner(const unsigned char* t);
		unsigned int get_line_number();
		
	private:
		enum CATEGORY {
			SPACES              , OTHER            , LEFT_BRACKET , RIGHT_BRACKET, QUOTE  , DQUOTE,
			ONE_LETTER_DELIMITER, PART_OF_DELIMITER, KEYWORD_BEGIN, LETTER       , DIGIT_0, DIGIT_1_9, DOLLAR, LATIN_LETTER, LETTER_sSdDeE, LETTER_xX
		};
		
		enum Automaton_name {
			a_START, a_UNKNOWN, a_MULTILETTER_DELIMETERS, a_COMMENT, a_NUMBERS, a_STRING_CHAR, a_IDENTIFIER, a_KEYWORD
		};
		
		Automaton_name Automaton;
		unsigned int   state;
		
		typedef bool (Scaner::*Automaton_proc)();
		static Automaton_proc procs[];
		
		bool start_proc();
		bool unknown_proc();
		bool multiletter_delimiters_proc();
		bool comment_proc();
		bool number_proc();
		bool string_char_proc();
		bool identifier_proc();
		bool keyword_proc();
		
		unsigned char* text;
		unsigned char* pcurrent_char;
		unsigned char* lexem_begin;
		
		unsigned char  currrent_char;
		CATEGORY       char_category;
		
		int commentary_level;
		
		static lexem_value lexem;
		
		unsigned int   current_line;
};

                  
