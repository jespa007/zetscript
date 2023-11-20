/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,ZS_STK_PROPERTY_UNDEFINED};



	/*zs_string		StackElement::toString(){
		if(ZS_STK_IS_STRING_SCRIPT_OBJECT(this)){
			return ((StringScriptObject *)this->value)->toString();
		}
		return stk_utils::stk_to_str(NULL,this);

	}*/

	void StackElement::setUndefined(){
		ZS_STK_SET_UNDEFINED(this);
	}

	StackElement	StackElement::typeOf(){
		StackElement *stk= this;
		StackElement result={-1,ZS_STK_PROPERTY_INDEX_CLASS_TYPE};

		if(stk->properties & ZS_STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(ZS_STK_VALUE_IS_UNDEFINED(stk)){
			result.value=ScriptTypeId::SCRIPT_TYPE_ID_UNDEFINED;
		}else if(ZS_STK_VALUE_IS_NULL(stk)){
			result.value=ScriptTypeId::SCRIPT_TYPE_ID_NULL;
		}else if(ZS_STK_VALUE_IS_INT(stk)){
			result.value=ScriptTypeId::SCRIPT_TYPE_ID_INT_C;
		}else if(ZS_STK_VALUE_IS_FLOAT(stk)){
			result.value=ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_C;
		}else if(ZS_STK_VALUE_IS_BOOLEAN(stk)){
			result.value=ScriptTypeId::SCRIPT_TYPE_ID_BOOL_C;
		}else if(ZS_STK_VALUE_IS_FUNCTION(stk)){
			result.value=ScriptTypeId::SCRIPT_TYPE_ID_FUNCTION;
		}else if(ZS_STK_VALUE_IS_TYPE(stk)){
			result=*stk;
		}else if(ZS_STK_VALUE_IS_CONTAINER_SLOT(stk)){
			result.value=((ContainerSlot *)stk->value)->getSrcContainerRef()->getScriptType()->id;;
		}else if(ZS_STK_VALUE_IS_SCRIPT_OBJECT(stk)){
			result.value=((ObjectScriptObject *)stk->value)->getScriptType()->id;
		}

		return result;
	}

}
