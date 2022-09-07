/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	VarRefScriptObject * VarRefScriptObject::newVarRefScriptObject(ZetScript *_zs,StackElement _stk){//,int idx_call){
		VarRefScriptObject *so=new VarRefScriptObject(_zs);
		so->setStackElement(_stk);
		return so;
	}

	VarRefScriptObject::VarRefScriptObject(
			ZetScript *_zs
	):ScriptObject(_zs){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_VAR_REF;
		stk_var_ref.setUndefined();
		//idx_call=ZS_IDX_UNDEFINED;

	}

	/*void VarRefScriptObject::setIdxCall(int _idx_call){
		this->idx_call = _idx_call;
	}

	int VarRefScriptObject::getIdxCall(){
		return idx_call;
	}*/

	StackElement *VarRefScriptObject::getStackElementPtr(){
		return &stk_var_ref;
	}
	void VarRefScriptObject::setStackElement(StackElement stk){
		stk_var_ref=stk;
	}

	zs_string VarRefScriptObject::toString(){

		return stk_to_str(zs,&stk_var_ref);
	}

}
