/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayIteratorScriptObject * ArrayIteratorScriptObject::newArrayIteratorObject(
			zetscript::ScriptEngine *_script_engine
			, ArrayScriptObject *_so_ref
	){
		return new ArrayIteratorScriptObject(_script_engine,_so_ref);
	}


	ArrayIteratorScriptObject::ArrayIteratorScriptObject(
		zetscript::ScriptEngine *_script_engine
		, ArrayScriptObject *_so_ref
	):RefScriptObject(_script_engine,_so_ref)
	{
		script_type_id=SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT;
		idx=0;
		stk_key.properties=STACK_ELEMENT_PROPERTY_INT;
		stk_value.properties=STACK_ELEMENT_PROPERTY_INT;
	}

	void ArrayIteratorScriptObject::get(){
		ArrayScriptObject *sov=(ArrayScriptObject *)ref_object;
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
