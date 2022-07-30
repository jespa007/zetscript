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
		ref_object = NULL;
		vm=NULL;
	}

	ScriptObjectWeakPointer::ScriptObjectWeakPointer(){
		setup();
	}

	ScriptObjectWeakPointer::ScriptObjectWeakPointer(ScriptObject *_so_reference){
		setup();
		// setup object
		this->init(_so_reference->getZetScript());
		ref_object=new RefObject(_so_reference,this);
	}

	void ScriptObjectWeakPointer::deRefObject(){
		printf("TODO ScriptObjectWeakPointer::deRefObject\n");
	}

	ScriptObjectWeakPointer::~ScriptObjectWeakPointer(){
		delete ref_object;
	}
}
