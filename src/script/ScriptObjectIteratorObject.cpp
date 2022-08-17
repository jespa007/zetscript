/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIteratorObject * ScriptObjectIteratorObject::newScriptObjectIteratorObject(ScriptObjectObject *_oo){
		ScriptObjectIteratorObject *oi= new ScriptObjectIteratorObject(_oo);
		return oi;
	}

	void ScriptObjectIteratorObject::setup(){
			idx_script_type=IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT;
			ref_object = NULL;
			vm=NULL;
			stk_key.setUndefined();
			stk_key.properties=(STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR);
	}

	ScriptObjectIteratorObject::ScriptObjectIteratorObject(){
		setup();
	}

	ScriptObjectIteratorObject::ScriptObjectIteratorObject(ScriptObjectObject *_oo){
		setup();
		// setup object
		this->init(_oo->getZetScript());

		it_map=_oo->getMapUserProperties();
		it=it_map->begin();
		//oo->refObject((ScriptObject **)&this->oo);
		ref_object=new RefObject(_oo,this);
	}


	bool	 ScriptObjectIteratorObject::end(){
		if(ref_object->getRefObject()==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		return it!=it_map->end();
	}

	void	 ScriptObjectIteratorObject::get(){
		if(ref_object->getRefObject()==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		if(it!=it_map->end()) return;

		// pass value through the stack
		stk_key.value=(zs_int)it->first.c_str();

		vm_push_stack_element(vm,*(it->second));  // last value/first value to get
		vm_push_stack_element(vm,stk_key);
	}


	void	 ScriptObjectIteratorObject::next(){
		if(ref_object->getRefObject()==NULL) {
			THROW_RUNTIME_ERRORF("Attached object was unreferenced");
		}
		it++;
	}

	ScriptObjectIteratorObject::~ScriptObjectIteratorObject(){
		delete ref_object;
	}
}
