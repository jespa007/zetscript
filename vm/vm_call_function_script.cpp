#define VM_INNER_ONLY_RETURN_CALL(so,sf,name,reset_offset)\
{\
StackElement *stk_def_afun_start=data->stk_vm_current;\
int n_returned_args_afun=0;\
if(((ScriptFunction *)sf)->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){\
	vm_call_function_native(\
			vm\
			,so\
			,((ScriptFunction *)sf)\
			,stk_def_afun_start\
			,0\
			,instruction\
	);\
}else{\
	vm_call_function_script(\
		vm\
		,so\
		,((ScriptFunction *)sf)\
		,stk_def_afun_start\
	);\
}\
if(data->vm_error == true){ \
    data->vm_error_callstack_str+=zs_strutils::format(\
        "\nat %s (file:%s line:%i)" /* TODO: get full symbol ? */ \
        , name \
        ,SFI_GET_FILE(calling_function,instruction)\
        ,SFI_GET_LINE(calling_function,instruction)\
    );\
    goto lbl_exit_function;\
}\
n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;\
/* we share pointer (true second arg) to not remove on pop in calling return */\
CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,true) \
/* reset stack */\
data->stk_vm_current=stk_def_afun_start+(reset_offset); \
}

#define PROCESS_MOD_OPERATION \
	msk_properties=(stk_result_op1->properties<<16)|stk_result_op2->properties;\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) % op2_int);\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_FLOAT(fmod(STK_VALUE_TO_ZS_INT(stk_result_op1),f_aux_value2));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		PUSH_FLOAT(fmod(f_aux_value1 , op2_int));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception mod operation by 0");\
		}\
		PUSH_FLOAT(fmod(f_aux_value1 , f_aux_value2));\
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

