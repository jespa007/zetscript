/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayScriptObject *DictionaryScriptObjectZs_keys(ZetScript *_zs,DictionaryScriptObject *o1){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv= ZS_NEW_ARRAY_SCRIPT_OBJECT(_zs);

		MapString *fields=o1->getMapStringFields();
		for(auto it=fields->begin(); !it.end(); it.next()){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
			so->set(it.key);

			// create and share pointer
			vm_create_shared_object(vm,so);
			vm_share_object(vm,so);

			stk->value=(zs_int)so;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;
		}

		return sv;

	}

	/*static DictionaryIteratorScriptObject *DictionaryScriptObject_iteratorSf(DictionaryScriptObject *o1){

	}*/


	bool DictionaryScriptObjectZs_contains(ZetScript *_zs,DictionaryScriptObject *o1, String * key){
		ZS_UNUSUED_PARAM(_zs);
		return o1->exists(key->toConstChar());
	}

	void DictionaryScriptObjectZs_clear(ZetScript *_zs,DictionaryScriptObject *o1){
		ZS_UNUSUED_PARAM(_zs);
		o1->eraseAll();
	}

	void DictionaryScriptObjectZs_erase(ZetScript *_zs,DictionaryScriptObject *o1, String * key){
		ZS_UNUSUED_PARAM(_zs);
		o1->erase(key->toConstChar());
	}

	DictionaryIteratorScriptObject * DictionaryScriptObjectZs_iter(ZetScript *_zs,DictionaryScriptObject *_oo){
		return ZS_NEW_DICTIONARY_ITERATOR_SCRIPT_OBJECT(_zs,_oo);
	}

	void						 	DictionaryScriptObjectZs_extend(ZetScript *_zs,DictionaryScriptObject *o1,DictionaryScriptObject *o2){
		DictionaryScriptObject::append(_zs,o1,o2);
	}

}
