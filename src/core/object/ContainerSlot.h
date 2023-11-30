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

	class ContainerScriptObject;
	class  ContainerSlot{

	public:


		static ContainerSlot *newContainerSlot(
				// container dst reference to store
				ContainerScriptObject	 	*	_dst_container_ref
				// id slot dst stored
				,zs_int 						_id_slot

				// stk element where slot is stored
				,StackElement  				*	_ptr_stk
		);

		static void deleteContainerSlot(ContainerSlot * _container, bool _remove_container_slot=true);

		void								setSrcContainerRef(ContainerScriptObject	 	*	_src_container_ref);
		ContainerScriptObject			*	getSrcContainerRef();
		ContainerScriptObject			*	getDstContainerRef();

		StackElement 					*	getPtrStackElement();

		ListNode<ContainerSlot *>  	*	getContainerSlotNode();

		zs_int								getIdSlot();
		bool								isReferenced();

		//bool								isCyclicReference();

		// it count all references from top slot to root
		//int								countReferences(ContainerScriptObject *_so_container_ref);


	private:


		ListNode<ContainerSlot *>  		container_slot_node;

		StackElement  					*	ptr_stk;

		// this is the value container ref
		ContainerScriptObject			*	src_container_ref;

		// this is the slot container ref
		ContainerScriptObject			*	dst_container_ref;

		zs_int 								id_slot;

		ContainerSlot(
			// container dst reference to store
			ContainerScriptObject	 	*	_dst_container_ref
			// id slot dst stored
			,zs_int 						_id_slot

			// stk element where slot is stored
			,StackElement  				*	_ptr_stk
		);

		~ContainerSlot();
	};

}
