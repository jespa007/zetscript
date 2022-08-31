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

	class  ScriptObjectContainerSlot:public ScriptObject{

	public:

		ScriptObjectContainerSlot * newScriptObjectContainerSlot(
				ScriptObjectContainer 		*	_so_container_ref
				,zs_int 						_id_slot
				,StackElement  				*	_ptr_stk
		);
		//
		// Helpers
		//
		//----------------------------------------------

		StackElement  							*	ptr_stk;

		ScriptObjectContainerSlot(
			ScriptObjectContainer	 	*	_so_container_ref
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
		);

		void add(ScriptObjectContainerSlot *_so_container_slot);
		void remove(ScriptObjectContainerSlot *_so_container_slot);

		ScriptObjectContainer			*	getScriptObjectContainerRef();
		zs_int								getIdSlot();
		void 								removeChilds();

		~ScriptObjectContainerSlot();
	protected:
		void setup();
	private:

		ScriptObjectContainerSlot 					*	parent;
		zs_list<ScriptObjectContainerSlot *> 			childs;
		zs_list_node<ScriptObjectContainerSlot *>  	*	list_node_child;

		void removeChilds(ScriptObjectContainerSlot *_so_container_slot);

		ScriptObjectContainer						*	so_container_ref;
		zs_int 											id_slot;
	};

}
