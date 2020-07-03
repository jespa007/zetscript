//#define zs_print_error_cr(s, ...) 	ZetScript::log::print(__FILE__,__LINE__,ZetScript::ZS_StrUtils::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define zs_print_warning_cr(s, ...)   	ZetScript::ZS_Log::print(__FILE__,__LINE__,ZetScript::ZS_Log::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define zs_print_info_cr(s, ...)   		ZetScript::ZS_Log::print(__FILE__,__LINE__,ZetScript::ZS_Log::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug_cr(s, ...)   	ZetScript::ZS_Log::print(__FILE__,__LINE__,ZetScript::ZS_Log::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define zs_print_debug_cr(s, ...)
#endif


//#define zs_print_error(s, ...) 			ZetScript::log::print(__FILE__,__LINE__,ZetScript::ZS_StrUtils::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		ZetScript::ZS_Log::print(__FILE__,__LINE__,ZetScript::ZS_Log::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		ZetScript::ZS_Log::print(__FILE__,__LINE__,ZetScript::ZS_Log::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		ZetScript::ZS_Log::print(__FILE__,__LINE__,ZetScript::ZS_Log::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define zs_print_debug(s, ...)
#endif


namespace ZetScript{
	namespace ZS_Log{

		typedef enum  {
			ZS_LogLevelError = 0,
			ZS_LogLevelWarning,
			ZS_LogLevelInfo,
			ZS_LogLevelDebug
		}ZS_LogLevel;

		void  Print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	}
}
