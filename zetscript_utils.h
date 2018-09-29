/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	#define STR_EXPAND(tok) #tok
	#define STR(tok) STR_EXPAND(tok)
	#define STRCMP(a, R, b) (strcmp(a,b) R 0)
	#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

	class CZetScriptUtils
	{
	public:
		//----------------------------------------------------------------------------------------------
		// LOG
		enum LOG_LEVEL {
			LOG_ERROR = 0,
			LOG_WARNING,
			LOG_INFO,
			LOG_DEBUG
		};

		ZETSCRIPT_MODULE_EXPORT static void  print_log(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);


		//----------------------------------------------------------------------------------------------
		// IO Utils
		static string  getFileName(const string &  _path);
		static string  getFileNameWithoutExtension(const string & _path);
		static bool fileExists(const string & m_file);
		//static bool readFile(const string & filename, char *buffer, int length);
		static char * readFile(const string & filename, int & n_readed_bytes);
		static int  getFileLength(const string & filename);

		//----------------------------------------------------------------------------------------------
		// STRING
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
		ZETSCRIPT_MODULE_EXPORT static string  sformat(const  char  *string_text, ...);

		static bool endsWith(const string & fullString, const string & ending);

		static vector<string> &split(const string &s, char delim, vector<string> &elems);
		static vector<string> split(const string &s, char delim);
		static bool isEmpty(const string & str);

		static bool isDigit(char c);
		static bool isHexaDigit(char c);
		static int isNumber(const string & test_number);
		static bool isBinary(const string & test_str_number);

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

	ZETSCRIPT_MODULE_EXPORT std::string demangle(const string & name);

}

//#define zs_print_error_cr(s, ...) 			zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define zs_print_warning_cr(s, ...)   		zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define zs_print_info_cr(s, ...)   		zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug_cr(s, ...)   		zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,CZetScriptUtils::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define zs_print_debug_cr(s, ...)
#endif


//#define zs_print_error(s, ...) 			zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		zetscript::CZetScriptUtils::print_log(__FILE__,__LINE__,zetscript::CZetScriptUtils::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define zs_print_debug(s, ...)
#endif
