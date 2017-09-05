#include "../CZetScript.h"
using std::string;

namespace zetscript{

	#define VAR_LOG(l) ((l) == CLog::LOG_ERROR?"ERR": (l)==CLog::LOG_WARNING?"WRN": (l) == CLog::LOG_INFO ? "INF" : (l) == CLog::LOG_DEBUG ? "DBG":"NAN" )
	//      #include                  "cbingq.h"
	//#include <string>


	//FILE  *fp;
	CLog *CLog::singleton_log = NULL;
	bool CLog::ansi_escape = false;
	//static char s_Filename[MAX_PATH];

	void CLog::setUseAnsiEscape(bool _use) {
		ansi_escape = _use;
	}

	CLog * CLog::getInstance()
	{
		if (singleton_log == NULL)
			singleton_log = new CLog();

		return singleton_log;
	}


	//---------------------------------------------------------------------------------------------------------
	#ifndef  __GNUC__
	#pragma  managed(push,  off)
	#endif


	void  CLog::print(const  char  *file, int  line, int level, bool with_cr, const  char  *string_text, ...) {
		//FILE *log_std=stdout;

		FILE *std_type = stdout;
		string filename = "";

		switch (level)
		{
		case CLog::LOG_WARNING:
		case CLog::LOG_ERROR:
			//std_type=stderr;
			break;
		default:
			break;
		}

		if (file != NULL)
			filename = CStringUtils::getFileName(file);

		char  text[4096] = { 0 };


		ZETSCRIPT_CAPTURE_VARIABLE_ARGS(text, string_text);
		//  Results  Are  Stored  In  Text
		if(ansi_escape){

			switch (level)
			{
			case CLog::LOG_ERROR:
				CColorTerm::setColor(std_type, CColorTerm::BRIGHT, CColorTerm::CYAN, CColorTerm::RED);
				break;
			case CLog::LOG_WARNING:
				CColorTerm::setColor(std_type, CColorTerm::BRIGHT, CColorTerm::CYAN, CColorTerm::YELLOW);
				break;
			default:
				break;
			}
		}
		

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


		if (ansi_escape) {
			CColorTerm::setColor(std_type, CColorTerm::BRIGHT, CColorTerm::GREEN, CColorTerm::BLACK);
		}

		if (with_cr)
			fprintf(std_type, "\n");

		// to ensure all data has been write...
		fflush(std_type);
		// to ensure all data has been write...
		//fflush(std_type);
		//fflush(stdout);
		//fflush(NULL);





		// CColorTerm::resetColor();

	}

	#ifndef  __GNUC__
	#pragma  managed(pop)
	#endif


}
