/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	StringIteratorScriptObject * StringIteratorScriptObject::newStringIteratorScriptObject(
			ScriptEngine *_se
			, StringScriptObject *_so_ref
	){
		return new StringIteratorScriptObject(_se,_so_ref);
	}

	StringIteratorScriptObject::StringIteratorScriptObject(
			ScriptEngine *_se
			, StringScriptObject *_so_ref
	):RefScriptObject(_se,_so_ref){
		script_type_id=SCRIPT_TYPE_ID_STRING_ITERATOR_SCRIPT_OBJECT;
		idx=0;
		stk_key.properties=STACK_ELEMENT_PROPERTY_INT;
		stk_value.properties=STACK_ELEMENT_PROPERTY_INT;
	}

	void StringIteratorScriptObject::get(){
		StringScriptObject *sos=(StringScriptObject *)ref_object;
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

	void	 StringIteratorScriptObject::next(){
		StringScriptObject *sos=(StringScriptObject *)ref_object;
		if(sos==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}

		if(idx<sos->length()){
			idx++;
		}
	}

	bool	 StringIteratorScriptObject::end(){
		StringScriptObject *sos=(StringScriptObject *)ref_object;
		if(sos==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return idx >= sos->length();
	}

	StringIteratorScriptObject::~StringIteratorScriptObject(){

	}

}
