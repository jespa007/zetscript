
#define PROCESS_MOD_OPERATION \
{ \
	unsigned short properties = GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){\
			PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) % STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(fmod(STK_VALUE_TO_ZS_INT(stk_result_op1) , f_aux_value2));\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(fmod(f_aux_value1 , STK_VALUE_TO_ZS_INT(stk_result_op2)));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(fmod(f_aux_value1 , f_aux_value2));\
	}\
	else{\
		if(applyMetamethod(\
			calling_function\
			,instruction\
			,BYTE_CODE_METAMETHOD_MOD\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PROCESS_ARITHMETIC_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){\
			PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ f_aux_value2);\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(f_aux_value1 __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(f_aux_value1 __C_OP__ f_aux_value2);\
	}\
	else{\
		if(applyMetamethod(\
				calling_function\
				,instruction\
				,__METAMETHOD__\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PROCESS_COMPARE_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){\
		PUSH_BOOLEAN(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}\
	else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}\
	else if(properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
		if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_BOOLEAN(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ f_aux_value2);\
		}else{\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			PUSH_BOOLEAN(f_aux_value1 __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
		}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
		COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
		COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
		PUSH_BOOLEAN(f_aux_value1 __C_OP__ f_aux_value2);\
	}\
	else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){\
		PUSH_BOOLEAN(ZS_STRCMP(STK_VALUE_TO_STRING(stk_result_op1), __C_OP__ ,STK_VALUE_TO_STRING(stk_result_op2)));\
	}else if((properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL))\
			&& (__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
			){\
		if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED){\
			PUSH_BOOLEAN(true  __C_OP__  true);\
		}else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL){\
			PUSH_BOOLEAN(true  __C_OP__  true);\
		}else if (STK_VALUE_IS_UNDEFINED(stk_result_op1)){\
			PUSH_BOOLEAN(false);\
		}\
	}else{\
		if(applyMetamethod(\
			 calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PROCESS_LOGIC_OPERATION(__C_OP__)\
{\
	unsigned short properties = GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(STR(__C_OP__));\
		goto lbl_exit_function;\
	}\
}

#define PROCESS_BINARY_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){\
		PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}else{\
		if(applyMetamethod(\
			calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PERFORM_PRE_POST_OPERATOR(stk_var, __OPERATOR__) \
{\
	void **ref=(void **)(&((stk_var)->stk_value));\
	if(stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->var_ref);\
	}\
	switch(GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:\
			(*((zs_int *)(ref)))__OPERATOR__;\
			break;\
	case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:\
			(*((float *)(ref)))__OPERATOR__;\
			break;\
	default:\
		VM_STOP_EXECUTE(" Cannot perform pre/post operator (%s)",stk_var->typeStr());\
		break;\
	}\
}

#define PUSH_VM_SCOPE(_scope,_ptr_info_function, _ptr_local_var,_properties) {\
	if(vm_current_scope >=  vm_scope_max){THROW_RUNTIME_ERROR("reached max scope");}\
	*vm_current_scope++={(Scope *)_scope,_ptr_info_function,_ptr_local_var,_properties};\
}

#define OP1_AND_OP2_ARE_FLOATS \
(STK_VALUE_IS_INT_OR_FLOAT(stk_result_op1->properties) && STK_VALUE_IS_INT_OR_FLOAT(stk_result_op2->properties))

#define OP1_IS_STRING_AND_OP2_IS_FLOAT \
(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
STK_VALUE_IS_INT_OR_FLOAT(stk_result_op2->type_var)

#define OP1_IS_FLOAT_AND_OP2_IS_STRING \
(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
STK_VALUE_IS_INT_OR_FLOAT(stk_result_op1->type_var)

#define OP1_IS_STRING_AND_OP2_IS_BOOLEAN \
(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL)

#define OP1_AND_OP2_ARE_BOOLEANS \
(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL) && \
(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL)

#define OP1_AND_OP2_ARE_STRINGS \
(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && \
(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)

#define OP1_AND_OP2_ARE_UNDEFINED \
		(stk_result_op1->stk_value == VM_UNDEFINED) && \
		(stk_result_op2->stk_value == VM_UNDEFINED)

#define OP1_AND_OP2_ARE_NULL \
		(stk_result_op1->stk_value == VM_NULL) && \
		(stk_result_op2->stk_value == VM_NULL)

#define OP1_OR_OP2_IS_UNDEFINED \
		(stk_result_op1->stk_value == VM_UNDEFINED) || \
		(stk_result_op2->stk_value == VM_UNDEFINED)

#define OP1_OR_OP2_ARE_NULL \
		(stk_result_op1->stk_value == VM_NULL) || \
		(stk_result_op2->stk_value == VM_NULL)

#define PUSH_UNDEFINED \
stk_vm_current->stk_value=0; \
stk_vm_current->var_ref=0; \
stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED; \
stk_vm_current++;

#define PUSH_NULL \
stk_vm_current->stk_value=0; \
stk_vm_current->var_ref=0; \
stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL; \
stk_vm_current++;


#define PUSH_BOOLEAN(init_value) \
stk_vm_current->stk_value=(void *)((zs_int)(init_value)); \
stk_vm_current->var_ref=0; \
stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL; \
stk_vm_current++;


#define PUSH_INTEGER(init_value) \
stk_vm_current->stk_value=(void *)((zs_int)(init_value)); \
stk_vm_current->var_ref=0; \
stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT; \
stk_vm_current++;

#define PUSH_FLOAT(init_value) \
{\
	float aux=(float)(init_value); \
	COPY_FLOAT(&stk_vm_current->stk_value,&aux); \
	stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT; \
	stk_vm_current++; \
}

#define PUSH_FUNCTION(ref) \
stk_vm_current->stk_value=0; \
stk_vm_current->var_ref=(void *)ref; \
stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION; \
stk_vm_current++;

#define PUSH_CLASS(ref) \
stk_vm_current->stk_value=0; \
stk_vm_current->var_ref=(void *)ref; \
stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_CLASS; \
stk_vm_current++;

// explains whether stk is this or not. Warning should be given as value and not as ptr
#define STK_IS_THIS(stk) (this_object != NULL && (stk)->var_ref == this_object)


#define POP_TWO \
stk_result_op2=--stk_vm_current;\
stk_result_op1=--stk_vm_current;

#define READ_TWO_POP_ONE \
stk_result_op2=--stk_vm_current;\
stk_result_op1=(stk_vm_current-1);

#define POP_ONE \
stk_result_op1=--stk_vm_current;


namespace zetscript{

	StackElement VirtualMachine::callFunctionScript(
			ScriptObject       		* this_object,
			ScriptFunction 			* calling_function,
			StackElement 		  	* _stk_local_var,
			//std::string 		  	* _str_start,
			unsigned char 			n_args,
			Instruction 			*calling_instruction){


		StackElement  stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		Instruction * instruction = calling_instruction;

		if(vm_idx_call >= MAX_FUNCTION_CALL){
			VM_ERROR_AND_RET("Reached max stack");
		}

		float aux_float=0.0;
		StackElement ret_obj;
		ScriptObject *script_var=NULL;
		ScriptObject *calling_object=NULL; // used on call instruction
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement *stk_var;

		unsigned short pre_post_properties=0;
		ScriptObject *var_object = NULL;

		unsigned short scope_type=0;

		StackElement *stk_dst=NULL;
		StackElement *stk_src=NULL;


		Instruction *instructions=calling_function->instructions; // starting instruction
		Instruction *instruction_it=instructions;
		Instruction *calling_object_instruction;
		//const char * symbol_access_str;
		const char *str_not_defined="";
		Symbol * symbol_not_defined=NULL;


		VM_Scope * vm_scope_start=vm_current_scope; // save current scope...

		Scope * scope = calling_function->symbol.scope;// ast->idx_scope;

		vm_idx_call++;

		zs_vector *registered_symbols=calling_function->registered_symbols;
		ZS_PRINT_DEBUG("Executing function %s ...",calling_function->symbol.name.c_str());

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		unsigned symbols_count=registered_symbols->count;
		StackElement *stk_start=&_stk_local_var[symbols_count];   // <-- here starts stk for aux vars for operations ..

		if(stk_start >= &vm_stack[VM_STACK_LOCAL_VAR_MAX-1]){
			VM_STOP_EXECUTE("Error MAXIMUM stack size reached");
		}

		// Init local vars ...
		if((calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN) && (vm_idx_call > 1)){
			StackElement *ptr_aux = _stk_local_var+n_args;
			for(unsigned i = n_args; i < (symbols_count); i++){
				Symbol *symbol=(Symbol *)registered_symbols->items[i];
				if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){
					ptr_aux->stk_value=0;
					ptr_aux->var_ref=(void *)symbol->ref_ptr;
					ptr_aux->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION; // starts undefined.

				}else{
					// if C then pass its properties...
					ptr_aux->stk_value=0;							 // no value assigned.
					ptr_aux->var_ref=0; 						     // no varref related.
					ptr_aux->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED; // starts undefined.
				}

				ptr_aux++;
			}
		}

		// PUSH STACK
		//=========================================

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// EXECUTE INSTRUCTION HERE
		//
		 for(;;){ // foreach byte code instruction ...

			instruction = instruction_it++;

			const unsigned char operator_type=instruction->byte_code;
			unsigned char value_op1=instruction->value_op1;

			switch(operator_type){
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_RESET_STACK:
				stk_vm_current=stk_start;
				continue;
			case BYTE_CODE_LOAD_TYPE_FIND:

				str_not_defined=SFI_GET_SYMBOL_NAME(calling_function,instruction);
				symbol_not_defined = main_function_object->getSymbol(MAIN_SCOPE(this),str_not_defined);//, NO_PARAMS_SYMBOL_ONLY, ScopeDirection::SCOPE_DIRECTION_DOWN);

				if(symbol_not_defined != NULL){
					if(symbol_not_defined->n_params==NO_PARAMS_SYMBOL_ONLY){ // variable
						instruction->byte_code=BYTE_CODE_LOAD_TYPE_VARIABLE;
						instruction->value_op2=symbol_not_defined->idx_position;
						goto lbl_load_type_variable;
					}else{ // function
						// assign script function ...
						instruction->byte_code=BYTE_CODE_LOAD_TYPE_FUNCTION;
						instruction->value_op2=symbol_not_defined->ref_ptr;
						goto lbl_load_type_function;
					}
				}else{ // load undefined as default!
					VM_STOP_EXECUTE("Symbol \"%s\" is not defined",str_not_defined);
				}
				continue;
			case BYTE_CODE_LOAD_TYPE_VARIABLE: // load variable ...
		lbl_load_type_variable:
				scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(instruction->properties);

				switch(scope_type){
				default: // global...
					if(instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_THIS){
						stk_var = this_object->getThisProperty();
					}else{
						stk_var = vm_stack + instruction->value_op2;
					}
					break;
				case MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL:
					stk_var = _stk_local_var+instruction->value_op2;
					break;
				case MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR:
					POP_TWO;
					var_object=NULL;
					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
						stk_result_op1 = (StackElement *)stk_result_op1->var_ref;
					}

					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
						var_object = (ScriptObject *)(stk_result_op1->var_ref);
					}

					stk_var=NULL;

					if(var_object != NULL){
						if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR){
							ScriptObjectVector * var_vector = (ScriptObjectVector *)var_object;
							if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){
								VM_STOP_EXECUTE("Expected std::vector-index as integer or std::string");
							}

							// determine object ...

							if((stk_var =var_vector->getUserProperty(STK_VALUE_TO_ZS_INT(stk_result_op2)))==NULL){
								goto lbl_exit_function;
							}
						}
					}
					if(stk_var == NULL){ // push undefined
						PUSH_UNDEFINED;
						continue;
						/*VM_STOP_EXECUTE("Variable \"%s\" is not type std::vector",
							SFI_GET_SYMBOL_NAME(calling_function,instructions+instruction->value_op2)
						);*/
					}
					break;
				case MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD:
				case MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS:

					if(instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR_NOT_FOUND) continue;

					stk_var=NULL;
					if(scope_type & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD){

						Instruction *previous_ins= (instruction-1);

						if(previous_ins->byte_code == BYTE_CODE_NEW){
							stk_result_op1=(stk_vm_current-1);
						}
						else{
							POP_ONE; // get var op1 and symbol op2
						}


						if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT)!= MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT)
						{
							VM_STOP_EXECUTE(
									"Cannot perform access operation [ ... %s.%s ]. Expected \"%s\" as object but is type \"%s\""
									,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
									,SFI_GET_SYMBOL_NAME(calling_function,instruction)
									,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
									,stk_result_op1->typeStr());
						}

						calling_object = (ScriptObject  *)stk_result_op1->var_ref;
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {
							StackElement *stk_ins=((StackElement *)stk_result_op1->var_ref);

							if(stk_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
								calling_object=((ScriptObject *)stk_ins->var_ref);
							}
						}

						if(calling_object == NULL)
						{
							VM_STOP_EXECUTE("var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(calling_function,previous_ins));
						}

						calling_object_instruction=previous_ins;

					}
					else{ // this scope ...
						calling_object_instruction=instruction;
						calling_object=this_object;
						if(instruction->value_op2!=ZS_IDX_UNDEFINED){ // not undefined load...
							if((stk_var=calling_object->getProperty(instruction->value_op2))==NULL){
								goto lbl_exit_function;
							}
						}
					}

					if(stk_var == NULL){ // try to get one...
						int idx_stk_element=ZS_IDX_UNDEFINED;
						const char *symbol_access_str=NULL;

						if(instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR){
							symbol_access_str=FUNCTION_MEMBER_CONSTRUCTOR_NAME;
						}else{
							symbol_access_str=SFI_GET_SYMBOL_NAME(calling_function,instruction);
						}

						stk_var=calling_object->getProperty(symbol_access_str, &idx_stk_element);


						// not exist ... add
						if(stk_var == NULL){
/*							Symbol *symbol=calling_object->getSymbol(idx_stk_element);
							// check if static
							if(symbol->properties & SYMBOL_PROPERTY_STATIC){
								VM_STOP_EXECUTE("Cannot access static symbols with \".\", static symbols are acceded by \"::\"");
							}
						}else{*/

							// something went wrong
							if(vm_error == true){
								goto lbl_exit_function;
							}

							stk_vm_current->stk_value=(void *)symbol_access_str;
							stk_vm_current->var_ref=calling_object;
							stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
							stk_vm_current++;
							continue;

							/*if((stk_var=calling_object->addProperty(symbol_access_str, error_str,NULL,&idx_stk_element))==NULL){
								VM_STOP_EXECUTE(error_str.c_str());
								return stk_result;
							}*/

						}

					}
					break;
				}

				//instruction_properties=instruction->properties;
				pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(instruction->properties);

				if(!HAS_PRE_POST_INC_DEC_OP(pre_post_properties)){
					// update var if needed ...
					if(stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){ // we assign the pointer directly...
						*stk_vm_current++=*stk_var;
					}
					else{ // is not pointer, so we do pointer
						stk_vm_current->stk_value=(stk_var)->stk_value;
						stk_vm_current->var_ref=stk_var;
						stk_vm_current->properties=(unsigned short)(((stk_var)->properties)|MSK_STACK_ELEMENT_PROPERTY_PTR_STK);

						stk_vm_current++;
					}
				}else{ // has pre post

					if((stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_READ_ONLY)!=0){

						VM_STOP_EXECUTE("Cannot perform self operations on constant elements");
					}

					/* all preoperators makes load var as constant ... */
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_INC:
							PERFORM_PRE_POST_OPERATOR(stk_var,++);
							(*stk_vm_current++)=*stk_var;
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_DEC:
							PERFORM_PRE_POST_OPERATOR(stk_var,--);
							(*stk_vm_current++)=*stk_var;
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_DEC:
							(*stk_vm_current++)=*stk_var;
							PERFORM_PRE_POST_OPERATOR(stk_var,--);
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_INC:
							(*stk_vm_current++)=*stk_var;
							PERFORM_PRE_POST_OPERATOR(stk_var,++);
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT:
							switch(GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_var->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									stk_vm_current->stk_value=(void *)(-(*((zs_int *)stk_var->var_ref)));
									stk_vm_current->var_ref=stk_var;
									stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_PTR_STK|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
								}else{
									stk_vm_current->stk_value=(void *)(-(((zs_int)stk_var->stk_value)));
									stk_vm_current->var_ref=stk_var;
									stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_PTR_STK;
								}
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									stk_vm_current->stk_value=(void *)(!(*((bool *)stk_var->var_ref)));
									stk_vm_current->var_ref=stk_var;
									stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL|MSK_STACK_ELEMENT_PROPERTY_PTR_STK|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
								}else{
									stk_vm_current->stk_value=(void *)(!(((bool)stk_var->stk_value)));
									stk_vm_current->var_ref=stk_var;
									stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL|MSK_STACK_ELEMENT_PROPERTY_PTR_STK;
								}
								break;

							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									COPY_FLOAT(&aux_float,stk_var->var_ref);
								}else{
									COPY_FLOAT(&aux_float,&stk_var->stk_value);
								}
								aux_float=-aux_float;
								COPY_FLOAT(&stk_vm_current->stk_value,&aux_float);
								stk_vm_current->var_ref=stk_var;
								stk_vm_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_PTR_STK;

								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									stk_vm_current->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
								}


								break;
							default:
								VM_STOP_EXECUTE("internal error:cannot perform pre operator - because is not number");
							}

							stk_vm_current++;
							continue;
					default:
						break;
					}
				}

				// ok in case is C we must udpate stk_value becaus it can be updated from C++. (only primitives)
				if(stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
					switch(GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_var->properties)){
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
						memcpy(&((stk_vm_current-1)->stk_value),stk_var->var_ref,sizeof(zs_int));
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
						COPY_FLOAT(&((stk_vm_current-1)->stk_value),stk_var->var_ref);
						break;
					case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
						(stk_vm_current-1)->stk_value=(void *)((*((bool *)stk_var->var_ref)));
						break;
					}
				}
				continue;
			case BYTE_CODE_LOAD_TYPE_NULL:
				PUSH_NULL;
				continue;
			case BYTE_CODE_LOAD_TYPE_UNDEFINED:
				PUSH_UNDEFINED;
				continue;
			case BYTE_CODE_LOAD_TYPE_FUNCTION: // expect constant and function has the same behaviour...
				lbl_load_type_function:
				PUSH_FUNCTION(instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_TYPE_CONSTANT:
				*stk_vm_current++=*((StackElement *)instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_TYPE_CLASS:
				PUSH_CLASS(instruction->value_op2);
				continue;
			case BYTE_CODE_STORE:
			case BYTE_CODE_PUSH_VECTOR_ELEMENT:
			case BYTE_CODE_PUSH_OBJECT_ELEMENT:
			case BYTE_CODE_STORE_CONST:

				{
					bool assign_metamethod=false;


					if(operator_type==BYTE_CODE_PUSH_VECTOR_ELEMENT){
						POP_ONE; // only pops the value, the last is the std::vector variable itself
						ScriptObject *vec_obj = NULL;
						if((stk_vm_current-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
							vec_obj = (ScriptObject *)(stk_vm_current-1)->var_ref;
							if(vec_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR){ // push value ...
								// op1 is now the src value ...
								stk_src=stk_result_op1;
								if(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
									stk_src=(StackElement *)stk_result_op1->var_ref;
								}

								stk_dst=((ScriptObjectVector *)vec_obj)->newSlot();
							}
						}

						if(vec_obj==NULL){
							VM_STOP_EXECUTE("Expected std::vector object");
						}
					}else if(operator_type==BYTE_CODE_PUSH_OBJECT_ELEMENT){

						POP_TWO; // first must be a string that describes variable name and the other the variable itself ...
						ScriptObject *obj = NULL;
						StackElement *stk_object=(stk_vm_current-1);
						if((stk_object->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT)==0){
							VM_STOP_EXECUTE("Expected object but is type ");
						}

						obj = (ScriptObject *)stk_object->var_ref;
						if(obj->idx_class != IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT){ // push value ...
							VM_STOP_EXECUTE("Expected object but is type ");
						}

						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
							VM_STOP_EXECUTE("internal error (operator2 is not std::string)");
						}
								// op1 is now the src value ...
						StackElement *se=NULL;
						const char *str = (const char *)stk_result_op1->stk_value;
						stk_src=stk_result_op2;
						if((se =((ScriptObject *)obj)->addProperty(str,vm_error_str))==NULL){
							VM_STOP_EXECUTE(vm_error_str.c_str());
							return stk_result;
						}

						stk_dst=se;

					}
					else{ // pop two parameters nothing ...
						POP_TWO; // op1:dst / op2:src

						stk_dst=stk_result_op1;

						if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_READ_ONLY){
							VM_STOP_EXECUTE("Assign to constant element is not allowed ");
						}

						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {
							stk_dst=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
						}else if(		(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED)
								&&  stk_result_op1->stk_value != NULL
								&&  stk_result_op1->var_ref != NULL
						) {

							ScriptObject *script_object=(ScriptObject *)stk_result_op1->var_ref;
							if((stk_dst=script_object->addProperty((const char *)stk_result_op1->stk_value, vm_error_str))==NULL){
								VM_STOP_EXECUTE(vm_error_str.c_str());
							}


						}else{
							VM_STOP_EXECUTE("Expected l-value on assignment ('=')");
						}


						stk_src=stk_result_op2; // store ptr instruction2 op as src_var_value

						// we need primitive stackelement in order to assign...
						if(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {// == ScriptObject::VAR_TYPE::OBJECT){
							stk_src=(StackElement *)stk_src->var_ref; // stk_value is expect to contents a stack variable
						}

						// ok load object pointer ...
						if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){

							if(((ScriptObject *)stk_dst->var_ref)->itHasSetMetamethod()){
								if(applyMetamethod(
										 calling_function
										,instruction
										,BYTE_CODE_METAMETHOD_SET
										,stk_result_op2 // it contents variable to be assigned
										,stk_result_op1 // it contects the result of expression or whatever
								)==false){
									goto lbl_exit_function;
								}
								assign_metamethod=true;
							}

							if(STK_IS_THIS(stk_dst)){
								VM_STOP_EXECUTE("\"this\" is not assignable");
							}
						}
					}

					if(! assign_metamethod){

						StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...
						{
							ScriptObject *script_var=NULL;
							std::string *aux_str=NULL;
							void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update stk_value on save) */
							void **stk_src_ref=&stk_src->stk_value;
							void **stk_dst_ref=&stk_dst->stk_value;
							if(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
								stk_src_ref=(void **)((stk_src)->var_ref);
							}
							if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
								if(!STK_VALUE_IS_FLOAT_OR_INT(stk_src) && STK_VALUE_IS_FLOAT_OR_INT(stk_dst)){
									if(GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_src->properties) != GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_dst->properties)
									){
										VM_STOP_EXECUTE("different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",SFI_GET_SYMBOL_NAME(calling_function,instruction));
									}else{
										if(
											(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT)
										){
											VM_STOP_EXECUTE("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");
										}
									}
								}
								stk_dst_ref=(void **)((stk_dst)->var_ref);
								copy_aux=&((stk_dst)->stk_value);
							}
							unsigned short type_var=stk_src->properties;
							//unsigned short runtime_var=0; /* there's no reason to heredate runtime_props ?!? GET_MSK_STACK_ELEMENT_PROPERTY_RUNTIME(type_var);*/

							// init stk_dst
							*stk_dst = {0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

							if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED){
								stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL){
								stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){
								stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT;
								*((zs_int *)stk_dst_ref)=*((zs_int *)stk_src_ref);
								if(copy_aux!=NULL)(*(zs_int *)copy_aux)=*((zs_int *)stk_src_ref);
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
								stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
								*((float *)stk_dst_ref)=*((float *)stk_src_ref);
								if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)stk_src_ref);
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL){
								stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL;
								*((bool *)stk_dst_ref)=*((bool *)stk_src_ref);
								if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)stk_src_ref);
							}else if(type_var  &  (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_CLASS) ){
								*stk_dst=*stk_src;
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
								if((stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) && (stk_dst->var_ref!=NULL)){ // dst is string reload
									script_var=(ScriptObject *)stk_dst->var_ref;
								}else{ // Generates a std::string var
									stk_dst->var_ref=script_var= NEW_STRING_VAR;
									if(!script_var->initSharedPtr()){
										goto lbl_exit_function;
									}
									if(!sharePointer(script_var->shared_pointer)){
										goto lbl_exit_function;
									}
									stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING;
								}

								// load std::string
								aux_str=(std::string *)(((ScriptObjectString *)script_var)->value);
								// save string src-->dst
								(*aux_str)=((const char *)stk_src->stk_value);
								// set stk_value
								stk_dst->stk_value=(void *)aux_str->c_str();
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
								script_var=(ScriptObject *)stk_src->var_ref;
								stk_dst->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT;
								stk_dst->stk_value=NULL;
								stk_dst->var_ref=script_var;
								if(!STK_IS_THIS(stk_src)){ // do not share this!
									if(!sharePointer(script_var->shared_pointer)){
										goto lbl_exit_function;
									}
								}
							}else{
								VM_STOP_EXECUTE("(internal) cannot determine var type %s",stk_src->typeStr());
							}
							if(copy_aux!=NULL)stk_dst->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
						}

						// check old var structure ...
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(old_stk_dst.properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION: // we aren't take care about nothing! :)
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING: // type std::string is really a std::string or variable ?!?!
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT: // we are getting script vars ...
							if((old_stk_dst.properties & (MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C))==(MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)==0){ // is not C class
								if(old_stk_dst.var_ref!=NULL){ // it had a pointer (no constant)...
									//ScriptObject *old_script_dst_var_ref=(ScriptObject *)(old_stk_dst.var_ref);
									if(
										old_stk_dst.var_ref != stk_dst->var_ref  // not same ref ...
									&&  STK_IS_THIS(&old_stk_dst)  // ... or this, do not share/unshare
									){ // unref pointer because new pointer has been attached...
										if(!unrefSharedScriptObject(((ScriptObject  *)old_stk_dst.var_ref)->shared_pointer,vm_idx_call)){
											goto lbl_exit_function;
										}
									}
								}
							}
							break;
						}
					}
					// to be able to do multiple assigns like a=b+=c=1 (1 will be pushed in each store instruction)
					if(operator_type ==BYTE_CODE_STORE
					){
						*stk_vm_current++=*stk_dst;
					}
					else if(operator_type ==BYTE_CODE_STORE_CONST){
						stk_dst->properties |= MSK_STACK_ELEMENT_PROPERTY_READ_ONLY;
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
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}
				PROCESS_LOGIC_OPERATION(&&);
				continue;
			case BYTE_CODE_LOGIC_OR:  // ||
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}
				PROCESS_LOGIC_OPERATION(||);
				continue;
			case BYTE_CODE_NOT: // !
				POP_ONE;
				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL){ // operation will result as integer.
					PUSH_BOOLEAN((!((bool)(stk_result_op1->stk_value))));
				}else{
					if(applyMetamethod(
						calling_function
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
				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){ // operation will result as integer.
					PUSH_INTEGER((-((zs_int)(stk_result_op1->stk_value))));
				}else if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
					COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
					PUSH_FLOAT(-f_aux_value1);
				}else{ // try metamethod ...
					if(!applyMetamethod(
							calling_function
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
				{
					if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}

					unsigned short mask_and_properties =GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties);
					if(mask_and_properties==MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){ // fast operation
						PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) + STK_VALUE_TO_ZS_INT(stk_result_op2));
					}else if(mask_and_properties== MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
						COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
						COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
						PUSH_FLOAT(f_aux_value1 + f_aux_value2);
					}else if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){ //if(mask_or_properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){
						COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
						PUSH_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) + f_aux_value2);
					}else if (STK_VALUE_IS_FLOAT(stk_result_op1) && STK_VALUE_IS_ZS_INT(stk_result_op2)){
						COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
						PUSH_FLOAT(f_aux_value1 + STK_VALUE_TO_ZS_INT(stk_result_op2));
					}else if((stk_result_op1->properties | stk_result_op2->properties) &  MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){ // replace all string from stk2 to empty
						*stk_vm_current++=performAddString(stk_result_op1,stk_result_op2);
					}else{ // try metamethod ...

						if(applyMetamethod(
							calling_function
							,instruction
							,BYTE_CODE_METAMETHOD_ADD
							,stk_result_op1
							,stk_result_op2
						)==false){
							goto lbl_exit_function;
						}
					}
				}
				continue;
			case BYTE_CODE_SUB: // -
				{
					if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}

					unsigned short mask_and_properties =GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties);
					if(mask_and_properties==MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){ // fast operation
						PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) - STK_VALUE_TO_ZS_INT(stk_result_op2));
					}else if(mask_and_properties== MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
						COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
						COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
						PUSH_FLOAT(f_aux_value1 - f_aux_value2);
					}else if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){ //if(mask_or_properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){
						COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
						PUSH_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) - f_aux_value2);
					}else if (STK_VALUE_IS_FLOAT(stk_result_op1) && STK_VALUE_IS_ZS_INT(stk_result_op2)){
						COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
						PUSH_FLOAT(f_aux_value1 - STK_VALUE_TO_ZS_INT(stk_result_op2));
					}else if(mask_and_properties==MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
						*stk_vm_current++=performSubString(stk_result_op1,stk_result_op2);
					}else{ // try metamethod ...

						if(applyMetamethod(
							calling_function
							,instruction
							,BYTE_CODE_METAMETHOD_SUB
							,stk_result_op1
							,stk_result_op2
						)==false){
							goto lbl_exit_function;
						}
					}
				}
				continue;
			case BYTE_CODE_MUL: // *
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
				continue;
			case BYTE_CODE_DIV: // /
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_ARITHMETIC_OPERATION(/, BYTE_CODE_METAMETHOD_DIV);
				continue;

			 case BYTE_CODE_MOD: // /
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_MOD_OPERATION;
				continue;
			 case BYTE_CODE_AND: // &
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(&, BYTE_CODE_METAMETHOD_AND);
				continue;
			 case BYTE_CODE_OR: // *
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(|, BYTE_CODE_METAMETHOD_OR);
				continue;
			 case BYTE_CODE_XOR: // ^
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(^, BYTE_CODE_METAMETHOD_XOR);
				continue;
			 case BYTE_CODE_SHR: // >>
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
				continue;
			 case BYTE_CODE_SHL: // <<
				if(instruction->properties&MSK_INSTRUCTION_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(<<, BYTE_CODE_METAMETHOD_SHL);
				continue;
			 case BYTE_CODE_JMP:
				instruction_it=instructions+instruction->value_op2;
				continue;
			 case BYTE_CODE_INSTANCEOF: // check instance of ...
				 POP_TWO;

				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {// == ScriptObject::VAR_TYPE::OBJECT){
					stk_result_op1=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
				}
				switch((zs_int)stk_result_op2->stk_value){
				case IDX_BUILTIN_TYPE_ZS_INT_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT)!=0);
					break;
				case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)!=0);
					break;
				case IDX_BUILTIN_TYPE_BOOL_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL)!=0);
					break;
				case IDX_BUILTIN_TYPE_STRING_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)!=0);
					break;
				case IDX_BUILTIN_TYPE_FUNCTION:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)!=0);
					break;
				default:
					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
						bool b = this->script_class_factory->isClassInheritsFrom(			//
								((ScriptObject *)(stk_result_op1->var_ref))->idx_class // A
								, (zs_int)stk_result_op2->stk_value				// B
						);
						PUSH_BOOLEAN(b);
					}else{
						PUSH_BOOLEAN(false);
					}
					break;
				}
				continue;
			 case BYTE_CODE_JNT: // goto if not true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value == 0){
					instruction_it=instructions+instruction->value_op2;
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value != 0){
					instruction_it=instructions+instruction->value_op2;
				}
				continue;
			case BYTE_CODE_JE:  // especial j for switch
				READ_TWO_POP_ONE;
				PROCESS_COMPARE_OPERATION(==, BYTE_CODE_METAMETHOD_EQU);
				POP_ONE; // retrieve result...
				if(stk_result_op1->stk_value != 0){ // if true goto
					instruction_it=instructions+instruction->value_op2;
				}
				continue;

			 case  BYTE_CODE_CALL: // calling function after all of args are processed...

				 {
					ScriptFunction *sf = NULL;
					unsigned char n_args=instruction->value_op1; // number arguments will pass to this function
					StackElement *stk_function_ref=NULL;
					unsigned short scope_type=0;
					zs_int idx_function=ZS_IDX_UNDEFINED;

					StackElement *stk_start_arg_call=(stk_vm_current-n_args);


					if(instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR_NOT_FOUND){ // points the object
						stk_vm_current=stk_start_arg_call;
						continue;

					}

					stk_function_ref = ((stk_start_arg_call-1));
					bool is_constructor=(char)instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR;
					scope_type = GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(instruction->properties);
					calling_object = this_object;

					if(stk_function_ref->stk_value != NULL){ // expects calling object
						calling_object=(ScriptObject *)stk_function_ref->stk_value;
					}

					if(stk_function_ref->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
						stk_function_ref=(StackElement *)stk_function_ref->var_ref;
					}

					if((stk_function_ref->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION))==0){
						VM_STOP_EXECUTE("%s is not function or not exist",SFI_GET_SYMBOL_NAME(calling_function,instruction));
					}

					sf=(ScriptFunction *)stk_function_ref->var_ref;

					if(sf == NULL){
						VM_STOP_EXECUTE("internal error ScriptFunction null");
					}

					// if a c function that it has more than 1 symbol with same number of parameters, so we have to solve and get the right one...
					if(sf->symbol.properties & SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME){

						StackElement *stk_element_ptr=vm_stack;
						int stk_element_len = main_function_object->registered_symbols->count;
						bool ignore_call=false;


						if(
							scope_type&(MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD|MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)
						){
							//bool is_constructor=sf->symbol.name==FUNCTION_MEMBER_CONSTRUCTOR_NAME;
							ignore_call= (is_constructor) && calling_object->isNativeObject() && n_args==0;
							zs_vector * list_props=calling_object->getAllProperties();//getFunctions();
							stk_element_ptr=(StackElement *)list_props->items;
							stk_element_len=list_props->count;
						}

						if(!ignore_call)
						{
							ScriptFunction *sf_aux;
							//zs_vector *global_symbols=main_function_object->registered_symbols;symbol_to_find
							if((sf_aux=findFunction(
									 calling_object
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
						if(n_args > 0){
							StackElement *stk_arg=stk_start_arg_call;
							ScriptObjectVector *var_args=NULL;
							zs_int *function_param=&sf->params->items[0];
							int effective_args=n_args < sf->params->count ? n_args:sf->params->count;
							int i=0;
							if(sf->params->count > 0){
								for(;;){

									if(((FunctionParam *)(*function_param))->by_ref == false){ // copy
										unsigned short properties = stk_arg->properties;
										if(properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
											*stk_arg=*((StackElement *)stk_arg->var_ref);
										}else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){ // remove
											ScriptObjectString *sc=new ScriptObjectString(this->zs);
											if(!sc->initSharedPtr()){
												goto lbl_exit_function;
											}
											sc->set((const char *)stk_arg->stk_value);
											stk_arg->stk_value=(void *)sc->str_value.c_str();
											stk_arg->var_ref=(void *)sc;
											stk_arg->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING;
										}
									}

									if(var_args!=NULL){
										var_args->push(stk_arg);

										if(i+1 >= (unsigned)n_args){
											break;
										}
									}else{
										if(((FunctionParam *)(*function_param))->var_args == true){ // enter var args
											var_args=new ScriptObjectVector(this->zs);
											if(!var_args->initSharedPtr()){
												goto lbl_exit_function;
											}

											// push first arg
											var_args->push(stk_arg);
											// replace for vector type...
											stk_arg->stk_value=0;
											stk_arg->var_ref=(void *)var_args;
											stk_arg->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT;
											if(i+1 >= (unsigned)n_args){
												break;
											}
										}else{
											function_param++;
											if(i+1 >= (unsigned)effective_args){
												break;
											}
										}
									}
									stk_arg++;
									i++;
								}
							}

						}

						// ... we must set the rest of parameters as undefined in case user put less params. If params exceds the number of accepted params in function,
						// will be ignored always.
						for(unsigned i = n_args; i < sf->params->count; i++){
							PUSH_UNDEFINED;
							n_args++;
						}

						ret_obj=callFunctionScript(
								calling_object
								,sf
								,stk_start_arg_call
								,n_args
								,instruction);
					}
					else{ // C function

						if((is_constructor && (sf->symbol.properties & SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS))
							){
							calling_object= (ScriptObject *)(stk_start_arg_call-2)->var_ref; // the object should be before (start_arg -1 (idx_function)  - 2 (idx_object))
						}

						ret_obj= callFunctionNative(
								 sf
								,stk_start_arg_call
								,n_args
								,instruction
								,calling_object
						);


					}

					if(vm_error == true){
						vm_error_callstack_str+=zs_strutils::format(
								"\nat %s (file:%s line:%i)" // TODO: get full symbol ?
								,sf->symbol.name.c_str()
								,SFI_GET_FILE(calling_function,instruction)
								,SFI_GET_LINE(calling_function,instruction)


						);
						goto lbl_exit_function;
					}

					// if a scriptvar --> init shared
					if(ret_obj.properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						ScriptObject *sv=(ScriptObject *)ret_obj.var_ref;
						if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
							if(!sv->initSharedPtr()){
								goto lbl_exit_function;
							}
						}
					}

					// reset stack (function+instruction (-1 op less))...
					stk_vm_current=stk_start_arg_call-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*stk_vm_current++ = ret_obj;
					}
				 }

				continue;
			 case  BYTE_CODE_RET:
				if(stk_vm_current>stk_start){ // can return something. value is +1 from stack
					stk_result=*(stk_vm_current-1);
					if(stk_result.properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
						stk_result=*((StackElement *)stk_result.var_ref); // unpack
					}

					// if scriptvariable and in the zeros list, deattach
					if(stk_result.properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						if(!STK_IS_THIS(&stk_result)){
							ScriptObject *script_var=(ScriptObject *)stk_result.var_ref;

							// deattach from zero shares if exist...
							if(deattachShareNode(script_var->shared_pointer->data.zero_shares,script_var->shared_pointer)==false){
								goto lbl_exit_function;
							}

							// and free
							free(script_var->shared_pointer);
							script_var->shared_pointer=NULL;
						}
					}
				}
				goto lbl_exit_function;
			 case  BYTE_CODE_NEW:
					script_var=NEW_CLASS_VAR_BY_IDX(this,value_op1);

					if(!script_var->initSharedPtr()){
						goto lbl_exit_function;
					}
					script_var->info_function_new=calling_function;
					script_var->instruction_new=instruction;
					(*stk_vm_current++)={NULL,script_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT};
					continue;
			 case BYTE_CODE_NEW_VECTOR: // Create new std::vector object...
					script_var=NEW_VECTOR_VAR;
					if(!script_var->initSharedPtr()){
						goto lbl_exit_function;
					}
					(*stk_vm_current++)={NULL,script_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_NEW_OBJECT: // Create new std::vector object...
				script_var=new ScriptObject(this->zs);
				if(!script_var->initSharedPtr()){
					goto lbl_exit_function;
				}
				(*stk_vm_current++)={NULL,script_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT};
				continue;
			 case  BYTE_CODE_DELETE:
					POP_ONE;
					//script_var
					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
						StackElement *se=stk_result_op1;
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
							se=(StackElement *)(stk_result_op1->var_ref);
						}

						script_var = (ScriptObject *)(se)->var_ref;
						if(script_var->idx_class >= IDX_BUILTIN_TYPE_MAX
						 ||script_var->idx_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR
						 ||script_var->idx_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT
						)
						{ // max ...
							if(!script_var->unrefSharedPtr(vm_idx_call)){
								goto lbl_exit_function;
							}

							if(script_var->isCreatedByContructor()){
								script_var->deleteNativeObjectOnDestroy(true);
							}

							se->stk_value=NULL;
							se->var_ref=NULL;
							se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
						}
					}
					else{
						VM_STOP_EXECUTE("Error deleting \"%s\". cannot perform delete on variables type \"%s\"",SFI_GET_SYMBOL_NAME(calling_function,instruction-1),stk_result_op1->typeStr());
					}
					continue;
			 case BYTE_CODE_PUSH_SCOPE:
				PUSH_VM_SCOPE(instruction->value_op2,calling_function,_stk_local_var,value_op1);
				continue;

			 case BYTE_CODE_POP_SCOPE:
				popVmScope();
				continue;
			 case BYTE_CODE_IT_NEXT:
				 VM_STOP_EXECUTE("BYTE_CODE_SET_AND_NEXT TODOOOOO!",
									 SFI_GET_SYMBOL_NAME(calling_function,instruction)
									);
				 //*((StackElement *)vm_foreach_current->key)=((ScriptObjectVector *)vm_foreach_current->ptr)->m_objVector[vm_foreach_current->idx_current++];
				 continue;
			 case BYTE_CODE_IT_END:
				 VM_STOP_EXECUTE("BYTE_CODE_IT_CHK_END TODOOOOO!",
									 SFI_GET_SYMBOL_NAME(calling_function,instruction)
									);
				 continue;
			//
			// END OPERATOR MANAGEMENT
			//
			//-----------------------------------------------------------------------------------------------------------------------
			}
			//VM_STOP_EXECUTE("operator type(%s) not implemented",ByteCodeToStr(instruction->byte_code));
		 }

	lbl_exit_function:

		//=========================
		// POP STACK
		if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN){ // if main function only remove empty shared pointers but preserve global variables!)
			// pop all scopes
			while(vm_scope_start<(vm_current_scope)){
				popVmScope(false); // do not check removeEmptySharedPointers to have better performance
			}

			removeEmptySharedPointers(vm_idx_call);
		}

		vm_idx_call--;

		// POP STACK
		//=========================
		return stk_result;
	}
}