#define PROCESS_ARITHMETIC_DIV_OPERATION \
	msk_properties=(stk_result_op1->properties<<16)|stk_result_op2->properties;\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) / STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) / f_aux_value2);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		op2_int=STK_VALUE_TO_ZS_INT(stk_result_op2);\
		if(op2_int == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		PUSH_FLOAT(f_aux_value1 / op2_int);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		if(f_aux_value2 == 0){\
			VM_STOP_EXECUTE("exception div operation by 0");\
		}\
		PUSH_FLOAT(f_aux_value1 / f_aux_value2);\
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

#define PROCESS_ARITHMETIC_OPERATION(__C_OP__, __METAMETHOD__)\
	msk_properties=(stk_result_op1->properties<<16)|stk_result_op2->properties;\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		PUSH_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ f_aux_value2);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		PUSH_FLOAT(f_aux_value1 __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		PUSH_FLOAT(f_aux_value1 __C_OP__ f_aux_value2);\
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




#define PROCESS_COMPARE_OPERATION(__C_OP__, __METAMETHOD__)\
	msk_properties=(stk_result_op1->properties<<16)|stk_result_op2->properties;\
	switch(msk_properties){\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_INT:\
		PUSH_BOOLEAN(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_BOOL_OP2_BOOL:\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		PUSH_BOOLEAN(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ f_aux_value2);\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		PUSH_BOOLEAN(f_aux_value1 __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
		break;\
	case MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT:\
		ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);\
		ZS_FLOAT_COPY(&f_aux_value2,&stk_result_op2->value);\
		PUSH_BOOLEAN(f_aux_value1 __C_OP__ f_aux_value2);\
		break;\
	default:\
		if( STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) && STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){\
			PUSH_BOOLEAN(ZS_STRCMP(stk_result_op1->toString().c_str(), __C_OP__ ,stk_result_op2->toString().c_str()));\
		}else if(  (stk_result_op1->properties==MSK_STK_PROPERTY_NULL || stk_result_op2->properties==MSK_STK_PROPERTY_NULL)\
				&& (__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
				){\
			if((stk_result_op1->properties&stk_result_op2->properties) == MSK_STK_PROPERTY_NULL){\
				PUSH_BOOLEAN(true  __C_OP__  true);\
			}else{\
				PUSH_BOOLEAN(false);\
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


#define PROCESS_LOGIC_OPERATION(__C_OP__)\
	if((stk_result_op1->properties&stk_result_op2->properties) == MSK_STK_PROPERTY_BOOL){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(ZS_STR(__C_OP__));\
		goto lbl_exit_function;\
	}\


#define PROCESS_BINARY_OPERATION(__C_OP__, __METAMETHOD__)\
	if((stk_result_op1->properties&stk_result_op2->properties) == MSK_STK_PROPERTY_ZS_INT){\
		PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
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

#define PERFORM_POST_OPERATOR(__PRE_OP__,__OPERATOR__, __METAMETHOD__) \
{\
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);\
	void **ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & MSK_STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case MSK_STK_PROPERTY_ZS_INT:\
			PUSH_INTEGER(__PRE_OP__(*((zs_int *)(ref))));\
			(*((zs_int *)(ref)))__OPERATOR__;\
			break;\
	case MSK_STK_PROPERTY_ZS_FLOAT:\
			PUSH_FLOAT(__PRE_OP__(*((zs_float *)(ref))));\
			(*((zs_float *)(ref)))__OPERATOR__;\
			break;\
	default:\
		if(vm_apply_metamethod(\
			vm\
			,calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_var\
			,NULL\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
}

#define PERFORM_PRE_OPERATOR(__OPERATOR__, __METAMETHOD__) \
{\
	stk_var=--data->stk_vm_current;\
	stk_var=(StackElement *)((stk_var)->value);\
	void **ref=(void **)(&((stk_var)->value));\
	if(stk_var->properties & MSK_STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case MSK_STK_PROPERTY_ZS_INT:\
			(*((zs_int *)(ref)))__OPERATOR__;\
			PUSH_INTEGER(*((zs_int *)(ref)));\
			break;\
	case MSK_STK_PROPERTY_ZS_FLOAT:\
			(*((zs_float *)(ref)))__OPERATOR__;\
			PUSH_FLOAT(*((zs_float *)(ref)));\
			break;\
	default:\
		if(vm_apply_metamethod(\
			vm\
			,calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_var\
			,NULL\
		)==false){\
			goto lbl_exit_function;\
		}\
		break;\
	}\
}

#define PUSH_VM_SCOPE(_scope,_ptr_info_function, _ptr_local_var,_properties)\
	if(data->vm_current_scope >=  data->vm_scope_max){THROW_RUNTIME_ERROR("reached max scope");}\
	data->vm_current_scope->scope=(Scope *)_scope;\
	data->vm_current_scope->script_function=_ptr_info_function;\
	data->vm_current_scope->stk_local_vars=_ptr_local_var;\
	data->vm_current_scope->properties=_properties;\
	data->vm_current_scope++;\

#define LOAD_FROM_STACK(offset,properties) \
	 ((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL) ? _stk_local_var+offset \
	:((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER) ? this_object->getBuiltinElementAt(offset) \
	:data->vm_stack + offset\

#define POP_TWO \
    switch(instruction->properties & MSK_INSTRUCTION_PROPERTY_ILOAD){\
    default:\
    case 0:\
		stk_result_op2=--data->stk_vm_current;\
		stk_result_op1=--data->stk_vm_current;\
		break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_K: /* only perfom with one constant*/\
		 stk_result_op1=--data->stk_vm_current;\
		 stk_result_op2=&stk_aux;\
		 stk_result_op2->value=(void *)instruction->value_op2;\
		 stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
         break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
		 stk_result_op1=--data->stk_vm_current;\
         stk_result_op2=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
         break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
		 stk_result_op1=&stk_aux;\
		 stk_result_op1->value=(void *)instruction->value_op2;\
		 stk_result_op1->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
         stk_result_op2=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
         break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
        stk_result_op1=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
		stk_result_op2=&stk_aux;\
		stk_result_op2->value=(void *)instruction->value_op2;\
		stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
        break;\
   case MSK_INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
        stk_result_op1=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
        stk_result_op2=LOAD_FROM_STACK(((instruction->value_op2&0xff0000)>>16),instruction->value_op2);\
        break;\
    }


#define READ_TWO_POP_ONE \
stk_result_op2=--data->stk_vm_current;\
stk_result_op1=(data->stk_vm_current-1);

#define POP_ONE \
stk_result_op1=--data->stk_vm_current;

#define LOAD_STK(stk_ptr) \
	data->stk_vm_current->value=(stk_ptr)->value;\
	data->stk_vm_current->properties=(stk_ptr)->properties;\
	data->stk_vm_current++;


#define PUSH_STK_PTR(stk_ptr) \
	data->stk_vm_current->value=(stk_ptr);\
	data->stk_vm_current->properties=MSK_STK_PROPERTY_PTR_STK;\
	data->stk_vm_current++;

#define CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_return, n_return,with_share)\
	for(int i=0; i < n_return; i++){\
		StackElement *stk_ret = stk_return+i;\
		if(stk_ret->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *sv=(ScriptObject *)stk_ret->value;\
			if(sv->shared_pointer == NULL){\
				if(!vm_create_shared_pointer(vm,sv)){\
					goto lbl_exit_function;\
				}\
				if(with_share==true){\
					if(!vm_share_pointer(vm,sv)){\
						goto lbl_exit_function;\
					}\
				}\
			}\
		}\
	}



namespace zetscript{

	void vm_call_function_script(
			VirtualMachine			* vm,
			ScriptObject			* this_object,
			ScriptFunction 			* calling_function,
			StackElement 		  	* _stk_local_var,
			unsigned char 			n_args=0
	    ){

		VirtualMachineData *data = (VirtualMachineData*)vm->data;
		Instruction * instruction = NULL;//calling_instruction;


		if(data->vm_idx_call >= MAX_FUNCTION_CALL){
			VM_ERROR_AND_RET("Reached max stack");
		}


		ScriptObject *so_aux=NULL;
		zs_float			f_aux_value1,f_aux_value2;
		//ScriptObject *so_object_aux=NULL;
		ScriptObjectClass *so_class_aux=NULL;
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement stk_aux=stk_undefined;
		StackElement *stk_var;
		const char *str_symbol=NULL,*str_aux=NULL;
		Symbol * symbol_aux=NULL;
		int idx_stk_element;
		unsigned short pre_post_properties=0;
		unsigned short scope_type=0;
		uint32_t msk_properties=0;
		int op2_int=0;
		StackElement *stk_dst=NULL;
		StackElement *stk_src=NULL;
		Instruction *instructions=calling_function->instructions; // starting instruction
		Instruction *instruction_it=instructions;
		VM_Scope * vm_scope_start=data->vm_current_scope; // save current scope...
		Scope * scope = calling_function->symbol.scope;// ast->idx_scope;
		zs_vector *registered_symbols=calling_function->registered_symbols;
		unsigned symbols_count=registered_symbols->count;
		StackElement *stk_start=&_stk_local_var[symbols_count];   // <-- here starts stk for aux vars for operations ..
		StackElement *ptr_aux = _stk_local_var+n_args;

		if(instructions==NULL){
			return;
		}

		if(stk_start+calling_function->min_stack_needed >= &data->vm_stack[VM_STACK_LOCAL_VAR_MAX-1]){
			VM_STOP_EXECUTE("Error MAXIMUM stack size reached");
		}

		data->stk_vm_current = stk_start;
		data->vm_idx_call++;

#ifdef __DEBUG__
		ZS_PRINT_DEBUG("Executing function %s ...",calling_function->symbol.name.c_str());
#endif

		// Init local vars ...
		if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN && (data->vm_idx_call > 1)){

			for(unsigned i = n_args; i < (symbols_count); i++){ // from n_args, setup local vars
				symbol_aux=(Symbol *)registered_symbols->items[i];
				if(symbol_aux->properties & SYMBOL_PROPERTY_FUNCTION){
					ptr_aux->value=(void *)symbol_aux->ref_ptr;
					ptr_aux->properties=MSK_STK_PROPERTY_FUNCTION;
				}else{
					STK_SET_NULL(ptr_aux);		// null as default
				}
				ptr_aux++;
			}
		}

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// MAIN LOOP
		//
		 for(;;){ // foreach byte code instruction ...

			instruction = instruction_it++;

			switch(instruction->byte_code){
			default:
				VM_STOP_EXECUTE("byte code \"%s\" not implemented",byte_code_to_str(instruction->byte_code));
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_FIND_VARIABLE:

				str_symbol=SFI_GET_SYMBOL_NAME(calling_function,instruction);

				if((str_aux=strstr(str_symbol,"::")) != NULL){ // static access
					ScriptClass *static_class=NULL;
					std::string static_error;
					char copy_aux[512]={0};

					// get class
					strncpy(copy_aux,str_symbol,str_aux-str_symbol);

					static_class=data->zs->getScriptClassFactory()->getScriptClass(copy_aux);

					if(static_class==NULL){
						VM_STOP_EXECUTE("Cannot link static access '%s' class '%s' not exist"
							,str_symbol
							,copy_aux
						);
					}

					// advance ::
					str_aux+=2;

					//get member
					strcpy(copy_aux,str_aux);

					symbol_aux=static_class->getSymbol(copy_aux); // ... and member as well we can define the instruction here

					if(symbol_aux == NULL){
						VM_STOP_EXECUTE("Cannot link static access '%s' class '%s' not exist"
							,str_symbol
							,copy_aux
						);
					}

					if(!eval_set_instruction_static_symbol(instruction,symbol_aux,static_error)){

						VM_STOP_EXECUTE("Symbol \"%s\" %s"
							,str_symbol
							,static_error.c_str()
						);
					}

				}else{
					symbol_aux = data->main_function_object->getSymbol(MAIN_SCOPE(data),str_symbol);//, NO_PARAMS_SYMBOL_ONLY, ScopeDirection::SCOPE_DIRECTION_DOWN);
				}

				if(symbol_aux != NULL){
					if(symbol_aux->n_params==NO_PARAMS_SYMBOL_ONLY){ // variable
						instruction->value_op2=symbol_aux->idx_position; // save idx pos
						if(instruction->properties & MSK_INSTRUCTION_USE_PUSH_STK){
							instruction->byte_code=BYTE_CODE_PUSH_STK_GLOBAL;
							PUSH_STK_PTR(data->vm_stack + instruction->value_op2);
						}else{
							instruction->byte_code=BYTE_CODE_LOAD_GLOBAL;
							*data->stk_vm_current++=*(data->vm_stack+instruction->value_op2);
						}

					}else{ // function
						// assign script function ...
						instruction->byte_code=BYTE_CODE_LOAD_FUNCTION;
						instruction->value_op2=symbol_aux->ref_ptr;
						PUSH_FUNCTION(instruction->value_op2);
					}
				}else{ // load null as default!
					VM_STOP_EXECUTE("Symbol \"%s\" is not defined",str_symbol);
				}

				continue;
			// store
			case BYTE_CODE_PUSH_STK_GLOBAL: // load variable ...
				PUSH_STK_PTR(data->vm_stack + instruction->value_op2);
				continue;
			case BYTE_CODE_PUSH_STK_LOCAL: // load variable ...
				PUSH_STK_PTR(_stk_local_var+instruction->value_op2);
				continue;
			case BYTE_CODE_PUSH_STK_REF:
				PUSH_STK_PTR(STK_GET_STK_VAR_REF(_stk_local_var+instruction->value_op2));
				continue;
			case BYTE_CODE_PUSH_STK_THIS: // load variable ...
				PUSH_STK_PTR(this_object->getThisProperty());
				continue;
			case BYTE_CODE_PUSH_STK_MEMBER_VAR: // direct load
				PUSH_STK_PTR(this_object->getBuiltinElementAt(instruction->value_op2));
				continue;
			case BYTE_CODE_PUSH_STK_ELEMENT_VECTOR:
			case BYTE_CODE_LOAD_ELEMENT_VECTOR:
				POP_TWO;
				so_aux=NULL;
				if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){
					stk_result_op1 = ((ScriptObjectVarRef *)stk_result_op1->value)->getStackElementPtr();
				}
				stk_var=NULL;
				/*if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){ \
					VM_STOP_EXECUTE("Expected integer index for Vector or String access");
				}*/
				// determine object ...
				if(stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					ScriptObject *obj=(ScriptObject *)stk_result_op1->value;
					if(		   obj->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR
							|| obj->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
							|| obj->idx_script_class>=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS
					){

						if(obj->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR){
							ScriptObjectVector *so_vector=(ScriptObjectVector *)obj;

							if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){ \
								VM_STOP_EXECUTE("Expected integer index for Vector access");
							}

							if((stk_var =so_vector->getUserElementAt(STK_VALUE_TO_ZS_INT(stk_result_op2)))==NULL){
								goto lbl_exit_function;
							} \
						}
						else{
							ScriptObjectObject *so_object = (ScriptObjectObject *)obj;
							if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)==false){ \
								VM_STOP_EXECUTE("Expected string for object access");
							}
							stk_var = so_object->getProperty(stk_result_op2->toString());
							if(stk_var == NULL){
								if(instruction->byte_code == BYTE_CODE_PUSH_STK_ELEMENT_VECTOR){
									if((stk_var =so_object->addProperty(stk_result_op2->toString(), data->vm_error_str))==NULL){
										VM_STOP_EXECUTE(data->vm_error_str.c_str());
									}
								}
							}

						}
						if(instruction->byte_code == BYTE_CODE_LOAD_ELEMENT_VECTOR){
							*data->stk_vm_current++=*stk_var;
						}else{
							PUSH_STK_PTR(stk_var);
						}

						continue;

					}else if(obj->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING){
						ScriptObjectString *so_string=(ScriptObjectString *)stk_result_op1->value;

						if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){ \
							VM_STOP_EXECUTE("Expected integer index for String access");
						}


						zs_char *ptr_char=(zs_char *)&((std::string *)so_string->value)->c_str()[STK_VALUE_TO_ZS_INT(stk_result_op2)];
						if(instruction->byte_code == BYTE_CODE_LOAD_ELEMENT_VECTOR){
							data->stk_vm_current->value=(void *)((zs_int)(*ptr_char));
							data->stk_vm_current->properties=MSK_STK_PROPERTY_ZS_INT;
						}else{ // push stk
							data->stk_vm_current->value=ptr_char;
							data->stk_vm_current->properties=MSK_STK_PROPERTY_ZS_CHAR | MSK_STK_PROPERTY_IS_VAR_C;
						}
						data->stk_vm_current++;
						continue;
					}else{
						VM_STOP_EXECUTE("Expected String,Vector or Object for access \"[]\" opertaion"); \
					}
				}else{
					VM_STOP_EXECUTE("Expected object for access \"[]\" opertaion"); \

				}

				continue;
			// load
			case BYTE_CODE_LOAD_GLOBAL: // load variable ...
				*data->stk_vm_current++=*(data->vm_stack+instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_LOCAL: // load variable ...
				*data->stk_vm_current++=*(_stk_local_var+instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_REF:
				*data->stk_vm_current++=*STK_GET_STK_VAR_REF(_stk_local_var+instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_THIS: // load variable ...
				*data->stk_vm_current++=*this_object->getThisProperty();
				continue;
			case BYTE_CODE_LOAD_MEMBER_VAR: // direct load
				*data->stk_vm_current++=*this_object->getBuiltinElementAt(instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_CONSTRUCTOR:
				so_class_aux=(ScriptObjectClass *)((data->stk_vm_current-1)->value);
				*data->stk_vm_current++=*(so_class_aux->getBuiltinElementAt(instruction->value_op2));
				continue;

			case BYTE_CODE_PUSH_STK_ELEMENT_OBJECT:
			case BYTE_CODE_PUSH_STK_ELEMENT_THIS:
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:
			case BYTE_CODE_LOAD_ELEMENT_THIS:
load_element_object:

				stk_var=NULL;
				so_aux=this_object; // take this as default
				if(instruction->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT || instruction->byte_code == BYTE_CODE_PUSH_STK_ELEMENT_OBJECT){

					Instruction *previous_ins= (instruction-1);

					if(previous_ins->byte_code == BYTE_CODE_NEW_CLASS){
						stk_result_op1=(data->stk_vm_current-1);
					}
					else{
						POP_ONE; // get var op1 and symbol op2
					}

					if((stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)!= MSK_STK_PROPERTY_SCRIPT_OBJECT)
					{
						VM_STOP_EXECUTE(
							"Cannot perform access operation [ ... %s.%s ]. Expected \"%s\" as object but is type \"%s\""
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,stk_result_op1->typeStr()
						);
					}

					so_aux=((ScriptObject *)stk_result_op1->value);


					if(so_aux == NULL)
					{
						VM_STOP_EXECUTE("var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(so_aux,previous_ins));
					}


				}else{ // if LOAD THIS OR PUSH_STK THIS ...
					if(instruction->value_op2>=0){ // on dynamic members fix idx when we find a C function
						stk_var=this_object->getBuiltinElementAt(instruction->value_op2);
					}
				}

				if(stk_var==NULL){ // load element from object or dynamic member element from this
					str_symbol=(char *)SFI_GET_SYMBOL_NAME(calling_function,instruction);

					if((stk_var=so_aux->getProperty(str_symbol, &idx_stk_element)) == NULL){
						// something went wrong
						if(data->vm_error == true){
							goto lbl_exit_function;
						}
						//------------------------------------------------------------------
						// pack member info for store information...
						if(instruction->properties & MSK_INSTRUCTION_USE_PUSH_STK){
							// save
							if((stk_var=so_aux->addProperty((const char *)str_symbol, data->vm_error_str))==NULL){
								VM_STOP_EXECUTE(data->vm_error_str.c_str());
							}
							PUSH_STK_PTR(stk_var);
						}
						else{
							data->stk_vm_current->value=0;
							data->stk_vm_current->properties=MSK_STK_PROPERTY_NULL;
							data->stk_vm_current++;
						}
						continue;
					}else{
						if(stk_var->properties & MSK_STK_PROPERTY_MEMBER_ATTRIBUTE){
							if((instruction->properties & MSK_INSTRUCTION_USE_PUSH_STK)==0){ // call getter if exist
								StackMemberAttribute *stk_ma=(StackMemberAttribute *)stk_var->value;
								if(stk_ma->member_attribute->getter != NULL){

									VM_INNER_ONLY_RETURN_CALL(stk_ma->so_object
											,stk_ma->member_attribute->getter
											,stk_ma->member_attribute->getter->symbol.name.c_str()
											,0);
									/*StackElement *stk_def_afun_start=data->stk_vm_current;
									int n_returned_args_afun=0;


									vm_call_function_script(
										 vm
										,stk_ma->so_object
										,stk_ma->member_attribute->getter
										,stk_def_afun_start
									);

									if(data->vm_error == true){
										data->vm_error_callstack_str+=zs_strutils::format(
											"\nat %s (file:%s line:%i)" // TODO: get full symbol ?
											,stk_ma->member_attribute->getter->symbol.name.c_str()
											,SFI_GET_FILE(calling_function,instruction)
											,SFI_GET_LINE(calling_function,instruction)
										);
										goto lbl_exit_function;
									}

									n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;

									CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,true) // we share pointer (true second arg) to not remove on pop in calling return


									stk_var=stk_def_afun_start;
									data->stk_vm_current=stk_def_afun_start; // reset stack -11*/
								}
							}
						}
					}
				}

				// copy the content of this value
				if(instruction->byte_code == BYTE_CODE_PUSH_STK_ELEMENT_OBJECT || instruction->byte_code == BYTE_CODE_PUSH_STK_ELEMENT_THIS){ // push ref because is gonna to be assigned
					PUSH_STK_PTR(stk_var);
				}else{ // load its value for read
					*data->stk_vm_current++=*stk_var;
				}

				if((instruction+1)->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT){ // fast load access without pass through switch instruction
					instruction++;
					goto load_element_object;
				}

				continue;
			case BYTE_CODE_LOAD_NULL:
				PUSH_NULL;
				continue;
			case BYTE_CODE_LOAD_FUNCTION: // expect constant and function has the same behaviour...
				PUSH_FUNCTION(instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_ZS_INT:
				data->stk_vm_current->value=(void *)instruction->value_op2;
				data->stk_vm_current->properties=MSK_STK_PROPERTY_ZS_INT;
				data->stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_FLOAT:
				data->stk_vm_current->value=(void *)instruction->value_op2;
				data->stk_vm_current->properties=MSK_STK_PROPERTY_ZS_FLOAT;
				data->stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_BOOL:
				data->stk_vm_current->value=(void *)instruction->value_op2;
				data->stk_vm_current->properties=MSK_STK_PROPERTY_BOOL;
				data->stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_STACK_ELEMENT:
			case BYTE_CODE_LOAD_STRING:
				*data->stk_vm_current++=*((StackElement *)instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_CLASS:
				PUSH_CLASS(instruction->value_op2);
				continue;
			case BYTE_CODE_STORE:
			case BYTE_CODE_PUSH_VECTOR_ELEMENT:
			case BYTE_CODE_PUSH_OBJECT_ELEMENT:
			case BYTE_CODE_STORE_CONST:
			case BYTE_CODE_STORE_ADD:
			case BYTE_CODE_STORE_SUB:
			case BYTE_CODE_STORE_MUL:
			case BYTE_CODE_STORE_DIV:
			case BYTE_CODE_STORE_MOD:
			case BYTE_CODE_STORE_BITWISE_AND:
			case BYTE_CODE_STORE_BITWISE_OR:
			case BYTE_CODE_STORE_BITWISE_XOR:
			case BYTE_CODE_STORE_SHL:
			case BYTE_CODE_STORE_SHR:

				{
					zs_vector *lst_functions=NULL;
					ScriptObject *obj_setter=NULL;

					char n_elements_left=0;
					StackElement *stk_multi_var_src=NULL;

					if(instruction->byte_code==BYTE_CODE_PUSH_VECTOR_ELEMENT){
						POP_ONE; // only pops the value, the last is the std::vector variable itself
						ScriptObjectObject *vec_obj = NULL;
						if((data->stk_vm_current-1)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
							vec_obj = (ScriptObjectObject *)(data->stk_vm_current-1)->value;
							if(vec_obj->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR){ // push value ...
								// op1 is now the src value ...
								stk_src=stk_result_op1;
								if(stk_src->properties & MSK_STK_PROPERTY_PTR_STK){
									stk_src=(StackElement *)stk_result_op1->value;
								}

								stk_dst=((ScriptObjectVector *)vec_obj)->pushNewUserSlot();
							}
						}

						if(vec_obj==NULL){
							VM_STOP_EXECUTE("Expected std::vector object");
						}
					}else if(instruction->byte_code==BYTE_CODE_PUSH_OBJECT_ELEMENT){

						POP_TWO; // first must be a string that describes variable name and the other the variable itself ...
						ScriptObjectObject *obj = NULL;
						StackElement *stk_object=(data->stk_vm_current-1);
						if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_object) == 0){
							VM_STOP_EXECUTE("Expected object but is type \"%s\"",stk_object->typeStr());
						}

						obj = (ScriptObjectObject *)stk_object->value;

						if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) == 0){
							VM_STOP_EXECUTE("Internal: Expected stk_result_op1 as string but is type \"%s\"",stk_result_op1->typeStr());
						}
								// op1 is now the src value ...
						StackElement *se=NULL;
						//const char *str = (const char *)stk_result_op1->value;
						stk_src=stk_result_op2;
						if((se =obj->addProperty(stk_result_op1->toString(),data->vm_error_str))==NULL){
							VM_STOP_EXECUTE(data->vm_error_str.c_str());
						}

						stk_dst=se;

					}
					else{ // can be assign or arithmetic and assign pop two parameters nothing ...

						n_elements_left=(char)instruction->value_op1;

						if(instruction->byte_code==BYTE_CODE_STORE && n_elements_left > 0){

							StackElement *stk_aux=data->stk_vm_current;

							//stk_vm_current--; // set to 0

							// check assigname stk and dec stk_aux
							for(int i=0; i < n_elements_left; i++){

								stk_aux--;

								if ((stk_aux->properties & MSK_STK_PROPERTY_PTR_STK) == 0){
									VM_STOP_EXECUTE("Expected stk ptr");
								}
							}

							stk_multi_var_src=stk_aux-n_elements_left-1; // now pos - n_elements
							n_elements_left=n_elements_left-1;
							stk_result_op2=--data->stk_vm_current;
							stk_result_op1=++stk_multi_var_src;


						}else{


							if(IS_BYTE_CODE_STORE_WITH_OPERATION(instruction->byte_code)){ // arithmetic
								StackElement stk_aux=*(data->stk_vm_current-1); // save dst value
								// get value

								//POP_TWO
								stk_result_op1=--data->stk_vm_current;
								stk_result_op2=--data->stk_vm_current;

								//-------- get stk from var
								if(stk_result_op1->properties & MSK_STK_PROPERTY_PTR_STK){
									stk_result_op1=(StackElement *)stk_result_op1->value;
								}

								if(stk_result_op2->properties & MSK_STK_PROPERTY_PTR_STK){
									stk_result_op2=(StackElement *)stk_result_op2->value;
								}

								//------ get stk from ref
								// check if by ref
								if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op1)){
									stk_result_op1=(StackElement *)((STK_GET_STK_VAR_REF(stk_result_op1)->value));
								}

								// check if by ref
								if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_result_op2)){
									stk_result_op2=(StackElement *)(STK_GET_STK_VAR_REF(stk_result_op2)->value);
								}


								switch(instruction->byte_code){
								case BYTE_CODE_STORE_ADD:
									PROCESS_ARITHMETIC_OPERATION(+,BYTE_CODE_METAMETHOD_ADD);
									break;
								case BYTE_CODE_STORE_SUB:
									PROCESS_ARITHMETIC_OPERATION(-,BYTE_CODE_METAMETHOD_SUB);
									break;
								case BYTE_CODE_STORE_MUL:
									PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
									break;
								case BYTE_CODE_STORE_DIV:
									PROCESS_ARITHMETIC_DIV_OPERATION;
									break;
								case BYTE_CODE_STORE_MOD:
									PROCESS_MOD_OPERATION;
									break;
								case BYTE_CODE_STORE_BITWISE_AND:
									PROCESS_BINARY_OPERATION(&,BYTE_CODE_METAMETHOD_ADD);
									break;
								case BYTE_CODE_STORE_BITWISE_OR:
									PROCESS_BINARY_OPERATION(|,BYTE_CODE_METAMETHOD_OR);
									break;
								case BYTE_CODE_STORE_BITWISE_XOR:
									PROCESS_BINARY_OPERATION(^,BYTE_CODE_METAMETHOD_XOR);
									break;
								case BYTE_CODE_STORE_SHL:
									PROCESS_BINARY_OPERATION(<<,BYTE_CODE_METAMETHOD_SHL);
									break;
								case BYTE_CODE_STORE_SHR:
									PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
									break;

								}

								// push var again
								*data->stk_vm_current++=stk_aux;
							}

							READ_TWO_POP_ONE
						}

	vm_store_next:
						lst_functions=NULL;
						obj_setter=NULL;
						stk_src=stk_result_op1; // store ptr instruction2 op as src_var_value
						stk_dst=stk_result_op2;


						//---- get stk var
						if((stk_dst->properties & MSK_STK_PROPERTY_PTR_STK)!=0) {
							stk_dst=(StackElement *)stk_dst->value; // value is expect to contents a stack variable
						}else {
							if((stk_dst->properties & MSK_STK_PROPERTY_IS_VAR_C)==0){
								VM_STOP_EXECUTE("Expected l-value on assignment but it was type \"%s\"",stk_dst->typeStr());
							}
						}

						// check if by ref
						if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_dst)){
							stk_dst=(StackElement *)(STK_GET_STK_VAR_REF(stk_dst)->value);
						}
						//-----------------------
						if(stk_dst->properties & MSK_STK_PROPERTY_READ_ONLY){
							VM_STOP_EXECUTE("Assignment to constant variable");
						}

						// ok load object pointer ...
						if(STK_IS_SCRIPT_OBJECT_CLASS(stk_dst)){
							ScriptObjectClass *script_object_class=((ScriptObjectClass *)stk_dst->value);
							if(script_object_class->itHasSetMetamethod()){
								lst_functions=script_object_class->getStkBuiltinListElements();
								obj_setter=script_object_class;
							}

							if(STK_IS_THIS(stk_dst)){
								VM_STOP_EXECUTE("\"this\" is not assignable");
							}
						}else if((stk_dst->properties & MSK_STK_PROPERTY_MEMBER_ATTRIBUTE)!=0){
							StackMemberAttribute *stk_ma=(StackMemberAttribute *)stk_dst->value;
							if(stk_ma->member_attribute->setters.count > 0){
								lst_functions=&stk_ma->member_attribute->setters;
								obj_setter=stk_ma->so_object;
							}else{
								VM_STOP_EXECUTE("Symbol X has not setter metamethod implemented");
							}
						}
					}

					if(lst_functions!=NULL){ // call metamethod
						// find appropiate function
						ScriptFunction *ptr_function_found=(ScriptFunction *)((StackElement *)lst_functions->items[0])->value;//(ScriptFunction *)stk_setter->value;
						StackElement *stk_vm_start=data->stk_vm_current;
						StackElement *stk_arg=stk_vm_start+1;
						*stk_arg=*stk_src;

						if(obj_setter->isNativeObject()){ // because object is native, we can have more than one _setter

							if((ptr_function_found=vm_find_function(
									vm
									,obj_setter
									,calling_function
									,instruction
									,false
									,lst_functions
									,lst_functions->count
									,"_set" // symbol to find
									,stk_arg
									,1))==NULL){

								VM_STOP_EXECUTE("cannot find metamethod \"_set\"");
							}
						}else if(lst_functions->count>1){ // it has all member list
							StackElement * stk = obj_setter->getProperty("_set",NULL);

							if(stk == NULL){
								VM_STOP_EXECUTE("Operator metamethod \"_set (aka =)\" is not implemented");
							}

							if((stk->properties & MSK_STK_PROPERTY_MEMBER_FUNCTION)==0){
								VM_STOP_EXECUTE("Operator metamethod \"_set (aka =)\" is not function");
							}

							StackMemberFunction *mf=(StackMemberFunction *)stk->value;

							ptr_function_found=mf->so_function;
						}

						if(ptr_function_found->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){
							vm_call_function_native(
									vm
									,obj_setter
									,ptr_function_found
									,stk_arg
									,1
									,instruction
							);
						}else{

							vm_call_function_script(
								vm
								,obj_setter
								,ptr_function_found
								,stk_arg
								,1
							);
						}

						// restore
						data->stk_vm_current=stk_vm_start;

					}else{


						if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_src)){
							stk_src=(StackElement *)((STK_GET_STK_VAR_REF(stk_src)->value));
						}

						StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...
						{

							void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update value on save) */
							void **stk_src_ref=&stk_src->value;
							void **stk_dst_ref=&stk_dst->value;
							if(stk_src->properties & MSK_STK_PROPERTY_IS_VAR_C){ // src is C pointer
								stk_src_ref=(void **)((stk_src)->value);
							}
							if(stk_dst->properties & MSK_STK_PROPERTY_IS_VAR_C){ // dst is a C pointer

								// particular case
								if(
									    stk_dst->properties != stk_src->properties
									&& (((stk_dst->properties & MSK_STK_PROPERTY_ZS_CHAR) && (stk_src->properties & MSK_STK_PROPERTY_ZS_INT))==0)){

									 if(stk_dst->properties != stk_src->properties){
										if(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_dst->properties) != GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_src->properties)
										){
											// check particular case
											VM_STOP_EXECUTE("different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",SFI_GET_SYMBOL_NAME(calling_function,instruction));
										}else{ // is object
											//if(
											//	(stk_src->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)
											//){
											VM_STOP_EXECUTE("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator (aka set metamethod) in order to perform assign operation");
											//}
										}
									}
								}
								stk_dst_ref=(void **)((stk_dst)->value);
								copy_aux=&((stk_dst)->value);
							}
							unsigned short type_var=stk_src->properties;

							// init stk_dst
							STK_SET_NULL(stk_dst);

							if(type_var == MSK_STK_PROPERTY_NULL){
								stk_dst->properties=MSK_STK_PROPERTY_NULL;
							}else if(type_var & MSK_STK_PROPERTY_ZS_INT){
								stk_dst->properties=MSK_STK_PROPERTY_ZS_INT;
								old_stk_dst.properties &  MSK_STK_PROPERTY_ZS_CHAR?
								*((zs_char *)stk_dst_ref)=*((zs_int *)stk_src_ref) & 0xff
								:*((zs_int *)stk_dst_ref)=*((zs_int *)stk_src_ref);
								if(copy_aux!=NULL)(*(zs_int *)copy_aux)=*((zs_int *)stk_src_ref);
							}else if(type_var & MSK_STK_PROPERTY_ZS_FLOAT){
								stk_dst->properties=MSK_STK_PROPERTY_ZS_FLOAT;
								*((zs_float *)stk_dst_ref)=*((zs_float *)stk_src_ref);
								if(copy_aux!=NULL)(*(zs_float *)copy_aux)=*((zs_float *)stk_src_ref);
							}else if(type_var & MSK_STK_PROPERTY_BOOL){
								stk_dst->properties=MSK_STK_PROPERTY_BOOL;
								*((bool *)stk_dst_ref)=*((bool *)stk_src_ref);
								if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)stk_src_ref);
							}else if(type_var  &  (MSK_STK_PROPERTY_FUNCTION | MSK_STK_PROPERTY_CLASS) ){
								*stk_dst=*stk_src;
							}else if(type_var & MSK_STK_PROPERTY_SCRIPT_OBJECT){

								if(STK_IS_SCRIPT_OBJECT_STRING(stk_src)){
									ScriptObjectString *str_object=NULL;

									if(STK_IS_SCRIPT_OBJECT_STRING(stk_dst)){ // dst is string reload
										str_object=(ScriptObjectString *)stk_dst->value;
									}else{ // Generates a std::string var
										stk_dst->value=str_object= ZS_NEW_OBJECT_STRING(data->zs);
										stk_dst->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;

										// create shared ptr
										if(!vm_create_shared_pointer(vm,str_object)){
											goto lbl_exit_function;
										}

										// share ptr
										if(!vm_share_pointer(vm,str_object)){
											goto lbl_exit_function;
										}
										//-------------------------------------
									}

									str_object->set(stk_src->toString());

								}else{ // object we pass its reference

									so_aux=(ScriptObject *)stk_src->value;

									stk_dst->value=so_aux;
									stk_dst->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;

									if(!STK_IS_THIS(stk_src)){ // do not share this!
										if(!vm_share_pointer(vm,so_aux)){
											goto lbl_exit_function;
										}
									}
								}
							}else{
								VM_STOP_EXECUTE("(internal) cannot determine var type %s",stk_src->typeStr());
							}
							if(copy_aux!=NULL)stk_dst->properties|=MSK_STK_PROPERTY_IS_VAR_C;
						}

						// check old dst value to unref if it was an object ...
						switch(GET_MSK_STK_PROPERTY_TYPES(old_stk_dst.properties)){
						case MSK_STK_PROPERTY_NULL:
						case MSK_STK_PROPERTY_ZS_INT:
						case MSK_STK_PROPERTY_ZS_FLOAT:
						case MSK_STK_PROPERTY_BOOL:
						case MSK_STK_PROPERTY_FUNCTION: // we aren't take care about nothing! :)
							break;
						case MSK_STK_PROPERTY_SCRIPT_OBJECT: // we are getting script vars ...

							if((old_stk_dst.properties & (MSK_STK_PROPERTY_IS_VAR_C))==(MSK_STK_PROPERTY_IS_VAR_C)==0){ // is not C class
								if(old_stk_dst.value!=NULL){ // it had a pointer (no constant)...
									if(! // if not...
										(old_stk_dst.value == stk_dst->value)  // ... same ref ...
									||  (STK_IS_THIS(&old_stk_dst)) // ... or this
									   // ... do share/unshare
									){

										// unref pointer because new pointer has been attached...
										StackElement *chk_ref=(StackElement *)stk_result_op2->value;
										ScriptObjectObject  *old_so=(ScriptObjectObject  *)old_stk_dst.value;
										int idx_call=data->vm_idx_call;
										if(chk_ref->properties & MSK_STK_PROPERTY_PTR_STK){
											chk_ref=(StackElement *)chk_ref->value;
										}
										if(STK_IS_SCRIPT_OBJECT_VAR_REF(chk_ref)){
											ScriptObjectVarRef *so_var_ref=(ScriptObjectVarRef *)chk_ref->value;
											data->vm_idx_call=so_var_ref->getIdxCall(); // put the vm_idx_call where it was created
										}

										if(!vm_unref_shared_script_object(vm,old_so,data->vm_idx_call)){
											goto lbl_exit_function;
										}
									}
								}
							}
							break;
						}
					}

					if(instruction->byte_code ==BYTE_CODE_STORE_CONST){
						stk_dst->properties |= MSK_STK_PROPERTY_READ_ONLY;
					}

					if(n_elements_left > 0){
						n_elements_left--;
						stk_result_op2=--data->stk_vm_current;//stk_multi_var_dest++; // left assigment
						stk_result_op1=++stk_multi_var_src; // result on the right
						goto vm_store_next;
					}
					else if(   instruction->byte_code!=BYTE_CODE_PUSH_VECTOR_ELEMENT
							&& instruction->byte_code!=BYTE_CODE_PUSH_OBJECT_ELEMENT
					){
						*data->stk_vm_current++=*stk_dst;
					}
				}
				continue;
			case BYTE_CODE_EQU:  // ==
				POP_TWO;
				PROCESS_COMPARE_OPERATION(==, BYTE_CODE_METAMETHOD_EQU);
				continue;
			case BYTE_CODE_NOT_EQU:  // !=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(!=, BYTE_CODE_METAMETHOD_NOT_EQU);
				continue;
			case BYTE_CODE_LT:  // <
				POP_TWO;
				PROCESS_COMPARE_OPERATION(<, BYTE_CODE_METAMETHOD_LT);
				continue;
			case BYTE_CODE_LTE:  // <=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(<=, BYTE_CODE_METAMETHOD_LTE);
				continue;
			case BYTE_CODE_GT:  // >
				POP_TWO;
				PROCESS_COMPARE_OPERATION(>,BYTE_CODE_METAMETHOD_GT);
				continue;
			case BYTE_CODE_GTE:  // >=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(>=,BYTE_CODE_METAMETHOD_GTE);
				continue;
			case BYTE_CODE_LOGIC_AND:  // &&
				POP_TWO;
				PROCESS_LOGIC_OPERATION(&&);
				continue;
			case BYTE_CODE_LOGIC_OR:  // ||
				POP_TWO;
				PROCESS_LOGIC_OPERATION(||);
				continue;
			case BYTE_CODE_NOT: // !
				POP_ONE;
				if(stk_result_op1->properties & MSK_STK_PROPERTY_BOOL){ // operation will result as integer.
					PUSH_BOOLEAN((!((bool)(stk_result_op1->value))));
				}else{
					if(vm_apply_metamethod(
						vm
						,calling_function
						,instruction
						,BYTE_CODE_METAMETHOD_NOT
						,stk_result_op1
						,stk_result_op2
					)==false){
						goto lbl_exit_function;
					}
				}
				continue;
			case BYTE_CODE_NEG: // -
				POP_ONE;
				if(stk_result_op1->properties & MSK_STK_PROPERTY_ZS_INT){ // operation will result as integer.
					PUSH_INTEGER((-((zs_int)(stk_result_op1->value))));
				}else if(stk_result_op1->properties & MSK_STK_PROPERTY_ZS_FLOAT){
					ZS_FLOAT_COPY(&f_aux_value1,&stk_result_op1->value);
					PUSH_FLOAT(-f_aux_value1);
				}else{ // try metamethod ...
					if(!vm_apply_metamethod(
							vm
							,calling_function
							,instruction
							,BYTE_CODE_METAMETHOD_NEG
							,stk_result_op1
							,stk_result_op2
					)){
						goto lbl_exit_function;
					}
				}
				continue;
			case BYTE_CODE_ADD: // +
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(+,BYTE_CODE_METAMETHOD_ADD);
				continue;
			case BYTE_CODE_SUB: // -
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(-,BYTE_CODE_METAMETHOD_SUB);
				continue;
			case BYTE_CODE_MUL: // *
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
				continue;
			case BYTE_CODE_DIV: // /
				POP_TWO;
				PROCESS_ARITHMETIC_DIV_OPERATION;
				continue;
			 case BYTE_CODE_MOD: // /
				POP_TWO;
				PROCESS_MOD_OPERATION;
				continue;
			 case BYTE_CODE_BITWISE_AND: // &
				POP_TWO;
				PROCESS_BINARY_OPERATION(&, BYTE_CODE_METAMETHOD_AND);
				continue;
			 case BYTE_CODE_BITWISE_OR: // *
				POP_TWO;
				PROCESS_BINARY_OPERATION(|, BYTE_CODE_METAMETHOD_OR);
				continue;
			 case BYTE_CODE_BITWISE_XOR: // ^
				POP_TWO;
				PROCESS_BINARY_OPERATION(^, BYTE_CODE_METAMETHOD_XOR);
				continue;
			 case BYTE_CODE_SHR: // >>
				POP_TWO;
				PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
				continue;
			 case BYTE_CODE_SHL: // <<
				POP_TWO;
				PROCESS_BINARY_OPERATION(<<, BYTE_CODE_METAMETHOD_SHL);
				continue;
			 case BYTE_CODE_INSTANCEOF: // check instance of ...
				 POP_TWO;
				switch((zs_int)stk_result_op2->value){
				case IDX_BUILTIN_TYPE_ZS_INT_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_ZS_INT)!=0);
					break;
				case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_ZS_FLOAT)!=0);
					break;
				case IDX_BUILTIN_TYPE_BOOL_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_BOOL)!=0);
					break;
				case IDX_BUILTIN_TYPE_FUNCTION:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_FUNCTION)!=0);
					break;
				default:
					if(stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						bool b = data->script_class_factory->isClassInheritsFrom(			//
								((ScriptObjectObject *)(stk_result_op1->value))->idx_script_class // A
								, (zs_int)stk_result_op2->value				// B
						);
						PUSH_BOOLEAN(b);
					}else{
						PUSH_BOOLEAN(false);
					}
					break;
				}
				continue;
			 case BYTE_CODE_JMP:
				instruction_it=instruction+instruction->value_op2;
				continue;
			 case BYTE_CODE_JNT: // goto if not true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->value == 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->value != 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			case BYTE_CODE_JE:  // especial j for switch
				READ_TWO_POP_ONE; // reads switch value and case value
				PROCESS_COMPARE_OPERATION(==, BYTE_CODE_METAMETHOD_EQU);
				POP_ONE; // retrieve result...
				if(stk_result_op1->value != 0){ // if true goto
					instruction_it=instruction+instruction->value_op2;
				}
				continue;

			 case  BYTE_CODE_CALL_CONSTRUCTOR:
			 case  BYTE_CODE_CALL: // calling function after all of args are processed...

				 {
					ScriptFunction *sf = NULL;
					StackElement *stk_function_ref=NULL;
					//bool calling_from_object_type=false;
					zs_int idx_function=ZS_IDX_UNDEFINED;
					ScriptObject *calling_object = this_object;
					uint16_t n_local_registered_symbols=0;
					unsigned char n_args=instruction->value_op1; // number arguments will pass to this function
					StackElement *stk_start_arg_call=(data->stk_vm_current-n_args);

					stk_function_ref = ((stk_start_arg_call-1));
					bool is_constructor=instruction->byte_code==BYTE_CODE_CALL_CONSTRUCTOR;

					if(stk_function_ref->properties & MSK_STK_PROPERTY_PTR_STK){
						stk_function_ref=(StackElement *)stk_function_ref->value;
					}

					if(stk_function_ref->properties & MSK_STK_PROPERTY_MEMBER_FUNCTION){
						StackMemberFunction *fm=(StackMemberFunction *)stk_function_ref->value;
						calling_object=fm->so_object;
						sf=fm->so_function;
					}else{

						if((stk_function_ref->properties & (MSK_STK_PROPERTY_FUNCTION))==0){
							VM_STOP_EXECUTE("%s is not function or not exist",SFI_GET_SYMBOL_NAME(calling_function,instruction));
						}

						sf=(ScriptFunction *)stk_function_ref->value;
					}

					if(sf == NULL){
						VM_STOP_EXECUTE("internal error ScriptFunction null");
					}

					// if a c function that it has more than 1 symbol with same number of parameters, so we have to solve and get the right one...
					if(sf->symbol.properties & SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME){

						void *stk_element_ptr=data->vm_stack;
						int stk_element_len = data->main_function_object->registered_symbols->count;
						bool ignore_call=false;

						if(
							stk_function_ref->properties & MSK_STK_PROPERTY_MEMBER_FUNCTION //scope_type&(MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD|MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)
						){
							ignore_call= (is_constructor) && calling_object->isNativeObject() && n_args==0;
							zs_vector * list_props=calling_object->getStkBuiltinListElements();//getFunctions();
							stk_element_ptr=list_props->items;
							stk_element_len=list_props->count;
						}

						if(ignore_call == false)
						{
							ScriptFunction *sf_aux;
							if((sf_aux=vm_find_function(
									vm
									,calling_object
									,calling_function
									,instruction
									,is_constructor
									,stk_element_ptr
									,stk_element_len
									,sf->symbol.name // symbol to find
									,stk_start_arg_call
									,n_args))==NULL){

								VM_STOP_EXECUTE("cannot find function \"%s\"",sf->symbol.name.c_str());
							}
							sf=sf_aux;
						}
					}

					// call function
					if((sf->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){ // if script function...
						// we pass everything by copy (TODO implement ref)
						if(n_args > 0 && sf->params->count > 0){
							StackElement *stk_arg=stk_start_arg_call;
							ScriptObjectVector *var_args=NULL;
							ScriptObject *so_param=NULL;
							bool end_args=false;
							int function_param=0;//&sf->params->items[0];
							int effective_args=n_args < sf->params->count ? n_args:sf->params->count;
							int i=0;
							for(;i < n_args && end_args==false;i++){
								so_param=NULL; // script object we passing
								uint16_t sfa_properties=((ScriptFunctionArg *)sf->params->items[function_param])->properties;// ((ScriptFunctionArg *)(*function_param))->properties;

								if(sfa_properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF){ // create or pass the var ref object...

									StackElement *check_ref=stk_arg;
									if(stk_arg->properties & MSK_STK_PROPERTY_PTR_STK){
										check_ref=(StackElement *)check_ref->value;
									}

									if(STK_IS_SCRIPT_OBJECT_VAR_REF(check_ref)==false) { // create new

										if((stk_arg->properties & MSK_STK_PROPERTY_PTR_STK) != MSK_STK_PROPERTY_PTR_STK){
											VM_STOP_EXECUTE("Calling function \"%s\", parameter \"%i\": Passing argument by reference has to be variable (not attribute or property))",sf->symbol.name.c_str(),i+1);
										}

										ScriptObjectVarRef *sc=ZS_NEW_OBJECT_VAR_REF(data->zs,*stk_arg,data->vm_idx_call);
										if(!vm_create_shared_pointer(vm,sc)){
											goto lbl_exit_function;
										}
										so_param=sc;
										stk_arg->value=(void *)sc;
										stk_arg->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;
									}else{ // else is already var ref ... set stk_Arg as check_ref
										if(check_ref->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
											so_param=(ScriptObject *)check_ref->value;
										}
										stk_arg->value=check_ref->value;
										stk_arg->properties=check_ref->properties;
									}

								}else{
re_evaluate_stk_arg:
									if(stk_arg->properties & MSK_STK_PROPERTY_PTR_STK){ // get its value
										*stk_arg=*(StackElement *)stk_arg->value;
									}

									if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk_arg)==true) { // not passing by ref it gets its value
										*stk_arg=*((ScriptObjectVarRef *)stk_arg->value)->getStackElementPtr();
									}

									if(stk_arg->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
										so_param=(ScriptObject *)stk_arg->value;
										StackElement *stk_getter=NULL;
										if(so_param->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
											//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
											ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(data->zs);
											if(!vm_create_shared_pointer(vm,sc)){
												goto lbl_exit_function;
											}
											sc->set(stk_arg->toString());
											so_param=sc;
											stk_arg->value=(void *)sc;
											stk_arg->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;
										}else if((stk_getter=so_param->getProperty(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_GET),NULL))!=NULL){
											VM_INNER_ONLY_RETURN_CALL(
													so_param
													,((StackMemberFunction *)stk_getter->value)->so_function
													,"_get"
													,0)
											// copy returned value if any
											*stk_arg = *data->stk_vm_current;
											so_param=NULL; // put NULL to not make it shared next it
											goto re_evaluate_stk_arg; // because we want to check current
											/*StackElement *stk_def_afun_start=data->stk_vm_current;
											int n_returned_args_afun=0;
											vm_call_function_script(
												 vm
												,so_param
												,(ScriptFunction *)stk_getter->value
												,stk_def_afun_start
											);

											n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;

											CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,true) // we share pointer (true second arg) to not remove on pop in calling return

											// reset stack
											data->stk_vm_current=stk_def_afun_start+1; // reset stack +1*/

										}
									}
								}

								if(var_args!=NULL){
									var_args->push(stk_arg); // we do not share pointer here due is already added in a vector
								}else{
									if(sfa_properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS){ // enter var args
										var_args=ZS_NEW_OBJECT_VECTOR(data->zs);
										if(!vm_create_shared_pointer(vm,var_args)){
											goto lbl_exit_function;
										}

										if(!vm_share_pointer(vm,var_args)){ // we share pointer +1 to not remove on pop in calling return
											goto lbl_exit_function;
										}

										// push first arg
										var_args->push(stk_arg);
										// replace for vector type...
										stk_arg->value=(void *)var_args;
										stk_arg->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;
									}else{ // not push in var arg

										if(so_param != NULL){ // share n+1 to function
											if(!vm_share_pointer(vm,so_param)){ // we share pointer +1 to not remove on pop in calling return
												goto lbl_exit_function;
											}
										}

										function_param++;
										if(i+1 >= (int)effective_args){
											end_args=true;
										}
									}
								}

								stk_arg++;
							}
						}

						// ... we must set the rest of parameters with default value in case user put less params. If params exceds the number of accepted params in function,
						// will be ignored always.
						for(unsigned i = n_args; i < sf->params->count; i++){
							ScriptFunctionArg *param=(ScriptFunctionArg *)sf->params->items[i];
							//StackElement *stk_def_afun_start=data->stk_vm_current;
							//param->default_var_value;
							//int n_returned_args_afun=0;

							switch(param->default_var_value.properties){
							case MSK_STK_PROPERTY_NULL:
							case MSK_STK_PROPERTY_ZS_INT:
							case MSK_STK_PROPERTY_BOOL:
							case MSK_STK_PROPERTY_ZS_FLOAT:
								*data->stk_vm_current++=param->default_var_value;
								break;
							case MSK_STK_PROPERTY_FUNCTION: // we call function in the middle of the function
								VM_INNER_ONLY_RETURN_CALL(NULL,param->default_var_value.value,"default",1)
								/*vm_call_function_script(
									 vm
									,NULL
									,(ScriptFunction *)param->default_var_value.value
									,stk_def_afun_start
								);

								n_returned_args_afun=data->stk_vm_current-stk_def_afun_start;

								CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_def_afun_start,n_returned_args_afun,true) // we share pointer (true second arg) to not remove on pop in calling return

								// reset stack
								data->stk_vm_current=stk_def_afun_start+1; // reset stack +1*/
								break;
							default:
								VM_STOP_EXECUTE("Internal error: Unexpected default stack element \"%s\"",param->default_var_value.typeStr());
								break;

							}
							n_args++;
						}

						vm_call_function_script(
							vm
							,calling_object
							,sf
							,stk_start_arg_call
							,n_args
						);
						n_local_registered_symbols=sf->registered_symbols->count;
					}
					else{ // C function
						if(is_constructor) {// && (sf->symbol.properties & SYMBOL_PROPERTY_MEMBER_FUNCTION))
							calling_object= (ScriptObjectObject *)(stk_start_arg_call-2)->value; // the object should be before (start_arg -1 (idx_function)  - 2 (idx_object))
						}

						vm_call_function_native(
							vm
							,calling_object
							,sf
							,stk_start_arg_call
							,n_args
							,instruction
						);

						// restore stk_start_arg_call due in C args are not considered as local symbols (only for scripts)
						n_local_registered_symbols=n_args;
					}

					if(data->vm_error == true){
						data->vm_error_callstack_str+=zs_strutils::format(
							"\nat calling function %s (file:%s line:%i)" // TODO: get full symbol ?
							,sf->symbol.name.c_str()
							,SFI_GET_FILE(calling_function,instruction)
							,SFI_GET_LINE(calling_function,instruction)
						);
						goto lbl_exit_function;
					}

					// calcule returned stack elements
					StackElement *stk_return=(stk_start_arg_call+n_local_registered_symbols); // +1 points to starting return...
					int n_returned_arguments_from_function=data->stk_vm_current-stk_return;

					// setup all returned variables from function
					CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(stk_return,n_returned_arguments_from_function,false)

					data->stk_vm_current=stk_start_arg_call-1; // set vm current before function pointer is

					if(instruction->value_op2 == ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK) {
						StackElement tmp;
						// return all elements in reverse order in order to get right assignment ...
						// reverse returned items
						for(int i=0; i<(n_returned_arguments_from_function>>1); i++){
							tmp=stk_return[n_returned_arguments_from_function-i-1];
							stk_return[n_returned_arguments_from_function-i-1]=stk_return[i];
							stk_return[i]=tmp;
						}

						// copy to vm stack
						data->stk_vm_current=stk_start_arg_call-1;//+n_returned_arguments_from_function; // stk_vm_current points to first stack element

						for(int i=0;i<n_returned_arguments_from_function;i++){
							*data->stk_vm_current++= *stk_return++; // only return first argument
						}

					}else if(instruction->byte_code!=BYTE_CODE_CALL_CONSTRUCTOR){ // return only first element
						if(n_returned_arguments_from_function > 0){
							*data->stk_vm_current++= stk_return[0]; // only return first argument
						}
						else{
							PUSH_NULL; // no return push null
						}
					}
				 }
				continue;
			 case  BYTE_CODE_RET:
				for(StackElement *stk_it=data->stk_vm_current-1;stk_it>=stk_start;stk_it--){ // can return something. value is +1 from stack
					// if scriptvariable and in the zeros list, deattach
					if(stk_it->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						if(!STK_IS_THIS(stk_it)){

							ScriptObject *script_var=(ScriptObject *)stk_it->value;

							// deattach from zero shares if exist...
							if(vm_deattach_shared_node(vm,script_var->shared_pointer->data.zero_shares,script_var->shared_pointer)==false){
								goto lbl_exit_function;
							}

							// and free
							free(script_var->shared_pointer);
							script_var->shared_pointer=NULL;
						}
					}
				}
				goto lbl_exit_function;
			 case  BYTE_CODE_NEW_CLASS:
				 	so_class_aux=NEW_CLASS_VAR_BY_IDX(data,instruction->value_op1);

					if(!vm_create_shared_pointer(vm,so_class_aux)){
						goto lbl_exit_function;
					}
					so_class_aux->info_function_new=calling_function;
					so_class_aux->instruction_new=instruction;
					(*data->stk_vm_current++)={so_class_aux,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case BYTE_CODE_NEW_VECTOR: // Create new std::vector object...
					so_aux=ZS_NEW_OBJECT_VECTOR(data->zs);
					if(!vm_create_shared_pointer(vm,so_aux)){
						goto lbl_exit_function;
					}
					(*data->stk_vm_current++)={so_aux,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_NEW_OBJECT: // Create new std::vector object...
				 	so_aux=ZS_NEW_OBJECT_OBJECT(data->zs);
					if(!vm_create_shared_pointer(vm,so_aux)){
						goto lbl_exit_function;
					}
					(*data->stk_vm_current++)={so_aux,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;

			 case  BYTE_CODE_NEW_STRING: // Create new std::vector object...
				 so_aux= ScriptObjectString::newScriptObjectString(data->zs,instruction->getConstantValueOp2ToString());
					if(!vm_create_shared_pointer(vm,so_aux)){
						goto lbl_exit_function;
					}

					(*data->stk_vm_current++)={so_aux,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_DELETE:
					POP_ONE;
					//script_var
					if(stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						ScriptObjectClass *script_object_class=NULL;
						StackElement *se=stk_result_op1;

						so_aux = (ScriptObject *)(se)->value;

						if(!vm_unref_shared_script_object(vm,so_aux,data->vm_idx_call)){
							goto lbl_exit_function;
						}

						if(so_aux->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS)
						{ // max ...
							script_object_class=(ScriptObjectClass *)so_aux;

							if(script_object_class->isCreatedByContructor()){
								script_object_class->deleteNativeObjectOnDestroy(true);
							}
							STK_SET_NULL(se);
						}
					}
					else{
						VM_STOP_EXECUTE("Error deleting \"%s\". cannot perform delete on variables type \"%s\"",SFI_GET_SYMBOL_NAME(calling_function,instruction-1),stk_result_op1->typeStr());
					}
					continue;
			 case BYTE_CODE_PUSH_SCOPE:
				PUSH_VM_SCOPE(instruction->value_op2,calling_function,_stk_local_var,instruction->value_op1);
				continue;

			 case BYTE_CODE_POP_SCOPE:
				POP_VM_SCOPE()
				if((data->zero_shares+data->vm_idx_call)->first!=NULL){ // there's empty shared pointers to remove
					vm_remove_empty_shared_pointers(vm,data->vm_idx_call);
				}
				continue;
			 case BYTE_CODE_IT_NEXT:
				 VM_STOP_EXECUTE("BYTE_CODE_SET_AND_NEXT TODOOOOO!",
					 SFI_GET_SYMBOL_NAME(calling_function,instruction)
				);
				 continue;
			 case BYTE_CODE_IT_END:
				 VM_STOP_EXECUTE("BYTE_CODE_IT_CHK_END TODOOOOO!",
				 SFI_GET_SYMBOL_NAME(calling_function,instruction)
				);
				 continue;
			 case BYTE_CODE_RESET_STACK:
				 data->stk_vm_current=stk_start;
				 continue;
			 case BYTE_CODE_POST_INC:
				 PERFORM_POST_OPERATOR(+,++,BYTE_CODE_METAMETHOD_NEXT);
				 continue;
			 case BYTE_CODE_POST_DEC:
				 PERFORM_POST_OPERATOR(+,--,BYTE_CODE_METAMETHOD_PREVIOUS);
				 continue;
			 case BYTE_CODE_PRE_INC:
				 PERFORM_PRE_OPERATOR(++,BYTE_CODE_METAMETHOD_NEXT);
				 continue;
			 case BYTE_CODE_PRE_DEC:
				 PERFORM_PRE_OPERATOR(--,BYTE_CODE_METAMETHOD_PREVIOUS);
				 continue;
			 case BYTE_CODE_NEG_POST_INC:
				 PERFORM_POST_OPERATOR(-,++,BYTE_CODE_METAMETHOD_NEXT);
				 continue;
			 case BYTE_CODE_NEG_POST_DEC:
				 PERFORM_POST_OPERATOR(-,--,BYTE_CODE_METAMETHOD_PREVIOUS);
				 continue;
			}
		 }

	lbl_exit_function:

		//=========================
		// POP STACK
		if(vm_scope_start<data->vm_current_scope){ // if main function only remove empty shared pointers but preserve global variables!)
			// pop all scopes
			while(vm_scope_start<data->vm_current_scope){
				POP_VM_SCOPE(); // do not check removeEmptySharedPointers to have better performance
			}

			if((data->zero_shares+data->vm_idx_call)->first!=NULL){
				vm_remove_empty_shared_pointers(vm,data->vm_idx_call);
			}
		}
		data->vm_idx_call--;
		// POP STACK
		//=========================
	}
}
