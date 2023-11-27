/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayIteratorObject * ArrayIteratorObject::newArrayIteratorObject(
			ZetScript *_zs
			, ArrayObject *_so_ref
	){
		return new ArrayIteratorObject(_zs,_so_ref);
	}


	ArrayIteratorObject::ArrayIteratorObject(
		ZetScript *_zs
		, ArrayObject *_so_ref
	):RefObjectObject(_zs,_so_ref)
	{
		type_id=TYPE_ID_OBJECT_ITERATOR_ARRAY;
		idx=0;
		stk_key.properties=STACK_ELEMENT_PROPERTY_INT;
		stk_value.properties=STACK_ELEMENT_PROPERTY_INT;
	}

	void ArrayIteratorObject::get(){
		ArrayObject *sov=(ArrayObject *)ref_object;
		if(sov==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			// set value
			stk_value=*(sov->getStackElementByIndex(idx));
		}

		stk_key.value=idx;

		// pass value through the stack

		vm_push_stack_element(vm,stk_value); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}

	void	 ArrayIteratorObject::next(){
		ArrayObject *sov=(ArrayObject *)ref_object;
		if(sov==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sov->length()){
			idx++;
		}
	}

	bool	 ArrayIteratorObject::end(){
		ArrayObject *sov=(ArrayObject *)ref_object;
		if(sov==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sov->length();
	}

	ArrayIteratorObject::~ArrayIteratorObject(){


	}
}
