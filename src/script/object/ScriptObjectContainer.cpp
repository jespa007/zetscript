/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ScriptObjectContainer::ScriptObjectContainer(){
		root=new ScriptObjectContainerSlot(
				NULL,0,NULL
		);
	}

	ScriptObjectContainerSlot 				*ScriptObjectContainer::getScriptObjectContainerSlotRoot(){
		return root;
	}

	ScriptObjectContainer::~ScriptObjectContainer(){
		root->removeChilds();
		delete root;
	}
}
