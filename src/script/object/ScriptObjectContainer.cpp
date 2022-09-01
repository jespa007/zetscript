/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ScriptObjectContainer::ScriptObjectContainer(){
		root=NULL;
	}

	void ScriptObjectContainer::initContainer(){
		root=new ScriptObjectContainerSlot(
				this,0,NULL
		);
	}

	void ScriptObjectContainer::init(ZetScript *_zs){
		ScriptObject::init(_zs);
		initContainer();
	}

	zs_list<ScriptObjectContainerSlot *>	*ScriptObjectContainer::getListContainerSlotsRef(){
		return &so_container_slots;
	}

	ScriptObjectContainerSlot 				*ScriptObjectContainer::getScriptObjectContainerSlotRoot(){
		return root;
	}

	void ScriptObjectContainer::countReferences(){
		if(so_container_slots.first!=NULL){
			auto current=so_container_slots.first;
			do{
				printf("%i refs %i shared\n",current->data->countReferences(this),this->shared_pointer->data.n_shares);
				current=current->next;
			}while(current!=so_container_slots.first);

		}
	}

	ScriptObjectContainer::~ScriptObjectContainer(){

		if(root != NULL){
			root->removeChilds();
			delete root;
		}
	}
}
