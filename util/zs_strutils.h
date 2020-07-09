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

		 bool  * ParseBoolean(const std::string & );
		 int   * ParseInteger(const std::string & );
		 float * ParseFloat(const std::string & );


		 std::string IntToString(int number);
		 std::string ToLower(const std::string & str);
		 std::string ToUpper(const std::string & str);
		 std::wstring ToWString(const std::string &s);
		  std::string  Format(const  char  *string_text, ...);

		 bool EndsWith(const std::string & fullString, const std::string & ending);

		  std::vector<std::string> Split(const std::string &s, char delim, std::vector<std::string> &elems);
		  std::vector<std::string> Split(const std::string &s, char delim);
		 bool is_empty(const std::string & str);
		 NumberType IsNumber(const std::string & test_str);
		 bool 		 IsBinary(const std::string & test_str);

		 std::string Remove(std::string & str_old, char ch_to_remove);

		 int Count(const std::string & s,char c);

		/**
		 * Given two pointers within that points within a std::string, this function copies std::string between its interval.
		 * @p1:start pointer
		 * @p2:end pointer
		 */
		 char * CopyFromPointerDiff(const char *p1, const char *p2);
	}
}
