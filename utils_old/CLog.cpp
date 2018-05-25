/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
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




}
