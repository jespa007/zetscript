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
		return sofm;
	}

	ScriptObjectMemberFunction::ScriptObjectMemberFunction(){
		setup();
	}

	void ScriptObjectMemberFunction::setup(){
		idx_type=IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
		this->so_function=NULL;
		this->so_object=NULL;
	}

	ScriptObjectMemberFunction::~ScriptObjectMemberFunction(){

	}

}
