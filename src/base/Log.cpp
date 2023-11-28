/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

#define ZS_VAR_LOG(l) ((l) == LOG_LEVEL_ERROR?"ERR": (l)==LOG_LEVEL_WARNING?"WRN": (l) == LOG_LEVEL_INFO ? "INF" : (l) == LOG_LEVEL_DEBUG ? "DBG":"NAN" )

namespace zetscript{
	
	void  Log::print(const  char  *file, int  line, int level, bool with_cr, const  char  *input_text, ...) {
		//FILE *log_std=stdout;

		FILE *std_type = stdout;
		String filename = "";

		switch (level)
		{
		case LOG_LEVEL_WARNING:
		case LOG_LEVEL_ERROR:
			//std_type=stderr;
			break;
		default:
			break;
		}

		if (file != NULL)
			filename = Path::getFilename(file);

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
			fprintf(std_type, "[ %30s:%04i - %3s]=%s", filename.toConstChar(), line, ZS_VAR_LOG(level), text);
		}

		if (with_cr)
			fprintf(std_type, "\n");

		// to ensure all data has been write...
		fflush(std_type);
	}


	
}
