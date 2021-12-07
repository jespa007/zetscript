/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define LOAD_PROPERTIES(__METAMETHOD__) \
	ptr_metamethod_members_aux=NULL;\
	if(stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY){\
		stk_var_copy=*stk_result_op1;\
		stk_mp_aux=(StackMemberProperty *)stk_result_op1->value;\
		ptr_metamethod_members_aux= &stk_mp_aux->member_property->metamethod_members;\
		so_aux = stk_mp_aux->so_object;\
	}else if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
		so_aux= (ScriptObject *)stk_result_op1->value;\
		ptr_metamethod_members_aux= &so_aux->getScriptClass()->metamethod_members;\
	}\
	else{\
		zs_strutils::format("Symbol '%s' not implements metamethod %s (aka '%s'') "\
			,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
			,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
			,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
		);\
		goto lbl_exit_function;\
	}\

#define VM_OPERATION_DIV \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value / stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(stk_result_op1->value / *((zs_float *)&stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) / stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(*((zs_float *)&stk_result_op1->value) / *((zs_float *)&stk_result_op2->value));\
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

#define VM_OPERATION_ADD(__METAMETHOD__) \
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
				vm_create_shared_pointer(vm,so_string);\
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
				vm_create_shared_pointer(vm,so_vector);\
				VM_PUSH_STK_SCRIPT_OBJECT(so_vector);\
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

#define VM_OPERATION_COMPARE(__C_OP__, __METAMETHOD__)\
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
			if(__METAMETHOD__ ==BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ ==BYTE_CODE_METAMETHOD_NOT_EQU){\
				if((stk_result_op1->value == IDX_TYPE_NULL) && (stk_result_op1->properties == 0) && (__METAMETHOD__ ==BYTE_CODE_METAMETHOD_EQU)){\
					VM_PUSH_STK_BOOLEAN(true);\
				}else if((stk_result_op1->value == IDX_TYPE_NULL) && (stk_result_op1->properties != 0) && (__METAMETHOD__ ==BYTE_CODE_METAMETHOD_NOT_EQU)){\
					VM_PUSH_STK_BOOLEAN(true);\
				}else{\
					VM_PUSH_STK_BOOLEAN(stk_result_op1->value __C_OP__ stk_result_op2->value);\
				}\
			}else{\
				VM_PUSH_STK_BOOLEAN(false);\
			}\
		}else if( STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) && STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){\
			VM_PUSH_STK_BOOLEAN(ZS_STRCMP(stk_to_str(data->zs, stk_result_op1).c_str(), __C_OP__ ,stk_to_str(data->zs,stk_result_op2).c_str()));\
		}else if(  (stk_result_op1->properties==STK_PROPERTY_NULL || stk_result_op2->properties==STK_PROPERTY_NULL)\
				&& (__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
				){\
			if((stk_result_op1->properties == 0) && (stk_result_op2->properties == 0)){\
				VM_PUSH_STK_BOOLEAN(__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU);\
			}else{\
				VM_PUSH_STK_BOOLEAN(__METAMETHOD__ != BYTE_CODE_METAMETHOD_EQU);\
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

#define VM_OPERATION_MOD(__METAMETHOD__) \
	msk_properties=(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties)<<16)|GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op2->properties);\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_INT(stk_result_op1->value % stk_result_op2->value);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(stk_result_op1->value,*((zs_float *)&stk_result_op2->value)));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		if(stk_result_op2->value == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(*((zs_float *)&stk_result_op1->value) , stk_result_op2->value));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		if(*((zs_float *)&stk_result_op2->value) == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		VM_PUSH_STK_ZS_FLOAT(fmod(*((zs_float *)&stk_result_op1->value) , *((zs_float *)&stk_result_op2->value)));\
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

#define VM_OPERATION_NEG_POST(__C_OP__, __METAMETHOD__,__POST_OPERATION_VARIABLE__) \
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);/* always expects ptr stk due it modifies the var */\
	ptr_ptr_void_ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & STK_PROPERTY_IS_VAR_C){\
		ptr_ptr_void_ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
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
			zs_strutils::format("%s '%s' not implements metamethod _neg (aka '-a'') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
			);\
			goto lbl_exit_function;\
		}\
		/* call _neg */\
		VM_INNER_CALL_ONLY_RETURN(\
				so_aux\
				,ptr_metamethod_members_aux->neg\
				,ptr_metamethod_members_aux->neg->function_name.c_str()\
				,true\
		);\
		data->stk_vm_current++; /* store negated value to stk to load after */\
		/* call inc metamethod */\
		if(__POST_OPERATION_VARIABLE__==NULL){\
			zs_strutils::format("%s '%s' not implements metamethod %s (aka '%s') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
					,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
					,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			);\
			goto lbl_exit_function;\
		}\
		VM_INNER_CALL_ONLY_RETURN(\
				so_aux\
				,__POST_OPERATION_VARIABLE__\
				,__POST_OPERATION_VARIABLE__->function_name.c_str()\
				,true\
		);\
	}


#define VM_OPERATION_POST(__C_OP__, __METAMETHOD__,__POST_OPERATION_VARIABLE__) \
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);/* always expects ptr stk due it modifies the var */\
	ptr_ptr_void_ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & STK_PROPERTY_IS_VAR_C){\
		ptr_ptr_void_ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
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
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,ptr_metamethod_members_aux->getter\
					,ptr_metamethod_members_aux->getter->function_name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
		/* call post operation metamethod */\
		if(__POST_OPERATION_VARIABLE__==NULL){\
			zs_strutils::format("%s '%s' not implements metamethod %s (aka '%s') "\
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
					,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
					,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			);\
			goto lbl_exit_function;\
		}\
		VM_INNER_CALL_ONLY_RETURN(\
				so_aux\
				,__POST_OPERATION_VARIABLE__\
				,__POST_OPERATION_VARIABLE__->function_name.c_str()\
				,true\
		);\
	}

#define VM_OPERATION_PRE(__C_OP__, __METAMETHOD__,__PRE_OPERATION_VARIABLE__) \
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);\
	ptr_ptr_void_ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & STK_PROPERTY_IS_VAR_C){\
		ptr_ptr_void_ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
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
			zs_strutils::format("%s '%s' not implements metamethod %s (aka '%s') " \
					,stk_var->properties & STK_PROPERTY_MEMBER_PROPERTY?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction)\
					,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
					,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			);\
			goto lbl_exit_function;\
		}\
		VM_INNER_CALL_ONLY_RETURN(\
				so_aux\
				,__PRE_OPERATION_VARIABLE__\
				,__PRE_OPERATION_VARIABLE__->function_name.c_str()\
				,true\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,ptr_metamethod_members_aux->getter\
					,ptr_metamethod_members_aux->getter->function_name.c_str()\
					,true\
			);\
		}else{ /* store object */ \
			if(stk_var->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				data->stk_vm_current->value=(zs_int)so_aux;\
				data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT;\
			}else{\
				*data->stk_vm_current=stk_var_copy;\
			}\
		}\
		data->stk_vm_current++;\
	}