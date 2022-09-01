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

		// retrieve list container slots references
		auto _so_container_ref=_so_container_slot->getScriptObjectContainerRef();
		auto list_container_slots=_so_container_ref->getListContainerSlotsRef();

		// create container slot node from script object container slot
		_so_container_slot->parent=this;

		// create list node to allow deattach later
		_so_container_slot->list_node_child=new zs_list_node<ScriptObjectContainerSlot *>();
		_so_container_slot->list_node_child->data=_so_container_slot;

		// insert node as child
		childs.insert(_so_container_slot->list_node_child);

		// insert node as reference container
		_so_container_slot->list_node_container=new zs_list_node<ScriptObjectContainerSlot *>();
		_so_container_slot->list_node_container->data=_so_container_slot;

		list_container_slots->insert(_so_container_slot->list_node_container);
	}

	void ScriptObjectContainerSlot::remove(ScriptObjectContainerSlot *_so_container_slot){
		if(_so_container_slot!=NULL){
			// dettach recursive

			_so_container_slot->removeChilds();

			// if node is not root remove child from its parent
			if(_so_container_slot->parent != NULL){
				_so_container_slot->parent->childs.remove(_so_container_slot->list_node_child);
			}

			// remove slot reference from container
			if(_so_container_slot->list_node_container != NULL){
				auto _so_container_ref=_so_container_slot->getScriptObjectContainerRef();
				auto list_container_slots=_so_container_ref->getListContainerSlotsRef();
				list_container_slots->remove(_so_container_slot->list_node_container);
			}

			// mark as unreferenced!
			_so_container_slot->so_container_ref=NULL;
			_so_container_slot->id_slot=-1;
		}
	}

	int	ScriptObjectContainerSlot::countReferences(ScriptObjectContainer *_so_container_ref){
		int count=0;

		if(this->childs.first!=NULL){
			zs_list_node<ScriptObjectContainerSlot *> *current=this->childs.first;
			do{
				// set stack element as undefined
				if(current->data->so_container_ref==_so_container_ref){
					count++;
				}

				// next
				current=current->next;
			}while(current->next != this->childs.first);
		}

		if(this->parent!=NULL){
			count+=this->parent->countReferences(_so_container_ref);
		}


		return count;
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

	ScriptObjectContainer *ScriptObjectContainerSlot::getScriptObjectContainerRef(){
		return so_container_ref;
	}

	zs_int								ScriptObjectContainerSlot::getIdSlot(){
		return id_slot;
	}

	ScriptObjectContainerSlot::~ScriptObjectContainerSlot(){
		remove(this);
		if(list_node_child!=NULL){
			delete list_node_child;
		}

		if(list_node_container!=NULL){
			delete list_node_container;
		}
	}
}
