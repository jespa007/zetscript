/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	StringIteratorObject * StringIteratorObject::newStringIteratorObject(
			ZetScript *_zs
			, StringObject *_so_ref
	){
		return new StringIteratorObject(_zs,_so_ref);
	}

	StringIteratorObject::StringIteratorObject(
			ZetScript *_zs
			, StringObject *_so_ref
	):RefObjectObject(_zs,_so_ref){
		type_id=TYPE_ID_OBJECT_ITERATOR_STRING;
		idx=0;
		stk_key.properties=STACK_ELEMENT_PROPERTY_INT;
		stk_value.properties=STACK_ELEMENT_PROPERTY_INT;
	}

	void StringIteratorObject::get(){
		StringObject *sos=(StringObject *)ref_object;
		if(sos==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		String *str_ref=sos->str_ptr;
		if(idx < (int)str_ref->length()){
			// set value
			zs_int ch=str_ref->at(idx);
			stk_value.value=ch;
		}

		stk_key.value=idx;

		// pass value through the stack
		vm_push_stack_element(vm,stk_value); // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}

	void	 StringIteratorObject::next(){
		StringObject *sos=(StringObject *)ref_object;
		if(sos==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sos->length()){
			idx++;
		}
	}

	bool	 StringIteratorObject::end(){
		StringObject *sos=(StringObject *)ref_object;
		if(sos==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sos->length();
	}

	StringIteratorObject::~StringIteratorObject(){

	}

}
