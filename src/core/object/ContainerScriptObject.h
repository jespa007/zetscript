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

		ContainerScriptObject(ScriptEngine *_zs, ScriptTypeId _ref_script_type_id);
		ContainerSlot 				*	getObjectContainerSlotRoot();
		List<ContainerSlot *>	*	getListContainerSlotsRef();

		// Container src references should use 'addSlots' and 'removeContainerSlot'
		void							addContainerSlot(ContainerSlot *_container_slot);
		void 							removeContainerSlot(ContainerSlot *_container_slot, VM_ScopeBlock 	*	_scope_block);

		void 							printReferences();
		~ContainerScriptObject();
	private:

		static void 	onDettachContainerSlotNode(ListNode<ContainerSlot *> *_current_node);

		// refs where this container is referenced
		void						removeAllSlots();
		List<ContainerSlot *>	*container_slots;

	};

}
