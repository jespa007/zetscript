/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void ContainerSlotScriptObject::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_CONTAINER_SLOT;
		vm=NULL;
		parent=NULL;
		root=this;
		list_node_child=NULL;
		src_so_container_ref=NULL;
		dst_so_container_ref=NULL;
	}

	ContainerSlotScriptObject::ContainerSlotScriptObject(
			ContainerScriptObject 		*	_dst_so_container_ref
			,ContainerScriptObject 		*	_src_so_container_ref
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
	){
		setup();
		// setup object
		this->init(_dst_so_container_ref->getZetScript());

		src_so_container_ref=_src_so_container_ref;
		dst_so_container_ref=_dst_so_container_ref;
		id_slot=_id_slot;
		ptr_stk=_ptr_stk;
	}

	void ContainerSlotScriptObject::add(ContainerSlotScriptObject *_so_container_slot, bool & _is_ciclic_reference){

		// create container slot node from script object container slot
		_so_container_slot->parent=this;
		_so_container_slot->root=this->root;


		// create list node to allow deattach later
		_so_container_slot->list_node_child=new zs_list_node<ContainerSlotScriptObject *>();
		_so_container_slot->list_node_child->data=_so_container_slot;

		// insert node as child
		childs.insert(_so_container_slot->list_node_child);

		/*if(_so_container_slot->so_container_ref == root->so_container_ref){
			// insert node as reference container if is itself reference from its root

			// retrieve list container slots references
			auto _so_container_ref=_so_container_slot->getScriptObjectContainerRef();
			auto list_container_slots=_so_container_ref->getListContainerSlotsRef();

			_so_container_slot->list_node_container=new zs_list_node<ContainerSlotScriptObject *>();
			_so_container_slot->list_node_container->data=_so_container_slot;

			list_container_slots->insert(_so_container_slot->list_node_container);

			_so_container_ref->printReferences();
		}*/

		_is_ciclic_reference=_so_container_slot->src_so_container_ref == root->dst_so_container_ref;

	}

	void ContainerSlotScriptObject::remove(ContainerSlotScriptObject *_so_container_slot){
		if(_so_container_slot!=NULL){
			// dettach recursive

			_so_container_slot->removeChilds();

			// if node is not root remove child from its parent
			if(_so_container_slot->parent != NULL){
				_so_container_slot->parent->childs.remove(_so_container_slot->list_node_child);
			}

			// remove slot reference from container
			/*if(_so_container_slot->list_node_container != NULL){
				auto _so_container_ref=_so_container_slot->getScriptObjectContainerRef();
				//auto list_container_slots=_so_container_ref->getListContainerSlotsRef();
				//list_container_slots->remove(_so_container_slot->list_node_container);
			}*/

			// mark as unreferenced!
			_so_container_slot->src_so_container_ref=NULL;
			_so_container_slot->dst_so_container_ref=NULL;
			_so_container_slot->id_slot=-1;
			_so_container_slot->parent=NULL;
			_so_container_slot->root=NULL;
		}
	}

	/*void	ContainerSlotScriptObject::countReferencesRecursive(ContainerScriptObject *_so_container_ref, int & _count ){
		if(this->so_container_ref == _so_container_ref){
			_count++;
		}

		if(this->parent!=NULL){
			this->parent->countReferencesRecursive(_so_container_ref,_count);
		}
	}

	int	ContainerSlotScriptObject::countReferences(ContainerScriptObject *_so_container_ref){
		int count=0;
		countReferencesRecursive(_so_container_ref,count);
		return count;

	}*/

	void ContainerSlotScriptObject::removeChilds(){
		if(this->childs.first!=NULL){
			zs_list_node<ContainerSlotScriptObject *> *current=this->childs.first;
			do{
				// set stack element as undefined
				remove(current->data);

				// next
				current=current->next;
			}while(current != this->childs.first);

			this->childs.remove_all();
		}
	}

	ContainerScriptObject *ContainerSlotScriptObject::getSrcContainerRef(){
		return src_so_container_ref;
	}

	ContainerScriptObject *ContainerSlotScriptObject::getDstContainerRef(){
		return dst_so_container_ref;
	}


	zs_int								ContainerSlotScriptObject::getIdSlot(){
		return id_slot;
	}

	ContainerSlotScriptObject::~ContainerSlotScriptObject(){
		remove(this);
		if(list_node_child!=NULL){
			delete list_node_child;
		}

		/*if(list_node_container!=NULL){
			delete list_node_container;
		}*/
	}
}
