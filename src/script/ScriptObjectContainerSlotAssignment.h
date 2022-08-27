/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_CONTAINER_SLOT_ASSIGNMENT(so) (ScriptObjectContainerSlotAssignment::newScriptObjectContainerSlotAssignment(so))

namespace zetscript{

	class  ScriptObjectContainerSlotAssignment:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectContainerSlotAssignment * newScriptObjectContainerSlotAssignment(ScriptObject *_so_target, ContainerSlotAssignment *_stk_item);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObject *getTargetObject();

		ScriptObjectContainerSlotAssignment();
		ScriptObjectContainerSlotAssignment(ScriptObject *_so, ContainerSlotAssignment *_stk_item);
		ContainerSlotAssignment *getContainerSlotAssignment();
		~ScriptObjectContainerSlotAssignment();
	protected:
		void setup();
	private:

		ContainerSlotAssignment	*container_slot_store;
		RefObject *ref_object;

	};

}
