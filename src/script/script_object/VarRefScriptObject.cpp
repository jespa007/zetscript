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
		script_type_id=ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_VAR_REF;
		stk_var_ref.setUndefined();
		//idx_call=ZS_UNDEFINED_IDX;

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
		if(stk_var_ref.properties & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){
			return ((ScriptObject *)stk_var_ref.value)->toString();
		}
		return zs->stackElementToString(&stk_var_ref);
	}

}
