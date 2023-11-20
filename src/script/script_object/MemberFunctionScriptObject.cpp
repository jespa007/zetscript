/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	MemberFunctionScriptObject * MemberFunctionScriptObject::newScriptObjectMemberFunction(ZetScript *_zs, ScriptObject *_so_object_ref, ScriptFunction *_sf_ref){

		return new MemberFunctionScriptObject(_zs,_so_object_ref,_sf_ref);
	}

	MemberFunctionScriptObject::MemberFunctionScriptObject(
			ZetScript *_zs
			, ScriptObject *_so_ref
			, ScriptFunction *_sf_ref
	):RefObjectScriptObject(_zs, _so_ref){
		script_type_id=ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_FUNCTION_MEMBER;
		this->sf_ref=_sf_ref;
	}



	MemberFunctionScriptObject::~MemberFunctionScriptObject(){
	}

}
