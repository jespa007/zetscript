/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ObjectIteratorScriptObject * ObjectIteratorScriptObject::newObjectIteratorScriptObject(
			ZetScript *_zs
			,ObjectScriptObject *_so_ref){
		return new ObjectIteratorScriptObject(_zs,_so_ref);
	}


	ObjectIteratorScriptObject::ObjectIteratorScriptObject(
		ZetScript *_zs
		,ObjectScriptObject *_so_ref
	):RefObjectScriptObject(_zs,_so_ref){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT;
		stk_key.setUndefined();
		stk_key.properties=(STK_PROPERTY_ZS_CHAR_PTR);

		// setup object
		it=_so_ref->getMapFields()->begin();
	}


	bool	 ObjectIteratorScriptObject::end(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return it.end();
	}

	void	 ObjectIteratorScriptObject::get(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		if(it.end()) return;

		// pass value through the stack
		stk_key.value=(zs_int)it.key;

		vm_push_stack_element(vm,*((StackElement *)it.value));  // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 ObjectIteratorScriptObject::next(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		it.next();
	}

	ObjectIteratorScriptObject::~ObjectIteratorScriptObject(){
	}
}
