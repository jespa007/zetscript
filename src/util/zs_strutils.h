/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_STR_EXPAND(tok) 	#tok
#define ZS_STR(tok) 		ZS_STR_EXPAND(tok)
#define ZS_STRCMP(a, R, b) 	(strcmp(a,b) R 0)
#define ZS_FORMAT_FILE_LINE	"[%s:%i]"
#define ZS_FORMAT_LINE		"[line %i]"

// Util to capture args by ...
#define 	ZS_CAPTURE_VARIABLE_ARGS(_str_out, _str_in)\
{va_list  ap;\
int max_len=(int)((sizeof(_str_out)/sizeof(char))-1);\
va_start(ap, _str_in);\
int n=vsnprintf(_str_out,max_len,_str_in,  ap);\
if(n>=(int)max_len || (n==-1)){\
	_str_out[max_len-1]=0;\
	_str_out[max_len-2]='.';\
	_str_out[max_len-3]='.';\
	_str_out[max_len-4]='.';\
}\
va_end(ap);}

#define ZS_STR_CONST_IS_EMPTY(str) ((str)==NULL || (*(str))==0)


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
		 zs_int		* parse_int(const zs_string & );
		 zs_float	* parse_float(const zs_string & );


		 zs_string 	zs_int_to_str(zs_int _number, const zs_string & _format = "");
		 zs_string 	zs_float_to_str(zs_float _number);
		 zs_string 	to_lower(const zs_string & str);
		 zs_string 	to_upper(const zs_string & str);
		 char 		*clone_to_char_ptr(const zs_string & _str_in);
		 zs_string  format(const  char  *string_text, ...);
		 zs_string	format_file_line(const char* _file, int _line, const  char* _str_in, ...);

		 bool is_empty(const zs_string & str);
		 bool ends_with(const zs_string & str, const zs_string & ending);
		 bool starts_with(const zs_string & str, const zs_string & starting);
		 zs_string replace(const zs_string & str_input, const zs_string & old_word, const zs_string & new_word);

		 zs_vector<zs_string> split(const zs_string &s, char delim);

		 zs_vector<zs_string> split(const zs_string &s_in, const zs_string & delim);

		 NumberType is_number(const zs_string & test_str);
		 bool 		 is_binary(const zs_string & test_str);

		 zs_string remove(const zs_string & str_old, char ch_to_remove);

		 int count(const zs_string & s,char c);

		/**
		 * Given two pointers within that points within a zs_string, this function copies zs_string between its interval.
		 * @p1:start pointer
		 * @p2:end pointer
		 */
		zs_string copy_from_ptr_diff(const char *p1, const char *p2);


		bool contains(const zs_string & _str_input, const zs_string & _str_containts,StringComparer sc=StringComparer::Exactly);
		bool contains(const zs_vector<zs_string> & _strings, const zs_string & _str_containts,StringComparer sc=StringComparer::Exactly);

		int index_of(const zs_string& text, const zs_string& pattern);

		zs_string substring(const zs_string & _str_in, int _start_index, int _end_index=-1);

		zs_string unescape(const zs_string & s);
	}
}
