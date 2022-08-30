/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIteratorString * ScriptObjectIteratorString::newScriptObjectIteratorString(ScriptObjectString *_so){
		ScriptObjectIteratorString *si= new ScriptObjectIteratorString(_so);
		return si;
	}


	void ScriptObjectIteratorString::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING;
		vm=NULL;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	ScriptObjectIteratorString::ScriptObjectIteratorString(){
		setup();
	}

	ScriptObjectIteratorString::ScriptObjectIteratorString(ScriptObjectString *_so){
		setup();
		// setup object
		this->init(_so->getZetScript());
	}

	void ScriptObjectIteratorString::get(){
		ScriptObjectString *sos=(ScriptObjectString *)ref_object;
		if(sos==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		zs_string *str_ref=(zs_string *)sos->value;
		if(idx < (int)str_ref->length()){
			// set value
			zs_int ch=str_ref->at(idx);
			stk_value.value=ch;
		}

		stk_key.value=idx;

		// pass value through the stack
		vm_push_stack_element(vm,stk_value); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}

	void	 ScriptObjectIteratorString::next(){
		ScriptObjectString *sos=(ScriptObjectString *)ref_object;
		if(sos==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sos->length()){
			idx++;
		}
	}

	bool	 ScriptObjectIteratorString::end(){
		ScriptObjectString *sos=(ScriptObjectString *)ref_object;
		if(sos==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sos->length();
	}

	ScriptObjectIteratorString::~ScriptObjectIteratorString(){

	}

}
