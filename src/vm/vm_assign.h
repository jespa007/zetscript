/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define ASSIGN_CONTAINER_ELEMENT \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_src)){ \
	stk_src=(StackElement *)((STK_GET_STK_VAR_REF(stk_src)->value)); \
} \
void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update value on save) */ \
zs_int *stk_src_ref=&stk_src->value; \
if(stk_src->properties & STK_PROPERTY_IS_VAR_C){ /* src is C pointer */ \
	stk_src_ref=(zs_int *)((stk_src)->value); \
}\
unsigned short type_var=stk_src->properties;\
if(type_var == STK_PROPERTY_NULL){\
	stk_dst->properties=STK_PROPERTY_NULL;\
}else if(type_var & STK_PROPERTY_ZS_INT){\
	stk_dst->properties=STK_PROPERTY_ZS_INT;\
	*((zs_int *)stk_dst_ref)=*((zs_int *)stk_src_ref);\
	if(copy_aux!=NULL)(*(zs_int *)copy_aux)=*((zs_int *)stk_src_ref);\
}else if(type_var & STK_PROPERTY_ZS_FLOAT){\
	stk_dst->properties=STK_PROPERTY_ZS_FLOAT;\
	*((zs_float *)stk_dst_ref)=*((zs_float *)stk_src_ref);\
	if(copy_aux!=NULL)(*(zs_float *)copy_aux)=*((zs_float *)stk_src_ref);\
}else if(type_var & STK_PROPERTY_BOOL){\
	stk_dst->properties=STK_PROPERTY_BOOL;\
	*((bool *)stk_dst_ref)=*((bool *)stk_src_ref);\
	if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)stk_src_ref);\
}else if(type_var  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){\
	*stk_dst=*stk_src;\
}else if(type_var & STK_PROPERTY_SCRIPT_OBJECT){\
	if(STK_IS_SCRIPT_OBJECT_STRING(stk_src)){\
		ScriptObjectString *str_object=NULL;\
		stk_dst->value=(zs_int)(str_object= ZS_NEW_OBJECT_STRING(data->zs));\
		stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;\
		if(!vm_create_shared_pointer(vm,str_object)){\
			goto lbl_exit_function;\
		}\
		if(!vm_share_pointer(vm,str_object)){\
			goto lbl_exit_function;\
		}\
		str_object->set(stk_to_str(data->zs, stk_src));\
	}else{ \
		so_aux=(ScriptObject *)stk_src->value;\
		stk_dst->value=(intptr_t)so_aux;\
		stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;\
		if(!IS_STK_THIS(stk_src)){ /* do not share this! */\
			if(!vm_share_pointer(vm,so_aux)){\
				goto lbl_exit_function;\
			}\
		}\
	}\
}else{\
	VM_STOP_EXECUTE("(internal) cannot determine var type %s"\
		,stk_to_typeof_str(data->zs,stk_src).c_str()\
	);\
}\
if(copy_aux!=NULL)stk_dst->properties|=STK_PROPERTY_IS_VAR_C;


#define ASSIGN_SETTER(stk_dst,stk_mp,so_aux, store_lst_setter_functions,__STR_SETTER_METAMETHOD__) \
	ScriptFunction *ptr_function_found=(ScriptFunction *)((StackElement *)store_lst_setter_functions->items[0])->value;\
	StackElement *stk_vm_start=data->stk_vm_current;\
	const char *
	StackElement *stk_arg=stk_vm_start+1;\
	*stk_arg=*stk_src;\
	if(so_aux->isNativeObject()){ /* because object is native, we can have more than one _setter */ \
		if((ptr_function_found=vm_find_function( \
				vm \
				,data->script_class_factory->getScriptClass(so_aux->idx_script_class)\
				,calling_function\
				,instruction\
				,false\
				,STK_IS_SCRIPT_OBJECT_CLASS(stk_dst)?__STR_SETTER_METAMETHOD__:__STR_SETTER_METAMETHOD__"@" /* symbol to find */\
				,stk_arg \
				,1))==NULL){ \
			if(stk_mp){ \
				VM_STOP_EXECUTE("Property '%s::%s' does not implement '%s' function"\
						,stk_mp->so_object->getScriptClass()->class_name.c_str()\
						,stk_mp->member_attribute->attribute_name.c_str()\
						,__STR_SETTER_METAMETHOD__\
						);\
			); \
			}else{\
				VM_STOP_EXECUTE("Class '%s' does not implement '%s' function" \
						,((ScriptObject *)stk_dst->value)->getScriptClass()->class_name.c_str() \
						,__STR_SETTER_METAMETHOD__\
				);\
			}\
		}\
	}else if(store_lst_setter_functions->count>1){ /* it has all member list */\
		Symbol * symbol_setter = obj_aux->getScriptClass()->getSymbol("_set"); \
		if(symbol_setter == NULL){\
			VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented");\
		}\
		if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
			VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function");\
		}\
		ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
	}\
	if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){\
		vm_call_function_native(\
				vm\
				,so_aux\
				,ptr_function_found\
				,stk_arg\
				,1\
				,calling_function\
				,instruction\
		);\
	}else{\
		vm_call_function_script(\
			vm\
			,so_aux\
			,ptr_function_found\
			,stk_arg\
			,1\
		);\
	}\
	if((stk_mp!=NULL) && (mp->member_attribute->getter != NULL)){ \
		VM_INNER_CALL_ONLY_RETURN( \
			stk_ma->so_object \
			,stk_ma->member_attribute->getter \
			,stk_ma->member_attribute->getter->function_name.c_str() \
			,true \
		);\
		dst_stk=data->stk_vm_current;\
	}else{\
		*dst_stk=k_stk_null;\
	}\
