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
			result= "Object";
		else if(STK_IS_SCRIPT_OBJECT_STRING(stk))
			result= "String";
		else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk))
			result= "Vector";
		else if(STK_IS_SCRIPT_OBJECT_OBJECT_ITERATOR(stk))
			result= "ObjectIterator";
		else if(STK_IS_SCRIPT_OBJECT_STRING_ITERATOR(stk))
			result= "StringIterator";
		else if(STK_IS_SCRIPT_OBJECT_VECTOR_ITERATOR(stk))
			result= "VectorIterator";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "Function";
		else if(STK_VALUE_IS_CLASS(stk))
			result= "Class";
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

	std::string StackElement::toString(const std::string & _format ){
		std::string result="null";
		StackElement *stk=this;

		if(this->properties & STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if((stk->properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_VAR_C)) == (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_VAR_C)){
			result= (const char *)stk->value;
		}else if(STK_VALUE_IS_ZS_INT(stk)){
			result= zs_strutils::zs_int_to_str((zs_int)stk->value,_format);
		}else if(STK_VALUE_IS_ZS_FLOAT(stk)){
			result= zs_strutils::zs_float_to_str(*((zs_float *)&stk->value));
		}else if(STK_VALUE_IS_BOOLEAN(stk)){
			result= stk->value?"true":"false";
		}else if(STK_VALUE_IS_FUNCTION(stk)){
			if(STK_VALUE_IS_MEMBER_FUNCTION(stk)){
				ScriptFunction *sf=(ScriptFunction *)stk->value;
				result= std::string("FunctionMember")+"@"+"UnknowClass(TODO)"+"::"+sf->symbol.name;
			}else{ // normal function
				result= std::string("Function")+"@"+((ScriptFunction *)stk->value)->symbol.name;
			}
		}else if(STK_VALUE_IS_CLASS(stk)){
			result= std::string("Class")+"@"+((ScriptClass *)stk->value)->class_name;
		}else{
			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *so=(ScriptObject *)stk->value;
				if(so->getClassName() == "DateTime"){
					result=((zs_datetime *)((ScriptObjectClass *)so)->getNativeObject())->to_string(_format);
				}else{
					result=so->toString();
				}
			}
		}

		return result;

	}

	zs_int	StackElement::toInt(){
		if(this->properties & STK_PROPERTY_ZS_INT){
			return (zs_int)this->value;
		}

		THROW_RUNTIME_ERROR("StackElement not is not int");

		return 0;
	}

	zs_float	StackElement::toFloat(){
		zs_float aux;
		if(this->properties & STK_PROPERTY_ZS_FLOAT){
			ZS_FLOAT_COPY(&aux,&this->value);
			return aux;
		}

		THROW_RUNTIME_ERROR("StackElement not is not float");

		return 0;
	}

	void StackElement::setUndefined(){
		*this=k_stk_undefined;
	}
}
