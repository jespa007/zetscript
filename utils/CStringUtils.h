
#pragma once

// Util to capture args by ...
#define CAPTURE_VARIABLE_ARGS(text_out, text_in)\
{va_list  ap;\
va_start(ap,  text_in);\
vsprintf(text_out,  text_in,  ap);\
va_end(ap);}

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define STRCMP(a, R, b) (strcmp(a,b) R 0)

class CStringUtils{
	enum{
		MAX_STRING_BUFFERS=256,
		MAX_LENGTH_BUFFER=512,
		MAX_BUFFER_COPY_FROM_INTERVAL=4096
	};
	static char m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER];
	static char aux_str_copy[MAX_BUFFER_COPY_FROM_INTERVAL];
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


	static bool  * ParseBoolean(const string & );
	static int   * ParseInteger(const string & );
	static float * ParseFloat(const string & );


	static string intToString(int number);
	static string floatToString(float number);
	static string doubleToString(double number);
	static string toLower(const string & str);
	static string toUpper(const string & str);
	static std::wstring toWString(const std::string &s);
	static string  getFileName(const string & _path);

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

	/**
	 * Given two pointers within that points within a string, this function copies string between its interval.
	 * @p1:start pointer
	 * @p2:end pointer
	 */
	static char * copyStringFromInterval(const char *p1, const char *p2);

};
