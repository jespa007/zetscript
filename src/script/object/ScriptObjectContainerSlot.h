/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_CONTAINER_SLOT_ASSIGNMENT(so) (ScriptObjectContainerSlot::newScriptObjectContainerSlot(so))

namespace zetscript{

	class  ScriptObjectContainerSlot:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectContainerSlot * newScriptObjectContainerSlot(
				ScriptObject 	*	_so_target
				,ScriptObject  	*	_object
				,zs_int 			_id_slot
				,StackElement  	*	_ptr_stk // should have ScriptObjectContainerSlot;
		);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObject *getTargetObject();

		ScriptObjectContainerSlot();
		ScriptObjectContainerSlot(
				ScriptObject 	*	_so
				,ScriptObject  	*	_object
				,zs_int 			_id_slot
				,StackElement  	*	_ptr_stk // should have ScriptObjectContainerSlot;
		);

		~ScriptObjectContainerSlot();
	protected:
		void setup();
	private:

		ScriptObject  *object; // should be vector,object or class
		zs_int 		  id_slot;
		StackElement  *ptr_stk; // should have ScriptObjectContainerSlot
		RefObject *ref_object;

	};

}
