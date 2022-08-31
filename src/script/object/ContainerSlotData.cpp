/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerSlotData::ContainerSlotData(
			ScriptObjectContainerSlot	*	_so_container_slot_ref
			,ScriptObjectContainer  	*	_so_container_ref
			,zs_int 	   					_id_slot
			,StackElement 				*	_ptr_stk
	){
		so_container_slot_ref=_so_container_slot_ref;
		so_container_ref=_so_container_ref;
		id_slot=_id_slot;
		ptr_stk=_ptr_stk;
	}
}
