/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectContainerSlot * ScriptObjectContainerSlot::newScriptObjectContainerSlot(
			ScriptObjectContainerSlot 	*	_so_origin_container_slot
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
	){
		ScriptObjectContainerSlot *si= new ScriptObjectContainerSlot(
			_so_origin_container_slot
			,_id_slot
			,_ptr_stk
		);
		return si;
	}


	void ScriptObjectContainerSlot::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_CONTAINER_SLOT;
		so_origin_container_slot = NULL;
		vm=NULL;
	}

	ScriptObjectContainerSlot::ScriptObjectContainerSlot(){
		setup();
	}

	ScriptObjectContainerSlot::ScriptObjectContainerSlot(
			ScriptObjectContainerSlot 	*	_so_origin_container_slot
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
	){
		setup();
		// setup object
		this->init(_so_origin_container_slot->getZetScript());

		so_origin_container_slot=_so_origin_container_slot;
		id_slot=_id_slot;
		ptr_stk=_ptr_stk;

	}


	ScriptObjectContainerSlot::~ScriptObjectContainerSlot(){

	}
}
