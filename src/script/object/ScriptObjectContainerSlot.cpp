/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectContainerSlot * ScriptObjectContainerSlot::newScriptObjectContainerSlot(ScriptObject *_so, ContainerSlot *_container_slot_store){
		ScriptObjectContainerSlot *si= new ScriptObjectContainerSlot(_so,_container_slot_store);
		return si;
	}


	void ScriptObjectContainerSlot::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_WEAK_POINTER;
		ref_object = NULL;
		vm=NULL;
	}

	ScriptObjectContainerSlot::ScriptObjectContainerSlot(){
		setup();
	}

	ScriptObjectContainerSlot::ScriptObjectContainerSlot(ScriptObject *_so_target, ContainerSlot *_container_slot_store){
		setup();
		// setup object
		this->init(_so_target->getZetScript());
		ref_object=new RefObject(_so_target,this);
		this->container_slot_store=_container_slot_store;
	}

	ContainerSlot *ScriptObjectContainerSlot::getContainerSlot(){
		return this->container_slot_store;
	}

	ScriptObject *ScriptObjectContainerSlot::getTargetObject(){
		return ref_object->getTargetObject();
	}

	ScriptObjectContainerSlot::~ScriptObjectContainerSlot(){
		delete ref_object;
		delete this->container_slot_store;
	}
}
