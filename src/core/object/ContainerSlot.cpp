/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ContainerSlot *ContainerSlot::newContainerSlot(
			// container dst reference to store
			ContainerScriptObject	 	*	_dst_container_ref
			// id slot dst stored
			,zs_int 						_id_slot

			// stk element where slot is stored
			,StackElement  				*	_ptr_stk
	)	{
		return new ContainerSlot(
				// container dst reference to store
				_dst_container_ref
				// id slot dst stored
				,_id_slot

				// stk element where slot is stored
				,_ptr_stk
		);
	}

	void ContainerSlot::deleteContainerSlot(ContainerSlot * _container_slot, bool _remove_container_slot){
		ContainerScriptObject *cso= _container_slot->getSrcContainerRef();
		if(cso!=NULL && _remove_container_slot==true){
			cso->removeContainerSlot(_container_slot, NULL);
		}
		delete _container_slot;
	}

	ContainerSlot::ContainerSlot(
			ContainerScriptObject 		*	_dst_container_ref
			,zs_int 						_id_slot
			,StackElement  				*	_ptr_stk
	){

		src_container_ref=NULL;
		dst_container_ref=_dst_container_ref;
		id_slot=_id_slot;
		ptr_stk=_ptr_stk;

		//ptr_stk->value=(zs_int)this;
		//ptr_stk->properties=STACK_ELEMENT_PROPERTY_CONTAINER_SLOT;
	}

	void ContainerSlot::setSrcContainerRef(ContainerScriptObject *_src_container_ref){

		if(src_container_ref!=_src_container_ref){
			//detach current
			if(src_container_ref!=NULL){
				src_container_ref->removeContainerSlot(this,NULL);
			}
		}

		if(_src_container_ref != NULL){
			auto list_container_slots=_src_container_ref->getListContainerSlotsRef();

			//container_slot_node=new ListNode<ContainerSlot *>();
			container_slot_node.data=this;

			list_container_slots->insert(&container_slot_node);

			_src_container_ref->printReferences();
		}

		src_container_ref=_src_container_ref ;

	}


	/*void	ContainerSlot::countReferencesRecursive(ContainerScriptObject *_so_container_ref, int & _count ){
		if(this->so_container_ref == _so_container_ref){
			_count++;
		}

		if(this->parent!=NULL){
			this->parent->countReferencesRecursive(_so_container_ref,_count);
		}
	}

	int	ContainerSlot::countReferences(ContainerScriptObject *_so_container_ref){
		int count=0;
		countReferencesRecursive(_so_container_ref,count);
		return count;

	}*/
	/*bool								ContainerSlot::isCyclicReference(){
		return src_container_ref == dst_container_ref;
	}*/

	ListNode<ContainerSlot *>  	*	ContainerSlot::getContainerSlotNode(){
		return &container_slot_node;
	}


	ContainerScriptObject *ContainerSlot::getSrcContainerRef(){
		return src_container_ref;
	}

	ContainerScriptObject *ContainerSlot::getDstContainerRef(){
		return dst_container_ref;
	}

	StackElement 					*	ContainerSlot::getPtrStackElement(){
		return ptr_stk;
	}


	zs_int								ContainerSlot::getIdSlot(){
		return id_slot;
	}

	bool ContainerSlot::isReferenced(){
		return src_container_ref!=NULL;
	}

	ContainerSlot::~ContainerSlot(){
		// set undefined the stk slot
	/*	if(src_container_ref != NULL){
			*ptr_stk=k_stk_undefined;
			src_container_ref->removeContainerSlot(this, NULL);
		}*/

	}
}
