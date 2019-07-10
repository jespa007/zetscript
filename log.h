//#define zs_print_error_cr(s, ...) 			zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define zs_print_warning_cr(s, ...)   		zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define zs_print_info_cr(s, ...)   		zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug_cr(s, ...)   		zs::CZetScriptUtils::print_log(__FILE__,__LINE__,CZetScriptUtils::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define zs_print_debug_cr(s, ...)
#endif


//#define zs_print_error(s, ...) 			zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		zs::CZetScriptUtils::print_log(__FILE__,__LINE__,zs::CZetScriptUtils::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define zs_print_debug(s, ...)
#endif


namespace zs{
	namespace log{

		typedef enum  {
			LOG_ERROR = 0,
			LOG_WARNING,
			LOG_INFO,
			LOG_DEBUG
		}LOG_LEVEL;

		void  print_log(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	}
}
