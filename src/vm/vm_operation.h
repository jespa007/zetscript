/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define EXTRACT_STK_RESULT_OP1 \
	stk_result_op1=(StackElement *)((stk_result_op1)->value);\




#define METAMETHOD_OPERATION_NOT_FOUND(__BYTE_CODE_METAMETHOD__) \
		if(member_property!=NULL){\
			VM_MAIN_ERROR(\
				VM_MAIN_ERROR_METAMETHOD_OPERATION_MEMBER_PROPERTY_NOT_IMPLEMENTED\
				,stk_result_op1\
				,__BYTE_CODE_METAMETHOD__\
			);\
		}else{\
			VM_MAIN_ERROR(\
				VM_MAIN_ERROR_METAMETHOD_OPERATION_SYMBOL_NOT_IMPLEMENTED\
				,stk_result_op1\
				,__BYTE_CODE_METAMETHOD__\
			);\
		}


#define LOAD_PROPERTIES(__METAMETHOD__) \
	ptr_metamethod_members_aux=NULL;\
	__STK_VAR_COPY__.value=0;\
	__STK_VAR_COPY__.properties=0;\
	so_aux=NULL;\
	member_property=NULL;\
	if(stk_result_op1->properties & STK_PROPERTY_MEMBER_PROPERTY){\
		__STK_VAR_COPY__=*stk_result_op1;\
		stk_mp_aux=(StackMemberProperty *)stk_result_op1->value;\
		member_property=stk_mp_aux->member_property;\
		ptr_metamethod_members_aux= &member_property->metamethod_members;\
		so_aux = stk_mp_aux->so_object;\
	}else if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){\
		so_aux= (ScriptObject *)stk_result_op1->value;\
		ptr_metamethod_members_aux= &so_aux->getScriptType()->metamethod_members;\
	}\
	else{\
		VM_MAIN_ERROR(\
				VM_MAIN_ERROR_LOAD_PROPERTIES_ERROR\
				,stk_result_op1\
				,__METAMETHOD__\
		);\
	}\

