/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_null={0,STK_PROPERTY_NULL};

	zs_string stk_to_typeof_str(ZetScript *_zs, StackElement *_stk){
		StackElement *stk=_stk;
		zs_string result="unknow";

		if(STK_VALUE_IS_NULL(stk))
			result=ZS_TYPE_NAME_NULL; //"null";
		else if(STK_VALUE_IS_ZS_INT(stk))
			result=ZS_TYPE_NAME_INT;
		else if(STK_VALUE_IS_ZS_FLOAT(stk))
			result=ZS_TYPE_NAME_FLOAT;
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result=ZS_TYPE_NAME_BOOL;
		else if(STK_IS_SCRIPT_OBJECT_STRING(stk))
			result=ZS_TYPE_NAME_OBJECT_STRING;
		else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk))
			result=ZS_TYPE_NAME_OBJECT_VECTOR;
		else if(STK_IS_SCRIPT_OBJECT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_OBJECT;
		else if(STK_IS_SCRIPT_OBJECT_ITERATOR_STRING(stk))
			result=ZS_TYPE_NAME_OBJECT_ITERATOR_STRING;
		else if(STK_IS_SCRIPT_OBJECT_ITERATOR_VECTOR(stk))
			result=ZS_TYPE_NAME_OBJECT_ITERATOR_VECTOR;
		else if(STK_IS_SCRIPT_OBJECT_ITERATOR_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT;
		else if(STK_VALUE_IS_FUNCTION(stk))
			result=zs_string("fun@")+((ScriptFunction *)stk->value)->name_script_function;
		else if(STK_VALUE_IS_TYPE(stk)) // is a class
			result=zs_string("type@")+_zs->getScriptTypeFactory()->getScriptTypeName(stk->value);
		else if(STK_VALUE_IS_MEMBER_PROPERTY(stk)){
			MemberProperty *ma=(MemberProperty *)stk->value;
			result="attr@"+ma->script_type->script_type_name+"::"+ma->property_name;
		}else if(STK_VALUE_IS_MEMBER_FUNCTION(stk)){
			Symbol *symbol=((Symbol *)stk->value);
			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
			result="fun@"+sf->scope_script_function->script_type->script_type_name+"::"+sf->name_script_function;
		}else{
			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result=((ScriptObjectObject *)stk->value)->getTypeName().c_str();
			}
		}

		return result;
	}

	zs_string stk_to_str(ZetScript *_zs, StackElement *_stk, const zs_string & _format ){
		zs_string result="unknown";
		StackElement *stk=_stk;

		if(stk->properties & STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(STK_VALUE_IS_NULL(stk)){
			result=ZS_TYPE_NAME_NULL;
		}else if((stk->properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR)) == (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR)){
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
				result= zs_string("FunctionMember")+"@"+"UnknowClass(TODO)"+"::"+sf->name_script_function;
			}else{ // normal function
				result= zs_string("Function")+"@"+((ScriptFunction *)stk->value)->name_script_function;
			}
		}else if(STK_VALUE_IS_TYPE(stk)){
			result= zs_string("type")+"@"+_zs->getScriptTypeFactory()->getScriptTypeName(stk->value);
		}else{
			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *so=(ScriptObject *)stk->value;
				if(so->getTypeName() == "DateTime"){
					result=((zs_datetime *)((ScriptObjectClass *)so)->getNativeObject())->to_string(_format);
				}else{
					result=so->toString();
				}
			}
		}

		return result;

	}

	zs_int	StackElement::toInt(){
		if((this->properties & STK_PROPERTY_ZS_INT)==0){
			THROW_RUNTIME_ERRORF("StackElement not is not int");
		}

		return (zs_int)this->value;
	}

	zs_float	StackElement::toFloat(){

		if((this->properties & STK_PROPERTY_ZS_FLOAT)==0){
			THROW_RUNTIME_ERRORF("StackElement not is not float");
		}

		return *((zs_float *)&this->value);



	}

	void StackElement::setUndefined(){
		STK_SET_NULL(this);;
	}

	StackElement	StackElement::typeOf(){
		StackElement *stk= this;
		StackElement result={-1,STK_PROPERTY_TYPE};

		if(stk->properties & STK_PROPERTY_PTR_STK){
			stk=(StackElement *)stk->value;
		}

		if(STK_VALUE_IS_NULL(stk)){
			result.value=IDX_TYPE_NULL;
		}else if(STK_VALUE_IS_ZS_INT(stk)){
			result.value=IDX_TYPE_ZS_INT_C;
		}else if(STK_VALUE_IS_ZS_FLOAT(stk)){
			result.value=IDX_TYPE_ZS_FLOAT_C;
		}else if(STK_VALUE_IS_BOOLEAN(stk)){
			result.value=IDX_TYPE_BOOL_C;
		}else if(STK_VALUE_IS_SCRIPT_OBJECT(stk)){
			result.value=((ScriptObjectObject *)stk->value)->getScriptType()->idx_script_type;
		}

		return result;
	}
}
