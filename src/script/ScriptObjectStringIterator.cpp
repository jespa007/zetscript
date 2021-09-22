/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectStringIterator * ScriptObjectStringIterator::newScriptObjectStringIterator(ScriptObjectString *_so){
		ScriptObjectStringIterator *si= new ScriptObjectStringIterator(_so);
		return si;
	}


	void ScriptObjectStringIterator::setup(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING_ITERATOR;
		so = NULL;
		vm=NULL;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	ScriptObjectStringIterator::ScriptObjectStringIterator(){
		setup();
	}

	ScriptObjectStringIterator::ScriptObjectStringIterator(ScriptObjectString *_so){
		setup();
		// setup object
		this->init(_so->getZetScript());
		so=_so;
	}

	void ScriptObjectStringIterator::get(){
		if(so==NULL) return;
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

	void	 ScriptObjectStringIterator::next(){
		if(so==NULL) return;
		if(idx<(int)so->length()){
			idx++;
		}
	}

	bool	 ScriptObjectStringIterator::end(){
		if(so==NULL) false;
		return idx >= (int)so->length();
	}

}
