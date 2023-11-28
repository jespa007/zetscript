/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STACK_ELEMENT_PROPERTY_UNDEFINED};



	/*String		StackElement::toString(){
		if(STACK_ELEMENT_IS_STRING_OBJECT(this)){
			return ((StringObject *)this->value)->toString();
		}
		return stk_utils::stk_to_str(NULL,this);

	}*/

	void StackElement::setUndefined(){
		STACK_ELEMENT_SET_UNDEFINED(this);
	}

	StackElement	StackElement::typeOf(){
		StackElement *stk= this;
		StackElement result={-1,STACK_ELEMENT_PROPERTY_TYPE_ID};

		if(stk->properties & STACK_ELEMENT_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(STACK_ELEMENT_IS_UNDEFINED(stk)){
			result.value=TYPE_ID_UNDEFINED;
		}else if(STACK_ELEMENT_IS_NULL(stk)){
			result.value=TYPE_ID_NULL;
		}else if(STACK_ELEMENT_IS_INT(stk)){
			result.value=TYPE_ID_INT_C;
		}else if(STACK_ELEMENT_IS_FLOAT(stk)){
			result.value=TYPE_ID_FLOAT_C;
		}else if(STACK_ELEMENT_IS_BOOLEAN(stk)){
			result.value=TYPE_ID_BOOL_C;
		}else if(STACK_ELEMENT_IS_FUNCTION(stk)){
			result.value=TYPE_ID_FUNCTION;
		}else if(STACK_ELEMENT_IS_TYPE(stk)){
			result=*stk;
		}else if(STACK_ELEMENT_IS_CONTAINER_SLOT(stk)){
			result.value=((ContainerSlot *)stk->value)->getSrcContainerRef()->getType()->id;
		}else if(STACK_ELEMENT_IS_OBJECT(stk)){
			result.value=((DictionaryObject *)stk->value)->getType()->id;
		}

		return result;
	}

}
