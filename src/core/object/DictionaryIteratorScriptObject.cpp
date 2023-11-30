/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	DictionaryIteratorScriptObject * DictionaryIteratorScriptObject::newDictionaryIteratorScriptObject(
			ZetScript *_zs
			,DictionaryScriptObject *_so_ref){
		return new DictionaryIteratorScriptObject(_zs,_so_ref);
	}


	DictionaryIteratorScriptObject::DictionaryIteratorScriptObject(
		ZetScript *_zs
		,DictionaryScriptObject *_so_ref
	):RefDictionaryScriptObject(_zs,_so_ref){
		script_type_id=SCRIPT_TYPE_ID_DICTIONARY_ITERATOR_SCRIPT_OBJECT;
		stk_key.setUndefined();
		stk_key.properties=(STACK_ELEMENT_PROPERTY_CHAR_PTR);

		// setup object
		it=_so_ref->getMapStringFields()->begin();
	}


	bool	 DictionaryIteratorScriptObject::end(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return it.end();
	}

	void	 DictionaryIteratorScriptObject::get(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		if(it.end()) return;

		// pass value through the stack
		stk_key.value=(zs_int)it.key;

		vm_push_stack_element(vm,*((StackElement *)it.value));  // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 DictionaryIteratorScriptObject::next(){
		if(ref_object==NULL) {
			ZS_THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		it.next();
	}

	DictionaryIteratorScriptObject::~DictionaryIteratorScriptObject(){
	}
}
