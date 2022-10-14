/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "vm_byte_code_operation.h"

#define LOAD_OPS_SET_OPERATION \
stk_result_op1=--data->vm_stk_current;\
EXTRACT_STK_RESULT_OP1 \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){ \
	stk_result_op1=(StackElement *)(STK_GET_STK_VAR_REF(stk_result_op1)->value); \
} \
stk_result_op2=--data->vm_stk_current;\
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op2)){ /*src stk*/ \
	stk_result_op2=(StackElement *)((STK_GET_STK_VAR_REF(stk_result_op2)->value)); \
}

#define VM_OPERATION_ADD_SET() \
	stk_var=stk_result_op1;\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
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
		if(!vm_call_metamethod_set(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, BYTE_CODE_METAMETHOD_ADD_SET\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->vm_stk_current=stk_start;\
	}\


#define VM_OPERATION_ARITHMETIC_SET(__C_OP__, __METAMETHOD__)\
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
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
		if(!vm_call_metamethod_set(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, __METAMETHOD__\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->vm_stk_current=stk_start;\
	}\

#define VM_OPERATION_DIV_SET() \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value /= stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value /= *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op2->value) /= stk_result_op2->value);\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) /= *((zs_float *)&stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	default:\
		if(!vm_call_metamethod_set(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, BYTE_CODE_METAMETHOD_DIV_SET\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->vm_stk_current=stk_start;\
	}\


#define VM_OPERATION_MOD_SET() \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value %= stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(stk_result_op1->value,*((zs_float *)&stk_result_op2->value)));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value)=fmod(*((zs_float *)&stk_result_op1->value) , stk_result_op2->value));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value)=fmod(*((zs_float *)&stk_result_op1->value) , *((zs_float *)&stk_result_op2->value)));\
		(*((zs_float *)(ptr_ptr_void_ref)))=*((zs_float *)&stk_result_op1->value);\
		break;\
	default:\
		if(!vm_call_metamethod_set(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, BYTE_CODE_METAMETHOD_MOD_SET\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->vm_stk_current=stk_start;\
	}\

#define VM_OPERATION_BINARY_SET(__C_OP__, __METAMETHOD__)\
	ptr_ptr_void_ref=(void **)(&((stk_result_op1)->value));\
	if(stk_result_op1->properties & STK_PROPERTY_IS_C_VAR_PTR){\
		ptr_ptr_void_ref=(void **)((stk_result_op1)->value);\
	}\
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_ZS_INT){\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		(*((zs_int *)(ptr_ptr_void_ref)))=stk_result_op1->value;\
	}else{\
		if(!vm_call_metamethod_set(\
			_vm\
			,_calling_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, __METAMETHOD__\
		)){\
			goto lbl_exit_function;\
		}\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->vm_stk_current=stk_start;\
	}\

