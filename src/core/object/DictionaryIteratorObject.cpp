/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ObjectIteratorObject * ObjectIteratorObject::newObjectIteratorObject(
			ZetScript *_zs
			,DictionaryObject *_so_ref){
		return new ObjectIteratorObject(_zs,_so_ref);
	}


	ObjectIteratorObject::ObjectIteratorObject(
		ZetScript *_zs
		,DictionaryObject *_so_ref
	):RefObjectObject(_zs,_so_ref){
		type_id=TYPE_ID_OBJECT_ITERATOR_OBJECT;
		stk_key.setUndefined();
		stk_key.properties=(STACK_ELEMENT_PROPERTY_CHAR_PTR);

		// setup object
		it=_so_ref->getMapStringFields()->begin();
	}


	bool	 ObjectIteratorObject::end(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return it.end();
	}

	void	 ObjectIteratorObject::get(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		if(it.end()) return;

		// pass value through the stack
		stk_key.value=(zs_int)it.key;

		vm_push_stack_element(vm,*((StackElement *)it.value));  // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 ObjectIteratorObject::next(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		it.next();
	}

	ObjectIteratorObject::~ObjectIteratorObject(){
	}
}
