/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerSlotAssignment::ContainerSlotAssignment(
			ScriptObject  *_object
			,zs_int 	   _id_slot
			,StackElement *_ptr_stk
	){
		object=_object;
		id_slot=_id_slot;
		ptr_stk=_ptr_stk;
	}
}
