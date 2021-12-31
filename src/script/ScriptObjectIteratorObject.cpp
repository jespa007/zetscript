/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIteratorObject * ScriptObjectIteratorObject::newScriptObjectIteratorObject(ScriptObjectObject *_oo){
		ScriptObjectIteratorObject *oi= new ScriptObjectIteratorObject(_oo);
		return oi;
	}

	void ScriptObjectIteratorObject::setup(){
			idx_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT;
			oo = NULL;
			vm=NULL;
			stk_key.setUndefined();
			stk_key.properties=STK_PROPERTY_SCRIPT_OBJECT;
	}

	ScriptObjectIteratorObject::ScriptObjectIteratorObject(){
		setup();
	}

	ScriptObjectIteratorObject::ScriptObjectIteratorObject(ScriptObjectObject *_oo){
		setup();
		// setup object
		this->init(_oo->getZetScript());
		oo=_oo;
		it=oo->getMapUserProperties()->begin();
	}


	bool	 ScriptObjectIteratorObject::end(){
		return it.end();
	}

	void	 ScriptObjectIteratorObject::get(){
		if(oo==NULL) return;
		if(it.end()) return;

		// pass value through the stack

		vm_push_stack_element(vm,*((StackElement *)it.value));  // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 ScriptObjectIteratorObject::next(){
		it.next();
	}
}
