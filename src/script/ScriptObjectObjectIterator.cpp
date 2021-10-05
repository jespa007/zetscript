/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectObjectIterator * ScriptObjectObjectIterator::newScriptObjectObjectIterator(ScriptObjectObject *_oo){
		ScriptObjectObjectIterator *oi= new ScriptObjectObjectIterator(_oo);
		return oi;
	}

	void ScriptObjectObjectIterator::setup(){
			idx_script_class=IDX_TYPE_SCRIPT_OBJECT_OBJECT_ITERATOR;
			oo = NULL;
			vm=NULL;
			stk_key.setUndefined();
			stk_key.properties=STK_PROPERTY_SCRIPT_OBJECT;
	}

	ScriptObjectObjectIterator::ScriptObjectObjectIterator(){
		setup();
	}

	ScriptObjectObjectIterator::ScriptObjectObjectIterator(ScriptObjectObject *_oo){
		setup();
		// setup object
		this->init(_oo->getZetScript());
		oo=_oo;
		it=oo->begin();

	}


	bool	 ScriptObjectObjectIterator::end(){
		return it.end();
	}

	void	 ScriptObjectObjectIterator::get(){
		if(oo==NULL) return;
		if(it.end()) return;

		// pass value through the stack

		vm_push_stack_element(vm,*((StackElement *)it.value)); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 ScriptObjectObjectIterator::next(){
		it.next();
	}
}
