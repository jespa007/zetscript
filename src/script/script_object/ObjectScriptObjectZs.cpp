/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayScriptObject *ObjectScriptObjectZs_keys(ZetScript *_zs,ObjectScriptObject *o1){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv= ZS_NEW_ARRAY_OBJECT(_zs);

		zs_map *fields=o1->getMapFields();
		for(auto it=fields->begin(); !it.end(); it.next()){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so=ZS_NEW_STRING_OBJECT(_zs);
			so->set(it.key);

			// create and share pointer
			vm_create_shared_script_object(vm,so);
			vm_share_script_object(vm,so);

			stk->value=(zs_int)so;
			stk->properties = ZS_STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;

	}

	/*static ObjectIteratorScriptObject *ObjectScriptObject_iteratorSf(ObjectScriptObject *o1){

	}*/


	bool ObjectScriptObjectZs_contains(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key){
		ZS_UNUSUED_PARAM(_zs);
		return o1->exists(key->c_str());
	}

	void ObjectScriptObjectZs_clear(ZetScript *_zs,ObjectScriptObject *o1){
		ZS_UNUSUED_PARAM(_zs);
		o1->eraseAll();
	}

	void ObjectScriptObjectZs_erase(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key){
		ZS_UNUSUED_PARAM(_zs);
		o1->erase(key->c_str());
	}

	ObjectIteratorScriptObject * ObjectScriptObjectZs_iter(ZetScript *_zs,ObjectScriptObject *_oo){
		return ZS_NEW_OBJECT_ITERATOR_OBJECT(_zs,_oo);
	}

	void						 	ObjectScriptObjectZs_extend(ZetScript *_zs,ObjectScriptObject *o1,ObjectScriptObject *o2){
		ObjectScriptObject::append(_zs,o1,o2);
	}

}
