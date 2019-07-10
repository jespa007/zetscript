
#define VAR_LOG(l) ((l) == zs::CZetScriptUtils::LOG_ERROR?"ERR": (l)==zs::CZetScriptUtils::LOG_WARNING?"WRN": (l) == zs::CZetScriptUtils::LOG_INFO ? "INF" : (l) == zs::CZetScriptUtils::LOG_DEBUG ? "DBG":"NAN" )

namespace zs{
	namespace log{
	void  CZetScriptUtils::print_log(const  char  *file, int  line, int level, bool with_cr, const  char  *input_text, ...) {
		//FILE *log_std=stdout;

		FILE *std_type = stdout;
		std::string filename = "";

		switch (level)
		{
		case CZetScriptUtils::LOG_WARNING:
		case CZetScriptUtils::LOG_ERROR:
			//std_type=stderr;
			break;
		default:
			break;
		}

		if (file != NULL)
			filename = CZetScriptUtils::getFileName(file);

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
