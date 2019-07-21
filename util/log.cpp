#include "../zetscript.h"

#define VAR_LOG(l) ((l) == LOG_ERROR?"ERR": (l)==LOG_WARNING?"WRN": (l) == LOG_INFO ? "INF" : (l) == LOG_DEBUG ? "DBG":"NAN" )

namespace zetscript{
	namespace log{
		void  print(const  char  *file, int  line, int level, bool with_cr, const  char  *input_text, ...) {
			//FILE *log_std=stdout;

			FILE *std_type = stdout;
			std::string filename = "";

			switch (level)
			{
			case LOG_WARNING:
			case LOG_ERROR:
				//std_type=stderr;
				break;
			default:
				break;
			}

			if (file != NULL)
				filename = path::get_filename(file);

			char  text[4096] = { 0 };


			va_list  ap;
			va_start(ap,  input_text);
			vsprintf(text,  input_text,  ap);
			va_end(ap);


			if (file == NULL
	#ifndef __ZETSCRIPT_DEBUG__
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
