/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#include "JsonDeserializeModule.cpp"
#include "JsonSerializeModule.cpp"


namespace zetscript{



	void JsonModule_deserialize(ScriptEngine *_script_engine,StringScriptObject *str_json){
		// deserialize ...

		String str=str_json->toString().unescape();
		VirtualMachine *vm=_script_engine->getVirtualMachine();
		StackElement return_stack_element=k_stk_undefined;
		json::JsonDeserializeData deserialize_data;
		deserialize_data.filename=NULL;
		deserialize_data.str_start=str.toConstChar();
		deserialize_data.script_engine=_script_engine;
		deserialize_data.first_element=&return_stack_element;
		int line=1;

		if(json::deserialize(&deserialize_data,deserialize_data.str_start,line,&return_stack_element)==NULL){
			vm_set_error(vm,(String("Error deserialize:")+deserialize_data.str_error).toConstChar());
			//if(return_stack_element.properties & STACK_ELEMENT_PROPERTY_OBJECT){
				//ScriptObject *so=(ScriptObject *)return_stack_element.value;
				//vm_unref_shared_object(vm,so,NULL);
			//}
			return_stack_element=k_stk_undefined;
		}

		vm_push_stack_element(vm,return_stack_element);
	}

	StringScriptObject * JsonModule_serialize(ScriptEngine *_script_engine,StackElement  *stk,bool *minimized){
		// iterate for all elements in the obj
		return StringScriptObject::newStringScriptObject(_script_engine,json::serialize(_script_engine,stk,*minimized).toConstChar());
	}

	StringScriptObject * JsonModule_serialize(ScriptEngine *_script_engine,StackElement *stk){
		// iterate for all elements in the obj
		bool minimize=false;
		return JsonModule_serialize(_script_engine,stk,&minimize);

	}
}
