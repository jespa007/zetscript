/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectContainerSlotStore * ScriptObjectContainerSlotStore::newScriptObjectContainerSlotStore(ScriptObject *_so, ContainerSlotStore *_container_slot_store){
		ScriptObjectContainerSlotStore *si= new ScriptObjectContainerSlotStore(_so,_container_slot_store);
		return si;
	}


	void ScriptObjectContainerSlotStore::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_WEAK_POINTER;
		ref_object = NULL;
		vm=NULL;
	}

	ScriptObjectContainerSlotStore::ScriptObjectContainerSlotStore(){
		setup();
	}

	ScriptObjectContainerSlotStore::ScriptObjectContainerSlotStore(ScriptObject *_so_target, ContainerSlotStore *_container_slot_store){
		setup();
		// setup object
		this->init(_so_target->getZetScript());
		ref_object=new RefObject(_so_target,this);
		this->container_slot_store=_container_slot_store;
	}

	ContainerSlotStore *ScriptObjectContainerSlotStore::getContainerSlotStore(){
		return this->container_slot_store;
	}

	ScriptObject *ScriptObjectContainerSlotStore::getTargetObject(){
		return ref_object->getTargetObject();
	}

	ScriptObjectContainerSlotStore::~ScriptObjectContainerSlotStore(){
		delete ref_object;
		delete this->container_slot_store;
	}
}
