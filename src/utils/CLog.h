#pragma once


 //#include <assert.h>


class  CLog{

  CLog();
  
  static bool ansi_escape;
public:


  enum LOG_LEVEL{
    LOG_ERROR=0,
    LOG_WARNING,
    LOG_INFO
  };
  
  static CLog *singleton_log;
  FILE  *fp;	
  

	void  ini();
	
	static CLog * getInstance();
	static void setUseAnsiEscape(bool _use);

	//void  print_with_file_line(const  char  *file,  int  line,  const  char  *string_text,...);
	//void  print(const  char  *string_text,...);
	static void  print(const  char  *file,  int  line, int level, bool with_cr, const  char  *string_text,...);

	~CLog();
	
	static void destroy();
};


#define print_error_cr(s, ...) 			CLog::print(__FILE__,__LINE__,CLog::LOG_ERROR	, true,s, ##__VA_ARGS__)
#define print_warning_cr(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_WARNING	, true,s, ##__VA_ARGS__)
#define print_info_cr(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_INFO	, true,s, ##__VA_ARGS__)

#define print_error(s, ...) 			CLog::print(__FILE__,__LINE__,CLog::LOG_ERROR	, false,s, ##__VA_ARGS__)
#define print_warning(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_WARNING	, false,s, ##__VA_ARGS__)
#define print_info(s, ...)   		CLog::print(__FILE__,__LINE__,CLog::LOG_INFO	, false,s, ##__VA_ARGS__)




