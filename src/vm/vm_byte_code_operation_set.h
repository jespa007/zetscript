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
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value += stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_STK_TO_FLOAT(stk_result_op1)=stk_result_op1->value;\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) += ZS_STK_TO_FLOAT(stk_result_op2));\
		stk_result_op1->properties=STK_PROPERTY_ZS_FLOAT;\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) += stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) += ZS_STK_TO_FLOAT(stk_result_op2));\
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
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_STK_TO_FLOAT(stk_result_op1)=stk_result_op1->value;\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) __C_OP__ ZS_STK_TO_FLOAT(stk_result_op2));\
		stk_result_op1->properties=STK_PROPERTY_ZS_FLOAT;\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) __C_OP__ stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) __C_OP__ ZS_STK_TO_FLOAT(stk_result_op2));\
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
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) = ((zs_float)stk_result_op1->value/(zs_float)stk_result_op2->value));\
		stk_result_op1->properties=STK_PROPERTY_ZS_FLOAT;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(ZS_STK_TO_FLOAT(stk_result_op2) == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) = (zs_float)stk_result_op1->value / ZS_STK_TO_FLOAT(stk_result_op2));\
		stk_result_op1->properties=STK_PROPERTY_ZS_FLOAT;\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op2) /= stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(ZS_STK_TO_FLOAT(stk_result_op2) == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1) /= ZS_STK_TO_FLOAT(stk_result_op2));\
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
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1)=fmod(stk_result_op1->value,stk_result_op2->value));\
		stk_result_op1->properties=STK_PROPERTY_ZS_FLOAT;\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(ZS_STK_TO_FLOAT(stk_result_op2) == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1)=fmod(stk_result_op1->value,ZS_STK_TO_FLOAT(stk_result_op2)));\
		stk_result_op1->properties=STK_PROPERTY_ZS_FLOAT;\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1)=fmod(ZS_STK_TO_FLOAT(stk_result_op1) , stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(ZS_STK_TO_FLOAT(stk_result_op2) == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(ZS_STK_TO_FLOAT(stk_result_op1)=fmod(ZS_STK_TO_FLOAT(stk_result_op1) , ZS_STK_TO_FLOAT(stk_result_op2)));\
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
	if((GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties)) == STK_PROPERTY_ZS_INT){\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
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

