/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define VM_SET_CONTAINER_ELEMENT \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_src)){ \
	stk_src=(StackElement *)((STK_GET_STK_VAR_REF(stk_src)->value)); \
} \
stk_src_ref_value_copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update value on save) */ \
stk_src_ref_value=&stk_src->value; \
stk_dst_ref_value=&stk_dst->value; \
if(stk_src->properties & STK_PROPERTY_IS_C_VAR_PTR){ /* src is C pointer */ \
	stk_src_ref_value=(zs_int *)((stk_src)->value); \
}\
stk_src_properties=stk_src->properties;\
if(stk_src_properties == STK_PROPERTY_NULL){\
	stk_dst->properties=STK_PROPERTY_NULL;\
}else if(stk_src_properties & STK_PROPERTY_ZS_INT){\
	stk_dst->properties=STK_PROPERTY_ZS_INT;\
	*((zs_int *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value);\
	if(stk_src_ref_value_copy_aux!=NULL)(*(zs_int *)stk_src_ref_value_copy_aux)=*((zs_int *)stk_src_ref_value);\
}else if(stk_src_properties & STK_PROPERTY_ZS_FLOAT){\
	stk_dst->properties=STK_PROPERTY_ZS_FLOAT;\
	*((zs_float *)stk_dst_ref_value)=*((zs_float *)stk_src_ref_value);\
	if(stk_src_ref_value_copy_aux!=NULL)(*(zs_float *)stk_src_ref_value_copy_aux)=*((zs_float *)stk_src_ref_value);\
}else if(stk_src_properties & STK_PROPERTY_BOOL){\
	stk_dst->properties=STK_PROPERTY_BOOL;\
	*((bool *)stk_dst_ref_value)=*((bool *)stk_src_ref_value);\
	if(stk_src_ref_value_copy_aux!=NULL)(*(bool *)stk_src_ref_value_copy_aux)=*((bool *)stk_src_ref_value);\
}else if(stk_src_properties  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){\
	*stk_dst=*stk_src;\
}else if(stk_src_properties & STK_PROPERTY_SCRIPT_OBJECT){\
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
	VM_STOP_EXECUTE("VM_SET_CONTAINER_ELEMENT:(internal) cannot determine var type %s"\
		,stk_to_typeof_str(data->zs,stk_src).c_str()\
	);\
}\
if(stk_src_ref_value_copy_aux!=NULL)stk_dst->properties|=STK_PROPERTY_IS_C_VAR_PTR;


#define VM_SET_METAMETHOD(stk_dst,stk_src,so_aux, store_lst_setter_functions) \
	StackElement *stk_vm_start=data->stk_vm_current;\
	StackElement *stk_arg=stk_vm_start+1; /*start from stk_src */\
	const char *__STR_SETTER_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET);\
	const char *__STR_AKA_SETTER_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_SET);\
	*stk_arg=*stk_src;\
	StackMemberProperty *	stk_mp=NULL;\
	if((store_lst_setter_functions == NULL) && (stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY)){ /* try member property...*/\
		stk_mp=(StackMemberProperty *)stk_dst->value;\
		if(stk_mp->member_property->metamethod_members.setters.count > 0){\
			store_lst_setter_functions=&stk_mp->member_property->metamethod_members.setters;\
			so_aux=stk_mp->so_object;\
		}\
	}\
	if(store_lst_setter_functions == NULL){\
		VM_STOP_EXECUTE("%s '%s' not implements metamethod add_set (aka '+='') " \
				,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol" \
				,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
		);\
	}\
	ScriptFunction *ptr_function_found=(ScriptFunction *)((StackElement *)store_lst_setter_functions->items[0])->value;\
	if(so_aux->isNativeObject()){ /* because object is native, we can have more than one _setter */ \
		if((ptr_function_found=vm_find_function( \
				vm \
				,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
				,calling_function\
				,instruction\
				,false\
				,stk_mp==NULL?"_set":(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(BYTE_CODE_METAMETHOD_ADD_SET,stk_mp->member_property->property_name)).c_str() /* symbol to find */\
				,stk_arg \
				,1))==NULL){ \
			if(stk_dst->properties & STK_PROPERTY_MEMBER_PROPERTY){ \
				VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
						,so_aux->getScriptType()->script_type_name.c_str()\
						,stk_mp->member_property->property_name.c_str()\
						,__STR_SETTER_METAMETHOD__\
				);\
			}else{\
				VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
						,so_aux->getScriptType()->script_type_name.c_str() \
						,__STR_SETTER_METAMETHOD__\
				);\
			}\
		}\
	}else if(store_lst_setter_functions->count>1){ /* it has overrided metamethods */\
		Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_SETTER_METAMETHOD__); \
		if(symbol_setter == NULL){\
			VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
					,__STR_SETTER_METAMETHOD__\
					,__STR_AKA_SETTER_METAMETHOD__\
			);\
		}\
		if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
			VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_SETTER_METAMETHOD__,__STR_AKA_SETTER_METAMETHOD__);\
		}\
		ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
	}\
	if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){\
		vm_call_function_native(\
				vm\
				,so_aux\
				,ptr_function_found\
				,calling_function\
				,instruction\
				,stk_arg\
				,1\
		);\
	}else{\
		vm_call_function_script(\
			vm\
			,so_aux\
			,ptr_function_found\
			,stk_arg\
		);\
	}\
	data->stk_vm_current=stk_vm_start;
