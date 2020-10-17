/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void ScriptObjectString::setup(){
		value = &str_value;
		str_value = "";
	}

	ScriptObjectString::ScriptObjectString(ZetScript *_zs):ScriptObject(_zs){
		str_value = "";
		init(SCRIPT_CLASS_STRING(this), (void *)this);
	}

	ScriptObjectString::ScriptObjectString(ZetScript *_zs, const std::string & s):ScriptObject(_zs){
		init(SCRIPT_CLASS_STRING(this), (void *)this);
		str_value = s;
	}

	std::string ScriptObjectString::toString(){
		return str_value;
	}

}
