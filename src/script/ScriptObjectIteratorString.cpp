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
		idx_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING;
		so = NULL;
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
		so=_so;
		so->refObject((ScriptObject **)&this->so);
	}

	void ScriptObjectIteratorString::get(){
		if(so==NULL) {
			THROW_EXCEPTION("string object null");
		}

		zs_string *str_ref=(zs_string *)so->value;
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
		if(so==NULL) {
			THROW_EXCEPTION("string object null");
		}

		if(idx<(int)so->length()){
			idx++;
		}
	}

	bool	 ScriptObjectIteratorString::end(){
		if(so==NULL) {
			THROW_EXCEPTION("string object null");
		}
		return idx >= (int)so->length();
	}

	ScriptObjectIteratorString::~ScriptObjectIteratorString(){
		if(so != NULL){
			so->derefObject((ScriptObject **)&this->so);
		}
	}

}
