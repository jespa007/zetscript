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

		ContainerSlotScriptObject * newScriptObjectContainerSlot(
				ContainerScriptObject 		*	_so_container_ref
				,zs_int 						_id_slot
				,StackElement  				*	_ptr_stk
		);
		//
		// Helpers
		//
		//----------------------------------------------

		StackElement  								*	ptr_stk;
		zs_list_node<ContainerSlotScriptObject *>  	*	list_node_container;

		ContainerSlotScriptObject(
			ContainerScriptObject	 	*	_so_container_ref
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
		);

		void add(ContainerSlotScriptObject *_so_container_slot);
		void remove(ContainerSlotScriptObject *_so_container_slot);

		ContainerScriptObject			*	getScriptObjectContainerRef();
		zs_int								getIdSlot();
		void 								removeChilds();
		int									countReferences(ContainerScriptObject *_so_container_ref);

		~ContainerSlotScriptObject();
	protected:
		void setup();
	private:

		ContainerSlotScriptObject 					*	parent;

		// root node will perform fast check about itself references
		ContainerSlotScriptObject 					*	root;

		zs_list<ContainerSlotScriptObject *> 			childs;
		zs_list_node<ContainerSlotScriptObject *>  	*	list_node_child;

		void removeChilds(ContainerSlotScriptObject *	_so_container_slot);

		ContainerScriptObject						*	so_container_ref;
		zs_int 											id_slot;
	};

}
