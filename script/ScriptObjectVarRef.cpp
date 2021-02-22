/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVarRef * ScriptObjectVarRef::newScriptObjectVarRef(ZetScript *zs,StackElement stk){
		ScriptObjectVarRef *so=new ScriptObjectVarRef();
		so->setStackElement(stk);
		so->setZetScript(zs);
		return so;
	}

	ScriptObjectVarRef::ScriptObjectVarRef(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF;
		stk_var_ref.setUndefined();

	}
	StackElement ScriptObjectVarRef::getStackElement(){
		return stk_var_ref;
	}
	void ScriptObjectVarRef::setStackElement(StackElement stk){
		stk_var_ref=stk;
	}


}
