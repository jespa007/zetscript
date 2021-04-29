/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


namespace zetscript{

	ScriptObjectDateTime * ScriptObjectDateTimeWrap_now(ZetScript *_zs){
		ScriptObjectDateTime *dt=ScriptObjectDateTime::newScriptObjectDateTime(_zs);
		return dt;
	}
}
