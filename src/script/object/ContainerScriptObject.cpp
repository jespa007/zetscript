/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerScriptObject::ContainerScriptObject(){
		container_slots=new zs_list<ContainerSlot *>;
	}

	zs_list<ContainerSlot *>	*ContainerScriptObject::getListContainerSlotsRef(){
		return container_slots;
	}

	void ContainerScriptObject::addSlot(ContainerSlot *_container_slot){
		_container_slot->setSrcContainerRef(this);
	}


	void ContainerScriptObject::removeSlot(ContainerSlot *_container_slot, VM_ScopeBlock 	*	_scope_block){
		if(_container_slot!=NULL){


			auto container_slot_node=_container_slot->getContainerSlotNode();

			container_slots->remove(container_slot_node);

			// if not ciclic, unref
			if(_container_slot->isCyclicReference()==false){
				vm_unref_shared_script_object(vm,_container_slot->getSrcContainerRef(),_scope_block );
			}

			delete _container_slot;

		}
	}



	void ContainerScriptObject::printReferences(){

		int count=0;

		/*auto current = so_container_slots.first;
		if(current!=NULL){
			do{
				count+=current->data->countReferences(this);
				current=current->next;

			}while(current!=so_container_slots.first);
		}

		printf("Ciclic references %i Shared references %i\n",root->countReferences(this),this->shared_pointer->data.n_shares);*/
	}

	ContainerScriptObject::~ContainerScriptObject(){
		delete container_slots;
		printf("TODO DELETE CONTAINER SLOTS");

	}
}
