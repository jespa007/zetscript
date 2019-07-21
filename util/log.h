//#define zs_print_error_cr(s, ...) 	zetscript::log::print(__FILE__,__LINE__,zetscript::string::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define zs_print_warning_cr(s, ...)   	zetscript::log::print(__FILE__,__LINE__,zetscript::log::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define zs_print_info_cr(s, ...)   		zetscript::log::print(__FILE__,__LINE__,zetscript::log::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug_cr(s, ...)   	zetscript::log::print(__FILE__,__LINE__,zetscript::log::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define zs_print_debug_cr(s, ...)
#endif


//#define zs_print_error(s, ...) 			zetscript::log::print(__FILE__,__LINE__,zetscript::string::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		zetscript::log::print(__FILE__,__LINE__,zetscript::log::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		zetscript::log::print(__FILE__,__LINE__,zetscript::log::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		zetscript::log::print(__FILE__,__LINE__,zetscript::log::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define zs_print_debug(s, ...)
#endif


namespace zetscript{
	namespace log{

		typedef enum  {
			LOG_ERROR = 0,
			LOG_WARNING,
			LOG_INFO,
			LOG_DEBUG
		}LOG_LEVEL;

		void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	}
}
