/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


namespace zetscript{

	ScriptObjectDateTime * ScriptObjectDateTime::newScriptObjectDateTime(ZetScript *_zs){
		ScriptObjectDateTime *dt=new ScriptObjectDateTime();
		dt->init(_zs);
		return dt;

	}

	ScriptObjectDateTime::ScriptObjectDateTime(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_DATETIME;
	}

}
