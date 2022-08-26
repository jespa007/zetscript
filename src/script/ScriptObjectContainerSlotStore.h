/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_CONTAINER_SLOT_STORE(so) (ScriptObjectContainerSlotStore::newScriptObjectContainerSlotStore(so))

namespace zetscript{

	class  ScriptObjectContainerSlotStore:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectContainerSlotStore * newScriptObjectContainerSlotStore(ScriptObject *_so_target, ContainerSlotStore *_stk_item);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObject *getTargetObject();

		ScriptObjectContainerSlotStore();
		ScriptObjectContainerSlotStore(ScriptObject *_so, ContainerSlotStore *_stk_item);
		ContainerSlotStore *getContainerSlotStore();
		~ScriptObjectContainerSlotStore();
	protected:
		void setup();
	private:

		ContainerSlotStore	*container_slot_store;
		RefObject *ref_object;

	};

}
