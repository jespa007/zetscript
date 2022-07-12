/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectMemberFunction * ScriptObjectMemberFunction::newScriptObjectMemberFunction(ZetScript *zs, ScriptObject *_so_object, ScriptFunction *_so_function){
		ScriptObjectMemberFunction *sofm=new ScriptObjectMemberFunction();
		sofm->init(zs);

		if(_so_object->idx_script_type<IDX_TYPE_SCRIPT_OBJECT_STRING){
			THROW_RUNTIME_ERRORF("Internal:Expected script variable type >= IDX_TYPE_SCRIPT_OBJECT_STRING");
		}

		sofm->so_object=_so_object;
		sofm->so_function=_so_function;
		sofm->so_object->refObject(&sofm->so_object);
		return sofm;
	}

	ScriptObjectMemberFunction::ScriptObjectMemberFunction(){
		setup();
	}

	void ScriptObjectMemberFunction::setup(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER;
		this->so_function=NULL;
		this->so_object=NULL;
	}

	ScriptObjectMemberFunction::~ScriptObjectMemberFunction(){
		if(so_object != NULL){
			so_object->derefObject(&this->so_object);
		}
	}

}
