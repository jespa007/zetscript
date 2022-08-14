/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define VM_SET_CONTAINER_ELEMENT \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_src)){ \
	stk_src=(StackElement *)((STK_GET_STK_VAR_REF(stk_src)->value)); \
} \
stk_src_ref_value=&stk_src->value; \
stk_dst_ref_value=&stk_dst->value; \
if(stk_src->properties & STK_PROPERTY_IS_C_VAR_PTR){ /* src is C pointer */ \
	stk_src_ref_value=(zs_int *)((stk_src)->value); \
}\
stk_src_properties=stk_src->properties;\
if(stk_src_properties == STK_PROPERTY_UNDEFINED){\
	stk_dst->properties=STK_PROPERTY_UNDEFINED;\
}else if(stk_src_properties == STK_PROPERTY_NULL){\
	stk_dst->properties=STK_PROPERTY_NULL;\
}else if(stk_src_properties & STK_PROPERTY_ZS_INT){\
	stk_dst->properties=STK_PROPERTY_ZS_INT;\
	*((zs_int *)stk_dst_ref_value)=*((zs_int *)stk_src_ref_value);\
}else if(stk_src_properties & STK_PROPERTY_ZS_FLOAT){\
	stk_dst->properties=STK_PROPERTY_ZS_FLOAT;\
	*((zs_float *)stk_dst_ref_value)=*((zs_float *)stk_src_ref_value);\
}else if(stk_src_properties & STK_PROPERTY_BOOL){\
	stk_dst->properties=STK_PROPERTY_BOOL;\
	*((bool *)stk_dst_ref_value)=*((bool *)stk_src_ref_value);\
}else if(stk_src_properties  &  (STK_PROPERTY_FUNCTION | STK_PROPERTY_TYPE | STK_PROPERTY_MEMBER_FUNCTION) ){\
	*stk_dst=*stk_src;\
}else if(stk_src_properties & STK_PROPERTY_SCRIPT_OBJECT){\
	if(STK_IS_SCRIPT_OBJECT_STRING(stk_src)){\
		ScriptObjectString *str_object=NULL;\
		stk_dst->value=(zs_int)(str_object= ZS_NEW_OBJECT_STRING(data->zs));\
		stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;\
		if(!vm_create_shared_script_object(vm,str_object)){\
			goto lbl_exit_function;\
		}\
		if(!vm_share_script_object(vm,str_object)){\
			goto lbl_exit_function;\
		}\
		str_object->set(stk_to_str(VM_STR_AUX_PARAM_0,data->zs, stk_src));\
	}else{ \
		so_aux=(ScriptObject *)stk_src->value;\
		stk_dst->value=(intptr_t)so_aux;\
		stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;\
		if(!vm_share_script_object(vm,so_aux)){\
			goto lbl_exit_function;\
		}\
	}\
}else{\
	VM_STOP_EXECUTE("VM_SET_CONTAINER_ELEMENT:(internal) cannot determine var type %s"\
		,stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_src)\
	);\
}\



