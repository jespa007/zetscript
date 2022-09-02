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
				this,this,0,NULL
		);
	}

	void ContainerScriptObject::init(ZetScript *_zs){
		ScriptObject::init(_zs);
		initContainer();
	}

	/*zs_list<ContainerSlotScriptObject *>	*ContainerScriptObject::getListContainerSlotsRef(){
		return &so_container_slots;
	}*/

	ContainerSlotScriptObject 				*ContainerScriptObject::getScriptObjectContainerSlotRoot(){
		return root;
	}

	/*void ContainerScriptObject::printReferences(){

		int count=0;

		auto current = so_container_slots.first;
		if(current!=NULL){
			do{
				count+=current->data->countReferences(this);
				current=current->next;

			}while(current!=so_container_slots.first);
		}

		printf("Ciclic references %i Shared references %i\n",root->countReferences(this),this->shared_pointer->data.n_shares);
	}*/

	ContainerScriptObject::~ContainerScriptObject(){

		if(root != NULL){
			root->removeChilds();
			delete root;
		}
	}
}
