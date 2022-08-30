/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIteratorVector * ScriptObjectIteratorVector::newScriptObjectIteratorVector(ScriptObjectVector *_so){
		ScriptObjectIteratorVector *si= new ScriptObjectIteratorVector(_so);

		return si;
	}


	void ScriptObjectIteratorVector::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR;
		vm=NULL;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	ScriptObjectIteratorVector::ScriptObjectIteratorVector(){
		setup();
	}

	ScriptObjectIteratorVector::ScriptObjectIteratorVector(ScriptObjectVector *_so){
		setup();
		// setup object
		this->init(_so->getZetScript());

	}

	void ScriptObjectIteratorVector::get(){
		ScriptObjectVector *sov=(ScriptObjectVector *)ref_object;
		if(sov==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			// set value
			stk_value=*((StackElement *)sov->getUserElementAt(idx));
		}

		stk_key.value=idx;

		// pass value through the stack

		vm_push_stack_element(vm,stk_value); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}

	void	 ScriptObjectIteratorVector::next(){
		ScriptObjectVector *sov=(ScriptObjectVector *)ref_object;
		if(sov==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			idx++;
		}
	}

	bool	 ScriptObjectIteratorVector::end(){
		ScriptObjectVector *sov=(ScriptObjectVector *)ref_object;
		if(sov==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sov->length();
	}

	ScriptObjectIteratorVector::~ScriptObjectIteratorVector(){


	}
}
