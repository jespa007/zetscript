#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define STRCMP(a, R, b) (strcmp(a,b) R 0)
#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

namespace zetscript{
	namespace string{

		 bool  * parse_boolean(const std::string & );
		 int   * parse_intenger(const std::string & );
		 float * parse_float(const std::string & );


		 std::string int_2_string(int number);
		 std::string float_2_string(float number);
		 std::string double_2_string(double number);
		 std::string to_lower(const std::string & str);
		 std::string to_upper(const std::string & str);
		 std::wstring to_wstring(const std::string &s);
		  const char *  sformat(const  char  *string_text, ...);

		 bool ends_with(const std::string & fullString, const std::string & ending);

		  std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems);
		  std::vector<std::string> split(const std::string &s, char delim);
		 bool is_empty(const std::string & str);

		 std::string replace(const std::string & str_old, const char old_ch, char new_ch);
		 void replace(std::string & str_input, const std::string & old_word, const std::string & new_word);

		 std::string remove(std::string & str_old, char ch_to_remove);

		 int count(const std::string & s,char c);

		/**
		 * Given two pointers within that points within a std::string, this function copies std::string between its interval.
		 * @p1:start pointer
		 * @p2:end pointer
		 */
		 char * copy_from_pointer_diff(const char *p1, const char *p2);
	}
}
