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

	class ContainerSlot;
	class  ContainerScriptObject:public ScriptObject{
	public:

		ContainerScriptObject();
		ContainerSlot 				*	getScriptObjectContainerSlotRoot();
		zs_list<ContainerSlot *>	*	getListContainerSlotsRef();
		void							addSlot(ContainerSlot *_container_slot, ContainerScriptObject	 	*	_src_so_container_ref);
		void 							removeSlot(ContainerSlot *_container_slot, VM_ScopeBlock 	*	_scope_block);
		void 							printReferences();
		~ContainerScriptObject();
	private:

		// refs where this container is referenced
		zs_list<ContainerSlot *>	*container_slots;

	};

}
