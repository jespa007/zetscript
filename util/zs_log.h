//#define zs_print_error_cr(s, ...) 	ZetScript::log::print(__FILE__,__LINE__,ZetScript::zs_strutils::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define ZS_PRINT_WARNING(s, ...)   	ZetScript::zs_log::print(__FILE__,__LINE__,ZetScript::zs_log::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define ZS_PRINT_INFO(s, ...)   		ZetScript::zs_log::print(__FILE__,__LINE__,ZetScript::zs_log::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define ZS_PRINT_DEBUG(s, ...)   	ZetScript::zs_log::print(__FILE__,__LINE__,ZetScript::zs_log::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define ZS_PRINT_DEBUG(s, ...)
#endif

/*
//#define zs_print_error(s, ...) 			ZetScript::log::print(__FILE__,__LINE__,ZetScript::zs_strutils::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		ZetScript::zs_log::print(__FILE__,__LINE__,ZetScript::zs_log::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		ZetScript::zs_log::print(__FILE__,__LINE__,ZetScript::zs_log::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		ZetScript::zs_log::print(__FILE__,__LINE__,ZetScript::zs_log::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define zs_print_debug(s, ...)
#endif
*/

namespace zetscript{
	namespace zs_log{

		typedef enum  {
			ZS_LOG_LEVEL_ERROR = 0,
			ZS_LOG_LEVEL_WARNING,
			ZS_LOG_LEVEL_INFO,
			ZS_LOG_LEVEL_DEBUG
		}ZsLogLevel;

		void  Print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	}
}
