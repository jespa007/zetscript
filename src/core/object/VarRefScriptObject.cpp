/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	VarRefObject * VarRefObject::newVarRefObject(ScriptEngine *_script_engine,StackElement _stk){//,int idx_call){
		VarRefObject *so=new VarRefObject(_script_engine);
		so->setStackElement(_stk);
		return so;
	}

	VarRefObject::VarRefObject(
			zetscript::ScriptEngine *_script_engine
	):ScriptObject(_script_engine){
		script_type_id = SCRIPT_TYPE_ID_VAR_REF_SCRIPT_OBJECT;
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
			return ((ScriptObject *)stk_var_ref.value)->toString();
		}
		return script_engine->stackElementValueToString(&stk_var_ref);
	}

}
