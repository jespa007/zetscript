#pragma once

#define	ZS_MAX_STR_BUFFER 	4096
#define ZS_STR_EXPAND(tok) 	#tok
#define ZS_STR(tok) 		ZS_STR_EXPAND(tok)
#define ZS_STRCMP(a, R, b) 	(strcmp(a,b) R 0)
#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))


#define ZS_CAPTURE_VARIABLE_ARGS(text_out, text_in)\
{va_list  ap;\
va_start(ap,  text_in);\
int n=vsnprintf(text_out,ZS_MAX_STR_BUFFER,text_in,  ap);\
if(n==ZS_MAX_STR_BUFFER){\
	text_out[sizeof(text_out)-2]='.';\
	text_out[sizeof(text_out)-3]='.';\
	text_out[sizeof(text_out)-4]='.';\
}\
va_end(ap);}

namespace zetscript{
	namespace zs_strutils{

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

		 bool  		* parse_bool(const std::string & );
		 zs_int		* parse_int(const std::string & );
		 zs_float	* parse_float(const std::string & );


		 std::string zs_int_to_str(zs_int number);
		 std::string float_to_str(zs_float number);
		 std::string to_lower(const std::string & str);
		 std::string to_upper(const std::string & str);
		 std::wstring to_wstring(const std::string &s);
		  std::string  format(const  char  *string_text, ...);

		 bool ends_with(const std::string & str, const std::string & ending);
		 bool starts_with(const std::string & str, const std::string & starting);
		 std::string replace(const std::string & str_input, const std::string & old_word, const std::string & new_word);


		  std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems);
		  std::vector<std::string> split(const std::string &s, char delim);

		 std::vector<std::string> split(const std::string &s_in, const std::string & delim);
		 bool is_empty(const std::string & str);
		 NumberType is_number(const std::string & test_str);
		 bool 		 is_binary(const std::string & test_str);

		 std::string remove(std::string & str_old, char ch_to_remove);

		 int count(const std::string & s,char c);

		/**
		 * Given two pointers within that points within a std::string, this function copies std::string between its interval.
		 * @p1:start pointer
		 * @p2:end pointer
		 */
		void copy_from_ptr_diff(std::string & str_dst,const char *p1, const char *p2);

		std::string substring(const std::string & str, size_t start_idx, size_t end_idx);

		bool contains(const std::string & input, const std::string & str_containts,StringComparer sc=StringComparer::Exactly);
		bool contains(const std::vector<std::string> & input, const std::string & str_containts,StringComparer sc=StringComparer::Exactly);

		int index_of(std::string& text, std::string& pattern);
	}
}
