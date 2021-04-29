/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	ScriptObjectVector *ScriptObjectObjectWrap_keys(ScriptObjectObject *o1){
		VirtualMachine *vm=o1->getZetScript()->getVirtualMachine();
		ScriptObjectVector *sv= ZS_NEW_OBJECT_VECTOR(o1->getZetScript());

		for(auto it=o1->begin(); !it.end(); it.next()){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so=ZS_NEW_OBJECT_STRING(o1->getZetScript());
			so->set(it.getKey());

			// create and share pointer
			if(!vm_create_shared_pointer(vm,so)){
				THROW_RUNTIME_ERROR("cannot creat shared pointer");
			}
			if(!vm_share_pointer(vm,so)){
				THROW_RUNTIME_ERROR("cannot share pointer");
			}

			stk->stk_value=so;
			stk->properties = MSK_STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;

	}

	/*static ScriptObjectObjectIterator *ScriptObjectObjectWrap_iteratorSf(ScriptObjectObject *o1){

	}*/


	bool ScriptObjectObjectWrap_contains(ScriptObjectObject *o1, std::string * key){
		return o1->existUserProperty(key->c_str());
	}

	void ScriptObjectObjectWrap_clear(ScriptObjectObject *o1){
		o1->eraseAllUserProperties();
	}

	void ScriptObjectObjectWrap_erase(ScriptObjectObject *o1, std::string * key){
		o1->eraseUserProperty(*key);
	}

	ScriptObjectObject * ScriptObjectObjectWrap_concat(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		std::string error="";
		ScriptObjectObject *obj = ZS_NEW_OBJECT_OBJECT(zs);
		//zs_map_iterator it_1=o1->map_user_property_keys->begin();
		//zs_map_iterator it_2=o2->map_user_property_keys->begin();

		for(auto it=o1->begin(); !it.end();it.next()){
			obj->addUserProperty(it.getKey(),error,(StackElement *)it.getValue());
		}

		for(auto it=o2->begin(); !it.end();it.next()){
			obj->addUserProperty(it.getKey(),error,(StackElement *)it.getValue());
		}
		return obj;
	}
}
