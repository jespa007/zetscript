/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectWeakPointer * ScriptObjectWeakPointer::newScriptObjectWeakPointer(ScriptObject *_so){
		ScriptObjectWeakPointer *si= new ScriptObjectWeakPointer(_so);

		return si;
	}


	void ScriptObjectWeakPointer::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_WEAK_POINTER;
		so_reference = NULL;
		vm=NULL;
	}

	ScriptObjectWeakPointer::ScriptObjectWeakPointer(){
		setup();
	}

	ScriptObjectWeakPointer::ScriptObjectWeakPointer(ScriptObject *_so_reference){
		setup();
		// setup object
		this->init(_so_reference->getZetScript());
		so_reference=_so_reference;
		so_reference->refObject((ScriptObject **)&this->so_reference);
	}

	ScriptObjectWeakPointer::~ScriptObjectWeakPointer(){
		if(so_reference != NULL){
			so_reference->derefObject(&this->so_reference);
		}
	}
}
