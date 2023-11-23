/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerScriptObject::ContainerScriptObject(
			ZetScript *_zs
			,ScriptTypeId _ref_script_type_id
	):ScriptObject(_zs, _ref_script_type_id){
		container_slots=new zs_list<ContainerSlot *>;
	}

	zs_list<ContainerSlot *>	*ContainerScriptObject::getListContainerSlotsRef(){
		return container_slots;
	}

	void ContainerScriptObject::addContainerSlot(ContainerSlot *_container_slot){
		_container_slot->setSrcContainerRef(this);
	}


	void ContainerScriptObject::removeContainerSlot(ContainerSlot *_container_slot, VM_ScopeBlock 	*	_scope_block){
		if(_container_slot!=NULL){


			auto container_slot_node=_container_slot->getContainerSlotNode();

			container_slots->remove(container_slot_node);

			if(container_slots->first==NULL){
				// there's no any cycli reference so container instance can be removed from cyclic references map
				vm_remove_container_instance_cyclic_references_map(vm,_container_slot->getSrcContainerRef());
			}

			vm_unref_shared_script_object(vm,_container_slot->getSrcContainerRef(),_scope_block );

		}
	}

	void ContainerScriptObject::onDettachContainerSlotNode(zs_list_node<ContainerSlot *> *_current_node){
		_current_node->data->setSrcContainerRef(NULL);
	}

	void ContainerScriptObject::removeAllSlots(){

		//if(container_slots->first!=NULL){
		container_slots->dettachAllNodes(onDettachContainerSlotNode);
			//vm_remove_container_instance_cyclic_references_map(vm,this);
		//}
	}


	void ContainerScriptObject::printReferences(){

		/*int count = 0;

		auto current = so_container_slots.first;
		if(current!=NULL){
			do{
				count+=current->data->countReferences(this);
				current=current->next;

			}while(current!=so_container_slots.first);
		}

		printf("Ciclic references %i Shared references %i\n",root->countReferences(this),this->shared_pointer->data.n_shares);*/
	}

	ContainerScriptObject::~ContainerScriptObject(){
		removeAllSlots();
		delete container_slots;
	}
}
