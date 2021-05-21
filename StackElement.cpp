#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STK_PROPERTY_NULL};

	const char * StackElement::typeOf(){
		StackElement *stk=this;
		const char * result="unknown";
		if(STK_VALUE_IS_NULL(stk))
			result= "null";
		else if(STK_VALUE_IS_ZS_INT(stk))
			result= "int";
		else if(STK_VALUE_IS_ZS_FLOAT(stk))
			result= "float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= "bool";
		else if(STK_IS_SCRIPT_OBJECT_OBJECT(stk))
			result= "ScriptObjectObject";
		else if(STK_IS_SCRIPT_OBJECT_STRING(stk))
			result= "ScriptObjectString";
		else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk))
			result= "ScriptObjectVector";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "ScriptFunction";
		else if(STK_VALUE_IS_CLASS(stk))
			result= "ScriptClass";
		else if(STK_VALUE_IS_MEMBER_ATTRIBUTE(stk))
			result= "MemberAttribute";
		else if(STK_VALUE_IS_MEMBER_FUNCTION(stk))
			result= "MemberFunction";
		else{
			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result=((ScriptObjectObject *)stk->value)->getClassName().c_str();
			}
		}

		return result;
	}

	std::string StackElement::toString(){
		std::string result="null";
		StackElement *stk=this;

		if(this->properties & STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if((stk->properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_VAR_C)) == (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_VAR_C))
			result= (const char *)stk->value;
		else if(STK_VALUE_IS_ZS_INT(stk))
			result= zs_strutils::zs_int_to_str((zs_int)stk->value);
		else if(STK_VALUE_IS_ZS_FLOAT(stk))
			result= zs_strutils::float_to_str(*((zs_float *)&stk->value));
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= stk->value?"true":"false";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= std::string("ScriptFunction")+"@"+((ScriptFunction *)stk->value)->symbol.name;
		else if(STK_VALUE_IS_CLASS(stk))
			result= std::string("ScriptClass")+"@"+((ScriptClass *)stk->value)->symbol_class.name;
		else{
			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result=((ScriptObject *)stk->value)->toString();
			}
		}

		return result;

	}

	void StackElement::setUndefined(){
		*this=k_stk_undefined;
	}
}
