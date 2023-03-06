/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STK_PROPERTY_UNDEFINED};



	/*zs_string		StackElement::toString(){
		if(STK_IS_SCRIPT_OBJECT_STRING(this)){
			return ((StringScriptObject *)this->value)->toString();
		}
		return stk_utils::stk_to_str(NULL,this);

	}*/

	void StackElement::setUndefined(){
		STK_SET_UNDEFINED(this);
	}

	StackElement	StackElement::typeOf(){
		StackElement *stk= this;
		StackElement result={-1,STK_PROPERTY_TYPE};

		if(stk->properties & STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(STK_VALUE_IS_UNDEFINED(stk)){
			result.value=IDX_TYPE_UNDEFINED;
		}else if(STK_VALUE_IS_NULL(stk)){
			result.value=IDX_TYPE_NULL;
		}else if(STK_VALUE_IS_ZS_INT(stk)){
			result.value=IDX_TYPE_ZS_INT_C;
		}else if(STK_VALUE_IS_ZS_FLOAT(stk)){
			result.value=IDX_TYPE_ZS_FLOAT_C;
		}else if(STK_VALUE_IS_BOOLEAN(stk)){
			result.value=IDX_TYPE_BOOL_C;
		}else if(STK_VALUE_IS_FUNCTION(stk)){
			result.value=IDX_TYPE_FUNCTION;
		}else if(STK_VALUE_IS_TYPE(stk)){
			result=*stk;
		}else if(STK_VALUE_IS_CONTAINER_SLOT(stk)){
			result.value=((ContainerSlot *)stk->value)->getSrcContainerRef()->getScriptType()->idx_script_type;;
		}else if(STK_VALUE_IS_SCRIPT_OBJECT(stk)){
			result.value=((ObjectScriptObject *)stk->value)->getScriptType()->idx_script_type;
		}

		return result;
	}

}
