/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "vm_operation.h"

#define ZS_LOAD_OPERANDS_ASSIGN_OPERATION \
stk_result_op1=--data->vm_stk_current;\
ZS_EXTRACT_STK_RESULT_PRE_POST_OPERATION \
if(STACK_ELEMENT_IS_VAR_REF_OBJECT(stk_result_op1)){ \
	stk_result_op1=(StackElement *)(ZS_GET_STK_VAR_REF(stk_result_op1)->value); \
} \
stk_result_op2=--data->vm_stk_current;\
if(STACK_ELEMENT_IS_VAR_REF_OBJECT(stk_result_op2)){ /*src stk*/ \
	stk_result_op2=(StackElement *)((ZS_GET_STK_VAR_REF(stk_result_op2)->value)); \
}

#define ZS_VM_OPERATION_ADD_ASSIGN() \
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value += stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux += ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_FLOAT;\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux += stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux += ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(!vm_call_metamethod_set(\
			_vm\
			,_script_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, METAMETHOD_ADD_ASSIGN\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}\


#define ZS_VM_OPERATION_ARITHMETIC_ASSIGN(__C_OP__, __METAMETHOD__)\
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux __C_OP__ ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_FLOAT;\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux __C_OP__ stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux __C_OP__ ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(!vm_call_metamethod_set(\
			_vm\
			,_script_function\
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
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}\

#define ZS_VM_OPERATION_DIV_ASSIGN() \
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	zs_float_aux=(zs_float *)&stk_result_op1->value;\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux = ((zs_float)stk_result_op1->value/(zs_float)stk_result_op2->value));\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_FLOAT;\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux = (zs_float)stk_result_op1->value / ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_FLOAT;\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux /= stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux /= ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(!vm_call_metamethod_set(\
			_vm\
			,_script_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, METAMETHOD_DIV_ASSIGN\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}\


#define ZS_VM_OPERATION_MOD_ASSIGN() \
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	zs_float_aux=(zs_float *)&stk_result_op1->value;\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux=fmod(stk_result_op1->value,stk_result_op2->value));\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_FLOAT;\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux=fmod(stk_result_op1->value,ZS_READ_INTPTR_FLOAT(stk_result_op2->value)));\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_FLOAT;\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux=fmod(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) , stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux=fmod(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) , ZS_READ_INTPTR_FLOAT(stk_result_op2->value)));\
		break;\
	default:\
		if(!vm_call_metamethod_set(\
			_vm\
			,_script_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, METAMETHOD_MOD_ASSIGN\
		)){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}\

#define ZS_VM_OPERATION_BINARY_ASSIGN(__C_OP__, __METAMETHOD__)\
	if((STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties&stk_result_op2->properties)) == STACK_ELEMENT_PROPERTY_INT){\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
	}else{\
		if(!vm_call_metamethod_set(\
			_vm\
			,_script_function\
			,instruction\
			,stk_result_op1\
			,stk_result_op2\
			, __METAMETHOD__\
		)){\
			goto lbl_exit_function;\
		}\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}\

