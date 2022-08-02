/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVector *ScriptObjectObjectWrap_keys(ZetScript *_zs,ScriptObjectObject *o1){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ScriptObjectVector *sv= ZS_NEW_OBJECT_VECTOR(_zs);

		zs_map *map=o1->getMapUserProperties();
		for(auto it=map->begin(); !it.end(); it.next()){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so=ZS_NEW_OBJECT_STRING(_zs);
			so->set(it.key);

			// create and share pointer
			if(!vm_create_shared_script_object(vm,so)){
				THROW_RUNTIME_ERRORF("cannot creat shared pointer");
			}
			if(!vm_share_script_object(vm,so)){
				THROW_RUNTIME_ERRORF("cannot share pointer");
			}

			stk->value=(zs_int)so;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;

	}

	/*static ScriptObjectIteratorObject *ScriptObjectObjectWrap_iteratorSf(ScriptObjectObject *o1){

	}*/


	bool ScriptObjectObjectWrap_contains(ZetScript *_zs,ScriptObjectObject *o1, zs_string * key){
		ZS_UNUSUED_PARAM(_zs);
		return o1->existUserProperty(key->c_str());
	}

	void ScriptObjectObjectWrap_clear(ZetScript *_zs,ScriptObjectObject *o1){
		ZS_UNUSUED_PARAM(_zs);
		o1->eraseAllUserProperties();
	}

	void ScriptObjectObjectWrap_erase(ZetScript *_zs,ScriptObjectObject *o1, zs_string * key){
		ZS_UNUSUED_PARAM(_zs);
		o1->eraseUserProperty(key->c_str());
	}

	ScriptObjectIteratorObject * ScriptObjectObjectWrap_iter(ZetScript *_zs,ScriptObjectObject *oo){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_OBJECT_ITERATOR_OBJECT(oo);
	}

	void						 	ScriptObjectObjectWrap_append(ZetScript *_zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		ScriptObjectObject::append(_zs,o1,o2);
	}

}
