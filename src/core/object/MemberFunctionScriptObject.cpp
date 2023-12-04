/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	MemberFunctionObject * MemberFunctionObject::newObjectMemberFunction(ZetScript *_zs, ScriptObject *_so_object_ref, ScriptFunction *_sf_ref){

		return new MemberFunctionObject(_zs,_so_object_ref,_sf_ref);
	}

	MemberFunctionObject::MemberFunctionObject(
			ZetScript *_zs
			, ScriptObject *_so_ref
			, ScriptFunction *_sf_ref
	):RefScriptObject(_zs, _so_ref){
		script_type_id=SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT;
		this->sf_ref=_sf_ref;
	}



	MemberFunctionObject::~MemberFunctionObject(){
	}

}
