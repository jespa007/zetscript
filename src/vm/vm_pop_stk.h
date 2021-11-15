/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
 
#define LOAD_STK_FROM_INSTRUCTION(offset,properties) \
	((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? vm_load_this_element(vm,this_object,calling_function,instruction,offset) \
    : _stk_local_var+offset \

#define POP_STK_ONE_LOAD2 \
stk_result_op2=--data->stk_vm_current;\
stk_result_op1=(data->stk_vm_current-1);

#define POP_STK_TWO_LOAD2 \
stk_result_op2=--data->stk_vm_current;\
stk_result_op1=--data->stk_vm_current;

#define POP_STK_ONE \
stk_result_op1=--data->stk_vm_current;


#define POP_STK_TWO \
    switch(instruction->properties & INSTRUCTION_PROPERTY_ILOAD){\
    default:\
    case 0:\
		stk_result_op2=--data->stk_vm_current;\
		stk_result_op1=--data->stk_vm_current;\
		break;\
    case INSTRUCTION_PROPERTY_ILOAD_K: /* only perfom with one constant*/\
		 stk_result_op1=--data->stk_vm_current;\
		 stk_result_op2=&stk_aux;\
		 stk_result_op2->value=instruction->value_op2;\
		 stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
         break;\
    case INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
		 stk_result_op1=--data->stk_vm_current;\
         stk_result_op2=LOAD_STK_FROM_INSTRUCTION(instruction->value_op1,instruction->properties);\
         break;\
    case INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
		 stk_result_op1=&stk_aux;\
		 stk_result_op1->value=instruction->value_op2;\
		 stk_result_op1->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
         stk_result_op2=LOAD_STK_FROM_INSTRUCTION(instruction->value_op1,instruction->properties);\
         break;\
    case INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
        stk_result_op1=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
		stk_result_op2=&stk_aux;\
		stk_result_op2->value=instruction->value_op2;\
		stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
        break;\
   case INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
        stk_result_op1=LOAD_STK_FROM_INSTRUCTION(instruction->value_op1,instruction->properties);\
        stk_result_op2=LOAD_STK_FROM_INSTRUCTION(((instruction->value_op2&0xff0000)>>16),instruction->value_op2);\
        break;\
    }
