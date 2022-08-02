/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define EXTRACT_STK_RESULT_OP1 \
	stk_result_op1=(StackElement *)((stk_result_op1)->value);\


#define METAMETHOD_OPERATION_NOT_FOUND(__BYTE_CODE_METAMETHOD__) \
		if(member_property!=NULL){\
				VM_STOP_EXECUTE("Member property '%s' not implements metamethod '%s' (aka '%s') " \
						,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
						,byte_code_metamethod_to_symbol_str(__BYTE_CODE_METAMETHOD__)\
						,byte_code_metamethod_to_operator_str(__BYTE_CODE_METAMETHOD__)\
				);\
		}else{\
				VM_STOP_EXECUTE("Symbol '%s' as type '%s' not implements metamethod '%s' (aka '%s') " \
						,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
						,stk_to_typeof_str(data->vm_str_aux[0],data->zs,stk_result_op1) \
						,byte_code_metamethod_to_symbol_str(__BYTE_CODE_METAMETHOD__)\
						,byte_code_metamethod_to_operator_str(__BYTE_CODE_METAMETHOD__)\
				);\
		}

#define LOAD_PROPERTIES(__METAMETHOD__) \
	ptr_metamethod_members_aux=NULL;\
	stk_var_copy.value=0;\
	stk_var_copy.properties=0;\
	so_aux=NULL;\
	member_property=NULL;\
	if(stk_result_op1->properties & STK_PROPERTY_MEMBER_PROPERTY){\
		stk_var_copy=*stk_result_op1;\
		stk_mp_aux=(StackMemberProperty *)stk_result_op1->value;\
		member_property=stk_mp_aux->member_property;\
		ptr_metamethod_members_aux= &member_property->metamethod_members;\
		so_aux = stk_mp_aux->so_object;\
	}else if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){\
		so_aux= (ScriptObject *)stk_result_op1->value;\
		ptr_metamethod_members_aux= &so_aux->getScriptType()->metamethod_members;\
	}\
	else{\
		VM_STOP_EXECUTE("Error performing '%s%s': Cannot perform operation with value as '%s%s%s'"\
			,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
			,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
			,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			,stk_to_str(data->vm_str_aux[0],data->zs,stk_result_op2)\
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
		if(		STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)\
					||\
				STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)\
			){\
				ScriptObjectString *so_string=ScriptObjectString::newScriptObjectStringAddStk(data->zs,stk_result_op1,stk_result_op2);\
				vm_create_shared_script_object(vm,so_string);\
				VM_PUSH_STK_SCRIPT_OBJECT(so_string);\
		}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
					&&\
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
		){\
				ScriptObjectVector *so_vector=ScriptObjectVector::newScriptObjectVectorAdd(\
						data->zs\
						,(ScriptObjectVector *)stk_result_op1->value\
						,(ScriptObjectVector *)stk_result_op2->value\
				);\
				vm_create_shared_script_object(vm,so_vector);\
				VM_PUSH_STK_SCRIPT_OBJECT(so_vector);\
		}else if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_result_op2)\
					&&\
				STK_IS_SCRIPT_OBJECT_OBJECT(stk_result_op2)\
		){\
				ScriptObjectObject *so_object=ScriptObjectObject::concat(\
						data->zs\
						,(ScriptObjectObject *)stk_result_op1->value\
						,(ScriptObjectObject *)stk_result_op2->value\
				);\
				vm_create_shared_script_object(vm,so_object);\
				VM_PUSH_STK_SCRIPT_OBJECT(so_object);\
		}else{\
			if(vm_call_metamethod(\
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
			VM_PUSH_STK_BOOLEAN(ZS_STRCMP(\
					stk_to_str(data->vm_str_aux[0],data->zs, stk_result_op1)\
					, __C_OP__ \
					,stk_to_str(data->vm_str_aux[1],data->zs,stk_result_op2))\
				);\
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
			StackElement stk1=*stk_result_op1, stk2=*stk_result_op2;\
			if(vm_call_metamethod(\
				vm\
				,calling_function\
				,instruction\
				, __BYTE_CODE_METAMETHOD_OPERATION__\
				,&stk1\
				,&stk2\
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

#define VM_OPERATION_NEG_POST(__C_OP__, __METAMETHOD__,__POST_OPERATION_VARIABLE__) \
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
		LOAD_PROPERTIES(__METAMETHOD__);\
		if(ptr_metamethod_members_aux->neg==NULL){\
			METAMETHOD_OPERATION_NOT_FOUND(BYTE_CODE_METAMETHOD_NEG); \
		}\
		/* call _neg */\
		VM_INNER_CALL(\
				so_aux\
				,(ScriptFunction *)ptr_metamethod_members_aux->neg->ref_ptr\
				,0\
				,ptr_metamethod_members_aux->neg->name.c_str()\
				,true\
		);\
		data->stk_vm_current++; /* store negated value to stk to load after */\
		/* call inc metamethod */\
		if(__POST_OPERATION_VARIABLE__==NULL){\
			METAMETHOD_OPERATION_NOT_FOUND(__METAMETHOD__); \
		}\
		VM_INNER_CALL(\
				so_aux\
				,(ScriptFunction *)__POST_OPERATION_VARIABLE__->ref_ptr\
				,0\
				,__POST_OPERATION_VARIABLE__->name.c_str()\
				,true\
		);\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}


#define VM_OPERATION_POST(__C_OP__, __METAMETHOD__,__POST_OPERATION_VARIABLE__) \
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
		LOAD_PROPERTIES(__METAMETHOD__);\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL(\
					so_aux\
					,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
					, 0 \
					,ptr_metamethod_members_aux->getter->name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
		/* call post operation metamethod */\
		if(__POST_OPERATION_VARIABLE__==NULL){\
			METAMETHOD_OPERATION_NOT_FOUND(__METAMETHOD__); \
		}\
		VM_INNER_CALL(\
				so_aux\
				,(ScriptFunction *)__POST_OPERATION_VARIABLE__->ref_ptr\
				, 0 \
				,__POST_OPERATION_VARIABLE__->name.c_str()\
				,true\
		);\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}

#define VM_OPERATION_PRE(__C_OP__, __METAMETHOD__,__PRE_OPERATION_VARIABLE__) \
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
		LOAD_PROPERTIES(__METAMETHOD__);\
		/* call pre operation metamethod */\
		if(__PRE_OPERATION_VARIABLE__==NULL){\
			METAMETHOD_OPERATION_NOT_FOUND(__METAMETHOD__); \
		}\
		VM_INNER_CALL(\
				so_aux\
				,(ScriptFunction *)__PRE_OPERATION_VARIABLE__->ref_ptr\
				,0 \
				,__PRE_OPERATION_VARIABLE__->name.c_str()\
				,true\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL(\
					so_aux\
					,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
					,0 \
					,ptr_metamethod_members_aux->getter->name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_result_op1->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}
