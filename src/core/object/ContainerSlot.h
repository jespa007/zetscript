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

	class ContainerObject;
	class  ContainerSlot{

	public:


		static ContainerSlot *newContainerSlot(
				// container dst reference to store
				ContainerObject	 	*	_dst_container_ref
				// id slot dst stored
				,zs_int 						_id_slot

				// stk element where slot is stored
				,StackElement  				*	_ptr_stk
		);

		static void deleteContainerSlot(ContainerSlot * _container, bool _remove_container_slot=true);

		void								setSrcContainerRef(ContainerObject	 	*	_src_container_ref);
		ContainerObject			*	getSrcContainerRef();
		ContainerObject			*	getDstContainerRef();

		StackElement 					*	getPtrStackElement();

		ListNode<ContainerSlot *>  	*	getContainerSlotNode();

		zs_int								getIdSlot();
		bool								isReferenced();

		//bool								isCyclicReference();

		// it count all references from top slot to root
		//int								countReferences(ContainerObject *_so_container_ref);


	private:


		ListNode<ContainerSlot *>  		container_slot_node;

		StackElement  					*	ptr_stk;

		// this is the value container ref
		ContainerObject			*	src_container_ref;

		// this is the slot container ref
		ContainerObject			*	dst_container_ref;

		zs_int 								id_slot;

		ContainerSlot(
			// container dst reference to store
			ContainerObject	 	*	_dst_container_ref
			// id slot dst stored
			,zs_int 						_id_slot

			// stk element where slot is stored
			,StackElement  				*	_ptr_stk
		);

		~ContainerSlot();
	};

}
