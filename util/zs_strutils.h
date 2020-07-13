#pragma once


#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define ZS_STRCMP(a, R, b) (strcmp(a,b) R 0)
#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

namespace zetscript{
	namespace zs_strutils{

		typedef enum{
			NUMBER_TYPE_INVALID=0,
			NUMBER_TYPE_HEXA,
			NUMBER_TYPE_INT,
			NUMBER_TYPE_DOUBLE
		}NumberType;

		 bool  * parseBoolean(const std::string & );
		 int   * parseInteger(const std::string & );
		 float * parseFloat(const std::string & );


		 std::string intToString(int number);
		 std::string toLower(const std::string & str);
		 std::string toUpper(const std::string & str);
		 std::wstring toWString(const std::string &s);
		  std::string  format(const  char  *string_text, ...);

		 bool endsWith(const std::string & fullString, const std::string & ending);

		  std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems);
		  std::vector<std::string> split(const std::string &s, char delim);
		 bool isEmpty(const std::string & str);
		 NumberType isNumber(const std::string & test_str);
		 bool 		 isBinary(const std::string & test_str);

		 std::string remove(std::string & str_old, char ch_to_remove);

		 int count(const std::string & s,char c);

	}
}
