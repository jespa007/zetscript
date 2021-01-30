/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectString * ScriptObjectString::newStringObject(ZetScript *zs){
		ScriptObjectString *so=new ScriptObjectString();
		so->setZetScript(zs);
		return so;
	}

	ScriptObjectString::ScriptObjectString(){
		idx_script_class=IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_STRING;
		str_value = "";
		value = &str_value;
	}

	void ScriptObjectString::set(const std::string & s){
		*((std::string *)value) = s;
	}

	std::string ScriptObjectString::toString(){
		return *((std::string *)value);
	}

	zs_int ScriptObjectString::count(){
		return ((std::string *)value)->size();
	}

}
