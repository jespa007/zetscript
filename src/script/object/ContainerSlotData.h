/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ScriptObjectContainer;
	class ScriptObjectContainerSlot;
	class ContainerSlotData{
	public:
		ScriptObjectContainerSlot  	*	so_container_slot_ref; 	// it references the slot container (if NULL is the first node of hierarchy)
		ScriptObjectContainer  		*	so_container_ref; 		// it references the container src
		zs_int 		  					id_slot;
		StackElement  				*	ptr_stk; // should have ScriptObjectContainerSlot

		ContainerSlotData(
				ScriptObjectContainerSlot	*	_so_container_slot_ref
				,ScriptObjectContainer  	*	_so_container_ref
				,zs_int 	  					_id_slot
				,StackElement 				*	_ptr_stk
		);

	};
}
