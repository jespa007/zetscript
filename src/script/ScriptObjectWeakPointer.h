/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_WEAK_POINTER(so) (ScriptObjectWeakPointer::newScriptObjectWeakPointer(so))

namespace zetscript{

	class  ScriptObjectWeakPointer:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectWeakPointer * newScriptObjectWeakPointer(ScriptObject *_so_target, ContainerSlotStore *_stk_item);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObject *getTargetObject();

		ScriptObjectWeakPointer();
		ScriptObjectWeakPointer(ScriptObject *_so, ContainerSlotStore *_stk_item);
		ContainerSlotStore *getContainerSlotStore();
		~ScriptObjectWeakPointer();
	protected:
		void setup();
	private:

		ContainerSlotStore	*container_slot_store;
		RefObject *ref_object;

	};

}
