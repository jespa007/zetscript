/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectMemberFunction * ScriptObjectMemberFunction::newScriptObjectMemberFunction(ZetScript *zs, ScriptObject *_so_object, ScriptFunction *_so_function){
		ScriptObjectMemberFunction *sofm=new ScriptObjectMemberFunction();
		sofm->init(zs);
		sofm->so_object=_so_object;
		sofm->so_function=_so_function;
		// shares current object because script function member needs to have the object alive...
		if(!vm_share_pointer(zs->getVirtualMachine(),_so_object)){
			return NULL;
		}
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
		vm_unref_shared_script_object(zs->getVirtualMachine(),this->so_object,IDX_CALL_STACK_MAIN);
	}

}
