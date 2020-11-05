/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectString::ScriptObjectString(ZetScript *_zs):ScriptObject(_zs){
		init(SCRIPT_CLASS_STRING(this), (void *)this);
		str_value = "";
		value = &str_value;
	}

	ScriptObjectString::ScriptObjectString(ZetScript *_zs, const std::string * s):ScriptObject(_zs){
		init(SCRIPT_CLASS_STRING(this), (void *)this);
		value = (void *)s;
	}

	std::string ScriptObjectString::toString(){
		return *((std::string *)value);
	}

}
