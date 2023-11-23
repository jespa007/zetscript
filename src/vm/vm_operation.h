/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*#define EXTRACT_STK_RESULT_OP1 \
	stk_result_op1=(StackElement *)((stk_result_op1)->value);\
*/
#define ZS_EXTRACT_STK_RESULT_PRE_POST_OPERATION \
	if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_PTR_STK){\
		stk_result_op1=(StackElement *)((stk_result_op1)->value);\
	}else if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT){\
		stk_result_op1->value=(zs_int)(((ContainerSlot *)stk_result_op1->value)->getSrcContainerRef());\
		stk_result_op1->properties=STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT;\
	}else{\
		ZS_VM_STOP_EXECUTEF("EXTRACT_STK_RESULT_OP1_POST : Unexpected stackelement type");\
	}\


#define ZS_METAMETHOD_OPERATION_NOT_FOUND(__METAMETHOD_ID__) \
		if(member_property!=NULL){\
			ZS_VM_MAIN_ERROR(\
				ZS_VM_MAIN_ERROR_METAMETHOD_OPERATION_MEMBER_PROPERTY_NOT_IMPLEMENTED\
				,stk_result_op1\
				,__METAMETHOD_ID__\
			);\
		}else{\
			ZS_VM_MAIN_ERROR(\
				ZS_VM_MAIN_ERROR_METAMETHOD_OPERATION_SYMBOL_NOT_IMPLEMENTED\
				,stk_result_op1\
				,__METAMETHOD_ID__\
			);\
		}


#define ZS_LOAD_PROPERTIES(__METAMETHOD__) \
	ptr_metamethod_members_aux=NULL;\
	stk_aux1.value=0;\
	stk_aux1.properties=0;\
	so_aux=NULL;\
	member_property=NULL;\
	if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY){\
		stk_aux1=*stk_result_op1;\
		stk_mp_aux=(StackElementMemberProperty *)stk_result_op1->value;\
		member_property=stk_mp_aux->member_property;\
		ptr_metamethod_members_aux= &member_property->metamethod_members;\
		so_aux = stk_mp_aux->so_object;\
	}else if(stk_result_op1->properties & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){\
		so_aux= (ScriptObject *)stk_result_op1->value;\
		ptr_metamethod_members_aux= &so_aux->getScriptType()->metamethod_members;\
	}\
	else{\
		ZS_VM_MAIN_ERROR(\
				ZS_VM_MAIN_ERROR_ZS_LOAD_PROPERTIES_ERROR\
				,stk_result_op1\
				,__METAMETHOD__\
		);\
	}\

