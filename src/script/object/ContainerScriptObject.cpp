/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerScriptObject::ContainerScriptObject(){
		root=NULL;
	}

	void ContainerScriptObject::initContainer(){
		root=new ContainerSlotScriptObject(
				this,0,NULL
		);
	}

	void ContainerScriptObject::init(ZetScript *_zs){
		ScriptObject::init(_zs);
		initContainer();
	}

	zs_list<ContainerSlotScriptObject *>	*ContainerScriptObject::getListContainerSlotsRef(){
		return &so_container_slots;
	}

	ContainerSlotScriptObject 				*ContainerScriptObject::getScriptObjectContainerSlotRoot(){
		return root;
	}

	void ContainerScriptObject::countReferences(){
		if(so_container_slots.first!=NULL){
			auto current=so_container_slots.first;
			do{
				printf("%i refs %i shared\n",current->data->countReferences(this),this->shared_pointer->data.n_shares);
				current=current->next;
			}while(current!=so_container_slots.first);

		}
	}

	ContainerScriptObject::~ContainerScriptObject(){

		if(root != NULL){
			root->removeChilds();
			delete root;
		}
	}
}
