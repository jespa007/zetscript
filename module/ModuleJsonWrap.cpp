/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	#include "ModuleJsonDeserializeWrap.cpp"
	#include "ModuleJsonSerializeWrap.cpp"

	ScriptObjectObject * ModuleJsonWrap_deserialize(ScriptObjectString *str_json){
		// deserialize ...
		return NULL;
	}

	ScriptObjectString * ModuleJsonWrap_serialize(ScriptObjectObject *obj,bool *minimized){
		// iterate for all elements in the obj
		return NULL;

	}

	ScriptObjectString * ModuleJsonWrap_serialize(ScriptObjectObject *obj){
		// iterate for all elements in the obj
		return NULL;

	}
}
