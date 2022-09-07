/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	VectorIteratorScriptObject * VectorIteratorScriptObject::newVectorIteratorScriptObject(
			ZetScript *_zs
			, VectorScriptObject *_so_ref
	){
		return new VectorIteratorScriptObject(_zs,_so_ref);
	}


	VectorIteratorScriptObject::VectorIteratorScriptObject(
		ZetScript *_zs
		, VectorScriptObject *_so_ref
	):RefObjectScriptObject(_zs,_so_ref)
	{
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR;
		idx=0;
		stk_key.properties=STK_PROPERTY_ZS_INT;
		stk_value.properties=STK_PROPERTY_ZS_INT;
	}

	void VectorIteratorScriptObject::get(){
		VectorScriptObject *sov=(VectorScriptObject *)ref_object;
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

	void	 VectorIteratorScriptObject::next(){
		VectorScriptObject *sov=(VectorScriptObject *)ref_object;
		if(sov==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			idx++;
		}
	}

	bool	 VectorIteratorScriptObject::end(){
		VectorScriptObject *sov=(VectorScriptObject *)ref_object;
		if(sov==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sov->length();
	}

	VectorIteratorScriptObject::~VectorIteratorScriptObject(){


	}
}
