/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

//#define zs_print_error_cr(s, ...) 		ScriptEngine::log::print(__FILE__,__LINE__,ScriptEngine::String::LOG_LEVEL_ERROR	, true,s, ##__VA_ARGS__)
#define ZS_LOG_WARNING(s, ...)   			zetscript::Log::print(__FILE__,__LINE__,zetscript::Log::LOG_LEVEL_WARNING	, true,s, ##__VA_ARGS__)
#define ZS_LOG_INFO(s, ...)   				zetscript::Log::print(__FILE__,__LINE__,zetscript::Log::LOG_LEVEL_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZS_VERBOSE_MESSAGE__
#define ZS_LOG_DEBUG(s, ...)   				zetscript::Log::print(__FILE__,__LINE__,zetscript::Log::LOG_LEVEL_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define ZS_LOG_DEBUG(s, ...)
#endif

namespace zetscript{
	class Log{
	public:
		typedef enum  {
			LOG_LEVEL_ERROR = 0,
			LOG_LEVEL_WARNING,
			LOG_LEVEL_INFO,
			LOG_LEVEL_DEBUG
		}LogLevel;

		static void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	};
}
