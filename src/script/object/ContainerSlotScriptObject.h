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

	class  ContainerSlotScriptObject:public ScriptObject{

	public:


		StackElement  								*	ptr_stk;
		//zs_list_node<ContainerSlotScriptObject *>  	*	list_node_container;

		ContainerSlotScriptObject(
			// container dst reference to store
			ContainerScriptObject	 	*	_dst_so_container_ref

			// container src reference to store
			,ContainerScriptObject	 	*	_src_so_container_ref

			// id slot dst stored
			,zs_int 						_id_slot

			// stk dst slot element
			,StackElement  				*	_ptr_stk
		);

		void add(ContainerSlotScriptObject *_so_container_slot, bool & _is_not_ciclic_reference);
		void remove(ContainerSlotScriptObject *_so_container_slot);

		ContainerScriptObject			*	getSrcContainerRef();
		ContainerScriptObject			*	getDstContainerRef();
		zs_int								getIdSlot();
		void 								removeChilds();

		// it count all references from top slot to root
		//int								countReferences(ContainerScriptObject *_so_container_ref);

		~ContainerSlotScriptObject();
	protected:
		void setup();
	private:

		ContainerSlotScriptObject 					*	parent;

		// root node will perform fast check about itself references
		ContainerSlotScriptObject 					*	root;
		//void											countReferencesRecursive(ContainerScriptObject *_so_container_ref, int & count );

		zs_list<ContainerSlotScriptObject *> 			childs;
		zs_list_node<ContainerSlotScriptObject *>  	*	list_node_child;

		void removeChilds(ContainerSlotScriptObject *	_so_container_slot);

		ContainerScriptObject						*	src_so_container_ref;
		ContainerScriptObject						*	dst_so_container_ref;
		zs_int 											id_slot;
	};

}
