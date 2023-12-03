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

	class String
	{
	public:

		typedef enum{
			Exactly=0,
			OrdinalIgnoreCase=1
		}StringComparer;

		typedef enum{
			NUMBER_TYPE_INVALID=0,
			NUMBER_TYPE_HEXA,
			NUMBER_TYPE_BINARY,
			NUMBER_TYPE_INT,
			NUMBER_TYPE_DOUBLE,
		}NumberType;

		static int npos;

		static NumberType		isNumber(const String & test_str);
		static bool 			contains(const Vector<String> & _strings, const String & _str_containts,StringComparer sc=StringComparer::Exactly);
		static String  			format(const  char  *string_text, ...);
		static String			formatFileLine(const char* _file, int _line, const  char* _str_in, ...);


		String();
		String(const char * _s);
		String(const String & _str);
		String(String && _str_tmp);

		String& operator=(const String & obj);
		String& operator=(String && _str_tmp);
		String& operator=(const char* buffer);

		String& operator+=(const String& rhs);
		String& operator+=(const char* buffer);
		String& operator+=(char _c);



	    char& operator[] (int pos);
	    const char& operator[] (int _pos) const;

	    char& at (int pos);
	    const char& at (int _pos) const;

	    void clear();


	    String  substr (int pos, int len = npos) const;

	    // +
		friend String operator+(const String & _s1, const String &_s2);
		friend String operator+(const String & _s1, const char *_s2);
		friend String operator+(const char * _s1, const String & _s2);

		friend String operator+(const String & _s1, char _s2);
		friend String operator+(char  _s1, const String & _s2);

		// ==
		friend bool operator==(const String & _s1, const String &_s2);
		friend bool operator==(const String & _s1, const char *_s2);
		friend bool operator==(const char * _s1, const String & _s2);

		// !=
		friend bool operator!=(const String & _s1, const String &_s2);
		friend bool operator!=(const String & _s1, const char *_s2);
		friend bool operator!=(const char * _s1, const String & _s2);

		int find(const String &_s, int _pos = 0) const;
		int find(const char *_s, int _pos = 0) const;
		int findLastOf(const char *_s, int _pos = npos) const;
		int findLastOf(const String & _str, int _pos = npos) const;

		String & replace(int _pos, int _len, const String & _to_replace);
		void append(char _c);
		void append(const char * _s);
		void append(const char * _s, int _len);
		void append(const String & _str);
						

		void erase(int _pos, int _len);
		void erase(int _pos);
		void insert(int _pos, char _c);
		void insert(int _pos, const String & _s1);

		 String 				toLower() const;
		 String 				toUpper() const;

		 bool 					endsWith( const String & ending);
		 bool 					startsWith(const String & starting);
		 String 				replace( const String & old_word, const String & new_word);

		 Vector<String> 		split(char delim) const;

		 Vector<String> 		split(const String & delim) const;


		 String 				remove(const String & str_old, char ch_to_remove);

		 //int 					count(char c);

		 bool 					contains(const String & _str_containts,StringComparer sc=StringComparer::Exactly);


		 int 					indexOf(const String& pattern);

		String 					substring(int _start_index, int _end_index=-1);

		String 					unescape() const;


		inline bool isEmpty() const{
			return count==0;
		}


		inline int length() const{
			return count;
		}

		inline const char * toConstChar() const{
			return buf==NULL?"":buf;
		}

		~String();

	private:



		char * buf = NULL;
		int count=0;
		int _size = 0;
		static String newFromTwo(const char *_s1,const char *_s2);
		static bool				isDigit(char c);
		static bool 			isHexaDigit(char _c);
		static char *			advanceDigits(char *aux_p);
		static char *			advanceHexaDigits(char *aux_p);



		void set(const String & _s);
		void set(const char * _s, int _length=npos);
		void inc_slots(int _n_slots);
		void __cleanup__();


	};

}
