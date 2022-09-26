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

		ContainerScriptObject(ZetScript *_zs);
		ContainerSlot 				*	getScriptObjectContainerSlotRoot();
		zs_list<ContainerSlot *>	*	getListContainerSlotsRef();

		// Container src references should use 'addSlots' and 'removeContainerSlot'
		void							addContainerSlot(ContainerSlot *_container_slot);
		void 							removeContainerSlot(ContainerSlot *_container_slot, VM_ScopeBlock 	*	_scope_block);

		void 							printReferences();
		~ContainerScriptObject();
	private:

		static void 	onDettachContainerSlotNode(zs_list_node<ContainerSlot *> *_current_node);

		// refs where this container is referenced
		void						removeAllSlots();
		zs_list<ContainerSlot *>	*container_slots;

	};

}
