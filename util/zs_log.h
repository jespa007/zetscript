#pragma once

#define ZS_LOG_FILE_LINE_STR(file,line)		(file==NULL?zetscript::zs_strutils::format("[line %i] ",line):zetscript::zs_strutils::format("[%s:%i] ",zs_path::get_filename(file).c_str(),line))
//#define zs_print_error_cr(s, ...) 	ZetScript::log::print(__FILE__,__LINE__,ZetScript::zs_strutils::ZS_LOG_LEVEL_ERROR	, true,s, ##__VA_ARGS__)
#define ZS_LOG_WARNING(s, ...)   			zetscript::zs_log::print(__FILE__,__LINE__,zetscript::zs_log::ZS_LOG_LEVEL_WARNING	, true,s, ##__VA_ARGS__)
#define ZS_LOG_INFO(s, ...)   				zetscript::zs_log::print(__FILE__,__LINE__,zetscript::zs_log::ZS_LOG_LEVEL_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define ZS_LOG_DEBUG(s, ...)   				zetscript::zs_log::print(__FILE__,__LINE__,zetscript::zs_log::ZS_LOG_LEVEL_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define ZS_LOG_DEBUG(s, ...)
#endif

namespace zetscript{
	namespace zs_log{

		typedef enum  {
			ZS_LOG_LEVEL_ERROR = 0,
			ZS_LOG_LEVEL_WARNING,
			ZS_LOG_LEVEL_INFO,
			ZS_LOG_LEVEL_DEBUG
		}ZsLogLevel;

		void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	}
}
