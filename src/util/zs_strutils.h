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

#define ZS_CONST_STR_IS_EMPTY(str) ((str)==NULL || (*(str))==0)

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

		 bool  		* parse_bool(const zs_string & );
		 zs_int		* parse_zs_int(const zs_string & );
		 zs_float	* parse_zs_float(const zs_string & );


		 zs_string zs_int_to_str(zs_int _number, const zs_string & _format = "");
		 zs_string zs_float_to_str(zs_float _number, const zs_string & _format = "");
		 zs_string to_lower(const zs_string & str);
		 zs_string to_upper(const zs_string & str);
		 zs_string  format(const  char  *string_text, ...);

		 bool ends_with(const zs_string & str, const zs_string & ending);
		 bool starts_with(const zs_string & str, const zs_string & starting);
		 zs_string replace(const zs_string & str_input, const zs_string & old_word, const zs_string & new_word);

		 zs_vector split(const zs_string &s, char delim);

		 zs_vector split(const zs_string &s_in, const zs_string & delim);

		 NumberType is_number(const zs_string & test_str);
		 bool 		 is_binary(const zs_string & test_str);

		 zs_string remove(zs_string & str_old, char ch_to_remove);

		 int count(const zs_string & s,char c);

		/**
		 * Given two pointers within that points within a zs_string, this function copies zs_string between its interval.
		 * @p1:start pointer
		 * @p2:end pointer
		 */
		void copy_from_ptr_diff(zs_string & str_dst,const char *p1, const char *p2);


		bool contains(const zs_string & input, const zs_string & str_containts,StringComparer sc=StringComparer::Exactly);
		bool contains(const zs_vector & input, const zs_string & str_containts,StringComparer sc=StringComparer::Exactly);

		int index_of(zs_string& text, zs_string& pattern);

		zs_string unescape(const zs_string & s);
	}
}
