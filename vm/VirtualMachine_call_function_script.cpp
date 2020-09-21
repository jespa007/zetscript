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
(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)


#define OP1_AND_OP2_ARE_BOOLEANS \
(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN) && \
(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)

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
*vm_stk_current++={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

#define PUSH_NULL \
*vm_stk_current++={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL};

#define PUSH_BOOLEAN(init_value) \
*vm_stk_current++={(void *)((intptr_t)(init_value)),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN};

#define PUSH_INTEGER(init_value) \
*vm_stk_current++={(void *)((intptr_t)(init_value)),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};

#define PUSH_FLOAT(init_value) \
{\
	float aux=(float)(init_value);\
	COPY_FLOAT(&vm_stk_current->stk_value,&aux);\
	vm_stk_current->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;\
	vm_stk_current++;\
}

#define PUSH_STRING(init_value)\
if(vm_str_current==vm_str_last){THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Error vm_str out-stack");}\
*vm_str_current++=init_value;\
*vm_stk_current++={(void *)((vm_str_current-1)->c_str()),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};\


#define PUSH_SCRIPTVAR(var_ref) \
*vm_stk_current++={NULL,var_ref,INS_PROPERTY_SCRIPTVAR};


// Push stack var value (as varValue and put ptr as ref second value)...
#define PUSH_STACK_VAR(stack_ref_var) \
	*vm_stk_current++={(stack_ref_var)->stk_value,stack_ref_var,(unsigned short)(((stack_ref_var)->properties)|MSK_STACK_ELEMENT_PROPERTY_PTR_STK)};

#define POP_TWO \
stk_result_op2=--vm_stk_current;\
stk_result_op1=--vm_stk_current;

#define READ_TWO_POP_ONE \
stk_result_op2=--vm_stk_current;\
stk_result_op1=(vm_stk_current-1);

#define POP_ONE \
stk_result_op1=--vm_stk_current;

/*#define STK_IS_SCRIPT_VAR_FUNCTION(stk)\
		(((stk)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR) && (((ScriptVar *)(stk)->var_ref)->idx_class==IDX_BUILTIN_TYPE_CLASS_FUNCTION))
*/
namespace zetscript{

	StackElement VirtualMachine::callFunctionScript(
			ScriptFunction 			* calling_function,
			ScriptVar       		* this_object,
			StackElement 		  	* _stk_start,
			std::string 		  	* _str_start,
			unsigned char 			n_args,
			Instruction 			*calling_instruction){

		StackElement stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		std::string *str_start;
		StackElement *stk_start;

		if(calling_function == NULL){
			THROW_RUNTIME_ERROR("calling function null");
		}

		zs_vector *registered_symbols=calling_function->registered_symbols;

		stk_start =_stk_start;
		str_start =_str_start;

		if(str_start == NULL){
			str_start=vm_str;
		}

		StackElement *stk_local_var=NULL;
		StackElement *stk_arg=NULL;
		VM_Scope * vm_scope_start=vm_current_scope;

		ZS_PRINT_DEBUG("Executing function %s ...",calling_function->symbol_info.symbol->name.c_str());
		int idx_stk_base=(stk_start-vm_stack);//>>sizeof(StackElement *);

		if(idx_stk_base<n_args){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,calling_instruction),"Internal error (idx_stk_base<n_args) (%i<%i)",idx_stk_base,n_args);
		}

		if(n_args>0){
			stk_arg=(stk_start-n_args);
		}

		stk_local_var=stk_start;

		if(idx_stk_current < MAX_FUNCTION_CALL){
			idx_stk_current++;
		}
		else{
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,calling_instruction),"Reached max stack");
		}

		if((calling_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) ){ // C-Call

			intptr_t  fun_ptr = calling_function->ref_native_function_ptr;

			if((calling_function->symbol.properties &  SYMBOL_PROPERTY_C_STATIC_REF) == 0){
				// is function member  ...

				if(this_object!= NULL){
					StackElement *stk_prop_fun = this_object->getProperty(calling_function->symbol.idx_position);
					fun_ptr=((ScriptFunction *)stk_prop_fun->var_ref)->ref_native_function_ptr; // var ref holds function ptr
				}else{
					THROW_RUNTIME_ERROR("Internal error: expected object for function member");
				}
			}

			StackElement se = callFunctionNative(
					fun_ptr
					,calling_function
					,stk_arg
					,n_args
					, calling_instruction
					,this_object
			);

			if(idx_stk_current > 0){
				// remove tmp variables created...
				removeEmptySharedPointers(idx_stk_current,NULL);

				idx_stk_current--;
			}
			else{
				THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,calling_instruction),"Reached min stack");
			}
			return se;
		}

		Scope * scope = calling_function->symbol.scope;// ast->idx_scope;

		stk_result ={ NULL,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		unsigned symbols_count=registered_symbols->count;
		unsigned n_total_vars=n_args+symbols_count;

		if((idx_stk_base+n_total_vars) >=  VM_STACK_LOCAL_VAR_MAX){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,calling_instruction),"Error MAXIMUM stack size reached");
		}

		// Init local vars ...
		if((calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN) && (idx_stk_current > 1)){
			StackElement *ptr_aux = stk_local_var;
			for(unsigned i = 0; i < symbols_count; i++){
				Symbol *symbol=(Symbol *)registered_symbols->items[i];
				if(symbol->properties & SYMBOL_PROPERTY_IS_FUNCTION){
					*ptr_aux++={
						 0
						,(void *)symbol->ref_ptr
						,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION // starts undefined.
					};
				}else{
					// if C then pass its properties...
					*ptr_aux++={
						0,							 // no value assigned.
						0, 						     // no varref related.
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED // starts undefined.
					};
				}
			}
		}

		// advance stack from total registered symbols...
		stk_start=&stk_local_var[symbols_count];

		// PUSH STACK
		//=========================================
		//bool	aux_boolean=false;
		float aux_float=0.0;

		//ScriptFunction * aux_function_info=NULL;
		StackElement ret_obj;
		ScriptVar *script_var=NULL;
		ScriptVar *calling_object=NULL; // used on call instruction
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement *stk_var;
		//StackElement stk_access_value_aux;
		unsigned short pre_post_properties=0;
		unsigned short instruction_properties=0;
		//StackElement *stk_variable_stack_element=NULL;
		ScriptVar *var_object = NULL;

		unsigned short scope_type=0;

		StackElement *stk_dst=NULL;
		StackElement *stk_src=NULL;
		bool ok=false;

		Instruction *instructions=calling_function->instructions; // starting instruction
		Instruction *instruction_it=instructions;
		Instruction *calling_object_instruction;
		const char * symbol_access_str;

		vm_str_current=str_start;
		vm_stk_current=stk_start;

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// EXECUTE INSTRUCTION HERE
		//
		 for(;;){ // foreach byte code instruction ...

			Instruction * instruction = instruction_it++;

			const unsigned char operator_type=instruction->byte_code;
			const unsigned char value_op1=instruction->value_op1;

			if(instruction->properties & MSK_INSTRUCTION_PROPERTY_START_EXPRESSION){
				vm_str_current=str_start; // reset op ptr
				vm_stk_current=stk_start;
			}

			switch(operator_type){
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_LOAD:
			case BYTE_CODE_VGET:

				if(value_op1==LoadType::LOAD_TYPE_VARIABLE || operator_type==BYTE_CODE_VGET){
					if(operator_type==BYTE_CODE_VGET){
						bool ok=false;
						POP_TWO;

						if( (stk_result_op1->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_PTR_STK)) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_PTR_STK)){
							var_object = (ScriptVar *)(((StackElement *)stk_result_op1->var_ref)->var_ref);
						}

						if(var_object != NULL){
							if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){
								ScriptVarVector * var_vector = (ScriptVarVector *)var_object;
								//zs_vector * stk_properties=script_var_vectors->getAllProperties();

								if(STK_VALUE_IS_INT(stk_result_op2)){
									// determine object ...
									try{
										stk_var =var_vector->getUserProperty(STK_VALUE_TO_INT(stk_result_op2));
									}catch(std::exception & ex){
										THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"%s",ex.what());
									}
									ok = true;
								}else{
									THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Expected std::vector-index as integer or std::string");
								}
							}
						}
						if(!ok){
							THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Variable \"%s\" is not type std::vector",
								SFI_GET_SYMBOL_NAME(calling_function,instructions+instruction->value_op2)
							);
						}
					}else{ // load variable ...

						instruction_properties=instruction->properties;
						scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(instruction_properties);

						switch(scope_type){
						default: // global...
							stk_var = &vm_stack[instruction->value_op2];
							break;
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS:
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS:

							stk_var=NULL;
							if(scope_type & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){

								Instruction *previous_ins= (instruction-1);

								if(previous_ins->byte_code == BYTE_CODE_NEW){
									stk_result_op1=(vm_stk_current-1);
								}
								else{
									POP_ONE; // get var op1 and symbol op2
								}


								if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)!= MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)
								{
									THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(calling_function,previous_ins));
								}

								calling_object = (ScriptVar  *)stk_result_op1->var_ref;
								if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {
									StackElement *stk_ins=((StackElement *)stk_result_op1->var_ref);

									if(stk_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
										calling_object=((ScriptVar *)stk_ins->var_ref);
									}
								}

								if(calling_object == NULL)
								{
									THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(calling_function,previous_ins));
								}

								calling_object_instruction=previous_ins;

							}
							else{ // this scope ...
								calling_object_instruction=instruction;
								calling_object=this_object;
								if(instruction->value_op2!=ZS_IDX_UNDEFINED){ // not undefined load...
									stk_var=calling_object->getProperty(instruction->value_op2);
								}
							}

							if(stk_var == NULL){ // try to get one...
								int idx_stk_element=ZS_IDX_UNDEFINED;

								if(instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR){
									symbol_access_str=FUNCTION_MEMBER_CONSTRUCTOR_NAME;
								}else{
									symbol_access_str=SFI_GET_SYMBOL_NAME(calling_function,instruction);
								}

								stk_var=calling_object->getProperty(symbol_access_str, &idx_stk_element);

								// not exist ... add
								if(stk_var == NULL){
									stk_var=calling_object->addProperty(symbol_access_str, calling_function,instruction,NULL,&idx_stk_element);
								}

								if(calling_object==this_object){ //this --> it save idx instruction to fast look up next time...
									instruction->value_op2=idx_stk_element;
								}
							}
							break;
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL:
							stk_var = &stk_local_var[instruction->value_op2];
							break;
						}
					}

					pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(instruction_properties);

					if(HAS_PRE_POST_INC_DEC_OP(instruction_properties)
							&&
						((stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_READ_ONLY)!=0)){
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),
							"Cannot perform self operations on constant elements",
							SFI_GET_SYMBOL_NAME(calling_function,instruction),
							symbol_access_str);
					}

					/* all preoperators makes load var as constant ... */
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_INC:
							PERFORM_PRE_POST_OPERATOR(stk_var,++);
							(*vm_stk_current++)=*stk_var;
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_DEC:
							PERFORM_PRE_POST_OPERATOR(stk_var,--);
							(*vm_stk_current++)=*stk_var;
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_DEC:
							(*vm_stk_current++)=*stk_var;
							PERFORM_PRE_POST_OPERATOR(stk_var,--);
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_INC:
							(*vm_stk_current++)=*stk_var;
							PERFORM_PRE_POST_OPERATOR(stk_var,++);
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT:
							switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_var->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*vm_stk_current++={(void *)(-(*((intptr_t *)stk_var->var_ref))),stk_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_PTR_STK|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C};
								}else{
									*vm_stk_current++={(void *)(-(((intptr_t)stk_var->stk_value))),stk_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_PTR_STK};
								}
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*vm_stk_current++={(void *)(!(*((bool *)stk_var->var_ref))),stk_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_PTR_STK|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C};
								}else{
									*vm_stk_current++={(void *)(!(((bool)stk_var->stk_value))),stk_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_PTR_STK};
								}
								break;

							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									COPY_FLOAT(&aux_float,stk_var->var_ref);
								}else{
									COPY_FLOAT(&aux_float,&stk_var->stk_value);
								}
								aux_float=-aux_float;
								COPY_FLOAT(&vm_stk_current->stk_value,&aux_float);
								*vm_stk_current={vm_stk_current->stk_value,stk_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_PTR_STK};

								if(stk_var->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									vm_stk_current->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
								}

								vm_stk_current++;
								break;
							default:
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"internal error:cannot perform pre operator - because is not number");
							}
							continue;
					default:
						// update var if needed ...
						PUSH_STACK_VAR(stk_var);
						break;
					}

					// ok in case is C we must udpate stk_value becaus it can be updated from C++. (only primitives)
					if(stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_var->properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
							//(vm_stk_current-1)->stk_value=(void *)((*((int *)stk_var->var_ref)));
							memcpy(&((vm_stk_current-1)->stk_value),stk_var->var_ref,sizeof(int));
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
							COPY_FLOAT(&((vm_stk_current-1)->stk_value),stk_var->var_ref);
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
							(vm_stk_current-1)->stk_value=(void *)((*((bool *)stk_var->var_ref)));
							break;
						}
					}
					continue;

				}else if(value_op1==LoadType::LOAD_TYPE_NULL){
					PUSH_NULL;
					continue;
				}else if(value_op1==LoadType::LOAD_TYPE_UNDEFINED){
					PUSH_UNDEFINED;
					continue;
				}else if(value_op1==LoadType::LOAD_TYPE_CONSTANT){
					(*vm_stk_current)=*(((ConstantValue *)instruction->value_op2));

					pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(instruction->properties);

					// all preoperators makes load var as constant ...
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT:
							switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(vm_stk_current->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
								vm_stk_current->stk_value=(void *)(-((intptr_t)vm_stk_current->stk_value));
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
								vm_stk_current->stk_value=(void *)(!((bool)vm_stk_current->stk_value));
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								COPY_FLOAT(&aux_float,&vm_stk_current->stk_value);
								aux_float=-aux_float;
								COPY_FLOAT(&vm_stk_current->stk_value,&aux_float);
								break;
							default:
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"internal error:cannot perform pre operator - constant because is not numeric");
							}
							break;
					}
					vm_stk_current++;
					continue;
				}else if(value_op1== LoadType::LOAD_TYPE_FUNCTION){ // expect load local/global symbol to load function...
					scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(instruction->properties);
					switch(scope_type){
					case 0: // global
						stk_var = &vm_stack[instruction->value_op2];
						break;
					case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL:
						stk_var = &stk_local_var[instruction->value_op2];
						break;
					default:
					case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS:
					case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS:
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"internal error: load function only contemplates global/local scope");
						break;
					}
					*vm_stk_current++=*stk_var;
					continue;
				}else if(value_op1== LoadType::LOAD_TYPE_ARGUMENT){
					*vm_stk_current++=stk_arg[instruction->value_op2]; // copy arg directly ...
					continue;
				}
				else{
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"BYTE_CODE_VGET/BYTE_CODE_VLOAD: unknow value op1 loadtype: %i",value_op1);
				}
				continue;

			case BYTE_CODE_STORE:
			case BYTE_CODE_VPUSH:
			case BYTE_CODE_PUSH_ATTR:

				{
					bool assign_metamethod=false;
					bool push_value=true;

					if(operator_type==BYTE_CODE_VPUSH){
						POP_ONE; // only pops the value, the last is the std::vector variable itself
						ScriptVar *vec_obj = NULL;
						if((vm_stk_current-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							vec_obj = (ScriptVar *)(vm_stk_current-1)->var_ref;
							if(vec_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){ // push value ...
								// op1 is now the src value ...
								stk_src=stk_result_op1;
								stk_dst=((ScriptVarVector *)vec_obj)->newSlot();
								ok=true;
							}
						}

						if(!ok){
							THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Expected std::vector object");
						}

						push_value=false;

					}else if(operator_type==BYTE_CODE_PUSH_ATTR){

						POP_TWO; // first must be a string that describes variable name and the other the variable itself ...
						ScriptVar *struct_obj = NULL;
						if((vm_stk_current-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							struct_obj = (ScriptVar *)(vm_stk_current-1)->var_ref;
							if(struct_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_DICTIONARY){ // push value ...
								// op1 is now the src value ...
								if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
									StackElement *se=NULL;
									const char *str = (const char *)stk_result_op1->stk_value;
									stk_src=stk_result_op2;
									se =((ScriptVarDictionary *)struct_obj)->addProperty(str,calling_function,instruction);

									if(se == NULL){
										throw "internal error se==NULL";
									}

									stk_dst=se;
									ok=true;
								}
								else{
									THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"internal error (operator2 is not std::string)");
								}
							}else{
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Expected dictionary object");
							}
						}else{
							THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Expected scriptvar");
						}
						push_value=false;
					}
					else{ // pop two parameters nothing ...
						POP_TWO;

						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {// == ScriptVar::VAR_TYPE::OBJECT){
							stk_dst=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
						}else{
							THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Expected l-value on assignment ('=')");
						}

						if(vm_foreach_current!=NULL){
							if(stk_dst == vm_foreach_current->key){
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"for-in key value is read only");
							}
						}

						stk_src=stk_result_op2; // store ptr instruction2 op as src_var_value

						// we need primitive stackelement in order to assign...
						if(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {// == ScriptVar::VAR_TYPE::OBJECT){
							stk_src=(StackElement *)stk_src->var_ref; // stk_value is expect to contents a stack variable

						}

						// ok load object pointer ...
						if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

							if(((ScriptVar *)stk_dst->var_ref)->itHasSetMetamethod()){
								if(!applyMetamethod(
										calling_object
										,calling_function
										,instruction
										,"="
										,BYTE_CODE_METAMETHOD_SET
										,stk_result_op1
										,stk_result_op2
								)){
									THROW_RUNTIME_ERROR("cannot perform operation '='");\
								}
								assign_metamethod=true;
							}
						}
					}

					if(! assign_metamethod){

						if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_READ_ONLY){
							THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Assign to constant element is not allowed ");
						}

						StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...
						{
							ScriptVar *script_var=NULL;
							std::string *aux_str=NULL;
							void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update stk_value on save) */
							void **stk_src_ref=&stk_src->stk_value;
							void **stk_dst_ref=&stk_dst->stk_value;
							if(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
								stk_src_ref=(void **)((stk_src)->var_ref);
							}
							if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
								if(!STK_VALUE_IS_FLOAT_OR_INT(stk_src) && STK_VALUE_IS_FLOAT_OR_INT(stk_dst)){
									if(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_src->properties) != GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_dst->properties)
									){
										THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",SFI_GET_SYMBOL_NAME(calling_function,instruction));
									}else{
										if(
											(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_src->properties) == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)
										){
											THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");
										}
									}
								}
								stk_dst_ref=(void **)((stk_dst)->var_ref);
								copy_aux=&((stk_dst)->stk_value);
							}
							unsigned short type_var=stk_src->properties;
							unsigned short runtime_var=0; /* there's no reason to heredate runtime_props ?!? GET_MSK_STACK_ELEMENT_PROPERTY_RUNTIME(type_var);*/
							if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED){
								stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL){
								stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){
								stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER;
								*((int *)stk_dst_ref)=*((int *)stk_src_ref);
								if(copy_aux!=NULL)(*(int *)copy_aux)=*((int *)stk_src_ref);
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
								stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
								*((float *)stk_dst_ref)=*((float *)stk_src_ref);
								if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)stk_src_ref);
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
								stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;
								*((bool *)stk_dst_ref)=*((bool *)stk_src_ref);
								if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)stk_src_ref);
							}else if(type_var  &  MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
								*stk_dst=*stk_src;
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
								if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*((std::string *)stk_dst->var_ref)=((const char *)stk_src->stk_value);/* Assign std::string */
									stk_dst->stk_value=(void *)(((std::string *)stk_dst->var_ref)->c_str());/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */
								}else{
									if(((stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)==0) || (stk_dst->var_ref==NULL)){/* Generates a std::string var */
										script_var= NEW_STRING_VAR;
										stk_dst->var_ref=script_var;
										aux_str=&(((ScriptVarString *)script_var)->str_value);
										stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
										script_var->initSharedPtr();
										sharePointer(script_var->ptr_shared_pointer_node);
									}
									(*aux_str)=((const char *)stk_src->stk_value);
									stk_dst->stk_value=(void *)aux_str->c_str();/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */
								}
							}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
								script_var=(ScriptVar *)stk_src->var_ref;
								stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
								stk_dst->stk_value=NULL;
								stk_dst->var_ref=script_var;
								sharePointer(script_var->ptr_shared_pointer_node);
							}else{
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"(internal) cannot determine var type %i",GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_src->properties));
							}
							if(copy_aux!=NULL)stk_dst->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
						}


						// check old var structure ...
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(old_stk_dst.properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION: // we aren't take care about nothing! :)
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING: // type std::string is really a std::string or variable ?!?!
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR: // we are getting script vars ...
							if(!(((old_stk_dst.properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C))==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)))){
								if(old_stk_dst.var_ref!=NULL){ // it had a pointer (no constant)...
									if(old_stk_dst.var_ref != stk_dst->var_ref){ // unref pointer because new pointer has been attached...
										unrefSharedScriptVar(((ScriptVar  *)old_stk_dst.var_ref)->ptr_shared_pointer_node);
									}
								}
							}
							break;
						}
					}
					if(push_value){ // to be able to do multiple assigns like a=b=c=1 (1 will be pushed in each store instruction)
						*vm_stk_current++=*stk_src;
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
				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){ // operation will result as integer.
					PUSH_BOOLEAN((!((bool)(stk_result_op1->stk_value))));
				}else{
					if(!applyMetamethod(
							calling_object
							,calling_function
							,instruction
							,"!"
							,BYTE_CODE_METAMETHOD_NOT
							,stk_result_op1
							,stk_result_op2
							)){
						THROW_RUNTIME_ERROR("cannot perform operation '!'");
					}
				}
				continue;
			case BYTE_CODE_NEG: // -
				POP_ONE;
				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){ // operation will result as integer.
					PUSH_INTEGER((-((intptr_t)(stk_result_op1->stk_value))));
				}else if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
					COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
					PUSH_FLOAT(-f_aux_value1);
				}else{ // try metamethod ...
					if(!applyMetamethod(
							 calling_object
							,calling_function
							,instruction
							,"-"
							,BYTE_CODE_METAMETHOD_NEG
							,stk_result_op1
							,stk_result_op2
					)){
						THROW_RUNTIME_ERROR("cannot perform operation '-'");
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

					unsigned short mask_properties =GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties);
					unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);
					if(mask_properties==MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){ // fast operation
						PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) + STK_VALUE_TO_INT(stk_result_op2));
					}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){
						if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){
							COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
							PUSH_FLOAT(STK_VALUE_TO_INT(stk_result_op1) + f_aux_value2);
						}else{
							COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
							PUSH_FLOAT(f_aux_value1 + STK_VALUE_TO_INT(stk_result_op2));
						}
					// TODO: Given 2 StackElement try to convert their values and return new stack element string
					}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						if (STK_VALUE_IS_STRING(stk_result_op1) && STK_VALUE_IS_INT(stk_result_op2)){
							sprintf(str_aux,"%s%i",((const char *)stk_result_op1->stk_value),(int)((intptr_t)stk_result_op2->stk_value));
							//PUSH_STRING(str_aux);
							if(vm_str_current==vm_str_last){
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Error vm_str out-stack");
							}
							*vm_str_current++=str_aux;\
							*vm_stk_current++={(void *)((vm_str_current-1)->c_str()),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};\

						}else{
							sprintf(str_aux,"%i%s",(int)((intptr_t)stk_result_op1->stk_value),((const char *)stk_result_op2->stk_value));
							PUSH_STRING(str_aux);
						}
					}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						if (STK_VALUE_IS_STRING(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){
							COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
							sprintf(str_aux,"%s%f",((const char *)stk_result_op1->stk_value),f_aux_value2);
							PUSH_STRING(str_aux);

						}else{
							COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
							sprintf(str_aux,"%f%s",f_aux_value1,((const char *)stk_result_op2->stk_value));
							PUSH_STRING(str_aux);
						}
					}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						if (STK_VALUE_IS_STRING(stk_result_op1) && STK_VALUE_IS_UNDEFINED(stk_result_op2)){
							sprintf(str_aux,"%s%s",((const char *)stk_result_op1->stk_value),"undefined");
							PUSH_STRING(str_aux);
						}else{
							sprintf(str_aux,"%s%s","undefined",((const char *)stk_result_op2->stk_value));
							PUSH_STRING(str_aux);
						}

					}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						if (STK_VALUE_IS_STRING(stk_result_op1) && STK_VALUE_IS_BOOLEAN(stk_result_op2)){
							sprintf(str_aux,"%s%s",((const char *)stk_result_op1->stk_value),((bool)(stk_result_op2->stk_value))?"true":"false");
							PUSH_STRING(str_aux);
						}else{
							sprintf(str_aux,"%s%s",((bool)(stk_result_op1->stk_value))?"true":"false",((const char *)stk_result_op2->stk_value));
							PUSH_STRING(str_aux);
						}
					}else if (mask_properties== MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
							sprintf(str_aux,"%s%s",((const char *)stk_result_op1->stk_value),((const char *)(stk_result_op2->stk_value)));
							PUSH_STRING(str_aux);
					}else if(mask_properties== MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
						COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
						COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
						PUSH_FLOAT(f_aux_value1 + f_aux_value2);
					}else{ // try metamethod ...

						if(!applyMetamethod(
								 calling_object
								,calling_function
								,instruction
								,"+"
								,BYTE_CODE_METAMETHOD_ADD
								,stk_result_op1
								,stk_result_op2
								)){
							THROW_RUNTIME_ERROR("cannot perform operation '+'");
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

				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) {// == ScriptVar::VAR_TYPE::OBJECT){
					stk_result_op1=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
				}
				switch((intptr_t)stk_result_op2->stk_value){
					case IDX_BUILTIN_TYPE_INT_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER)!=0);
						break;
					case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)!=0);
						break;
					case IDX_BUILTIN_TYPE_BOOL_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN)!=0);
						break;
					case IDX_BUILTIN_TYPE_STRING_PTR_C:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)!=0);
						break;
					case IDX_BUILTIN_TYPE_FUNCTION:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)!=0);
						break;
					default:
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							bool b = this->script_class_factory->isClassInheritsFrom(			//
									((ScriptVar *)(stk_result_op1->var_ref))->idx_class // A
									, (intptr_t)stk_result_op2->stk_value				// B
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
				// check whether signatures matches or not ...
				// 1. get function object ...
				 {
					ScriptFunction *sf = NULL;
					unsigned char n_args=instruction->value_op1;
					StackElement *stk_function_ref=NULL;
					unsigned short scope_type=0;
					intptr_t idx_function=ZS_IDX_UNDEFINED;
					bool is_constructor=(char)instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR;

					StackElement *stk_start_arg_call=(vm_stk_current-n_args);
					stk_function_ref = ((stk_start_arg_call-1));

					scope_type = GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(instruction->properties);
					calling_object = this_object;

					if(stk_function_ref->stk_value != NULL){ // expects calling object
						calling_object=(ScriptVar *)stk_function_ref->stk_value;
					}

					if(stk_function_ref->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
						stk_function_ref=(StackElement *)stk_function_ref->var_ref;
					}

					if((stk_function_ref->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION))==0){
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"%s is not function",SFI_GET_SYMBOL_NAME(calling_function,instruction));
					}

					sf=(ScriptFunction *)stk_function_ref->var_ref;

					if(sf == NULL){
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"internal error ScriptFunction null");
					}

					// if there's more than 1 symbol with same number of parameters have to get the right one...
					if(sf->function_should_be_deduced_at_runtime){

						StackElement *stk_element_ptr=vm_stack;
						int stk_element_len = main_function_object->registered_symbols->count;
						bool ignore_call=false;

						if(
							scope_type&(MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS|MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS)
						){
							ignore_call= is_constructor && calling_object->isNativeObject() && n_args==0;
							zs_vector * list_props=calling_object->getAllProperties();//getFunctions();
							stk_element_ptr=(StackElement *)list_props->items;
							stk_element_len=list_props->count;
						}

						if(!ignore_call)
						{
							//zs_vector *global_symbols=main_function_object->registered_symbols;
							if((sf=findFunction(
									 calling_object
									,calling_function
									,instruction
									,stk_element_ptr
									,stk_element_len
									,is_constructor
									,sf->symbol.name
									,stk_start_arg_call
									,n_args))==NULL){

								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"cannot find function \"%s\"",symbol_to_find.c_str());
							}
						}
					}

					// call function
					if(sf !=NULL)
					{
						if((sf->symbol.properties & SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS) && ((sf->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)== 0)){

						}

						if((sf->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){ // if script function...
							// we must set the rest of parameters as undefined in case user put less params as original function ...
							for(unsigned i = n_args; i < sf->params->count; i++){
								*vm_stk_current++={
									0,							 // no value assigned.
									NULL,			     // no varref related.
									MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED // starts undefined.
								};
								n_args++;
							}
						}

						// by default virtual machine gets main object class in order to run functions ...
						ret_obj=callFunctionScript(sf,calling_object,vm_stk_current,vm_str_current,n_args,instruction);

						if(ret_obj.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							// if c pointer is not from application share ...
							((ScriptVar *)(ret_obj.var_ref))->initSharedPtr();
						}

						if(cancel_execution) {
							if(custom_error!=NULL){
								THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),custom_error);
							}
						}
					}

					// reset stack (function+instruction (-1 op less))...
					vm_stk_current=stk_start_arg_call-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*vm_stk_current++ = ret_obj;
					}
				 }
				continue;
			 case  BYTE_CODE_NEW:
					script_var=NEW_CLASS_VAR_BY_IDX(this,value_op1);

					script_var->initSharedPtr();
					script_var->info_function_new=calling_function;
					script_var->instruction_new=instruction;
					(*vm_stk_current++)={NULL,script_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
					continue;
			 case  BYTE_CODE_DELETE:
					POP_ONE;
					//script_var
					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						StackElement *se=stk_result_op1;
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
							se=(StackElement *)(stk_result_op1->var_ref);
						}

						script_var = (ScriptVar *)(se)->var_ref;
						if(script_var->idx_class >= IDX_BUILTIN_TYPE_MAX
						 ||script_var->idx_class==IDX_BUILTIN_TYPE_CLASS_VECTOR
						 ||script_var->idx_class==IDX_BUILTIN_TYPE_CLASS_DICTIONARY
						)
						{ // max ...
							script_var->unrefSharedPtr();

							if(script_var->isCreatedByContructor()){
								script_var->setDelete_C_ObjectOnDestroy(true);
							}

							se->stk_value=NULL;
							se->var_ref=NULL;
							se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
						}
					}
					else{
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"delete op: expected scriptvar var but it was \"%s\"",stk_result_op1->toString());
					}
					continue;
			 case BYTE_CODE_INST_VEC: // Create new std::vector object...
					script_var=NEW_VECTOR_VAR;
					script_var->initSharedPtr();
					(*vm_stk_current++)={NULL,script_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
					continue;

			 case  BYTE_CODE_INST_DICT: // Create new std::vector object...
				script_var=NEW_STRUCT_VAR;
				script_var->initSharedPtr();
				(*vm_stk_current++)={NULL,script_var,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
				continue;
			 case  BYTE_CODE_RET:
				if(vm_stk_current>stk_start){ // can return something
					stk_result=*(vm_stk_current-1);
					// remove shared pointer if scriptvar ...
					if(stk_result.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						if(stk_result.properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
							stk_result=*((StackElement *)((StackElement *)stk_result.var_ref));
						}
						// unref pointer to be deallocated from gc...
						//((ScriptVar *)stk_result.var_ref)->ptr_shared_pointer_node->data.shared_ptr=NULL;
						if(((ScriptVar *)stk_result.var_ref)->ptr_shared_pointer_node!=NULL){
							free(((ScriptVar *)stk_result.var_ref)->ptr_shared_pointer_node);
							((ScriptVar *)stk_result.var_ref)->ptr_shared_pointer_node=NULL;
						}
						// share pointer  + 1
					}
					else{ // remove all involved inforamtion
						stk_result.properties&=~MSK_STACK_ELEMENT_PROPERTY_PTR_STK; // now is not an scriptvar anymore (if it was)
						stk_result.var_ref =NULL;
					}
				}
				goto lbl_exit_function;
			 case BYTE_CODE_PUSH_SCOPE:

				PUSH_VM_SCOPE(instruction->value_op2,calling_function,stk_local_var,value_op1);

				if(value_op1 & ScopeProperty::SCOPE_PROPERTY_FOR_IN){
					if(vm_foreach_current == &vm_foreach[VM_FOREACH_MAX-1]){
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Max foreach reached");
					}

					vm_foreach_current++;
				}
				continue;

			 case BYTE_CODE_POP_SCOPE:
				vm_str_current=str_start; // reset op ptr
				vm_stk_current=stk_start;
				if(!popVmScope(idx_stk_current,NULL,value_op1)){
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Error pop scope");
				}

				if(value_op1 & ScopeProperty::SCOPE_PROPERTY_FOR_IN){
					if(vm_foreach_current == &vm_foreach[0]){
						THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Min foreach reached");
					}

					memset(vm_foreach_current,0,sizeof(VM_Foreach));
					vm_foreach_current--;
				}
				continue;

			 case BYTE_CODE_IT_INI:
				 POP_TWO; // op2:std::vector or struct op1:key iterator


				 if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK) == 0){
					 THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Internal error: Expected stackvar");
				 }
				 vm_foreach_current->key=(StackElement *)stk_result_op1->var_ref;
				 vm_foreach_current->ptr=NULL;
				 vm_foreach_current->idx_current=0;

				 var_object = (ScriptVar *)stk_result_op2->var_ref;
				 if( (stk_result_op2->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_PTR_STK)) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_PTR_STK)){
						var_object = (ScriptVar *)(((StackElement *)stk_result_op2->var_ref)->var_ref);
				 }

				if(var_object != NULL && (var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR || var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_DICTIONARY)){

					if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){ // integer as iterator...
						*vm_foreach_current->key={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};
					}
					else{ // struct -> std::string as iterator...
						*vm_foreach_current->key={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};
					}

					vm_foreach_current->ptr = var_object;
				}else{
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Variable \"%s\" is not type std::vector or struct",
						SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
					);
				}
				 continue;
			 case BYTE_CODE_SET_AND_NEXT:
				 THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"BYTE_CODE_SET_AND_NEXT TODOOOOO!",
									 SFI_GET_SYMBOL_NAME(calling_function,instruction)
									);
				 //*((StackElement *)vm_foreach_current->key)=((ScriptVarVector *)vm_foreach_current->ptr)->m_objVector[vm_foreach_current->idx_current++];
				 continue;
			 case BYTE_CODE_IT_CHK_END:
				 THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"BYTE_CODE_IT_CHK_END TODOOOOO!",
									 SFI_GET_SYMBOL_NAME(calling_function,instruction)
									);
				 continue;
			//
			// END OPERATOR MANAGEMENT
			//
			//-----------------------------------------------------------------------------------------------------------------------
			}
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"operator type(%s) not implemented",ByteCodeToStr(instruction->byte_code));
		 }

	lbl_exit_function:

		//=========================
		// POP STACK
		if(calling_function->idx_script_function == IDX_SCRIPT_FUNCTION_MAIN){ // if main function only remove empty shared pointers but preserve global variables!)
			removeEmptySharedPointers(idx_stk_current,NULL);
		}
		else{ // else any other script function pop all scopes...
			// pop all scopes
			while(vm_scope_start<(vm_current_scope)){
				popVmScope(idx_stk_current,stk_result.var_ref,0);
			}
		}
		idx_stk_current--;
		// POP STACK
		//=========================
		return stk_result;
	}
}
