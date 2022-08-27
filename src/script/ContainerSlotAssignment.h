/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ContainerSlotAssignment{
	public:
		ScriptObject  *object; // should be vector,object or class
		zs_int 		  id_slot;
		StackElement  *ptr_stk;

		ContainerSlotAssignment(	ScriptObject  *_object
								,zs_int 	  id_slot
								,StackElement *_ptr_stk
		);
	};
}
