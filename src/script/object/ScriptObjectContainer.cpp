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

	ScriptObjectContainerSlot 				*ScriptObjectContainer::getScriptObjectContainerSlotRoot(){
		return root;
	}

	ScriptObjectContainer::~ScriptObjectContainer(){
		if(root != NULL){
			root->removeChilds();
			delete root;
		}
	}
}
