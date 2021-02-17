/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVarRef * ScriptObjectVarRef::newScriptObjectVarRef(ZetScript *zs){
		ScriptObjectVarRef *so=new ScriptObjectVarRef();
		return so;
	}

	ScriptObjectVarRef::ScriptObjectVarRef(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF;
	}



}
