/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayScriptObject *ObjectScriptObjectWrap_keys(ZetScript *_zs,ObjectScriptObject *o1){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv= ZS_NEW_ARRAY_OBJECT(_zs);

		zs_map *fields=o1->getMapFields();
		for(auto it=fields->begin(); !it.end(); it.next()){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so=ZS_NEW_STRING_OBJECT(_zs);
			so->set(it.key);

			// create and share pointer
			if(!vm_create_shared_script_object(vm,so)){
				ZS_THROW_RUNTIME_ERRORF("cannot creat shared pointer");
			}
			if(!vm_share_script_object(vm,so)){
				ZS_THROW_RUNTIME_ERRORF("cannot share pointer");
			}

			stk->value=(zs_int)so;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;

	}

	/*static ObjectIteratorScriptObject *ObjectScriptObjectWrap_iteratorSf(ObjectScriptObject *o1){

	}*/


	bool ObjectScriptObjectWrap_contains(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key){
		ZS_UNUSUED_PARAM(_zs);
		return o1->exists(key->c_str());
	}

	void ObjectScriptObjectWrap_clear(ZetScript *_zs,ObjectScriptObject *o1){
		ZS_UNUSUED_PARAM(_zs);
		o1->eraseAll();
	}

	void ObjectScriptObjectWrap_erase(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key){
		ZS_UNUSUED_PARAM(_zs);
		o1->erase(key->c_str());
	}

	ObjectIteratorScriptObject * ObjectScriptObjectWrap_iter(ZetScript *_zs,ObjectScriptObject *_oo){
		return ZS_NEW_OBJECT_ITERATOR_OBJECT(_zs,_oo);
	}

	void						 	ObjectScriptObjectWrap_extend(ZetScript *_zs,ObjectScriptObject *o1,ObjectScriptObject *o2){
		ObjectScriptObject::append(_zs,o1,o2);
	}

}
