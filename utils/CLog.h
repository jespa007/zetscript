#pragma once


//#include <assert.h>


class ZETSCRIPT_API CLog {

	CLog();

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


	void  ini();

	static CLog * getInstance();
	static void setUseAnsiEscape(bool _use);

	//void  print_with_file_line(const  char  *file,  int  line,  const  char  *string_text,...);
	//void  print(const  char  *string_text,...);
	static void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...);

	~CLog();

	static void destroy();
};


#define print_error_cr(s, ...) 			CLog::print(__FILE__,__LINE__,CLog::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define print_warning_cr(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define print_info_cr(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_INFO	, true,s, ##__VA_ARGS__)
#ifdef __DEBUG__
#define print_debug_cr(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_DEBUG	, true,s, ##__VA_ARGS__)
#else
#define print_debug_cr(s, ...)
#endif


#define print_error(s, ...) 			CLog::print(__FILE__,__LINE__,CLog::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define print_warning(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define print_info(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_INFO	, false,s, ##__VA_ARGS__)
#ifdef __DEBUG__
#define print_debug(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_DEBUG	, false,s, ##__VA_ARGS__)
#else
#define print_debug(s, ...)
#endif


