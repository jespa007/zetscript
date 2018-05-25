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






