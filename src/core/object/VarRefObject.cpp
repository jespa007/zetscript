/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	VarRefObject * VarRefObject::newVarRefObject(ZetScript *_zs,StackElement _stk){//,int idx_call){
		VarRefObject *so=new VarRefObject(_zs);
		so->setStackElement(_stk);
		return so;
	}

	VarRefObject::VarRefObject(
			ZetScript *_zs
	):Object(_zs){
		type_id = TYPE_ID_OBJECT_VAR_REF;
		stk_var_ref.setUndefined();
		//idx_call=ZS_UNDEFINED_IDX;

	}

	/*void VarRefObject::setIdxCall(int _idx_call){
		this->idx_call = _idx_call;
	}

	int VarRefObject::getIdxCall(){
		return idx_call;
	}*/

	StackElement *VarRefObject::getStackElementPtr(){
		return &stk_var_ref;
	}
	void VarRefObject::setStackElement(StackElement stk){
		stk_var_ref=stk;
	}

	String VarRefObject::toString(){
		if(stk_var_ref.properties & STACK_ELEMENT_PROPERTY_OBJECT){
			return ((Object *)stk_var_ref.value)->toString();
		}
		return zs->stackElementToString(&stk_var_ref);
	}

}