#define ZS_VM_OPERATION_DIV \
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(((zs_float)stk_result_op1->value) / ((zs_float)stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(stk_result_op1->value / ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) / stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception div operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) / ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(vm_call_metamethod(\
				_vm\
				,_script_function\
				,instruction\
				,METAMETHOD_DIV\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define ZS_VM_OPERATION_ADD \
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value + stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		ZS_VM_PUSH_STK_FLOAT(stk_result_op1->value + ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		ZS_VM_PUSH_STK_FLOAT(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) + stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		ZS_VM_PUSH_STK_FLOAT(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) + ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(vm_call_metamethod(\
				_vm\
				,_script_function\
				,instruction\
				,METAMETHOD_ADD\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\

#define ZS_VM_OPERATION_ARITHMETIC(__C_OP__, __METAMETHOD__)\
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		ZS_VM_PUSH_STK_FLOAT(stk_result_op1->value __C_OP__ ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		ZS_VM_PUSH_STK_FLOAT(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		ZS_VM_PUSH_STK_FLOAT(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) __C_OP__ ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(vm_call_metamethod(\
				_vm\
				,_script_function\
				,instruction\
				,__METAMETHOD__\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\

#define ZS_VM_OPERATION_COMPARE(__C_OP__, __METAMETHOD_ID_OPERATION__,__IS_JE_CASE__)\
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		ZS_VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_BOOL_OP2_BOOL:\
		ZS_VM_PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		ZS_VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		ZS_VM_PUSH_STK_BOOLEAN(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) __C_OP__ stk_result_op2->value);\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		ZS_VM_PUSH_STK_BOOLEAN(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) __C_OP__ ZS_READ_INTPTR_FLOAT(stk_result_op2->value));\
		break;\
	default:\
		if(STACK_ELEMENT_IS_TYPE(stk_result_op1)){\
			PRAGMA_PUSH\
			PRAGMA_DISABLE_WARNING(4127)\
			if(__METAMETHOD_ID_OPERATION__ == METAMETHOD_EQU || __METAMETHOD_ID_OPERATION__ == METAMETHOD_NEQU){\
				PRAGMA_POP\
				if((stk_result_op1->value == SCRIPT_TYPE_ID_UNDEFINED) && (stk_result_op1->properties == SCRIPT_TYPE_ID_UNDEFINED) && (__METAMETHOD_ID_OPERATION__ ==METAMETHOD_EQU)){\
					ZS_VM_PUSH_STK_BOOLEAN(true);\
				}else if((stk_result_op1->value == SCRIPT_TYPE_ID_UNDEFINED) && (stk_result_op1->properties != SCRIPT_TYPE_ID_UNDEFINED) && (__METAMETHOD_ID_OPERATION__ ==METAMETHOD_NEQU)){\
					ZS_VM_PUSH_STK_BOOLEAN(true);\
				}else{\
					ZS_VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
				}\
			}else{\
				ZS_VM_PUSH_STK_BOOLEAN(false);\
			}\
		}else if( STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(stk_result_op1) && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(stk_result_op2)){\
			vm_push_stk_boolean_equal_strings(_vm,stk_result_op1,stk_result_op2,__METAMETHOD_ID_OPERATION__);\
		}else if(  (stk_result_op1->properties==STACK_ELEMENT_PROPERTY_UNDEFINED || stk_result_op2->properties==STACK_ELEMENT_PROPERTY_UNDEFINED)\
				&& (__METAMETHOD_ID_OPERATION__ == METAMETHOD_EQU || __METAMETHOD_ID_OPERATION__ == METAMETHOD_NEQU)\
				){\
			if((stk_result_op1->properties == STACK_ELEMENT_PROPERTY_UNDEFINED) && (stk_result_op2->properties == STACK_ELEMENT_PROPERTY_UNDEFINED)){\
				ZS_VM_PUSH_STK_BOOLEAN(__METAMETHOD_ID_OPERATION__ == METAMETHOD_EQU);\
			}else{\
				ZS_VM_PUSH_STK_BOOLEAN(__METAMETHOD_ID_OPERATION__ != METAMETHOD_EQU);\
			}\
		}else if(  (stk_result_op1->properties==STACK_ELEMENT_PROPERTY_NULL || stk_result_op2->properties==STACK_ELEMENT_PROPERTY_NULL)\
				&& (__METAMETHOD_ID_OPERATION__ == METAMETHOD_EQU || __METAMETHOD_ID_OPERATION__ == METAMETHOD_NEQU)\
				){\
			if((stk_result_op1->properties == STACK_ELEMENT_PROPERTY_NULL) && (stk_result_op2->properties == STACK_ELEMENT_PROPERTY_NULL)){\
				ZS_VM_PUSH_STK_BOOLEAN(__METAMETHOD_ID_OPERATION__ == METAMETHOD_EQU);\
			}else{\
				ZS_VM_PUSH_STK_BOOLEAN(__METAMETHOD_ID_OPERATION__ != METAMETHOD_EQU);\
			}\
		}else{\
			stk_aux1=*stk_result_op1, stk_aux2=*stk_result_op2;\
			if(vm_call_metamethod(\
				_vm\
				,_script_function\
				,instruction\
				, __METAMETHOD_ID_OPERATION__\
				,&stk_aux1\
				,&stk_aux2\
				,true\
				,__IS_JE_CASE__\
			)==false){\
				goto lbl_exit_function;\
			}\
		}\
	}\

#define ZS_VM_OPERATION_MOD(__METAMETHOD__) \
	msk_properties=(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties)<<16)|STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op2->properties);\
	switch(msk_properties){\
	case STACK_ELEMENT_OP1_INT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(fmod(stk_result_op1->value, stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_INT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(fmod(stk_result_op1->value,ZS_READ_INTPTR_FLOAT(stk_result_op2->value)));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_INT:\
		if(stk_result_op2->value == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(fmod(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) , stk_result_op2->value));\
		break;\
	case STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT:\
		if(ZS_READ_INTPTR_FLOAT(stk_result_op2->value) == 0){\
			ZS_VM_STOP_EXECUTEF("exception mod operation by 0");\
		}\
		ZS_VM_PUSH_STK_FLOAT(fmod(ZS_READ_INTPTR_FLOAT(stk_result_op1->value) , ZS_READ_INTPTR_FLOAT(stk_result_op2->value)));\
		break;\
	default:\
		if(vm_call_metamethod(\
			_vm\
			,_script_function\
			,instruction\
			,METAMETHOD_MOD\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define ZS_VM_OPERATION_LOGIC(__C_OP__)\
	if((STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties&stk_result_op2->properties)) == STACK_ELEMENT_PROPERTY_BOOL){\
		ZS_VM_PUSH_STK_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		ZS_PRINT_DUAL_ERROR_OP(ZS_STR(__C_OP__));\
		goto lbl_exit_function;\
	}\


#define ZS_VM_OPERATION_BINARY(__C_OP__, __METAMETHOD__)\
	if((STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(stk_result_op1->properties&stk_result_op2->properties)) == STACK_ELEMENT_PROPERTY_INT){\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value __C_OP__ stk_result_op2->value);\
	}else{\
		if(vm_call_metamethod(\
			_vm\
			,_script_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\

#define VM_POST_OPERATION(__C_OP__, __METAMETHOD__) \
	stk_result_op1=--data->vm_stk_current;\
	ZS_EXTRACT_STK_RESULT_PRE_POST_OPERATION\
	switch(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES((stk_result_op1)->properties)){\
	case STACK_ELEMENT_PROPERTY_INT:\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value);\
		stk_result_op1->value __C_OP__;\
		break;\
	case STACK_ELEMENT_PROPERTY_FLOAT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux);\
		(*zs_float_aux) __C_OP__;\
		break;\
	default:/*metamethod*/\
		if(vm_call_metamethod_operation_pre_post(\
			_vm\
			,_script_function\
			,instruction\
			,stk_result_op1\
			,__METAMETHOD__\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}

#define VM_PRE_OPERATION(__C_OP__, __METAMETHOD__) \
	stk_result_op1=--data->vm_stk_current;\
	ZS_EXTRACT_STK_RESULT_PRE_POST_OPERATION\
	switch(STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES((stk_result_op1)->properties)){\
	case STACK_ELEMENT_PROPERTY_INT:\
		__C_OP__ stk_result_op1->value;\
		ZS_VM_PUSH_STK_INT(stk_result_op1->value);\
		break;\
	case STACK_ELEMENT_PROPERTY_FLOAT:\
		zs_float_aux=(zs_float *)&stk_result_op1->value;\
		(*zs_float_aux) __C_OP__;\
		ZS_VM_PUSH_STK_FLOAT(*zs_float_aux);\
		break;\
	default:\
		if(vm_call_metamethod_operation_pre_post(\
			_vm\
			,_script_function\
			,instruction\
			,stk_result_op1\
			,__METAMETHOD__\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		/*data->vm_stk_current=stk_start;*/\
		vm_reset_stack(_vm,stk_start);\
	}
