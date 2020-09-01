/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptVarVector::ScriptVarVector(ZetScript *_zs):ScriptVar(_zs){
		this->init(SCRIPT_CLASS_VECTOR(this), (void *)this);
	}


	void ScriptVarVector::push(StackElement  * _stk){

		StackElement *new_stk=newSlot();//(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;

		// update n_refs +1
		if(_stk->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
			virtual_machine->sharePointer(((ScriptVar *)(_stk->var_ref))->ptr_shared_pointer_node);
		}
	}

	StackElement * ScriptVarVector::pop(){
		return popUserProperty();
	}


	/*int ScriptVarVector::size(){
		return  stk_properties->count;
	}*/



	/*ScriptVarVector::~ScriptVarVector(){
		destroy();
	}*/
}
