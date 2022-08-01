/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
 
/*#define LOAD_STK_FROM_INSTRUCTION(offset,properties) \
	((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? vm_load_this_element(vm,this_object,calling_function,instruction,offset) \
    : _stk_local_var+offset \*/


#define LOAD_STK_FROM_INSTRUCTION(_stk_result,_offset,_properties) \
	if((_properties & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR)==0){ \
		_stk_result=_stk_local_var+_offset; \
	}else{\
		_stk_result=NULL;\
		if(_offset != (uint8_t)ZS_IDX_UNDEFINED){ \
			_stk_result = this_object->getBuiltinElementAt(_offset); \
		} \
		if(_stk_result != NULL && (_stk_result->properties & STK_PROPERTY_MEMBER_PROPERTY)){ \
			stk_mp_aux=(StackMemberProperty *)_stk_result->value; \
			if(stk_mp_aux->member_property->metamethod_members.getter != NULL){ \
				VM_INNER_CALL( \
						stk_mp_aux->so_object \
						,(ScriptFunction *)stk_mp_aux->member_property->metamethod_members.getter->ref_ptr \
						,0 \
						,stk_mp_aux->member_property->metamethod_members.getter->name.c_str() \
						,true \
				); \
				/* getter requires stack to save value and avoid destroy previuos value*/ \
				_stk_result=data->stk_vm_current; \
			}else{ \
				VM_STOP_EXECUTE( \
						"Property '%s' does not implements _get metamethod",SFI_GET_SYMBOL_NAME(calling_function,instruction) \
				); \
			}\
		} \
	}\


#define VM_POP_STK_ONE_LOAD2 \
stk_result_op2=--data->stk_vm_current;\
stk_result_op1=(data->stk_vm_current-1);

#define VM_POP_STK_TWO_LOAD2 \
stk_result_op2=--data->stk_vm_current;\
stk_result_op1=--data->stk_vm_current;

#define VM_POP_STK_ONE \
stk_result_op1=--data->stk_vm_current;


#define VM_POP_STK_TWO \
	switch(instruction->properties & INSTRUCTION_PROPERTY_ILOAD){\
	default:\
	case 0: /* no optimization get 2 ops */ \
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
		 LOAD_STK_FROM_INSTRUCTION(stk_result_op2,instruction->value_op1,instruction->properties);\
		 break;\
	case INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
		 stk_result_op1=&stk_aux;\
		 stk_result_op1->value=INSTRUCTION_CONST_TO_STK_CONST_VALUE(instruction->value_op2,instruction->properties);\
		 stk_result_op1->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
		 LOAD_STK_FROM_INSTRUCTION(stk_result_op2,instruction->value_op1,instruction->properties);\
		 break;\
	case INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
		LOAD_STK_FROM_INSTRUCTION(stk_result_op1,instruction->value_op1,instruction->properties);\
		stk_result_op2=&stk_aux;\
		stk_result_op2->value=INSTRUCTION_CONST_TO_STK_CONST_VALUE(instruction->value_op2,instruction->properties);\
		stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
		break;\
   case INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
		LOAD_STK_FROM_INSTRUCTION(stk_result_op1,instruction->value_op1,instruction->properties); \
		LOAD_STK_FROM_INSTRUCTION(stk_result_op2,((instruction->value_op2&0xff0000)>>16),instruction->value_op2); \
		break;\
	}
