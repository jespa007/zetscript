/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STK_PROPERTY_UNDEFINED};

	zs_string stk_to_typeof_str(ZetScript *_zs, StackElement *_stk){
		StackElement *stk=_stk;
		zs_string result="unknow";
		if(STK_VALUE_IS_UNDEFINED(stk))
			result=ZS_TYPE_NAME_UNDEFINED; //"undefined";
		else if(STK_VALUE_IS_NULL(stk))
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
			result="fun@"+sf->scope_script_function->script_type_owner->script_type_name+"::"+sf->name_script_function;
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
		bool is_constant=false;
		StackElement stk=*_stk;

		if(stk.properties & STK_PROPERTY_PTR_STK){
			stk=*((StackElement *)stk.value);
		}

		if(stk.properties & STK_PROPERTY_READ_ONLY){
			is_constant=true;
			stk.properties&=~STK_PROPERTY_READ_ONLY;
		}
		if(STK_VALUE_IS_UNDEFINED(&stk)){
			result=ZS_TYPE_NAME_UNDEFINED;
		}else if(STK_VALUE_IS_NULL(&stk)){
			result=ZS_TYPE_NAME_NULL;
		}else if((stk.properties & (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR)) == (STK_PROPERTY_ZS_CHAR | STK_PROPERTY_IS_C_VAR_PTR)){
			result= (const char *)stk.value;
		}else if(STK_VALUE_IS_ZS_INT(&stk)){
			result= zs_strutils::zs_int_to_str((zs_int)stk.value,_format);
		}else if(STK_VALUE_IS_ZS_FLOAT(&stk)){
			result= zs_strutils::zs_float_to_str(*((zs_float *)&stk.value));
		}else if(STK_VALUE_IS_BOOLEAN(&stk)){
			result= stk.value?"true":"false";
		}else if(STK_VALUE_IS_FUNCTION(&stk)){
			Symbol *symbol=((Symbol *)stk.value);
			ScriptType *st=symbol->scope->getScriptTypeOwner();
			if(st==((_zs->getScriptTypeFactory())->getScriptType(IDX_TYPE_CLASS_MAIN))){
				result= zs_string("function<")+symbol->name+">";
			}else{
				zs_string s="";

				if(symbol->properties & SYMBOL_PROPERTY_STATIC){
					s=zs_string("static_function<");
				}else{
					s=zs_string("member_function<");
				}
				result=s+st->script_type_name+"::"+symbol->name+">";
			}

			/*if(STK_VALUE_IS_MEMBER_FUNCTION(stk)){
				ScriptFunction *sf=(ScriptFunction *)stk->value;
				result= zs_string("function@")+"UnknowClass(TODO)"+"::"+sf->name_script_function;
			}else{ // normal function
				result= zs_string("function@")+((Symbol *)stk->value)->name;
			}*/
		}else if(STK_VALUE_IS_TYPE(&stk)){
			result= zs_string("type")+"@"+_zs->getScriptTypeFactory()->getScriptTypeName(stk.value);
		}else{
			if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *so=(ScriptObject *)stk.value;
				if(so->getTypeName() == "DateTime"){
					result=((zs_datetime *)((ScriptObjectClass *)so)->getNativeObject())->to_string(_format);
				}else if(so->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER){
					ScriptObjectMemberFunction *somf=(ScriptObjectMemberFunction *)so;
					ScriptType *st=somf->so_object->getScriptType();
					result= zs_string("member_function<")+st->script_type_name+"::"+somf->so_function->name_script_function+">";
				}else{
					result=so->toString();
				}
			}
		}

		return result;

	}

	void			stk_assign(ZetScript *_zs,StackElement *_stk_dst, StackElement *_stk_src){
		*_stk_dst=*_stk_src;

		// update n_refs +1
		if(_stk_src->properties&STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)_stk_src->value;
			VirtualMachine *vm=_zs->getVirtualMachine();
			if(so->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && so->shared_pointer==NULL){
				//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
				ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(_zs);
				if(!vm_create_shared_pointer(vm,sc)){
					return;
				}
				sc->set(so->toString());
				so=sc;
			}

			vm_share_pointer(vm,so);

			// assign object value into stk
			_stk_dst->value=(zs_int)so;
			_stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
		}
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
		}else if(STK_VALUE_IS_SCRIPT_OBJECT(stk)){
			result.value=((ScriptObjectObject *)stk->value)->getScriptType()->idx_script_type;
		}

		return result;
	}
}
