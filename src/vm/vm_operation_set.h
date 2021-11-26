/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define LOAD_OPS_SET_OPERATION \
stk_result_op1=--data->stk_vm_current;\
stk_result_op1=(StackElement *)stk_result_op1->value; /* dst ptr stk */ \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){ \
	stk_result_op1=(StackElement *)(STK_GET_STK_VAR_REF(stk_result_op1)->value); \
} \
stk_result_op2=--data->stk_vm_current;\
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op2)){ /*src stk*/ \
	stk_result_op2=(StackElement *)((STK_GET_STK_VAR_REF(stk_result_op2)->value)); \
}


#define VM_OPERATION_ADD_SET(__METAMETHOD__) \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_VAR_C){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value += stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(stk_result_op1->value += *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) += stk_result_op2->value);\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) += *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	default:\
		if(	STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){\
			((zs_string *)(((ScriptObjectString *)stk_result_op1->value)->value))->append(stk_to_str(data->zs,stk_result_op2));\
			VM_PUSH_STK_SCRIPT_OBJECT(stk_result_op1);\
		}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
					&&\
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
		){\
				ScriptObjectObject::append(data->zs, (ScriptObjectObject *)stk_result_op1->value,(ScriptObjectObject *)stk_result_op1->value);\
				VM_PUSH_STK_SCRIPT_OBJECT(stk_result_op1);\
		}else{\
			LOAD_PROPERTIES(__METAMETHOD__); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
			if(mp_aux->add_setters.count==0){\
				zs_strutils::format("%s '%s' not implements metamethod add_set (aka '+='') " \
						,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol" \
						,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
				);\
				goto lbl_exit_function;\
			}\
			/* find function if c */ \
			/* call metamethod  */ \
			VM_INNER_CALL(\
				so_aux\
				,sf_setter_function_found\
				,sf_setter_function_found->function_name.c_str()\
				,true\
				,1 \
				,true\
			);\
			/*getter after*/\
			if(mp_aux->getter!=NULL){\
				/* call _neg */\
				VM_INNER_CALL_ONLY_RETURN(\
						so_aux\
						,mp_aux->getter\
						,mp_aux->getter->function_name.c_str()\
						,true\
				);\
			}else{ /* store object */ \
				if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
					data->stk_vm_current->value=(zs_int)so_aux;\
					data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
				}else{\
					*data->stk_vm_current=stk_var_copy;\
				}\
			}\
			data->stk_vm_current++;\
		}\
		break;\
	}\

#define VM_OPERATION_ARITHMETIC_SET(__C_OP__, __METAMETHOD__,__ACCESSOR_METAMETHOD_PROPERTY__)\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_VAR_C){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(stk_result_op1->value __C_OP__ *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) __C_OP__ *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	default:\
		LOAD_PROPERTIES(__METAMETHOD__); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
		if(__ACCESSOR_METAMETHOD_PROPERTY__.count==0){\
			zs_strutils::format("%s '%s' not implements metamethod %s (aka '%s'') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
					,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
					,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			);\
			goto lbl_exit_function;\
		}\
		/* find function if c */ \
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,sf_setter_function_found\
			,sf_setter_function_found->function_name.c_str()\
			,true\
			,1 \
			,true\
		);\
		/*getter after*/\
		if(mp_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,mp_aux->getter\
					,mp_aux->getter->function_name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
	}\


#define VM_OPERATION_DIV_SET(__METAMETHOD__) \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_VAR_C){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value /= stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value /= *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op2->value) /= stk_result_op2->value);\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) /= *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	default:\
		LOAD_PROPERTIES(__METAMETHOD__); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
		if(mp_aux->div_setters.count==0){\
			zs_strutils::format("%s '%s' not implements metamethod div_set (aka '/='') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
			);\
			goto lbl_exit_function;\
		}\
		/* find function if c */ \
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,sf_setter_function_found\
			,sf_setter_function_found->function_name.c_str()\
			,true\
			,1 \
			,true\
		);\
		/*getter after*/\
		if(mp_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,mp_aux->getter\
					,mp_aux->getter->function_name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
		break;\
	}\

#define VM_OPERATION_MOD_SET(__METAMETHOD__) \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value %= stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(stk_result_op1->value,*((zs_float *)&stk_result_op2->value)));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value)=fmod(*((zs_float *)&stk_result_op1->value) , stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value)=fmod(*((zs_float *)&stk_result_op1->value) , *((zs_float *)&stk_result_op2->value)));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	default:\
		LOAD_PROPERTIES(__METAMETHOD__);/* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */ \
		if(mp_aux->div_setters.count==0){\
			zs_strutils::format("%s '%s' not implements metamethod div_set (aka '/='') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
			);\
			goto lbl_exit_function;\
		}\
		/* find function if c */ \
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,sf_setter_function_found\
			,sf_setter_function_found->function_name.c_str()\
			,true\
			,1 \
			,true\
		);\
		/*getter after*/\
		if(mp_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,mp_aux->getter\
					,mp_aux->getter->function_name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
		break;\
	}\

#define VM_OPERATION_BINARY_SET(__C_OP__, __METAMETHOD__,__ACCESSOR_METAMETHOD_PROPERTY__)\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_ZS_INT){\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
	}else{\
		LOAD_PROPERTIES(__METAMETHOD__); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
		if(__ACCESSOR_METAMETHOD_PROPERTY__.count==0){\
			zs_strutils::format("%s '%s' not implements metamethod %s (aka '%s'') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
					,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
					,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			);\
			goto lbl_exit_function;\
		}\
		/* find function if c */ \
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,sf_setter_function_found\
			,sf_setter_function_found->function_name.c_str()\
			,true\
			,1 \
			,true\
		);\
		/*getter after*/\
		if(mp_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,mp_aux->getter\
					,mp_aux->getter->function_name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
	}\

