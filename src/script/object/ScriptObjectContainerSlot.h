/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectContainerSlot:public ScriptObject{

	public:


		ScriptObjectContainerSlot * newScriptObjectContainerSlot(
				ScriptObjectContainerSlot 	*	_so_origin_container_slot
				,zs_int 						_id_slot
				,StackElement  				*	_ptr_stk
		);
		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectContainerSlot();
		ScriptObjectContainerSlot(
			ScriptObjectContainerSlot 	*	_so_origin_container_slot
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
		);

		ScriptObjectContainerSlot	*	getOriginContainerSlotObject();
		~ScriptObjectContainerSlot();
	protected:
		void setup();
	private:

		zs_list<ScriptObjectContainerSlot *>		container_slots;
		ScriptObjectContainerSlot				*	so_origin_container_slot;
		zs_int 										id_slot;
		StackElement  							*	ptr_stk;
	};

}
