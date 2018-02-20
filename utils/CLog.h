/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	//#include <assert.h>


	class CLog {

		static bool ansi_escape;
	public:


		enum LOG_LEVEL {
			LOG_ERROR = 0,
			LOG_WARNING,
			LOG_INFO,
			LOG_DEBUG
		};

		static CLog *singleton_log;
		FILE  *fp;


		static CLog * getInstance();
		ZETSCRIPT_MODULE_EXPORT static void setUseAnsiEscape(bool _use);

		//void  print_with_file_line(const  char  *file,  int  line,  const  char  *string_text,...);
		//void  print(const  char  *string_text,...);
		ZETSCRIPT_MODULE_EXPORT static void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);



		static void destroy();
	};

}

#define zs_print_error_cr(s, ...) 			zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define zs_print_warning_cr(s, ...)   		zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define zs_print_info_cr(s, ...)   		zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug_cr(s, ...)   		zetscript::CLog::print(__FILE__,__LINE__,CLog::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define zs_print_debug_cr(s, ...)
#endif


#define zs_print_error(s, ...) 			zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		zetscript::CLog::print(__FILE__,__LINE__,zetscript::CLog::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define zs_print_debug(s, ...)
#endif




