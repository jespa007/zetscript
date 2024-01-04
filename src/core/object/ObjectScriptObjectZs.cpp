/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ArrayScriptObject *ObjectScriptObjectZs_keys(ScriptEngine *_script_engine,ObjectScriptObject *o1){
		VirtualMachine *vm=_script_engine->getVirtualMachine();
		ArrayScriptObject *sv= ZS_NEW_ARRAY_SCRIPT_OBJECT(_script_engine);

		MapString *fields=o1->getMapStringFields();
		for(auto it=fields->begin(); !it.end(); it.next()){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so=ZS_NEW_STRING_SCRIPT_OBJECT(_script_engine);
			so->set(it.key);

			// create and share pointer
			vm_create_shared_object(vm,so);
			vm_share_object(vm,so);

			stk->value=(zs_int)so;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;
		}

		return sv;

	}

	/*static ObjectIteratorScriptObject *ObjectScriptObject_iteratorSf(ObjectScriptObject *o1){

	}*/


	bool ObjectScriptObjectZs_contains(ScriptEngine *_script_engine,ObjectScriptObject *o1, String * key){
		ZS_UNUSUED_PARAM(_script_engine);
		return o1->exists(key->toConstChar());
	}

	void ObjectScriptObjectZs_clear(ScriptEngine *_script_engine,ObjectScriptObject *o1){
		ZS_UNUSUED_PARAM(_script_engine);
		o1->eraseAll();
	}

	void ObjectScriptObjectZs_erase(ScriptEngine *_script_engine,ObjectScriptObject *o1, String * key){
		ZS_UNUSUED_PARAM(_script_engine);
		o1->erase(key->toConstChar());
	}

	ObjectIteratorScriptObject * ObjectScriptObjectZs_iter(ScriptEngine *_script_engine,ObjectScriptObject *_oo){
		return ZS_NEW_OBJECT_ITERATOR_SCRIPT_OBJECT(_script_engine,_oo);
	}

	void						 	ObjectScriptObjectZs_extend(ScriptEngine *_script_engine,ObjectScriptObject *o1,ObjectScriptObject *o2){
		ObjectScriptObject::append(_script_engine,o1,o2);
	}

}
