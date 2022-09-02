/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	StringIteratorScriptObject * StringIteratorScriptObject::newStringIteratorScriptObject(StringScriptObject *_so){
		StringIteratorScriptObject *si= new StringIteratorScriptObject(_so);
		return si;
	}


	void StringIteratorScriptObject::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING;
		vm=NULL;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	StringIteratorScriptObject::StringIteratorScriptObject(){
		setup();
	}

	StringIteratorScriptObject::StringIteratorScriptObject(StringScriptObject *_so){
		setup();
		// setup object
		this->init(_so->getZetScript());
	}

	void StringIteratorScriptObject::get(){
		StringScriptObject *sos=(StringScriptObject *)ref_object;
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

	void	 StringIteratorScriptObject::next(){
		StringScriptObject *sos=(StringScriptObject *)ref_object;
		if(sos==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sos->length()){
			idx++;
		}
	}

	bool	 StringIteratorScriptObject::end(){
		StringScriptObject *sos=(StringScriptObject *)ref_object;
		if(sos==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sos->length();
	}

	StringIteratorScriptObject::~StringIteratorScriptObject(){

	}

}
