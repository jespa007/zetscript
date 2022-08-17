/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

#define VAR_LOG(l) ((l) == ZS_LOG_LEVEL_ERROR?"ERR": (l)==ZS_LOG_LEVEL_WARNING?"WRN": (l) == ZS_LOG_LEVEL_INFO ? "INF" : (l) == ZS_LOG_LEVEL_DEBUG ? "DBG":"NAN" )

namespace zetscript{
	namespace zs_log{
		void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *input_text, ...) {
			//FILE *log_std=stdout;

			FILE *std_type = stdout;
			std::string filename = "";

			switch (level)
			{
			case ZS_LOG_LEVEL_WARNING:
			case ZS_LOG_LEVEL_ERROR:
				//std_type=stderr;
				break;
			default:
				break;
			}

			if (file != NULL)
				filename = zs_path::get_filename(file);

			char  text[ZS_MAX_STR_BUFFER] = { 0 };

			va_list  ap;
			va_start(ap,  input_text);
			vsprintf(text,  input_text,  ap);
			va_end(ap);

			if (file == NULL
	#ifndef __DEBUG__
					|| true  // because in release mode we don't put (file:line) information.
	#endif
					) {
				fprintf(std_type, "%s", text);
			}
			else {
				fprintf(std_type, "[ %30s:%04i - %3s]=%s", filename.c_str(), line, VAR_LOG(level), text);
			}

			if (with_cr)
				fprintf(std_type, "\n");

			// to ensure all data has been write...
			fflush(std_type);
		}


	}
}
