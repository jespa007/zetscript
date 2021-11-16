/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */


#define PERFORM_ARITHMETIC_DIV_OPERATION \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_STK_ZS_INT(STK_VALUE_TO_ZS_INT(stk_result_op1) / STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) / f_aux_value2);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		PUSH_STK_ZS_FLOAT(f_aux_value1 / op2_int);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(f_aux_value1 / f_aux_value2);\
		break;\
	default:\
		if(vm_apply_metamethod(\
				vm\
				,calling_function\
				,instruction\
				,BYTE_CODE_METAMETHOD_DIV\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define PERFORM_ADD_OPERATION \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		PUSH_STK_ZS_INT(stk_result_op1->value + stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		PUSH_STK_ZS_FLOAT(stk_result_op1->value + *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) + stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) + *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(		STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)\
					||\
				STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)\
			){\
				ScriptObjectString *so_string=ScriptObjectString::newScriptObjectStringAddStk(data->zs,stk_result_op1,stk_result_op2);\
				vm_create_shared_pointer(vm,so_string);\
				PUSH_STK_SCRIPT_OBJECT(so_string);\
		}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
					&&\
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
		){\
				ScriptObjectVector *so_vector=ScriptObjectVector::newScriptObjectVectorAdd(\
						data->zs\
						,(ScriptObjectVector *)stk_result_op1->value\
						,(ScriptObjectVector *)stk_result_op2->value\
				);\
				vm_create_shared_pointer(vm,so_vector);\
				PUSH_STK_SCRIPT_OBJECT(so_vector);\
		}else{\
			if(vm_apply_metamethod(\
					vm\
					,calling_function\
					,instruction\
					,BYTE_CODE_METAMETHOD_ADD\
					,stk_result_op1\
					,stk_result_op2\
			)==false){\
				goto lbl_exit_function;\
			}\
		}\
	}\

