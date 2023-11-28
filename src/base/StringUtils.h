

namespace zetscript{

	typedef enum{
		Exactly=0,
		OrdinalIgnoreCase=1
	}StringComparer;

	typedef enum{
		NUMBER_TYPE_INVALID=0,
		NUMBER_TYPE_HEXA,
		NUMBER_TYPE_INT,
		NUMBER_TYPE_DOUBLE
	}NumberType;

	class StringUtils
	{
	public:
    
		 static bool  		* 	parseBool(const String & );




		 static String 			intToString(zs_int _number, const String & _format = "");
		 static String 			floatToString(zs_float _number);
		 static String 			toLower(const String & str);
		 static String 			toUpper(const String & str);
		 static String  		format(const  char  *string_text, ...);
		 static String			formatFileLine(const char* _file, int _line, const  char* _str_in, ...);

		 static bool 			isEmpty(const String & str);
		 static bool 			endsWith(const String & str, const String & ending);
		 static bool 			startsWith(const String & str, const String & starting);
		 static String 			replace(const String & str_input, const String & old_word, const String & new_word);

		 static Vector<String> 	split(const String &s, char delim);

		 static Vector<String> 	split(const String &s_in, const String & delim);

		 static NumberType 		isNumber(const String & test_str);
		 static bool 		 	isBinary(const String & test_str);

		 static String 			remove(const String & str_old, char ch_to_remove);

		 static int 			count(const String & s,char c);


		static bool 			contains(const String & _str_input, const String & _str_containts,StringComparer sc=StringComparer::Exactly);
		static bool 			contains(const Vector<String> & _strings, const String & _str_containts,StringComparer sc=StringComparer::Exactly);

		static int 				indexOf(const String& text, const String& pattern);

		static String 			substring(const String & _str_in, int _start_index, int _end_index=-1);

		static String 			unescape(const String & s);

private:

		 static char 			toLower(char _c);
		 static char 			toUpper(char _c);
		 static bool			isDigit(char c);
		 static bool 			isHexaDigit(char _c);
		 static char *			advanceDigits(char *aux_p);
 		 static char *			advanceHexaDigits(char *aux_p);


    };
}
