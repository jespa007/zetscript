/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ObjectIteratorScriptObject * ObjectIteratorScriptObject::newObjectIteratorScriptObject(ObjectScriptObject *_oo){
		ObjectIteratorScriptObject *oi= new ObjectIteratorScriptObject(_oo);
		return oi;
	}

	void ObjectIteratorScriptObject::setup(){
			idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT;
			vm=NULL;
			stk_key.setUndefined();
			stk_key.properties=(STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR);
	}

	ObjectIteratorScriptObject::ObjectIteratorScriptObject(){
		setup();
	}

	ObjectIteratorScriptObject::ObjectIteratorScriptObject(ObjectScriptObject *_oo){
		setup();
		// setup object
		this->init(_oo->getZetScript());
		it=_oo->getMapUserProperties()->begin();
	}


	bool	 ObjectIteratorScriptObject::end(){
		if(ref_object==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return it.end();
	}

	void	 ObjectIteratorScriptObject::get(){
		if(ref_object==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		if(it.end()) return;

		// pass value through the stack
		stk_key.value=(zs_int)it.key;

		vm_push_stack_element(vm,*((StackElement *)it.value));  // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 ObjectIteratorScriptObject::next(){
		if(ref_object==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		it.next();
	}

	ObjectIteratorScriptObject::~ObjectIteratorScriptObject(){
	}
}
