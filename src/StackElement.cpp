#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STK_PROPERTY_NULL};

	zs_string stk_typeof_str(ZetScript *_zs, StackElement *_stk){
		StackElement *stk=_stk;
		zs_string result="unknow";

		if(STK_VALUE_IS_NULL(stk))
			result="null";
		else if(STK_VALUE_IS_ZS_INT(stk))
			result="zs_int";
		else if(STK_VALUE_IS_ZS_FLOAT(stk))
			result="zs_float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result="bool";
		else if(STK_IS_SCRIPT_OBJECT_OBJECT(stk))
			result="Object";
		else if(STK_IS_SCRIPT_OBJECT_STRING(stk))
			result="String";
		else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk))
			result="Vector";
		else if(STK_IS_SCRIPT_OBJECT_OBJECT_ITERATOR(stk))
			result="ObjectIterator";
		else if(STK_IS_SCRIPT_OBJECT_STRING_ITERATOR(stk))
			result="StringIterator";
		else if(STK_IS_SCRIPT_OBJECT_VECTOR_ITERATOR(stk))
			result="VectorIterator";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result=zs_string("fun@")+((ScriptFunction *)stk->value)->symbol.name;
		else if(STK_VALUE_IS_TYPE_INFO(stk)) // is a class
			result=zs_string("type@")+_zs->getScriptClassFactory()->getScriptClassName(stk->value);
		else if(STK_VALUE_IS_MEMBER_ATTRIBUTE(stk)){
			MemberAttribute *ma=(MemberAttribute *)stk->value;
			result="_mattr@"+ma->script_class->class_name+"::"+ma->attribute_name;
		}else if(STK_VALUE_IS_MEMBER_FUNCTION(stk)){
			ScriptFunction *sf=((ScriptFunction *)stk->value);
			result="_mfun@"+sf->symbol.scope->script_class->class_name+"@"+sf->symbol.name;
		}else{
			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result=((ScriptObjectObject *)stk->value)->getClassName().c_str();
			}
		}

		return result;
	}

	zs_string stk_to_string(ZetScript *_zs, StackElement *_stk, const zs_string & _format ){
		zs_string result="null";
		StackElement *stk=_stk;

		if(stk->properties & STK_PROPERTY_PTR_STK){
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
				result= zs_string("FunctionMember")+"@"+"UnknowClass(TODO)"+"::"+sf->symbol.name;
			}else{ // normal function
				result= zs_string("Function")+"@"+((ScriptFunction *)stk->value)->symbol.name;
			}
		}else if(STK_VALUE_IS_TYPE_INFO(stk)){
			result= zs_string("type")+"@"+_zs->getScriptClassFactory()->getScriptClassName(stk->value);
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

	StackElement	StackElement::typeOf(){
		StackElement *stk= this;
		StackElement result={0,0};

		if(stk->properties & STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(STK_VALUE_IS_ZS_INT(stk)){
			result.value=IDX_BUILTIN_TYPE_ZS_INT_C;
		}else if(STK_VALUE_IS_ZS_FLOAT(stk)){
			result.value=IDX_BUILTIN_TYPE_ZS_FLOAT_C;
		}else if(STK_VALUE_IS_BOOLEAN(stk)){
			result.value=IDX_BUILTIN_TYPE_BOOL_C;
		}else if(STK_PROPERTY_SCRIPT_OBJECT(stk)){
			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result.value=((ScriptObjectObject *)stk->value)->getScriptClass()->idx_class;
			}
		}

		return result;
	}
}
