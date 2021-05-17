/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

#include "ModuleJsonDeserializeWrap.cpp"
#include "ModuleJsonSerializeWrap.cpp"


namespace zetscript{

	void ModuleJsonWrap_deserialize(ScriptObjectString *str_json){
		// deserialize ...
		std::string str=zs_strutils::unescape(str_json->toString());
		VirtualMachine *vm=str_json->getZetScript()->getVirtualMachine();
		StackElement return_stack_element=k_stk_undefined;

		try{
			json::deserialize(str);
		}catch(std::exception & ex){
			vm_set_error(vm,std::string("Error deserialize:")+ex.what());
		}

		vm_push_stack_element(vm,return_stack_element);

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
