#include "zetscript.h"

namespace zetscript{

	extern const StackElement stk_undefined={0,MSK_STK_PROPERTY_NULL};

	const char * StackElement::typeStr(){
		StackElement *stk=this;
		const char * result="null";
		if(STK_VALUE_IS_ZS_INT(stk))
			result= "int";
		else if(STK_VALUE_IS_FLOAT(stk))
			result= "float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= "bool";
		else if(STK_IS_SCRIPT_OBJECT_STRING(stk))
			result= "string";
		else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk))
			result= "vector";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "function";
		else if(STK_VALUE_IS_CLASS(stk))
			result= "class";
		else if(STK_VALUE_IS_MEMBER_ATTRIBUTE(stk))
			result= "member attribute";
		else if(STK_VALUE_IS_MEMBER_FUNCTION(stk))
			result= "member function";
		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			if(this->properties & MSK_STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->stk_value;
			}
			result=((ScriptObjectObject *)stk->stk_value)->getClassName().c_str();
		}

		return result;
	}

	std::string StackElement::toString(){
		std::string result="null";
		StackElement *stk=this;

		if(this->properties & MSK_STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->stk_value;
		}

		if(STK_VALUE_IS_ZS_INT(stk))
			result= zs_strutils::zs_int_to_str((zs_int)stk->stk_value);
		else if(STK_VALUE_IS_FLOAT(stk))
			result= zs_strutils::float_to_str(*((zs_float *)&stk->stk_value));
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= stk->stk_value?"true":"false";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= std::string("Function")+"@"+((ScriptFunction *)stk->stk_value)->symbol.name;
		else if(STK_VALUE_IS_CLASS(stk))
			result= std::string("Class")+"@"+((ScriptClass *)stk->stk_value)->symbol_class.name;
		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			result=((ScriptObjectObject *)stk->stk_value)->toString();
		}

		return result;

	}

	void StackElement::setUndefined(){
		*this=stk_undefined;
	}
}
