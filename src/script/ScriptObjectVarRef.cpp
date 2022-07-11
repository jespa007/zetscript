/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVarRef * ScriptObjectVarRef::newScriptObjectVarRef(ZetScript *zs,StackElement stk){//,int idx_call){
		ScriptObjectVarRef *so=new ScriptObjectVarRef();
		so->setStackElement(stk);
		so->init(zs);
		//so->setIdxCall(idx_call);
		return so;
	}

	ScriptObjectVarRef::ScriptObjectVarRef(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_VAR_REF;
		stk_var_ref.setUndefined();
		//idx_call=ZS_IDX_UNDEFINED;

	}

	/*void ScriptObjectVarRef::setIdxCall(int _idx_call){
		this->idx_call = _idx_call;
	}

	int ScriptObjectVarRef::getIdxCall(){
		return idx_call;
	}*/

	StackElement *ScriptObjectVarRef::getStackElementPtr(){
		return &stk_var_ref;
	}
	void ScriptObjectVarRef::setStackElement(StackElement stk){
		stk_var_ref=stk;
	}

	zs_string ScriptObjectVarRef::toString(const zs_string _format){
		return stk_to_str(zs,&stk_var_ref,_format);
	}


}
