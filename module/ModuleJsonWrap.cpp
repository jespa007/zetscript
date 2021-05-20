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
		ZetScript *zs=str_json->getZetScript();
		std::string str=zs_strutils::unescape(str_json->toString());
		VirtualMachine *vm=str_json->getZetScript()->getVirtualMachine();
		StackElement return_stack_element=k_stk_undefined;
		json::JsonDeserializeData deserialize_data;
		deserialize_data.filename=NULL;
		deserialize_data.str_start=str.c_str();
		deserialize_data.zs=zs;
		int line=1;

		if(json::deserialize(&deserialize_data,deserialize_data.str_start,line,&return_stack_element)==NULL){
			vm_set_error(vm,std::string("Error deserialize:")+deserialize_data.str_error);
			if(return_stack_element.properties & STK_PROPERTY_SCRIPT_OBJECT){
				delete (ScriptObject *)return_stack_element.value;
			}
			return_stack_element=k_stk_undefined;
		}

		vm_push_stack_element(vm,return_stack_element);
	}

	ScriptObjectString * ModuleJsonWrap_serialize(ZetScript *zs,StackElement  *stk,bool *minimized){
		// iterate for all elements in the obj
		return ScriptObjectString::newScriptObjectString(zs,json::serialize(stk,*minimized));
	}

	ScriptObjectString * ModuleJsonWrap_serialize(ZetScript *zs,StackElement *stk){
		// iterate for all elements in the obj
		bool minimize=false;
		return ModuleJsonWrap_serialize(zs,stk,&minimize);

	}
}
