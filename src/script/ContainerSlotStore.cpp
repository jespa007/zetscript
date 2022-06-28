/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerSlotStore::ContainerSlotStore(
			ScriptObject  *_object
			,zs_int 	   _id_slot
			,StackElement *_content
	){
		object=_object;
		id_slot=_id_slot;
		content=*_content;
	}
}
