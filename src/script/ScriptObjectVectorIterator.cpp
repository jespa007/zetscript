/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVectorIterator * ScriptObjectVectorIterator::newScriptObjectVectorIterator(ScriptObjectVector *_so){
		ScriptObjectVectorIterator *si= new ScriptObjectVectorIterator(_so);
		return si;
	}


	void ScriptObjectVectorIterator::setup(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR_ITERATOR;
		vo = NULL;
		vm=NULL;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	ScriptObjectVectorIterator::ScriptObjectVectorIterator(){
		setup();
	}

	ScriptObjectVectorIterator::ScriptObjectVectorIterator(ScriptObjectVector *_so){
		setup();
		// setup object
		this->init(_so->getZetScript());
		vo=_so;
	}

	void ScriptObjectVectorIterator::get(){
		if(vo==NULL) return;
		if(idx<(int)vo->length()){
			// set value
			stk_value=*((StackElement *)vo->getUserElementAt(idx));
		}

		stk_key.value=idx;

		// pass value through the stack

		vm_push_stack_element(vm,stk_value); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}

	void	 ScriptObjectVectorIterator::next(){
		if(vo==NULL) return;
		if(idx<(int)vo->length()){
			idx++;
		}
	}

	bool	 ScriptObjectVectorIterator::end(){
		if(vo==NULL) false;
		return idx >= (int)vo->length();
	}

}
