/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class ContainerSlotData{
	public:
		ScriptObjectContainerSlot  	*	so_container_slot_ref; // should be vector,object or class
		zs_int 		  					id_slot;
		StackElement  				*	ptr_stk; // should have ScriptObjectContainerSlot

		ContainerSlotData(
				ScriptObjectContainerSlot  	*	_so_container_slot_ref
				,zs_int 	  					_id_slot
				,StackElement 				*	_ptr_stk
		);
	};
}
