/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectContainerSlot * ScriptObjectContainerSlot::newScriptObjectContainerSlot(
			ScriptObjectContainer 		*	_so_container_ref
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
	){
		ScriptObjectContainerSlot *si= new ScriptObjectContainerSlot(
			_so_container_ref
			,_id_slot
			,_ptr_stk
		);
		return si;
	}

	void ScriptObjectContainerSlot::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_CONTAINER_SLOT;
		so_container_ref = NULL;
		vm=NULL;
	}

	ScriptObjectContainerSlot::ScriptObjectContainerSlot(
			ScriptObjectContainer 		*	_so_container_ref
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
	){
		setup();
		// setup object
		this->init(_so_container_ref->getZetScript());

		so_container_ref=_so_container_ref;
		id_slot=_id_slot;
		ptr_stk=_ptr_stk;
	}

	void ScriptObjectContainerSlot::add(ScriptObjectContainerSlot *_so_container_slot){
		// create container slot node from script object container slot
		_so_container_slot->parent=this;

		// create list node to allow deattach later
		_so_container_slot->list_node_child=new zs_list_node<ScriptObjectContainerSlot *>();

		_so_container_slot->list_node_child->data=_so_container_slot;

		// insert in the list of childs
		childs.insert(_so_container_slot->list_node_child);
	}

	void ScriptObjectContainerSlot::removeChilds(){
		if(this->childs.first!=NULL){
			zs_list_node<ScriptObjectContainerSlot *> *current=this->childs.first;
			do{
				// set stack element as undefined
				remove(current->data);

				// next
				current=current->next;
			}while(current->next != this->childs.first);

			this->childs.remove_all();
		}
	}

	void ScriptObjectContainerSlot::remove(ScriptObjectContainerSlot *_so_container_slot){
		// dettach recursive

		_so_container_slot->removeChilds();

		// if node is not root remove child from its parent
		if(_so_container_slot->parent != NULL){
			_so_container_slot->parent->childs.remove(_so_container_slot->list_node_child);
		}

		_so_container_slot->ptr_stk->value=0;
		_so_container_slot->ptr_stk->properties=0;

		vm_unref_shared_script_object(vm, _so_container_slot, NULL);
	}

	ScriptObjectContainer *ScriptObjectContainerSlot::getScriptObjectContainerRef(){
		return so_container_ref;
	}

	zs_int								ScriptObjectContainerSlot::getIdSlot(){
		return id_slot;
	}

	ScriptObjectContainerSlot::~ScriptObjectContainerSlot(){
		removeChilds();
	}
}
