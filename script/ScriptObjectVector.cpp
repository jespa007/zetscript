/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void    			ScriptObjectVector::pushSf(ScriptObjectVector *sv,StackElement  * stk){
		return sv->push(stk);
	}

	StackElement *  	ScriptObjectVector::popSf(ScriptObjectVector *sv){
		return sv->pop();
	}

	ScriptObjectVector::ScriptObjectVector(ZetScript *_zs):ScriptObject(_zs){
		this->init(SCRIPT_CLASS_VECTOR(this), (void *)this);
	}

	void ScriptObjectVector::push(StackElement  * _stk){

		StackElement *new_stk=newSlot();
		*new_stk=*_stk;

		// update n_refs +1
		if(_stk->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
			virtual_machine->sharePointer(((ScriptObject *)(_stk->var_ref))->shared_pointer);
		}
	}

	StackElement * ScriptObjectVector::pop(){
		return popUserProperty();
	}

}
