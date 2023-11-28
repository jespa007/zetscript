/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayObject *ObjectObjectZs_keys(ZetScript *_zs,DictionaryObject *o1){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayObject *sv= ZS_NEW_ARRAY_OBJECT(_zs);

		MapString *fields=o1->getMapStringFields();
		for(auto it=fields->begin(); !it.end(); it.next()){
			StackElement *stk=sv->newSlot();
			StringObject *so=ZS_NEW_STRING_OBJECT(_zs);
			so->set(it.key);

			// create and share pointer
			vm_create_shared_object(vm,so);
			vm_share_object(vm,so);

			stk->value=(zs_int)so;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;
		}

		return sv;

	}

	/*static ObjectIteratorObject *ObjectObject_iteratorSf(DictionaryObject *o1){

	}*/


	bool ObjectObjectZs_contains(ZetScript *_zs,DictionaryObject *o1, String * key){
		ZS_UNUSUED_PARAM(_zs);
		return o1->exists(key->toConstChar());
	}

	void ObjectObjectZs_clear(ZetScript *_zs,DictionaryObject *o1){
		ZS_UNUSUED_PARAM(_zs);
		o1->eraseAll();
	}

	void ObjectObjectZs_erase(ZetScript *_zs,DictionaryObject *o1, String * key){
		ZS_UNUSUED_PARAM(_zs);
		o1->erase(key->toConstChar());
	}

	ObjectIteratorObject * ObjectObjectZs_iter(ZetScript *_zs,DictionaryObject *_oo){
		return ZS_NEW_OBJECT_ITERATOR_OBJECT(_zs,_oo);
	}

	void						 	ObjectObjectZs_extend(ZetScript *_zs,DictionaryObject *o1,DictionaryObject *o2){
		DictionaryObject::append(_zs,o1,o2);
	}

}
