#include "../CZetScript.h"

#define VAR_LOG(l) ((l) == CLog::LOG_ERROR?"ERR": (l)==CLog::LOG_WARNING?"WRN": (l) == CLog::LOG_INFO ? "INF" : (l) == CLog::LOG_DEBUG ? "DBG":"NAN" )
//      #include                  "cbingq.h"
//#include <string>

using std::string;
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

//--------------------------------------------------------------------------------
CLog::CLog()
{
	//setbuf(stdout, NULL);
	fp = NULL;
}
//--------------------------------------------------------------------------------


void  CLog::ini()
{

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

	//#ifdef  __DEBUG__
	char  text[4096] = { 0 };





	CAPTURE_VARIABLE_ARGS(text, string_text);
	//  Results  Are  Stored  In  Text



	if (!ansi_escape) {
#ifdef _WIN32
		int colors[7];
		/*colors[0] = FOREGROUND_RED   | FOREGROUND_INTENSITY;//FOREGROUND_BLUE  | FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		colors[1] = FOREGROUND_GREEN | FOREGROUND_RED   | FOREGROUND_INTENSITY;
		colors[2] = FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_RED   | FOREGROUND_INTENSITY;
		colors[3] = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		colors[4] = FOREGROUND_BLUE  | FOREGROUND_INTENSITY;
		colors[5] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		colors[6] = FOREGROUND_RED   | FOREGROUND_INTENSITY;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colors[level]);*/
#endif
	}
	else {

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

	if (file == NULL) {
		fprintf(std_type, "%s", text);
	}
	else {
		fprintf(std_type, "[ %30s:%04i - %3s]=%s", filename.c_str(), line, VAR_LOG(level), text);
	}


	if (!ansi_escape) {
#ifdef _WIN32

#endif
	}
	else {
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

CLog::~CLog()
{

}

