
#pragma once

/*#include <string>
#include <vector>
#include <cstdarg>

using std::string;
using std::vector;
*/
// Util to capture args by ...
#define CAPTURE_VARIABLE_ARGS(text_out, text_in)\
{va_list  ap;\
va_start(ap,  text_in);\
vsprintf(text_out,  text_in,  ap);\
va_end(ap);}

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)


class CStringUtils{
	enum{
		MAX_STRING_BUFFERS=256,
		MAX_LENGTH_BUFFER=512
	};
	static char m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER];
	static unsigned char m_index_buffer;

	static bool IS_SINGLE_COMMENT(char *str);
	static bool IS_START_COMMENT(char *str);
	static bool IS_END_COMMENT(char *str);

public:

	enum{
		STRING_IS_INVALID_NUMBER=0,
		STRING_IS_HEXA,
		STRING_IS_INT,
		STRING_IS_DOUBLE
	};

	template<typename T>
	static T fromString(const std::string& str){

		try{
	    std::istringstream ss(str);
	    T ret;
	    ss >> ret;
	    return ret;
		}
		catch(std::exception & ex){
			throw std::exception(ex);
		}
	}


	static char *ADVANCE_TO_CHAR(char *str,char c);
	static char *ADVANCE_TO_END_COMMENT(char *aux_p, int &m_line);
	static char *IGNORE_BLANKS(char *str, int &m_line);
	static char *ADVANCE_TO_ONE_OF_COLLECTION_CHAR(char *str,char *end_char_standard_value, int &m_line);

	static string intToString(int number);
	static string floatToString(float number);
	static string doubleToString(double number);
	static string toLower(const string & str);
	static string toUpper(const string & str);
	static std::wstring toWString(const std::string &s);
	static const char * formatString(const char *str, ...);

	static bool endsWith(const string & fullString, const string & ending);

	static vector<string> &split(const string &s, char delim, vector<string> &elems);
	static vector<string> split(const string &s, char delim);
	static bool isEmpty(const string & str);

	static bool isDigit(char c);
	static bool isHexaDigit(char c);
	static int isNumber(const string & test_number);

	static string replace(const string & str_old, const char old_ch, char new_ch);
	static void replace(string & str_input, const string & old_word, const string & new_word);

	static string remove(string & str_old, char ch_to_remove);

	static int count(const string & s,char c);

};
