/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define LOAD_OPS_SET_OPERATION \
stk_result_op1=--data->stk_vm_current;\
stk_result_op1=(StackElement *)stk_result_op1->value; /* dst ptr stk */ \
if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){ \
	stk_result_op1=(StackElement *)(STK_GET_STK_VAR_REF(stk_result_op1)->value); \
} \
stk_result_op2=--data->stk_vm_current;\
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
		if(	STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){\
			((zs_string *)(((ScriptObjectString *)stk_result_op1->value)->value))->append(stk_to_str(data->zs,stk_result_op2));\
			VM_PUSH_STK_SCRIPT_OBJECT(stk_result_op1->value);\
		}else if(STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
					&&\
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)\
		){\
				ScriptObjectObject::append(data->zs, (ScriptObjectObject *)stk_result_op1->value,(ScriptObjectObject *)stk_result_op1->value);\
				VM_PUSH_STK_SCRIPT_OBJECT(stk_result_op1->value);\
		}else{\
			const char *__STR_ADD_SET_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_ADD_SET);\
			const char *__STR_AKA_ADD_SET_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_ADD_SET);\
			LOAD_PROPERTIES(BYTE_CODE_METAMETHOD_ADD_SET); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
			if(ptr_metamethod_members_aux->add_setters.count==0){\
				VM_STOP_EXECUTE("%s '%s' not implements metamethod %s (aka '%s'') " \
						,member_property!=NULL?"Member property":"Symbol" \
						,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
						,byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_ADD_SET)\
				);\
			}\
			ScriptFunction *ptr_function_found=(ScriptFunction *)((Symbol *)((StackElement *)(ptr_metamethod_members_aux->add_setters.items[0]))->value)->ref_ptr;\
			/* find function if c */ \
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
				if((ptr_function_found=vm_find_function( \
					vm \
					,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
					,calling_function\
					,instruction\
					,false\
					,member_property==NULL?"_add_set":(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(BYTE_CODE_METAMETHOD_ADD_SET,member_property->property_name)).c_str()  /* symbol to find */\
					,data->stk_vm_current \
					,1))==NULL\
				){ \
					if(member_property != NULL){ \
						VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->script_type_name.c_str()\
								,member_property->property_name.c_str()\
								,__STR_ADD_SET_METAMETHOD__\
						);\
					}else{\
						VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->script_type_name.c_str() \
								,__STR_ADD_SET_METAMETHOD__\
						);\
					}\
				}\
			}else if(ptr_metamethod_members_aux->add_setters.count>1){\
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_ADD_SET_METAMETHOD__); \
				if(symbol_setter == NULL){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_ADD_SET_METAMETHOD__\
							,__STR_AKA_ADD_SET_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_ADD_SET_METAMETHOD__,__STR_AKA_ADD_SET_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			/* call metamethod  */ \
			VM_INNER_CALL(\
				so_aux\
				,ptr_function_found\
				,ptr_function_found->name_script_function.c_str()\
				,true\
				,1 \
				,true\
			);\
			/*getter after*/\
			if(ptr_metamethod_members_aux->getter!=NULL){\
				/* call _neg */\
				VM_INNER_CALL_ONLY_RETURN(\
						so_aux\
						,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
						,ptr_metamethod_members_aux->getter->name.c_str()\
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
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
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
		{\
			const char *__STR_ARITHMETIC_SET_METAMETHOD__=byte_code_metamethod_to_symbol_str(__METAMETHOD__);\
			const char *__STR_AKA_ARITHMETIC_SET_METAMETHOD__=byte_code_metamethod_to_operator_str(__METAMETHOD__);\
			LOAD_PROPERTIES(__METAMETHOD__); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
			MetamethodMemberSetterInfo setter_info=ptr_metamethod_members_aux->getSetterInfo(__METAMETHOD__);\
			if(setter_info.setters->count==0){\
				VM_STOP_EXECUTE("%s '%s' not implements metamethod %s (aka '%s'') "\
						,member_property!=NULL?"Member property":"Symbol"\
						,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
						,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
						,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
				);\
			}\
			ScriptFunction *ptr_function_found=(ScriptFunction *)((Symbol *)(((StackElement *)setter_info.setters->items[0])->value))->ref_ptr;\
			/* find function if c */ \
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
				if((ptr_function_found=vm_find_function( \
					vm \
					,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
					,calling_function\
					,instruction\
					,false\
					,member_property==NULL?__STR_ARITHMETIC_SET_METAMETHOD__:(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(__METAMETHOD__,member_property->property_name)).c_str()  /* symbol to find */\
					,data->stk_vm_current \
					,1))==NULL\
				){ \
					if(member_property!=NULL){ \
						VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->script_type_name.c_str()\
								,member_property->property_name.c_str()\
								,__STR_ARITHMETIC_SET_METAMETHOD__\
						);\
					}else{\
						VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->script_type_name.c_str() \
								,__STR_ARITHMETIC_SET_METAMETHOD__\
						);\
					}\
				}\
			}else if(setter_info.setters->count>1){\
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_ARITHMETIC_SET_METAMETHOD__); \
				if(symbol_setter == NULL){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_ARITHMETIC_SET_METAMETHOD__\
							,__STR_AKA_ARITHMETIC_SET_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_ARITHMETIC_SET_METAMETHOD__,__STR_AKA_ARITHMETIC_SET_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			/* call metamethod  */ \
			VM_INNER_CALL(\
				so_aux\
				,ptr_function_found\
				,ptr_function_found->name_script_function.c_str()\
				,true\
				,1 \
				,true\
			);\
			/*getter after*/\
			if(ptr_metamethod_members_aux->getter!=NULL){\
				/* call _neg */\
				VM_INNER_CALL_ONLY_RETURN(\
						so_aux\
						,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
						,ptr_metamethod_members_aux->getter->name.c_str()\
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
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
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
		{\
			const char *__STR_DIV_SET_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_DIV_SET);\
			const char *__STR_AKA_DIV_SET_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_DIV_SET);\
			LOAD_PROPERTIES(BYTE_CODE_METAMETHOD_DIV_SET); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
			if(ptr_metamethod_members_aux->div_setters.count==0){\
				VM_STOP_EXECUTE("%s '%s' not implements metamethod %s (aka '%s'') " \
						,member_property!=NULL?"Member property":"Symbol" \
						,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
						,byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_DIV_SET)\
				);\
			}\
			ScriptFunction *ptr_function_found=(ScriptFunction *)((Symbol *)((StackElement *)(ptr_metamethod_members_aux->div_setters.items[0]))->value)->ref_ptr;\
			/* find function if c */ \
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
				if((ptr_function_found=vm_find_function( \
					vm \
					,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
					,calling_function\
					,instruction\
					,false\
					,member_property==NULL?"_div_set":(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(BYTE_CODE_METAMETHOD_DIV_SET,member_property->property_name)).c_str()  /* symbol to find */\
					,data->stk_vm_current \
					,1))==NULL\
				){ \
					if(member_property != NULL){ \
						VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->script_type_name.c_str()\
								,member_property->property_name.c_str()\
								,__STR_DIV_SET_METAMETHOD__\
						);\
					}else{\
						VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->script_type_name.c_str() \
								,__STR_DIV_SET_METAMETHOD__\
						);\
					}\
				}\
			}else if(ptr_metamethod_members_aux->div_setters.count>1){\
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_DIV_SET_METAMETHOD__); \
				if(symbol_setter == NULL){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_DIV_SET_METAMETHOD__\
							,__STR_AKA_DIV_SET_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_DIV_SET_METAMETHOD__,__STR_AKA_DIV_SET_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			/* call metamethod  */ \
			VM_INNER_CALL(\
				so_aux\
				,ptr_function_found\
				,ptr_function_found->name_script_function.c_str()\
				,true\
				,1 \
				,true\
			);\
			/*getter after*/\
			if(ptr_metamethod_members_aux->getter!=NULL){\
				/* call _neg */\
				VM_INNER_CALL_ONLY_RETURN(\
						so_aux\
						,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
						,ptr_metamethod_members_aux->getter->name.c_str()\
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
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
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
		{\
			const char *__STR_MOD_SET_METAMETHOD__=byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_MOD_SET);\
			const char *__STR_AKA_MOD_SET_METAMETHOD__=byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_MOD_SET);\
			LOAD_PROPERTIES(BYTE_CODE_METAMETHOD_MOD_SET); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
			if(ptr_metamethod_members_aux->mod_setters.count==0){\
				VM_STOP_EXECUTE("%s '%s' not implements metamethod %s (aka '%s'') " \
						,member_property!=NULL?"Member property":"Symbol" \
						,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
						,byte_code_metamethod_to_operator_str(BYTE_CODE_METAMETHOD_MOD_SET)\
				);\
			}\
			ScriptFunction *ptr_function_found=(ScriptFunction *)((Symbol *)((StackElement *)(ptr_metamethod_members_aux->mod_setters.items[0]))->value)->ref_ptr;\
			/* find function if c */ \
			if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
				if((ptr_function_found=vm_find_function( \
					vm \
					,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
					,calling_function\
					,instruction\
					,false\
					,member_property==NULL?"_mod_set":(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(BYTE_CODE_METAMETHOD_MOD_SET,member_property->property_name)).c_str()  /* symbol to find */\
					,data->stk_vm_current \
					,1))==NULL\
				){ \
					if(member_property != NULL){ \
						VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
								,so_aux->getScriptType()->script_type_name.c_str()\
								,member_property->property_name.c_str()\
								,__STR_MOD_SET_METAMETHOD__\
						);\
					}else{\
						VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
								,so_aux->getScriptType()->script_type_name.c_str() \
								,__STR_MOD_SET_METAMETHOD__\
						);\
					}\
				}\
			}else if(ptr_metamethod_members_aux->mod_setters.count>1){\
				Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_MOD_SET_METAMETHOD__); \
				if(symbol_setter == NULL){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
							,__STR_MOD_SET_METAMETHOD__\
							,__STR_AKA_MOD_SET_METAMETHOD__\
					);\
				}\
				if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
					VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_MOD_SET_METAMETHOD__,__STR_AKA_MOD_SET_METAMETHOD__);\
				}\
				ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
			}\
			/* call metamethod  */ \
			VM_INNER_CALL(\
				so_aux\
				,ptr_function_found\
				,ptr_function_found->name_script_function.c_str()\
				,true\
				,1 \
				,true\
			);\
			/*getter after*/\
			if(ptr_metamethod_members_aux->getter!=NULL){\
				/* call _neg */\
				VM_INNER_CALL_ONLY_RETURN(\
						so_aux\
						,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
						,ptr_metamethod_members_aux->getter->name.c_str()\
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
		}\
		break;\
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
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
		const char *__STR_ARITHMETIC_SET_METAMETHOD__=byte_code_metamethod_to_symbol_str(__METAMETHOD__);\
		const char *__STR_AKA_ARITHMETIC_SET_METAMETHOD__=byte_code_metamethod_to_operator_str(__METAMETHOD__);\
		LOAD_PROPERTIES(__METAMETHOD__); /* saves stk_var_copy --> stk_vm_current points to stk_result_op2 that is the a parameter to pass */\
		MetamethodMemberSetterInfo setter_info=ptr_metamethod_members_aux->getSetterInfo(__METAMETHOD__);\
		if(setter_info.setters->count==0){\
			VM_STOP_EXECUTE("%s '%s' not implements metamethod %s (aka '%s'') "\
					,member_property!=NULL?"Member property":"Symbol"\
					,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)\
					,byte_code_metamethod_to_symbol_str(__METAMETHOD__)\
					,byte_code_metamethod_to_operator_str(__METAMETHOD__)\
			);\
		}\
		ScriptFunction *ptr_function_found=(ScriptFunction *)((Symbol *)(((StackElement *)setter_info.setters->items[0])->value))->ref_ptr;\
		/* find function if c */ \
		if(ptr_function_found->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ /* because object is native, we can have more than one _setter */ \
			if((ptr_function_found=vm_find_function( \
				vm \
				,data->script_type_factory->getScriptType(so_aux->idx_script_type)\
				,calling_function\
				,instruction\
				,false\
				,member_property==NULL?__STR_ARITHMETIC_SET_METAMETHOD__:(ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(__METAMETHOD__,member_property->property_name)).c_str()  /* symbol to find */\
				,data->stk_vm_current \
				,1))==NULL\
			){ \
				if(member_property!=NULL){ \
					VM_STOP_EXECUTE("Property '%s::%s' does not implement metamethod '%s'"\
							,so_aux->getScriptType()->script_type_name.c_str()\
							,member_property->property_name.c_str()\
							,__STR_ARITHMETIC_SET_METAMETHOD__\
					);\
				}else{\
					VM_STOP_EXECUTE("Class '%s' does not implement '%s' metamethod" \
							,so_aux->getScriptType()->script_type_name.c_str() \
							,__STR_ARITHMETIC_SET_METAMETHOD__\
					);\
				}\
			}\
		}else if(setter_info.setters->count>1){\
			Symbol * symbol_setter = so_aux->getScriptType()->getSymbol(__STR_ARITHMETIC_SET_METAMETHOD__); \
			if(symbol_setter == NULL){\
				VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not implemented"\
						,__STR_ARITHMETIC_SET_METAMETHOD__\
						,__STR_AKA_ARITHMETIC_SET_METAMETHOD__\
				);\
			}\
			if((symbol_setter->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)==0){\
				VM_STOP_EXECUTE("Operator metamethod '%s' (aka %s) is not function",__STR_ARITHMETIC_SET_METAMETHOD__,__STR_AKA_ARITHMETIC_SET_METAMETHOD__);\
			}\
			ptr_function_found=(ScriptFunction *)symbol_setter->ref_ptr;\
		}\
		/* call metamethod  */ \
		VM_INNER_CALL(\
			so_aux\
			,ptr_function_found\
			,ptr_function_found->name_script_function.c_str()\
			,true\
			,1 \
			,true\
		);\
		/*getter after*/\
		if(ptr_metamethod_members_aux->getter!=NULL){\
			/* call _neg */\
			VM_INNER_CALL_ONLY_RETURN(\
					so_aux\
					,(ScriptFunction *)ptr_metamethod_members_aux->getter->ref_ptr\
					,ptr_metamethod_members_aux->getter->name.c_str()\
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
	}\
	if(instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK){\
		data->stk_vm_current=stk_start;\
	}\

