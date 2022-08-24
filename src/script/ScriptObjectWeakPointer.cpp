/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectWeakPointer * ScriptObjectWeakPointer::newScriptObjectWeakPointer(ScriptObject *_so, ContainerSlotStore *_container_slot_store){
		ScriptObjectWeakPointer *si= new ScriptObjectWeakPointer(_so,_container_slot_store);
		return si;
	}


	void ScriptObjectWeakPointer::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_WEAK_POINTER;
		ref_object = NULL;
		vm=NULL;
	}

	ScriptObjectWeakPointer::ScriptObjectWeakPointer(){
		setup();
	}

	ScriptObjectWeakPointer::ScriptObjectWeakPointer(ScriptObject *_so_target, ContainerSlotStore *_container_slot_store){
		setup();
		// setup object
		this->init(_so_target->getZetScript());
		ref_object=new RefObject(_so_target,this);
		this->container_slot_store=_container_slot_store;
	}

	ContainerSlotStore *ScriptObjectWeakPointer::getContainerSlotStore(){
		return this->container_slot_store;
	}

	ScriptObject *ScriptObjectWeakPointer::getTargetObject(){
		return ref_object->getTargetObject();
	}

	ScriptObjectWeakPointer::~ScriptObjectWeakPointer(){
		delete ref_object;
		delete this->container_slot_store;
	}
}
