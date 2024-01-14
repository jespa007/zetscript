/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STACK_ELEMENT_PROPERTY_UNDEFINED};



	/*String		StackElement::toString(){
		if(STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(this)){
			return ((StringScriptObject *)this->value)->toString();
		}
		return stk_utils::stk_to_str(NULL,this);

	}*/

	void StackElement::setUndefined(){
		STACK_ELEMENT_SET_UNDEFINED(this);
	}

	StackElement	StackElement::typeOf(){
		StackElement *stk= this;
		StackElement result={-1,STACK_ELEMENT_PROPERTY_SCRIPT_TYPE_ID};

		if(stk->properties & STACK_ELEMENT_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(STACK_ELEMENT_IS_UNDEFINED(stk)){
			result.value=SCRIPT_TYPE_ID_UNDEFINED;
		}else if(STACK_ELEMENT_IS_NULL(stk)){
			result.value=SCRIPT_TYPE_ID_NULL;
		}else if(STACK_ELEMENT_IS_INT(stk)){
			result.value=SCRIPT_TYPE_ID_INT;
		}else if(STACK_ELEMENT_IS_FLOAT(stk)){
			result.value=SCRIPT_TYPE_ID_FLOAT;
		}else if(STACK_ELEMENT_IS_BOOLEAN(stk)){
			result.value=SCRIPT_TYPE_ID_BOOL;
		}else if(STACK_ELEMENT_IS_FUNCTION(stk)){
			result.value=SCRIPT_TYPE_ID_SCRIPT_FUNCTION;
		}else if(STACK_ELEMENT_IS_TYPE(stk)){
			result=*stk;
		}else if(STACK_ELEMENT_IS_CONTAINER_SLOT(stk)){
			result.value=((ContainerSlot *)stk->value)->getSrcContainerRef()->getScriptType()->id;
		}else if(stk->properties & (STACK_ELEMENT_PROPERTY_OBJECT | STACK_ELEMENT_PROPERTY_OBJECT_IN_CONTAINER)){
			result.value=((ScriptObject *)stk->value)->getScriptType()->id;
		}

		return result;
	}

}
