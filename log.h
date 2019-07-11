//#define zs_print_error_cr(s, ...) 			zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_ERROR	, true,s, ##__VA_ARGS__)
#define zs_print_warning_cr(s, ...)   		zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_WARNING	, true,s, ##__VA_ARGS__)
#define zs_print_info_cr(s, ...)   		zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug_cr(s, ...)   		zetscript::string_utilsprint_log(__FILE__,__LINE__,string_utilsLOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define zs_print_debug_cr(s, ...)
#endif


//#define zs_print_error(s, ...) 			zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_ERROR	, false,s, ##__VA_ARGS__)
#define zs_print_warning(s, ...)   		zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_WARNING	, false,s, ##__VA_ARGS__)
#define zs_print_info(s, ...)   		zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __ZETSCRIPT_VERBOSE_MESSAGE__
#define zs_print_debug(s, ...)   		zetscript::string_utilsprint_log(__FILE__,__LINE__,zetscript::string_utilsLOG_DEBUG	, false,s, ##__VA_ARGS__)
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

		void  print_log(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	}
}
