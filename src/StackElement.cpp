/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	extern const StackElement k_stk_undefined={0,STK_PROPERTY_UNDEFINED};

	zs_string stk_to_typeof_str(ZetScript *_zs, StackElement *_stk){
		// PRE: _str_out should allocated a minimum of 100 bytes
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
		else if(STK_VALUE_IS_CONTAINER_SLOT(stk))
			result=((ContainerSlot *)stk->value)->getSrcContainerRef()->getTypeName();
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
			result=zs_string("fun@")+((ScriptFunction *)(((Symbol *)stk->value)->ref_ptr))->name_script_function;
		else if(STK_VALUE_IS_TYPE(stk)) // is a type
			result=zs_string("type@")+_zs->getScriptTypeFactory()->getScriptTypeName(stk->value);
		else if(STK_VALUE_IS_MEMBER_PROPERTY(stk)){
			StackMemberProperty *ma=(StackMemberProperty *)stk->value;
			result="prop@"+zs_string(ma->member_property->script_type->str_script_type)+"::"+zs_string(ma->member_property->property_name);
		}else if(STK_VALUE_IS_MEMBER_FUNCTION(stk)){
			Symbol *symbol=((Symbol *)stk->value);
			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
			result="fun@"+zs_string(sf->scope_script_function->script_type_owner->str_script_type)+"::"+sf->name_script_function;
		}else{
			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result=((ScriptObject *)stk->value)->getTypeName();
			}
		}

		return result;
	}

	const char *stk_to_typeof_str(char *_str_out,ZetScript *_zs, StackElement *_stk){
		auto str=stk_to_str(_zs,_stk);

		strcpy(_str_out,str.c_str());

		return _str_out;
	}

	zs_string stk_to_str(ZetScript *_zs, StackElement *_stk, const zs_string  & _format ){
		// PRE: _str_out should allocated a minimum of 100 bytes
		zs_string result="unknown";
		StackElement stk=*_stk;

		if(stk.properties & STK_PROPERTY_PTR_STK){
			stk=*((StackElement *)stk.value);
		}

		if(stk.properties & STK_PROPERTY_CONTAINER_SLOT){
			stk.value=(zs_int)(((ContainerSlot *)stk.value)->getSrcContainerRef());
			stk.properties=STK_PROPERTY_SCRIPT_OBJECT;
		}

		if(stk.properties & STK_PROPERTY_READ_ONLY){
			stk.properties&=~STK_PROPERTY_READ_ONLY;
		}

		if(STK_VALUE_IS_UNDEFINED(&stk)){
			result=ZS_TYPE_NAME_UNDEFINED;
		}else if(STK_VALUE_IS_NULL(&stk)){
			result=ZS_TYPE_NAME_NULL;
		}else if(stk.properties & STK_PROPERTY_ZS_CHAR_PTR){
			result= (const char *)stk.value;
		}else if(STK_VALUE_IS_ZS_INT(&stk)){
			result= zs_strutils::zs_int_to_str((zs_int)stk.value,_format);
		}else if(STK_VALUE_IS_ZS_FLOAT(&stk)){
			result= zs_strutils::zs_float_to_str(ZS_READ_INTPTR_FLOAT(stk.value));
		}else if(STK_VALUE_IS_BOOLEAN(&stk)){
			result= stk.value?"true":"false";
		}else if(STK_VALUE_IS_MEMBER_PROPERTY(&stk)){
			StackMemberProperty *ma=(StackMemberProperty *)stk.value;
			result="prop@"+zs_string(ma->member_property->script_type->str_script_type)+"::"+zs_string(ma->member_property->property_name);
		}else if(STK_VALUE_IS_FUNCTION(&stk)){
			Symbol *symbol=((Symbol *)stk.value);
			ScriptType *st=symbol->scope->getScriptTypeOwner();
			if(st->idx_script_type==IDX_TYPE_CLASS_MAIN){
				result= zs_string("fun@")+symbol->name;
			}else{
				zs_string s="";

				if(symbol->properties & SYMBOL_PROPERTY_STATIC){
					s=zs_string("static_fun@");
				}else{
					s=zs_string("member_function<");
				}
				result=s+st->str_script_type+"::"+symbol->name;
			}
		}else if(STK_VALUE_IS_TYPE(&stk)){
			if(_zs!=NULL){
				result= zs_string("type")+"@"+_zs->getScriptTypeFactory()->getScriptTypeName(stk.value);
			}else{
				result= zs_strutils::format("type@%i",(int)stk.value);
			}
		}else{
			if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *so=(ScriptObject *)stk.value;
				if(so->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER){
					MemberFunctionScriptObject *somf=(MemberFunctionScriptObject *)so;
					ScriptType *st=somf->getRefObject()->getScriptType();
					result= zs_string("member_function<")+st->str_script_type+"::"+somf->sf_ref->name_script_function+">";
				}else{
					// PROTECTION: do not give you big strings, instead they will retrieve from particular parts of code like JsonSerialize or Console::*)
					result="Object::"+zs_string(so->getTypeName());
				}
			}
		}

		return result;
	}

	const char		*stk_to_str(char *_str_out, ZetScript *_zs, StackElement *_stk,const zs_string & _format){
		auto str=stk_to_str(_zs,_stk,_format);

		strcpy(_str_out,str.c_str());

		return _str_out;
	}

	void			stk_assign(ZetScript *_zs,StackElement *_stk_dst, StackElement *_stk_src){
		*_stk_dst=*_stk_src;

		if(_stk_dst->properties & STK_PROPERTY_PTR_STK){
			*_stk_dst=*((StackElement *)_stk_dst->value);
		}

		// erase read only property if set
		_stk_dst->properties&=~(STK_PROPERTY_READ_ONLY);


		// update n_refs +1
		if(_stk_dst->properties&STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)_stk_dst->value;
			VirtualMachine *vm=_zs->getVirtualMachine();
			if(so->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && so->shared_pointer==NULL){
				//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
				StringScriptObject *sc=ZS_NEW_STRING_OBJECT(_zs);
				if(!vm_create_shared_script_object(
						vm
						,sc
				)){
					return;
				}
				sc->set(so->toString().c_str());
				so=sc;
			}

			vm_share_script_object(vm,so);

			// assign object value into stk
			_stk_dst->value=(zs_int)so;
			_stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
		}
	}

	bool stk_to(ZetScript *_zs, StackElement * _stack_element, int _idx_builtin_type, zs_int *_ptr_var, zs_string  & _error){
		zs_int val_ret=0;

		ScriptObject *script_object=NULL;

		// save return type ...
		if(_stack_element->properties & STK_PROPERTY_PTR_STK){
			_stack_element=((StackElement *)_stack_element->value);
		}

		if(_idx_builtin_type == IDX_TYPE_STACK_ELEMENT){
			*_ptr_var=(zs_int)_stack_element;
			return true;
		}

		switch(GET_STK_PROPERTY_TYPES(_stack_element->properties)){
		case STK_PROPERTY_NULL:
			break;
		case STK_PROPERTY_BOOL:
			if(_idx_builtin_type == IDX_TYPE_BOOL_C){// *ScriptType::k_str_bool_type){
				val_ret=(zs_int)(_stack_element->value);
			}else if(_idx_builtin_type == IDX_TYPE_BOOL_PTR_C){//*ScriptType::k_str_bool_type_ptr){
				val_ret=(zs_int)(&_stack_element->value);
			}else{
				_error="cannot convert '"
					+zs_rtti::demangle(k_str_bool_type_ptr)
					+"' to '"
					+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type)).c_str()
					+"'";
				return false;
			}

			break;
		case STK_PROPERTY_ZS_FLOAT:
			switch(_idx_builtin_type){
			case IDX_TYPE_ZS_FLOAT_C:
				ZS_FLOAT_COPY(&val_ret,&_stack_element->value);
				break;
			case IDX_TYPE_ZS_FLOAT_PTR_C:
				val_ret=(zs_int)(&_stack_element->value);
				break;
			case IDX_TYPE_ZS_INT_C:
				{
					zs_int *aux_dst = ((zs_int *)&val_ret);
					zs_float *aux_src=(zs_float *)&_stack_element->value;
					*aux_dst=(zs_int)(*aux_src);
				}
				break;
			default:
				_error="cannot convert '"
					+zs_rtti::demangle(k_str_zs_float_type_ptr)
					+"' to '"
					+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type))
					+"'";
				return false;
			}
			break;
		case STK_PROPERTY_ZS_INT:
			switch(_idx_builtin_type){
			case IDX_TYPE_ZS_INT_C:
				val_ret=(zs_int)(_stack_element->value);
				break;
			case IDX_TYPE_ZS_INT_PTR_C:
				val_ret=(zs_int)(&_stack_element->value);
				break;
			default:
				_error="cannot convert 'int' to '"
				+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type))
				+"'";
				return false;
			}
			break;
		// it expects the ScriptFunction directly
		case STK_PROPERTY_FUNCTION:
			val_ret=((Symbol *)_stack_element->value)->ref_ptr;
			break;
		default: // script variable by default ...

			if(_stack_element->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_CONTAINER_SLOT)){

				if(_stack_element->properties & STK_PROPERTY_CONTAINER_SLOT){
					script_object=((ContainerSlot *)(_stack_element->value))->getSrcContainerRef();
				}else{
					script_object=(ScriptObject *)_stack_element->value;
				}
				ScriptType *c_class=NULL;
				val_ret=(zs_int)script_object;

				if(script_object==NULL){
					_error="Variable is not defined";
					return false;
				}

				if(_idx_builtin_type!=script_object->idx_script_type){

					if(script_object->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING){ // string
						if(_stack_element->value == 0){ // if not created try to create a tmp scriptvar it will be removed...
							_error="internal error var_ref is NULL";
							return false;
						}

						if(_idx_builtin_type == IDX_TYPE_ZS_STRING_PTR_C){
							val_ret=(zs_int)(((StringScriptObject *)script_object)->str_ptr);
						}else if (_idx_builtin_type == IDX_TYPE_CONST_CHAR_PTR_C){
							val_ret=(zs_int)(((zs_string *)(((StringScriptObject *)script_object)))->c_str());
						}else{
							_error="cannot convert '"
									+zs_rtti::demangle((k_str_zs_string_type_ptr))
									+"' to '"
									+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type))
									+"'";
							return false;
						}
					}else if(script_object->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){
						ClassScriptObject *script_object_class = (ClassScriptObject *)script_object;
						c_class=script_object_class->getNativeScriptClass(); // get the pointer directly ...

						if(c_class != NULL){
							if((val_ret=c_class->extendsFrom(
									_idx_builtin_type
								))==0
							){//c_class->idx_script_type==idx_builtin_type){
								_error="cannot convert '"
										+zs_rtti::demangle(c_class->str_script_type_ptr.c_str())
										+"' to '"
										+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type))
										+"'";
								return false;
							}
							val_ret=(zs_int)script_object_class->getNativeObject();
						}else{ // Is script class, set directly
							val_ret=_stack_element->value;
							/*error = " Error calling function, no C-object parameter! Unexpected script variable (";
							error.append(zs_rtti::demangle(script_object->getTypeName().c_str()));
							error.append(")");
							return false;*/
						}
					}else{ // cannot convert...
						_error="cannot convert '"
							+zs_rtti::demangle(script_object->getTypeName())
							+"' to '"
							+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type))
							+"'";
						return false;
					}
				}else{ // get native object...
					val_ret=(zs_int)script_object->getNativeObject();
				}
			}else{
				_error=zs_strutils::format("Cannot know how to convert type '%s' from '%s'"
					,zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(_zs->getScriptTypeFactory(),_idx_builtin_type)).c_str()
					,stk_to_typeof_str(_zs,_stack_element).c_str()
				);
				return false;
			}
			break;
		}

		*_ptr_var = val_ret;

		return true;
	}

	// Helpers...
	 StackElement to_stk(ZetScript *_zs, zs_int ptr_var, short idx_builtin_type_var){
		//zs_int ptr_var = (zs_int)input_var;
			zs_string s_return_value;
			StackElement stk_result=k_stk_undefined;
			StringScriptObject *so=NULL;

			//int idx_builtin_type=getIdxScriptTypeFromTypeNamePtr(typeid(T).name());
			// save return type ...
			switch(idx_builtin_type_var){
			 case IDX_TYPE_VOID_C:
				break;
			 case IDX_TYPE_ZS_INT_PTR_C:
				 if(ptr_var==0) return stk_result;
				 stk_result={(*((zs_int *)ptr_var)),STK_PROPERTY_ZS_INT};
				 break;
			 case IDX_TYPE_ZS_INT_C:
				 stk_result={(((zs_int)ptr_var)),STK_PROPERTY_ZS_INT};
				 break;
			 case IDX_TYPE_ZS_FLOAT_C:
				 stk_result.properties=STK_PROPERTY_ZS_FLOAT;//{};
				 ZS_FLOAT_COPY(&stk_result.value,&ptr_var);
				 break;
			 case IDX_TYPE_ZS_FLOAT_PTR_C:
				 if(ptr_var==0) return stk_result;
				 stk_result.properties=STK_PROPERTY_ZS_FLOAT;//{};
				 ZS_FLOAT_COPY(&stk_result.value,&(*(zs_float *)ptr_var));
				 break;
			 case IDX_TYPE_BOOL_PTR_C:
				 if(ptr_var==0) return stk_result;
				 stk_result={(*((bool *)ptr_var)),STK_PROPERTY_BOOL};
				 break;
			 case IDX_TYPE_BOOL_C:
				 stk_result={(((bool)ptr_var)),STK_PROPERTY_BOOL};
				 break;
			 case IDX_TYPE_CONST_CHAR_PTR_C:
			 case IDX_TYPE_ZS_STRING_PTR_C:
			 case IDX_TYPE_ZS_STRING_C:


				 so=ZS_NEW_STRING_OBJECT(_zs);
				 if(ptr_var!=0) { // not null
					 if(idx_builtin_type_var==IDX_TYPE_ZS_STRING_PTR_C){
						// assing zs_string reference
						so->str_ptr=(zs_string *)ptr_var;
					 }else if(idx_builtin_type_var==IDX_TYPE_ZS_STRING_C){ // zs_string passed as pointer
						 so->set(*((zs_string *)ptr_var));
					 }else{ // const char
						 so->set((const char *)ptr_var);
					 }
				 }

				 stk_result={(intptr_t)so,STK_PROPERTY_SCRIPT_OBJECT};
				 break;
			 case IDX_TYPE_STACK_ELEMENT:
				 if(ptr_var==0) return stk_result;
				 stk_result=*((StackElement *)ptr_var);
				 break;
			 case IDX_TYPE_SCRIPT_OBJECT_VECTOR:
			 case IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR:
			 case IDX_TYPE_SCRIPT_OBJECT_OBJECT:
			 case IDX_TYPE_SCRIPT_OBJECT_CLASS:
			 case IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT:
			 case IDX_TYPE_SCRIPT_OBJECT_STRING:
			 case IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING:
				 if(ptr_var==0) return stk_result;
				stk_result = {
					 (intptr_t)ptr_var
					 ,STK_PROPERTY_SCRIPT_OBJECT
				 };

				 break;
			 default:
				 //if(ptr_var==0) return stk_result;
				 if(ptr_var == 0){ // null value
					 stk_result={0,STK_PROPERTY_NULL};
				 }else{
					 stk_result = {
						 (intptr_t)_zs->getScriptTypeFactory()->instanceScriptObjectByTypeIdx(idx_builtin_type_var,(void *)ptr_var)
						 ,STK_PROPERTY_SCRIPT_OBJECT
					 };
				 }
				 break;
			}
			return stk_result;
	}

	zs_int	StackElement::toInt(){
		if((this->properties & (STK_PROPERTY_ZS_INT|STK_PROPERTY_ZS_FLOAT))==0){
			ZS_THROW_RUNTIME_ERRORF("StackElement not is not int");
		}

		if((this->properties & STK_PROPERTY_ZS_INT)!=0){
			return (zs_int)this->value;
		}

		return ZS_READ_INTPTR_FLOAT(this->value);
	}

	zs_float	StackElement::toFloat(){
		if((this->properties & STK_PROPERTY_ZS_FLOAT)==0){
			ZS_THROW_RUNTIME_ERRORF("StackElement not is not float");
		}

		return ZS_READ_INTPTR_FLOAT(this->value);
	}

	bool	StackElement::toBool(){
		if((this->properties & STK_PROPERTY_BOOL)==0){
			ZS_THROW_RUNTIME_ERRORF("StackElement not is not bool");
		}

		return this->value;
	}

	zs_string		StackElement::toString(){
		if(STK_IS_SCRIPT_OBJECT_STRING(this)){
			return ((StringScriptObject *)this->value)->toString();
		}
		return stk_to_str(NULL,this);

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
