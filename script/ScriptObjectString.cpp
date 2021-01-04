/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectString::ScriptObjectString(ZetScript *_zs):ScriptObject(_zs){
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
