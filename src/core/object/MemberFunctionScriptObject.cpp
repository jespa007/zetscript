/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	MemberFunctionScriptObject * MemberFunctionScriptObject::newObjectMemberFunction(ScriptEngine *_script_engine, ScriptObject *_so_object_ref, ScriptFunction *_sf_ref){

		return new MemberFunctionScriptObject(_script_engine,_so_object_ref,_sf_ref);
	}

	MemberFunctionScriptObject::MemberFunctionScriptObject(
			zetscript::ScriptEngine *_script_engine
			, ScriptObject *_so_ref
			, ScriptFunction *_sf_ref
	):RefScriptObject(_script_engine, _so_ref){
		script_type_id=SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT;
		this->sf_ref=_sf_ref;
	}



	MemberFunctionScriptObject::~MemberFunctionScriptObject(){
	}

}
