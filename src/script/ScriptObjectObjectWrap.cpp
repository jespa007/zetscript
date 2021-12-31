/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVector *ScriptObjectObjectWrap_keys(ScriptObjectObject *o1){
		VirtualMachine *vm=o1->getZetScript()->getVirtualMachine();
		ScriptObjectVector *sv= ZS_NEW_OBJECT_VECTOR(o1->getZetScript());

		zs_map *map=o1->getMapUserProperties();
		for(auto it=map->begin(); !it.end(); it.next()){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so=ZS_NEW_OBJECT_STRING(o1->getZetScript());
			so->set(it.key);

			// create and share pointer
			if(!vm_create_shared_pointer(vm,so)){
				THROW_RUNTIME_ERROR("cannot creat shared pointer");
			}
			if(!vm_share_pointer(vm,so)){
				THROW_RUNTIME_ERROR("cannot share pointer");
			}

			stk->value=(zs_int)so;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;

	}

	/*static ScriptObjectIteratorObject *ScriptObjectObjectWrap_iteratorSf(ScriptObjectObject *o1){

	}*/


	bool ScriptObjectObjectWrap_contains(ScriptObjectObject *o1, zs_string * key){
		return o1->existUserProperty(key->c_str());
	}

	void ScriptObjectObjectWrap_clear(ScriptObjectObject *o1){
		o1->eraseAllUserProperties();
	}

	void ScriptObjectObjectWrap_erase(ScriptObjectObject *o1, zs_string * key){
		o1->eraseUserProperty(*key);
	}

	ScriptObjectIteratorObject * ScriptObjectObjectWrap_iter(ScriptObjectObject *oo){
		return ZS_NEW_OBJECT_ITERATOR_OBJECT(oo);
	}

	void						 	ScriptObjectObjectWrap_append(ScriptObjectObject *o1,ScriptObjectObject *o2){
		ScriptObjectObject::append(o1->getZetScript(),o1,o2);
	}

}
