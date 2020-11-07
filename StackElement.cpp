#include "zetscript.h"

namespace zetscript{

	const char * StackElement::toString(){
		StackElement *stk=this;
		const char * result="undefined";
		if(STK_VALUE_IS_ZS_INT(stk))
			result= "Integer";
		else if(STK_VALUE_IS_FLOAT(stk))
			result= "Float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= "Boolean";
		else if(STK_VALUE_IS_STRING(stk))
			result= "String";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "Function";
		else if(STK_VALUE_IS_CLASS(stk))
			result= "Class";
		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			if(this->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->var_ref;
			}
			result=((ScriptObject *)stk->var_ref)->getClassName().c_str();
		}

		return result;
	}
}
