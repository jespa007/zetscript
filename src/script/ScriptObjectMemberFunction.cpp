/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectMemberFunction * ScriptObjectMemberFunction::newScriptObjectMemberFunction(ZetScript *zs){
		ScriptObjectMemberFunction *sofm=new ScriptObjectMemberFunction();
		sofm->init(zs);

		return sofm;

	}

	ScriptObjectMemberFunction::ScriptObjectMemberFunction(){
		setup();
	}

	void ScriptObjectMemberFunction::setup(){
		idx_script_class=IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
		this->so_function=NULL;
		this->so_object=NULL;
	}


	ScriptObjectMemberFunction::~ScriptObjectMemberFunction(){
		vm_unref_shared_script_object(zs->getVirtualMachine(),this->so_object,IDX_MAIN_CALL);
	}

}
