/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectContainerSlotAssignment * ScriptObjectContainerSlotAssignment::newScriptObjectContainerSlotAssignment(ScriptObject *_so, ContainerSlotAssignment *_container_slot_store){
		ScriptObjectContainerSlotAssignment *si= new ScriptObjectContainerSlotAssignment(_so,_container_slot_store);
		return si;
	}


	void ScriptObjectContainerSlotAssignment::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_WEAK_POINTER;
		ref_object = NULL;
		vm=NULL;
	}

	ScriptObjectContainerSlotAssignment::ScriptObjectContainerSlotAssignment(){
		setup();
	}

	ScriptObjectContainerSlotAssignment::ScriptObjectContainerSlotAssignment(ScriptObject *_so_target, ContainerSlotAssignment *_container_slot_store){
		setup();
		// setup object
		this->init(_so_target->getZetScript());
		ref_object=new RefObject(_so_target,this);
		this->container_slot_store=_container_slot_store;
	}

	ContainerSlotAssignment *ScriptObjectContainerSlotAssignment::getContainerSlotAssignment(){
		return this->container_slot_store;
	}

	ScriptObject *ScriptObjectContainerSlotAssignment::getTargetObject(){
		return ref_object->getTargetObject();
	}

	ScriptObjectContainerSlotAssignment::~ScriptObjectContainerSlotAssignment(){
		delete ref_object;
		delete this->container_slot_store;
	}
}
