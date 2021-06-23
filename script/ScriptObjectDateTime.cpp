/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ONE_YEAR_DAYS 		86400*365 //24 hours * 60 mins * 60 secs

#define ONE_DAY_SECS 		86400 //24 hours * 60 mins * 60 secs
#define ONE_HOUR_SECS  		3600 //60 mins * 60 secs
#define ONE_MINUTE_SECS 	60 // 60 secs

//https://github.com/jeremydumais/CPP-DateTime-library/blob/master/src/datetime.h

namespace zetscript{

	ScriptObjectDateTime * ScriptObjectDateTime::newScriptObjectDateTime(ZetScript *_zs){
		ScriptObjectDateTime *dt=new ScriptObjectDateTime();
		dt->init(_zs);
		return dt;
	}
	//----------------------------

	ScriptObjectDateTime::ScriptObjectDateTime(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_DATETIME;
	}

	std::string ScriptObjectDateTime::toString(){
		return datetime.to_string();
	}

}
