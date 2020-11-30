#include "zetscript.h"

namespace zetscript{

	const char * StackElement::typeStr(){
		StackElement *stk=this;
		const char * result="undefined";
		if(STK_VALUE_IS_ZS_INT(stk))
			result= "int";
		else if(STK_VALUE_IS_FLOAT(stk))
			result= "float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= "bool";
		else if(STK_VALUE_IS_STRING(stk))
			result= "string";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "function";
		else if(STK_VALUE_IS_CLASS(stk))
			result= "class";
		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			if(this->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->var_ref;
			}
			result=((ScriptObject *)stk->var_ref)->getClassName().c_str();
		}

		return result;
	}

	std::string StackElement::toString(){
		std::string result="undefined";
		StackElement *stk=this;

		if(STK_VALUE_IS_ZS_INT(stk))
			result= zs_strutils::int_to_str((zs_int)stk->stk_value);
		else if(STK_VALUE_IS_FLOAT(stk))
			result= zs_strutils::float_to_str(*((float *)&stk->stk_value));
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= stk->stk_value?"true":"false";
		else if(STK_VALUE_IS_STRING(stk))
			result= (const char *)stk->stk_value;
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= std::string("Function")+"@"+((ScriptFunction *)stk->var_ref)->symbol.name;
		else if(STK_VALUE_IS_CLASS(stk))
			result= std::string("Class")+"@"+((ScriptClass *)stk->var_ref)->symbol_class.name;
		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			if(this->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->var_ref;
			}

			result=((ScriptObject *)stk->var_ref)->toString();
		}

		return result;

	}

	void StackElement::setUndefined(){
		this->stk_value=0;
		this->var_ref=0;
		this->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
	}
}