#define VM_OPERATION_DIV \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value / stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(stk_result_op1->value / *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) / stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) / *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(vm_call_metamethod(\
				_vm\
				,_calling_function\
				,instruction\
				,BYTE_CODE_METAMETHOD_DIV\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define VM_OPERATION_ADD \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value + stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(stk_result_op1->value + *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) + stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) + *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(vm_call_metamethod(\
				_vm\
				,_calling_function\
				,instruction\
				,BYTE_CODE_METAMETHOD_ADD\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\

#define VM_OPERATION_ARITHMETIC(__C_OP__, __METAMETHOD__)\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(stk_result_op1->value __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(vm_call_metamethod(\
				_vm\
				,_calling_function\
				,instruction\
				,__METAMETHOD__\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\

#define VM_OPERATION_COMPARE(__C_OP__, __BYTE_CODE_METAMETHOD_OPERATION__,__IS_JE_CASE__)\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_BOOL_OP2_BOOL:\
		VM_PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_BOOLEAN(*((zs_float *)&stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_BOOLEAN(*((zs_float *)&stk_result_op1->value) __C_OP__ *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(STK_VALUE_IS_TYPE(stk_result_op1)){\
			PRAGMA_PUSH\
			PRAGMA_DISABLE_WARNING(4127)\
			if(__BYTE_CODE_METAMETHOD_OPERATION__ ==BYTE_CODE_METAMETHOD_EQU || __BYTE_CODE_METAMETHOD_OPERATION__ ==BYTE_CODE_METAMETHOD_NOT_EQU){\
				PRAGMA_POP\
				if((stk_result_op1->value == IDX_TYPE_UNDEFINED) && (stk_result_op1->properties == IDX_TYPE_UNDEFINED) && (__BYTE_CODE_METAMETHOD_OPERATION__ ==BYTE_CODE_METAMETHOD_EQU)){\
					VM_PUSH_STK_BOOLEAN(true);\
				}else if((stk_result_op1->value == IDX_TYPE_UNDEFINED) && (stk_result_op1->properties != IDX_TYPE_UNDEFINED) && (__BYTE_CODE_METAMETHOD_OPERATION__ ==BYTE_CODE_METAMETHOD_NOT_EQU)){\
					VM_PUSH_STK_BOOLEAN(true);\
				}else{\
					VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
				}\
			}else{\
				VM_PUSH_STK_BOOLEAN(false);\
			}\
		}else if( STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) && STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){\
			vm_push_stk_boolean_equal_strings(_vm,stk_result_op1,stk_result_op2,__BYTE_CODE_METAMETHOD_OPERATION__);\
		}else if(  (stk_result_op1->properties==STK_PROPERTY_UNDEFINED || stk_result_op2->properties==STK_PROPERTY_UNDEFINED)\
				&& (__BYTE_CODE_METAMETHOD_OPERATION__ == BYTE_CODE_METAMETHOD_EQU || __BYTE_CODE_METAMETHOD_OPERATION__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
				){\
			if((stk_result_op1->properties == STK_PROPERTY_UNDEFINED) && (stk_result_op2->properties == STK_PROPERTY_UNDEFINED)){\
				VM_PUSH_STK_BOOLEAN(__BYTE_CODE_METAMETHOD_OPERATION__ == BYTE_CODE_METAMETHOD_EQU);\
			}else{\
				VM_PUSH_STK_BOOLEAN(__BYTE_CODE_METAMETHOD_OPERATION__ != BYTE_CODE_METAMETHOD_EQU);\
			}\
		}else if(  (stk_result_op1->properties==STK_PROPERTY_NULL || stk_result_op2->properties==STK_PROPERTY_NULL)\
				&& (__BYTE_CODE_METAMETHOD_OPERATION__ == BYTE_CODE_METAMETHOD_EQU || __BYTE_CODE_METAMETHOD_OPERATION__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
				){\
			if((stk_result_op1->properties == STK_PROPERTY_NULL) && (stk_result_op2->properties == STK_PROPERTY_NULL)){\
				VM_PUSH_STK_BOOLEAN(__BYTE_CODE_METAMETHOD_OPERATION__ == BYTE_CODE_METAMETHOD_EQU);\
			}else{\
				VM_PUSH_STK_BOOLEAN(__BYTE_CODE_METAMETHOD_OPERATION__ != BYTE_CODE_METAMETHOD_EQU);\
			}\
		}else{\
			stk_aux1=*stk_result_op1, stk_aux2=*stk_result_op2;\
			if(vm_call_metamethod(\
				_vm\
				,_calling_function\
				,instruction\
				, __BYTE_CODE_METAMETHOD_OPERATION__\
				,&stk_aux1\
				,&stk_aux2\
				,true\
				,__IS_JE_CASE__\
			)==false){\
				goto lbl_exit_function;\
			}\
		}\
	}\

#define VM_OPERATION_MOD(__METAMETHOD__) \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value % stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(stk_result_op1->value,*((zs_float *)&stk_result_op2->value)));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(*((zs_float *)&stk_result_op1->value) , stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(*((zs_float *)&stk_result_op1->value) , *((zs_float *)&stk_result_op2->value)));\
		break;\
	default:\
		if(vm_call_metamethod(\
			_vm\
			,_calling_function\
			,instruction\
			,BYTE_CODE_METAMETHOD_MOD\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define VM_OPERATION_LOGIC(__C_OP__)\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_BOOL){\
		VM_PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(ZS_STR(__C_OP__));\
		goto lbl_exit_function;\
	}\


#define VM_OPERATION_BINARY(__C_OP__, __METAMETHOD__)\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_ZS_INT){\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
	}else{\
		if(vm_call_metamethod(\
			_vm\
			,_calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define VM_OPERATION_NEG_POST(__C_OP__, __METAMETHOD__) \
	stk_result_op1=--data->stk_vm_current;\
	EXTRACT_STK_RESULT_OP1\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_result_op1)->properties)){\
	case STK_PROPERTY_ZS_INT:\
		VM_PUSH_STK_ZS_INT(-(*((zs_int *)(ptr_ptr_void_ref))));\
		(*((zs_int *)(ptr_ptr_void_ref)))__C_OP__;\
		break;\
	case STK_PROPERTY_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(-(*((zs_float *)(ptr_ptr_void_ref))));\
		(*((zs_float *)(ptr_ptr_void_ref)))__C_OP__;\
		break;\
	default:/*metamethod*/\
		if(vm_call_metamethod_operation_post(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,__METAMETHOD__\
			,true\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}


#define VM_OPERATION_POST(__C_OP__, __METAMETHOD__) \
	stk_result_op1=--data->stk_vm_current;\
	EXTRACT_STK_RESULT_OP1\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_result_op1)->properties)){\
	case STK_PROPERTY_ZS_INT:\
		VM_PUSH_STK_ZS_INT(-(*((zs_int *)(ptr_ptr_void_ref))));\
		(*((zs_int *)(ptr_ptr_void_ref)))__C_OP__;\
		break;\
	case STK_PROPERTY_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(-(*((zs_float *)(ptr_ptr_void_ref))));\
		(*((zs_float *)(ptr_ptr_void_ref)))__C_OP__;\
		break;\
	default:/*metamethod*/\
		if(vm_call_metamethod_operation_post(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,__METAMETHOD__\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}

#define VM_OPERATION_PRE(__C_OP__, __METAMETHOD__) \
	stk_result_op1=--data->stk_vm_current;\
	EXTRACT_STK_RESULT_OP1\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_result_op1)->properties)){\
	case STK_PROPERTY_ZS_INT:\
		(*((zs_int *)(ptr_ptr_void_ref)))__C_OP__;\
		VM_PUSH_STK_ZS_INT(*((zs_int *)(ptr_ptr_void_ref)));\
		break;\
	case STK_PROPERTY_ZS_FLOAT:\
		(*((zs_float *)(ptr_ptr_void_ref)))__C_OP__;\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)(ptr_ptr_void_ref)));\
		break;\
	default:\
		if(vm_call_metamethod_operation_pre(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,__METAMETHOD__\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}