#define PERFORM_ARITHMETIC_OPERATION(__C_OP__, __METAMETHOD__)\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		PUSH_STK_ZS_FLOAT(stk_result_op1->value __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(vm_apply_metamethod(\
				vm\
				,calling_function\
				,instruction\
				,__METAMETHOD__\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define PERFORM_COMPARE_OPERATION(__C_OP__, __METAMETHOD__)\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_BOOL_OP2_BOOL:\
		PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		PUSH_STK_BOOLEAN(*((zs_float *)&stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		PUSH_STK_BOOLEAN(*((zs_float *)&stk_result_op1->value) __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(STK_VALUE_IS_TYPE(stk_result_op1)){\
			if(__METAMETHOD__ ==BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ ==BYTE_CODE_METAMETHOD_NOT_EQU){\
				if((stk_result_op1->value == IDX_TYPE_NULL) && (stk_result_op1->properties == 0) && (__METAMETHOD__ ==BYTE_CODE_METAMETHOD_EQU)){\
					PUSH_STK_BOOLEAN(true);\
				}else if((stk_result_op1->value == IDX_TYPE_NULL) && (stk_result_op1->properties != 0) && (__METAMETHOD__ ==BYTE_CODE_METAMETHOD_NOT_EQU)){\
					PUSH_STK_BOOLEAN(true);\
				}else{\
					PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
				}\
			}else{\
				PUSH_STK_BOOLEAN(false);\
			}\
		}else if( STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) && STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){\
			PUSH_STK_BOOLEAN(ZS_STRCMP(stk_to_str(data->zs, stk_result_op1).c_str(), __C_OP__ ,stk_to_str(data->zs,stk_result_op2).c_str()));\
		}else if(  (stk_result_op1->properties==STK_PROPERTY_NULL || stk_result_op2->properties==STK_PROPERTY_NULL)\
				&& (__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
				){\
			if((stk_result_op1->properties == 0) && (stk_result_op2->properties == 0)){\
				PUSH_STK_BOOLEAN(__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU);\
			}else{\
				PUSH_STK_BOOLEAN(__METAMETHOD__ != BYTE_CODE_METAMETHOD_EQU);\
			}\
		}else{\
			if(vm_apply_metamethod(\
				vm\
				,calling_function\
				,instruction\
				, __METAMETHOD__\
				,stk_result_op1\
				,stk_result_op2\
			)==false){\
				goto lbl_exit_function;\
			}\
		}\
	}\

#define PERFORM_MOD_OPERATION \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		op2_int=;\
		if( == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_STK_ZS_INT(stk_result_op1->value % stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(fmod(stk_result_op1->value,*((zs_float *)&stk_result_op2->value)));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(fmod(*((zs_float *)&stk_result_op1->value) , stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(fmod(*((zs_float *)&stk_result_op1->value) , *((zs_float *)&stk_result_op2->value)));\
		break;\
	default:\
		if(vm_apply_metamethod(\
			vm\
			,calling_function\
			,instruction\
			,BYTE_CODE_METAMETHOD_MOD\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define PERFORM_LOGIC_OPERATION(__C_OP__)\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_BOOL){\
		PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(ZS_STR(__C_OP__));\
		goto lbl_exit_function;\
	}\


#define PERFORM_BINARY_OPERATION(__C_OP__, __METAMETHOD__)\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_ZS_INT){\
		PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
	}else{\
		if(vm_apply_metamethod(\
			vm\
			,calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define PERFORM_POST_INC_OPERATION \
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);/* always expects ptr stk due it modifies the var */\
	void **ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case STK_PROPERTY_ZS_INT:\
			PUSH_STK_ZS_INT((*((zs_int *)(ref))));\
			(*((zs_int *)(ref)))++;\
			break;\
	case STK_PROPERTY_ZS_FLOAT:\
			PUSH_STK_ZS_FLOAT((*((zs_float *)(ref))));\
			(*((zs_float *)(ref)))++;\
			break;\
	default:/*metamethod*/\
		if(stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY){\
			StackMemberProperty *stk_ma= (StackMemberProperty *)stk_result_op1->value;\
			script_object = stk_ma->so_object;\
			ptr_function_found=stk_ma->member_property->post_inc;\
			if(ptr_function_found!=NULL){\
				if((ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){\
					if(vm_share_pointer(vm,(ScriptObject *)stk_var->value)==false){ /*we have to share any object to avoid be removed on function exit*/ \
						goto apply_metamethod_error;\
					}\
					vm_call_function_script(\
						vm\
						,script_object\
						,ptr_function_found\
						,stk_args\
						,n_stk_args\
					);\
				}else{ \
					vm_call_function_native(\
							vm\
							,script_object\
							,ptr_function_found\
							,stk_args\
							,n_stk_args\
							,calling_function\
							,instruction\
					);\
				}\
				\
				data->stk_vm_current=stk_vm_current_backup;\
				ret_obj.value=(uintptr_t)script_object;\
				ret_obj.properties=STK_PROPERTY_SCRIPT_OBJECT;\
				*data->stk_vm_current++ = ret_obj;\
			}\
		}else{\
			zs_strutils::format("Member attribute '%s' has not implemented metamethod _post_inc (aka '%s++') ",stk_ma->member_property->property_name.c_str(),stk_ma->member_property->property_name.c_str());\
		}

/*
#define PERFORM_POST_OPERATOR(__PRE_OP__,__OPERATOR__, __METAMETHOD__) \
{\
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);\
	void **ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case STK_PROPERTY_ZS_INT:\
			PUSH_STK_ZS_INT(__PRE_OP__(*((zs_int *)(ref))));\
			(*((zs_int *)(ref)))__OPERATOR__;\
			break;\
	case STK_PROPERTY_ZS_FLOAT:\
			PUSH_STK_ZS_FLOAT(__PRE_OP__(*((zs_float *)(ref))));\
			(*((zs_float *)(ref)))__OPERATOR__;\
			break;\
	default:\
		if(vm_apply_metamethod(\
			vm\
			,calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_var\
			,NULL\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
}


#define PERFORM_PRE_OPERATOR(__OPERATOR__, __METAMETHOD__) \
{\
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);\
	void **ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case STK_PROPERTY_ZS_INT:\
			(*((zs_int *)(ref)))__OPERATOR__;\
			PUSH_STK_ZS_INT(*((zs_int *)(ref)));\
			break;\
	case STK_PROPERTY_ZS_FLOAT:\
			(*((zs_float *)(ref)))__OPERATOR__;\
			PUSH_STK_ZS_FLOAT(*((zs_float *)(ref)));\
			break;\
	default:\
		if(vm_apply_metamethod(\
			vm\
			,calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_var\
			,NULL\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
}
*/
