/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define LOAD_SET_OPERATION \
stk_result_op1=--data->stk_vm_current;\
stk_result_op1=(StackElement *)stk_result_op1->value; /* src ptr stk */ \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){ \
	stk_result_op1=(StackElement *)(STK_GET_STK_VAR_REF(stk_result_op1)->value); \
} \
stk_result_op2=--data->stk_vm_current;\
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op2)){ /*src stk*/ \
	stk_result_op2=(StackElement *)((STK_GET_STK_VAR_REF(stk_result_op2)->value)); \
}

#define PERFORM_DIV_SET_OPERATION \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_STK_ZS_INT(STK_VALUE_TO_ZS_INT(stk_result_op1) /= STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) /= f_aux_value2);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		PUSH_STK_ZS_FLOAT(f_aux_value1 /= op2_int);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(f_aux_value1 /= f_aux_value2);\
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

#define PERFORM_ADD_SET_OPERATION \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		PUSH_STK_ZS_INT(stk_result_op1->value += stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		PUSH_STK_ZS_FLOAT(stk_result_op1->value += *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) += stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) += *((zs_float *)&stk_result_op2->value));\
		break;\
	default:\
		if(	STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1){\
			((ScriptStringObject *)stk_result_op1->value)->append(((ScriptStringObject *)stk_result_op1->getString());\
			PUSH_STK_SCRIPT_OBJECT(stk_result_op1);\
		}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
					&&\
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
		){\
				ScriptObjectObject::append(data->zs, (ScriptObjectObject *)stk_result_op1->value,(ScriptObjectObject *)stk_result_op1->value);\
				PUSH_STK_SCRIPT_OBJECT(stk_result_op1);\
		}else{\
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
	}\

#define PERFORM_ARITHMETIC_SET_OPERATION(__C_OP__, __METAMETHOD__)\
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


#define PERFORM_MOD_SET_OPERATION \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		op2_int=;\
		if( == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_STK_ZS_INT(stk_result_op1->value %= stk_result_op2->value);\
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
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value)=fmod(*((zs_float *)&stk_result_op1->value) , stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value)=fmod(*((zs_float *)&stk_result_op1->value) , *((zs_float *)&stk_result_op2->value)));\
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

#define PERFORM_LOGIC_SET_OPERATION(__C_OP__)\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_BOOL){\
		PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(ZS_STR(__C_OP__));\
		goto lbl_exit_function;\
	}\


#define PERFORM_BINARY_SET_OPERATION(__C_OP__, __METAMETHOD__)\
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

