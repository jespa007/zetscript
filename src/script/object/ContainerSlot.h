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

		ContainerSlot(
			// container dst reference to store
			ContainerScriptObject	 	*	_dst_container_ref
			// id slot dst stored
			,zs_int 						_id_slot

			// stk element where slot is stored
			,StackElement  				*	_ptr_stk
		);

		void								setSrcContainerRef(ContainerScriptObject	 	*	_src_container_ref);
		ContainerScriptObject			*	getSrcContainerRef();
		ContainerScriptObject			*	getDstContainerRef();

		StackElement 					*	getPtrStackElement();

		zs_list_node<ContainerSlot *>  	*	getContainerSlotNode();

		zs_int								getIdSlot();

		//bool								isCyclicReference();

		// it count all references from top slot to root
		//int								countReferences(ContainerScriptObject *_so_container_ref);

		~ContainerSlot();
	private:

		zs_list_node<ContainerSlot *>  		container_slot_node;

		StackElement  					*	ptr_stk;
		ContainerScriptObject			*	src_container_ref;
		ContainerScriptObject			*	dst_container_ref;
		zs_int 								id_slot;
	};

}
