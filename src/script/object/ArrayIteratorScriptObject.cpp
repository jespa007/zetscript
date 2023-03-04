/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayIteratorScriptObject * ArrayIteratorScriptObject::newArrayIteratorScriptObject(
			ZetScript *_zs
			, ArrayScriptObject *_so_ref
	){
		return new ArrayIteratorScriptObject(_zs,_so_ref);
	}


	ArrayIteratorScriptObject::ArrayIteratorScriptObject(
		ZetScript *_zs
		, ArrayScriptObject *_so_ref
	):RefObjectScriptObject(_zs,_so_ref)
	{
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_ARRAY;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	void ArrayIteratorScriptObject::get(){
		ArrayScriptObject *sov=(ArrayScriptObject *)ref_object;
		if(sov==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			// set value
			stk_value=*((StackElement *)sov->get(idx));
		}

		stk_key.value=idx;

		// pass value through the stack

		vm_push_stack_element(vm,stk_value); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}

	void	 ArrayIteratorScriptObject::next(){
		ArrayScriptObject *sov=(ArrayScriptObject *)ref_object;
		if(sov==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			idx++;
		}
	}

	bool	 ArrayIteratorScriptObject::end(){
		ArrayScriptObject *sov=(ArrayScriptObject *)ref_object;
		if(sov==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sov->length();
	}

	ArrayIteratorScriptObject::~ArrayIteratorScriptObject(){


	}
}
