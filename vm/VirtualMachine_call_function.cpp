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
*stk_current_data++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

#define PUSH_NULL \
*stk_current_data++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL};

#define PUSH_BOOLEAN(init_value) \
*stk_current_data++={(void *)((intptr_t)(init_value)),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN};

#define PUSH_INTEGER(init_value) \
*stk_current_data++={(void *)((intptr_t)(init_value)),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};

#define PUSH_FLOAT(init_value) \
{\
	float aux=(float)(init_value);\
	COPY_FLOAT(&stk_current_data->stk_value,&aux);\
	stk_current_data->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;\
	stk_current_data++;\
}

#define PUSH_STRING(init_value)\
if(ptr_current_str==ptr_last_str){writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Error stk_string out-stack");RETURN_ERROR;}\
*ptr_current_str++=init_value;\
*stk_current_data++={(void *)((ptr_current_str-1)->c_str()),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};\



#define PUSH_FUNCTION(fun_obj,class_obj,function_prop) \
*stk_current_data++={(void *)(fun_obj),class_obj,(unsigned short)(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION|function_prop)};

#define PUSH_SCRIPTVAR(var_ref) \
*stk_current_data++={NULL,var_ref,INS_PROPERTY_SCRIPTVAR};


// Push stack var value (as varValue and put ptr as ref second value)...
#define PUSH_STACK_VAR(stack_ref_var) \
	*stk_current_data++={(stack_ref_var)->stk_value,stack_ref_var,(unsigned short)(((stack_ref_var)->properties)|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)};

#define POP_TWO \
stk_result_op2=--stk_current_data;\
stk_result_op1=--stk_current_data;

#define READ_TWO_POP_ONE \
stk_result_op2=--stk_current_data;\
stk_result_op1=(stk_current_data-1);

#define POP_ONE \
stk_result_op1=--stk_current_data;

namespace zetscript{

	StackElement VirtualMachine::callFunctionInternal(
			ScriptFunction 			* calling_script_function,
			ScriptVar       		* this_object,
			bool & error,
			StackElement 		  	* _ptrStartOp,
			std::string 		  	* _ptrStartStr,
			unsigned char n_args,
			Instruction 			*calling_instruction){

		error=false;
		StackElement callc_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		std::string *ptrStartStr;
		StackElement *ptrStartOp;

		if(calling_script_function == NULL){
			RETURN_ERROR;
		}

		std::vector<SymbolInfo> * local_var=&calling_script_function->local_variable;

		ptrStartOp =_ptrStartOp;
		ptrStartStr =_ptrStartStr;

		if(ptrStartStr == NULL){
			ptrStartStr=stk_string;
		}

		StackElement *ptrLocalVar=NULL;
		StackElement *ptrArg=NULL;
		VM_ScopeInfo * ptrStartScopeInfo=NULL;

		ZS_PRINT_DEBUG("Executing function %s ...",calling_script_function->symbol_info.symbol->name.c_str());
		int idxBaseStk=(ptrStartOp-stack);//>>sizeof(StackElement *);

		if(idxBaseStk<n_args){
			writeError(SFI_GET_FILE_LINE(calling_script_function,calling_instruction),"Internal error (idxBaseStk<n_args) (%i<%i)",idxBaseStk,n_args);
			RETURN_ERROR;
		}

		if(n_args>0){
			ptrArg=(ptrStartOp-n_args);
		}

		ptrLocalVar=ptrStartOp;

		if(idx_current_stack < MAX_FUNCTION_CALL){

			/* VM_ScopeInfo		scope_info[VM_LOCAL_VAR_MAX_STACK];
			std::string 				stk_string[VM_LOCAL_VAR_MAX_STACK]; // aux values for std::string ...
			std::string              *ptr_last_str;
			std::string              *ptr_current_str;

			 StackElement     stack[VM_LOCAL_VAR_MAX_STACK];*/

			//printf("call:%i left:%lu\n",idx_current_stack,max_scope_info-current_scope_info_ptr);// scope_info[VM_LOCAL_VAR_MAX_STACK-1]);
			idx_current_stack++;
		}
		else{
			writeError(SFI_GET_FILE_LINE(calling_script_function,calling_instruction),"Reached max stack");
			RETURN_ERROR;
		}

		if((calling_script_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) ){ // C-Call

			if((calling_script_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_IS_POLYMORPHIC)){ // cannot call...
				writeError(SFI_GET_FILE_LINE(calling_script_function,calling_instruction),"Function \"%s%s\" derives from polymorphic class and cannot be executed due pointer changes at runtime. You have two options:\n"
						"1. Set register_C_baseSymbols(false) and  re-register the function using REGISTER_C_FUNCTION_MEMBER\n"
						"2. Adapt all virtual functions/classes to no non-virtual\n"
						,this_object==NULL?"":this_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(this_object->getClassName()+"::").c_str():""
						,calling_script_function->symbol_info.symbol->name.c_str());
				RETURN_ERROR;
			}

			intptr_t  fun_ptr = calling_script_function->symbol_info.ref_ptr;

			if((calling_script_function->symbol_info.symbol_info_properties &  SYMBOL_INFO_PROPERTY_STATIC_REF) == 0){ // if not static then is function depends of object ...

				if(this_object!= NULL){
					fun_ptr = this_object->getFunctionSymbolByIndex(calling_script_function->symbol_info.idx_symbol)->proxy_ptr;
				}
			}

			StackElement se = callFunction_C(
					fun_ptr
					,calling_script_function
					,error
					,ptrArg
					,n_args
					, calling_instruction
					,this_object
			);

			if(idx_current_stack > 0){
				idx_current_stack--;
			}
			else{
				writeError(SFI_GET_FILE_LINE(calling_script_function,calling_instruction),"Reached min stack");
				RETURN_ERROR;
			}
			return se;
		}

		short scope_index = calling_script_function->symbol_info.symbol->idx_scope;// ast->idx_scope;

		if(calling_script_function->idx_script_function != 0){

			BYTE_CODE_PUSH_SCOPE(scope_index,calling_script_function,ptrLocalVar,0);
			ptrStartScopeInfo = current_scope_info_ptr;
		}

		callc_result ={ NULL,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		unsigned size_local_var=local_var->size();
		unsigned n_total_vars=n_args+size_local_var;

		if((idxBaseStk+n_total_vars) >=  VM_LOCAL_VAR_MAX_STACK){
			writeError(SFI_GET_FILE_LINE(calling_script_function,calling_instruction),"Error MAXIMUM stack size reached");
			RETURN_ERROR;
		}

		// Init local vars ...
		if(calling_script_function->idx_script_function != 0){ // is not main function, so we have to initialize vars.


			if(idx_current_stack > 1){ // not global vars, then initialize variables as undefined...
				StackElement *ptr_aux = ptrLocalVar;
				for(unsigned i = 0; i < size_local_var; i++){

					// if C then pass its properties...
					*ptr_aux++={
						0,							 // no value assigned.
						0, 						     // no varref related.
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED // starts undefined.
					};
				}
			}
		}

		ptrStartOp=&ptrLocalVar[size_local_var];

		// PUSH STACK
		//=========================================
		//bool	aux_boolean=false;
		float aux_float=0.0;

		ScriptFunction * aux_function_info=NULL;
		StackElement ret_obj;
		ScriptVar *svar=NULL;
		ScriptVar *calling_object=NULL;
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement *ldrVar;
		unsigned short pre_post_properties=0;
		unsigned short properties=0;
		StackElement *variable_stack_element;
		//FunctionSymbol *si;
		ScriptVar *var_object = NULL;

		unsigned short scope_type=0;

		StackElement *dst_ins=NULL;
		StackElement *src_ins=NULL;
		bool ok=false;

		Instruction *start_it=calling_script_function->instruction;
		Instruction *instruction_it=start_it;

		ptr_current_str=ptrStartStr;
		stk_current_data=ptrStartOp;

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// EXECUTE INSTRUCTION HERE
		//
		 for(;;){ // foreach asm instruction ...

			Instruction * instruction = instruction_it++;

			const unsigned char operator_type=instruction->byte_code;
			const unsigned char value_op1=instruction->value_op1;

			switch(operator_type){
			//default:
			//	writeError(SFI_GET_FILE_LINE(instruction->idxAstNode),"operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
			//	RETURN_ERROR;
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_LOAD:
			case BYTE_CODE_VGET:

				if(value_op1==LoadType::LOAD_TYPE_VARIABLE || operator_type==BYTE_CODE_VGET){
					if(operator_type==BYTE_CODE_VGET){

						bool ok=false;

						POP_TWO;

						if( (stk_result_op1->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)){
							var_object = (ScriptVar *)(((StackElement *)stk_result_op1->var_ref)->var_ref);
						}

						if(var_object != NULL){
							if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){
								ScriptVarVector * script_var_vectors = (ScriptVarVector *)var_object;

								if(STK_VALUE_IS_INT(stk_result_op2)){
									// determine object ...
									int v_index = STK_VALUE_TO_INT(stk_result_op2);

									// check indexes ...
									if(v_index < 0){
										writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Negative index std::vector (%i)",v_index);
										RETURN_ERROR;
									}

									if(v_index >= (int)(script_var_vectors->variable.size())){
										writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Index std::vector out of bounds (%i)",v_index);
										RETURN_ERROR;
									}

									ldrVar = &script_var_vectors->variable[v_index];;
									ok = true;
								}else{
									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Expected std::vector-index as integer or std::string");
									RETURN_ERROR;
								}
							}
						}

						if(!ok){
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Variable \"%s\" is not type std::vector",
								SFI_GET_SYMBOL_NAME(calling_script_function,&calling_script_function->instruction[instruction->value_op2])
							);
							RETURN_ERROR;
						}

					}else{ // load variable ...

						properties=instruction->properties;
						scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(properties);

						switch(scope_type){
						default: // global...
							ldrVar = &stack[instruction->value_op2];
							break;
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS:
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS:

							if(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){
								POP_ONE; // get var op1 and symbol op2

								Instruction *previous_ins= (instruction-1);

								if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)!= MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)
								{

									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(calling_script_function,previous_ins));
									RETURN_ERROR;
								}


								ScriptVar  * base_var = (ScriptVar  *)stk_result_op1->var_ref;
								if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {
									StackElement *stk_ins=((StackElement *)stk_result_op1->var_ref);

									if(stk_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
										base_var=((ScriptVar *)stk_ins->var_ref);
									}
								}

								if(base_var == NULL)
								{
									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(calling_script_function,previous_ins));

									RETURN_ERROR;
								}

								if((variable_stack_element = base_var->getVariableSymbol(SFI_GET_SYMBOL_NAME(calling_script_function,instruction)))==NULL){

									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction)
											,"Variable \"%s\" as type \"%s\" has not symbol \"%s\""
											,SFI_GET_SYMBOL_NAME(calling_script_function,previous_ins)
											,base_var->getClassName().c_str()
											, SFI_GET_SYMBOL_NAME(calling_script_function,instruction));
									RETURN_ERROR;
								}
							}
							else{ // this scope ...
								if((variable_stack_element = this_object->getVariableSymbolByIndex(instruction->value_op2))==NULL){
									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"cannot find symbol \"this.%s\"",SFI_GET_SYMBOL_NAME(calling_script_function,instruction));
									RETURN_ERROR;
								}
							}
							ldrVar=variable_stack_element;
							break;
						case MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL:
							ldrVar = &ptrLocalVar[instruction->value_op2];
							break;
						}

						if(instruction->properties&MSK_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION){
							if((ldrVar->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION) != MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
								writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"\"%s\" expected function variable but is \"%s\""
										, SFI_GET_SYMBOL_NAME(calling_script_function,instruction)
										, indexInstructionVarTypeToStr(ldrVar));
								RETURN_ERROR;
							}
						}
					}

					pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(properties);

					/* all preoperators makes load var as constant ... */
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_INC:
							PERFORM_PRE_POST_OPERATOR(ldrVar,++);
							(*stk_current_data++)=*ldrVar;
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_DEC:
							PERFORM_PRE_POST_OPERATOR(ldrVar,--);
							(*stk_current_data++)=*ldrVar;
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_DEC:
							(*stk_current_data++)=*ldrVar;
							PERFORM_PRE_POST_OPERATOR(ldrVar,--);
							continue;
					case MSK_INSTRUCTION_PROPERTY_POST_INC:
							(*stk_current_data++)=*ldrVar;
							PERFORM_PRE_POST_OPERATOR(ldrVar,++);
							continue;
					case MSK_INSTRUCTION_PROPERTY_PRE_NOT:
							switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(ldrVar->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*stk_current_data++={(void *)(-(*((intptr_t *)ldrVar->var_ref))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C};
								}else{
									*stk_current_data++={(void *)(-(((intptr_t)ldrVar->stk_value))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT};
								}
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									*stk_current_data++={(void *)(!(*((bool *)ldrVar->var_ref))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C};
								}else{
									*stk_current_data++={(void *)(!(((bool)ldrVar->stk_value))),ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT};
								}
								break;

							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									COPY_FLOAT(&aux_float,ldrVar->var_ref);
								}else{
									COPY_FLOAT(&aux_float,&ldrVar->stk_value);
								}
								aux_float=-aux_float;
								COPY_FLOAT(&stk_current_data->stk_value,&aux_float);
								*stk_current_data={stk_current_data->stk_value,ldrVar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT};

								if(ldrVar->properties& MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
									stk_current_data->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;
								}

								stk_current_data++;
								break;
							default:
								writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"internal error:cannot perform pre operator - because is not number");
								RETURN_ERROR;
							}
							continue;
					default:

						// update var if needed ...
						PUSH_STACK_VAR(ldrVar);
						break;
					}

					// ok in case is C we must udpate stk_value becaus it can be updated from C++. (only primitives)
					if(ldrVar->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(ldrVar->properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
							//(stk_current_data-1)->stk_value=(void *)((*((int *)ldrVar->var_ref)));
							memcpy(&((stk_current_data-1)->stk_value),ldrVar->var_ref,sizeof(int));
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
							COPY_FLOAT(&((stk_current_data-1)->stk_value),ldrVar->var_ref);
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
							(stk_current_data-1)->stk_value=(void *)((*((bool *)ldrVar->var_ref)));
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
					(*stk_current_data)=*(((ConstantValue *)instruction->value_op2));

					pre_post_properties = GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(instruction->properties);

					// all preoperators makes load var as constant ...
					switch(pre_post_properties){
					case MSK_INSTRUCTION_PROPERTY_PRE_NOT:
							switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_current_data->properties)){
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
								stk_current_data->stk_value=(void *)(-((intptr_t)stk_current_data->stk_value));
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
								stk_current_data->stk_value=(void *)(!((bool)stk_current_data->stk_value));
								break;
							case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
								COPY_FLOAT(&aux_float,&stk_current_data->stk_value);
								aux_float=-aux_float;
								COPY_FLOAT(&stk_current_data->stk_value,&aux_float);
								break;
							default:
								writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"internal error:cannot perform pre operator - constant because is not numeric");
								RETURN_ERROR;
							}
							break;
					}
					stk_current_data++;
					continue;
				}else if(value_op1== LoadType::LOAD_TYPE_FUNCTION){

					unsigned short extra_flags=(instruction->properties&MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL)?MSK_STACK_ELEMENT_PROPERTY_CONSTRUCTOR_FUNCTION:0;
					extra_flags|=(instruction->properties&MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL) ?MSK_STACK_ELEMENT_PROPERTY_UNRESOLVED_FUNCTION:0;
					//void *function_obj=NULL;
					//std::vector<ScriptFunction *> *vec_functions;
					ScriptVar * class_obj=NULL;
					//intptr_t function_obj =  instruction->value_op2;
					intptr_t value_op2 = instruction->value_op2;
					properties=instruction->properties;
					scope_type=GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(properties);

					/*if(scope_type==MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){ // local gets functions from calling_script_function ...
						vec_functions=&calling_script_function->local_function;
					}else*/
					if(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){
						StackElement *var=NULL;
						if(properties & MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL){
							var=(stk_current_data-1);
						}else{
							POP_ONE;
							var = stk_result_op1;
						}

						StackElement *stk_ins=NULL;
						if(var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {
							stk_ins=((StackElement *)var->var_ref);
						}
						else{
							stk_ins=var;
						}

						if(stk_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							class_obj=(ScriptVar *)(stk_ins->var_ref);
							//ScriptClass *sc =GET_SCRIPT_CLASS(((ScriptVar *)class_obj)->idx_class);
							//vec_functions=&sc->local_function;
						}
						else{
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Cannot access symbol \"%s\" (type of %s is %s)",SFI_GET_SYMBOL_NAME(calling_script_function,instruction),SFI_GET_SYMBOL_NAME(calling_script_function,instruction-1),indexInstructionVarTypeToStr(stk_ins));
							RETURN_ERROR;
						}


						/*if(value_op2 == 0){//ZS_UNDEFINED_IDX || value_op2 == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in BYTE_CODE_CALL instruction ...
							function_obj= 0;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else{
							function_obj= value_op2;
						}*/
						/*else if((value_op2<(int)vec_functions->size())) // get the function ...
						{
							function_obj = (*vec_functions)[value_op2];

						}
						else{
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"cannot find symbol global \"%s\""
									,SFI_GET_SYMBOL_NAME(calling_script_function,instruction)
									);
							RETURN_ERROR;
						}*/
					}/*else if(scope_type ==MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){
						if((si = this_object->getFunctionSymbolByIndex(value_op2))==NULL){
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"cannot find function \"this.%s\"",SFI_GET_SYMBOL_NAME(calling_script_function,instruction));
							RETURN_ERROR;
						}

						function_obj =(ScriptFunction *)si->object.stk_value;

					}else if(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER){ // super scope ?
						if((si = this_object->getFunctionSymbolByIndex(value_op2))==NULL){
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"cannot find function \"super.%s\"",SFI_GET_SYMBOL_NAME(calling_script_function,instruction));
							RETURN_ERROR;
						}
						function_obj =(ScriptFunction *)si->object.stk_value;
					}else{ // global
						vec_functions = &(main_function_object->local_function);
					}


					if(!(scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS || scope_type == MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER)){

						if(value_op2 == ZS_UNDEFINED_IDX || value_op2 == ZS_FUNCTION_NOT_FOUND_IDX){ // is will be processed after in BYTE_CODE_CALL instruction ...
							function_obj= NULL;//(void *)instruction; // saves current instruction in order to resolve its idx later (in call instruction)
						}else if((value_op2<(int)vec_functions->size())) // get the function ...
						{
							function_obj = (*vec_functions)[value_op2];

						}
						else{
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"cannot find symbol global \"%s\""
									,SFI_GET_SYMBOL_NAME(calling_script_function,instruction)
									);
							RETURN_ERROR;
						}
					}*/

					if(value_op2 == 0){
						*stk_current_data++={instruction,class_obj,(unsigned short)(MSK_STACK_ELEMENT_PROPERTY_IS_VAR_INSTRUCTION|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION|extra_flags)};
					}else{
						PUSH_FUNCTION(value_op2,class_obj,extra_flags);
					}
					continue;

				}else if(value_op1== LoadType::LOAD_TYPE_ARGUMENT){
					*stk_current_data++=ptrArg[instruction->value_op2]; // copy arg directly ...
					continue;
				}
				else{

					writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"runtime error. Try to restart");
					RETURN_ERROR;

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
						if((stk_current_data-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							vec_obj = (ScriptVar *)(stk_current_data-1)->var_ref;
							if(vec_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){ // push value ...
								// op1 is now the src value ...
								src_ins=stk_result_op1;
								dst_ins=((ScriptVarVector *)vec_obj)->push();
								ok=true;
							}
						}

						if(!ok){
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Expected std::vector object");
							RETURN_ERROR;
						}

						push_value=false;

					}else if(operator_type==BYTE_CODE_PUSH_ATTR){


						POP_TWO; // first must be the value name and the other the variable name ...
						ScriptVar *struct_obj = NULL;
						if((stk_current_data-1)->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							struct_obj = (ScriptVar *)(stk_current_data-1)->var_ref;
							if(struct_obj->idx_class == IDX_BUILTIN_TYPE_CLASS_DICTIONARY){ // push value ...
								// op1 is now the src value ...
								if(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
									StackElement *se=NULL;
									const char *str = (const char *)stk_result_op2->stk_value;
									src_ins=stk_result_op1;
									if(src_ins->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
										FunctionSymbol *si =((ScriptVarDictionary *)struct_obj)->addFunctionSymbol(str,NULL);
										if(si!=NULL){
											se=&si->object;;
										}
									}else{
										se =((ScriptVarDictionary *)struct_obj)->addVariableSymbol(str);
									}

									if(se == NULL){
										RETURN_ERROR;
									}

									dst_ins=se;
									ok=true;
								}
								else{
									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"internal error (operator2 is not std::string)");
									RETURN_ERROR;
								}
							}else{
								writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Expected struct object");
								RETURN_ERROR;
							}
						}else{
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Expected scriptvar");
							RETURN_ERROR;
						}
						push_value=false;
					}
					else{ // pop two parameters nothing ...
						POP_TWO;

						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {// == ScriptVar::VAR_TYPE::OBJECT){
							dst_ins=(StackElement *)stk_result_op1->var_ref; // stk_value is expect to contents a stack variable
						}else{
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Expected l-value on assignment ('=')");
							RETURN_ERROR;
						}

						if(current_foreach!=NULL){
							if(dst_ins == current_foreach->key){
								writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"for-in key value is read only");
								RETURN_ERROR;
							}
						}

						src_ins=stk_result_op2; // store ptr instruction2 op as src_var_value

						// we need primitive stackelement in order to assign...
						if(src_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {// == ScriptVar::VAR_TYPE::OBJECT){
							src_ins=(StackElement *)src_ins->var_ref; // stk_value is expect to contents a stack variable

						}

						// ok load object pointer ...
						if(dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

							if(((ScriptVar *)dst_ins->var_ref)->itHasSetMetamethod()){
								if(!applyMetamethod(
										calling_object
										,calling_script_function
										,instruction
										,"="
										,BYTE_CODE_METAMETHOD_SET
										,stk_result_op1
										,stk_result_op2
								)){
									RETURN_ERROR;
								}
								assign_metamethod=true;
							}
						}
					}

					if(! assign_metamethod){

						if(dst_ins->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS){
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"\"this\" is not assignable");
							RETURN_ERROR;
						}

						StackElement old_dst_ins = *dst_ins; // save dst_var to check after assignment...

						assignStackVar(dst_ins,src_ins);

						// check old var structure ...
						switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(old_dst_ins.properties)){
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION: // we aren't take care about nothing! :)
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING: // type std::string is really a std::string or variable ?!?!
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR: // we are getting script vars ...
							if(!(((old_dst_ins.properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C))==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)))){
								if(old_dst_ins.var_ref!=NULL){ // it had a pointer (no constant)...
									if(old_dst_ins.var_ref != dst_ins->var_ref){ // unref pointer because new pointer has been attached...
										unrefSharedScriptVar(((ScriptVar  *)old_dst_ins.var_ref)->ptr_shared_pointer_node);
									}
								}
							}
							break;
						}
					}
					if(push_value){ // to be able to do multiple assigns like a=b=c=1 (1 will be pushed in each store instruction)
						*stk_current_data++=*src_ins;
					}
				}
				continue;
			case BYTE_CODE_EQU:  // ==
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}
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
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE;
				}else{
					POP_TWO;
				}
				PROCESS_LOGIC_OPERATION(&&);
				continue;
			case BYTE_CODE_LOGIC_OR:  // ||
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
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
							,calling_script_function
							,instruction
							,"!"
							,BYTE_CODE_METAMETHOD_NOT
							,stk_result_op1
							,stk_result_op2
							)){
						RETURN_ERROR;
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
							,calling_script_function
							,instruction
							,"-"
							,BYTE_CODE_METAMETHOD_NEG
							,stk_result_op1
							,stk_result_op2
					)){
						RETURN_ERROR;
					}

				}
				continue;
			case BYTE_CODE_ADD: // +
				{
					if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
						READ_TWO_POP_ONE;
					}else{
						POP_TWO;
					}

					unsigned short mask_properties =GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties&stk_result_op2->properties);
					unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);
					if(mask_properties==MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){
						PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) + STK_VALUE_TO_INT(stk_result_op2));
					}else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){
						if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){
							COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);
							PUSH_FLOAT(STK_VALUE_TO_INT(stk_result_op1) + f_aux_value2);
						}else{
							COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
							PUSH_FLOAT(f_aux_value1 + STK_VALUE_TO_INT(stk_result_op2));
						}
					}
					else if(properties==(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)){
						if (STK_VALUE_IS_STRING(stk_result_op1) && STK_VALUE_IS_INT(stk_result_op2)){
							sprintf(str_aux,"%s%i",((const char *)stk_result_op1->stk_value),(int)((intptr_t)stk_result_op2->stk_value));
							//PUSH_STRING(str_aux);
							if(ptr_current_str==ptr_last_str){
									writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Error stk_string out-stack");RETURN_ERROR;}\
									*ptr_current_str++=str_aux;\
									*stk_current_data++={(void *)((ptr_current_str-1)->c_str()),NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};\

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
					}
					else{ // try metamethod ...

						if(!applyMetamethod(
								 calling_object
								,calling_script_function
								,instruction
								,"+"
								,BYTE_CODE_METAMETHOD_ADD
								,stk_result_op1
								,stk_result_op2
								)){
							RETURN_ERROR;
						}
					}
				}
				continue;
			case BYTE_CODE_MUL: // *
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
				continue;
			case BYTE_CODE_DIV: // /
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_ARITHMETIC_OPERATION(/, BYTE_CODE_METAMETHOD_DIV);
				continue;

			 case BYTE_CODE_MOD: // /
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_MOD_OPERATION;
				continue;
			 case BYTE_CODE_AND: // &
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(&, BYTE_CODE_METAMETHOD_AND);
				continue;
			 case BYTE_CODE_OR: // *
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(|, BYTE_CODE_METAMETHOD_OR);
				continue;
			 case BYTE_CODE_XOR: // ^
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(^, BYTE_CODE_METAMETHOD_XOR);
				continue;

			 case BYTE_CODE_SHR: // >>
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}
				PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
				continue;

			 case BYTE_CODE_SHL: // <<
				if(instruction->properties&MSK_STACK_ELEMENT_PROPERTY_POP_ONE){
					READ_TWO_POP_ONE
				}else{
					POP_TWO;
				}

				PROCESS_BINARY_OPERATION(<<, BYTE_CODE_METAMETHOD_SHL);
				continue;
			 case BYTE_CODE_JMP:
				instruction_it=&start_it[instruction->value_op2];
				continue;
			 case BYTE_CODE_INSTANCEOF: // check instance of ...
				 POP_TWO;

				if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) {// == ScriptVar::VAR_TYPE::OBJECT){
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
					case IDX_BUILTIN_TYPE_CLASS_FUNCTOR:
						PUSH_BOOLEAN((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)!=0);
						break;
					default:
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							bool b = this->script_class_factory->class_C_BaseOf(((ScriptVar *)(stk_result_op1->var_ref))->idx_class, (intptr_t)stk_result_op2->stk_value);
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
					instruction_it=&start_it[instruction->value_op2];
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value != 0){
					instruction_it=&start_it[instruction->value_op2];
				}
				continue;
			 case  BYTE_CODE_CALL: // calling function after all of args are processed...
				// check whether signatures matches or not ...
				// 1. get function object ...
				 {
					aux_function_info = NULL;
					unsigned char n_args=0;

					bool is_c = false;
					StackElement *start_arg=stk_current_data;
					StackElement *callAle=NULL;

					while(n_args <= MAX_N_ARGS && (((start_arg-1)->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)==0)){
						start_arg--;
						n_args++;
					}

					callAle = ((start_arg-1));
					calling_object = this_object;
					if(callAle->var_ref!=NULL){
						calling_object=(ScriptVar *)callAle->var_ref;
					}


					aux_function_info = NULL;//(ScriptFunction *)callAle->stk_value;


					bool is_constructor = (callAle->properties & MSK_STACK_ELEMENT_PROPERTY_CONSTRUCTOR_FUNCTION)!=0;

					// try to find the function ...
					if(((callAle)->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_INSTRUCTION)){// || deduce_function){
						Instruction *callAleInstruction = (Instruction *)(callAle)->stk_value;
						//PASTNode ast_node_call_ale = vec_ast_node[iao->idxAstNode];

						symbol_to_find = SFI_GET_SYMBOL_NAME(calling_script_function,callAleInstruction);
						unsigned short scope_type = GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(callAleInstruction->properties);


						// local vars as functions ...

						// registered symbols in case is MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS...
						std::vector<FunctionSymbol> *function_symbol=NULL;
						if(scope_type==MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){
							calling_object = (ScriptVar *)callAle->var_ref;

							// we have to no to call default constructor...is implicit
							if(is_constructor) {

								is_c=calling_object->Is_C_Object();

								if(n_args == 0 && is_c){
									aux_function_info = NULL;
									callAleInstruction->properties |= MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL;
								}
							}

							function_symbol=calling_object->getVectorFunctionSymbol();
						}

						//bool all_check=true;
						if((callAleInstruction->properties & MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL)==0)
						{
							std::vector<ScriptFunction *> *global_functions=&(main_function_object->local_function);
							if((aux_function_info=findFunction(
									 calling_object
									,calling_script_function
									,instruction
									,callAleInstruction

									,function_symbol
									,global_functions
									,is_constructor
									,symbol_to_find
									,stk_result_op1
									,stk_result_op2
									,start_arg
									,n_args
									,NULL))==NULL){

								if((callAleInstruction->properties & MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL)==0){
									RETURN_ERROR;
								}
							}


							// saves function pointer found ...
							callAle->stk_value=aux_function_info;

						}

					}
					else{
						if(((callAle)->properties & MSK_STACK_ELEMENT_PROPERTY_UNRESOLVED_FUNCTION)==0){
							aux_function_info=(ScriptFunction *) (callAle->stk_value);
						}
					}

					if(aux_function_info !=NULL)
					{
						if(n_args > MAX_N_ARGS){
							writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Max arguments reached function at line XXX");
							RETURN_ERROR;
						}

						if((aux_function_info->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == 0){ // is function script ...
							unsigned aux_function_info_m_arg_size=aux_function_info->arg_info.size();
							if( n_args < aux_function_info_m_arg_size){ // we must push undefined parameters ...
								for(unsigned i = n_args; i < aux_function_info_m_arg_size; i++){
									*stk_current_data++={

										0,							 // no value assigned.
										NULL,			     // no varref related.
										MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED // starts undefined.
									};
									n_args++;
								}
							}
						}

						// by default virtual machine gets main object class in order to run functions ...
						ret_obj=callFunctionInternal(aux_function_info,calling_object,error,stk_current_data,ptr_current_str,n_args,instruction);

						if(error)
						{
							RETURN_ERROR;
						}

						if(ret_obj.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

							// if c pointer is not from application share ...
							if(!((ScriptVar *)(ret_obj.var_ref))->initSharedPtr()){
								RETURN_ERROR;
							}
						}

						if(cancel_execution) {
							if(custom_error!=NULL){
								writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),custom_error);
							}
							RETURN_ERROR;
						}
					}

					// reset stack (function+instruction (-1 op less))...
					stk_current_data=start_arg-1;

					// ... and push result if not function constructor...
					if(!is_constructor){
						*stk_current_data++ = ret_obj;
					}
				 }
				continue;
			 case  BYTE_CODE_NEW:
					svar=NEW_CLASS_VAR_BY_IDX(this,value_op1);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}
					svar->info_function_new=calling_script_function;
					svar->instruction_new=instruction;
					(*stk_current_data++)={NULL,svar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
					continue;
			 case  BYTE_CODE_DELETE:
					POP_ONE;
					//svar
					if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						StackElement *se=stk_result_op1;
						if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
							se=(StackElement *)(stk_result_op1->var_ref);
						}

						svar = (ScriptVar *)(se)->var_ref;
						if(svar->idx_class >= IDX_BUILTIN_TYPE_MAX
						 ||svar->idx_class==IDX_BUILTIN_TYPE_CLASS_VECTOR
						 ||svar->idx_class==IDX_BUILTIN_TYPE_CLASS_DICTIONARY
						)
						{ // max ...
							svar->unrefSharedPtr();

							if(svar->isCreatedByContructor()){
								svar->setDelete_C_ObjectOnDestroy(true);
							}

							se->stk_value=NULL;
							se->var_ref=NULL;
							se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;

						}
					}
					else{
						writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"delete op: expected scriptvar var but it was \"%s\"",indexInstructionVarTypeToStr(stk_result_op1));
						RETURN_ERROR;
					}
					continue;
			 case BYTE_CODE_DECL_VEC: // Create new std::vector object...
					svar=NEW_VECTOR_VAR;
					//PUSH_VAR(svar,NULL,0,false);

					if(!svar->initSharedPtr()){
						RETURN_ERROR;
					}

					(*stk_current_data++)={NULL,svar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};

					continue;

			 case  BYTE_CODE_DECL_DICTIONARY: // Create new std::vector object...
				svar=NEW_STRUCT_VAR;
				//PUSH_VAR(svar,NULL,0,false);

				if(!svar->initSharedPtr()){
					RETURN_ERROR;
				}

				(*stk_current_data++)={NULL,svar,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};

				continue;


			 case  BYTE_CODE_RET:

				if(stk_current_data>ptrStartOp){ // can return something
					callc_result=*(stk_current_data-1);

					// remove shared pointer if scriptvar ...
					if(callc_result.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						if(callc_result.properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
							callc_result=*((StackElement *)((StackElement *)callc_result.var_ref));

						}

						// unref pointer to be deallocated from gc...
						//((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node->data.shared_ptr=NULL;
						if(((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node!=NULL){
							free(((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node);
							((ScriptVar *)callc_result.var_ref)->ptr_shared_pointer_node=NULL;
						}
						// share pointer  + 1
					}
					else{ // remove all involved inforamtion
						callc_result.properties&=~MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT; // remove stack var if any...
						callc_result.var_ref =NULL;
					}
				}
				// }
				goto lbl_exit_function;
			 case BYTE_CODE_PUSH_SCOPE:

				BYTE_CODE_PUSH_SCOPE(instruction->value_op2,calling_script_function,ptrLocalVar,value_op1);

				if(value_op1 & ScopeProperty::SCOPE_PROPERTY_FOR_IN){
					if(current_foreach == &stk_foreach[VM_MAX_FOREACH-1]){
						writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Max foreach reached");
						RETURN_ERROR;
					}

					current_foreach++;
				}
				continue;

			 case BYTE_CODE_POP_SCOPE:
				ptr_current_str=ptrStartStr; // reset op ptr
				stk_current_data=ptrStartOp;
				if(!popScopeCall(idx_current_stack,NULL,value_op1)){
					writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Error pop scope");
					RETURN_ERROR;
				}

				if(value_op1 & ScopeProperty::SCOPE_PROPERTY_FOR_IN){
					if(current_foreach == &stk_foreach[0]){
						writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Min foreach reached");
						RETURN_ERROR;
					}

					memset(current_foreach,0,sizeof(ForeachInfo));
					current_foreach--;
				}
				continue;

			 case BYTE_CODE_IT_INI:
				 POP_TWO; // op2:std::vector or struct op1:key iterator


				 if((stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT) == 0){
						writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Internal error: Expected stackvar");
						RETURN_ERROR;

				 }
				 current_foreach->key=(StackElement *)stk_result_op1->var_ref;
				 current_foreach->ptr=NULL;
				 current_foreach->idx_current=0;

				 var_object = (ScriptVar *)stk_result_op2->var_ref;
				 if( (stk_result_op2->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)) == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR | MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)){
						var_object = (ScriptVar *)(((StackElement *)stk_result_op2->var_ref)->var_ref);
				 }

				if(var_object != NULL && (var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR || var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_DICTIONARY)){

					if(var_object->idx_class == IDX_BUILTIN_TYPE_CLASS_VECTOR){ // integer as iterator...
						*current_foreach->key={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};
					}
					else{ // struct -> std::string as iterator...
						*current_foreach->key={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};
					}

					current_foreach->ptr = var_object;
				}else{
					writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"Variable \"%s\" is not type std::vector or struct",
						SFI_GET_SYMBOL_NAME(calling_script_function,instruction-1)
					);
					RETURN_ERROR;
				}
				 continue;
			 case BYTE_CODE_SET_AND_NEXT:
				 writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"TODOOOOO!",
									 SFI_GET_SYMBOL_NAME(calling_script_function,instruction)
									);
									RETURN_ERROR;
				 //*((StackElement *)current_foreach->key)=((ScriptVarVector *)current_foreach->ptr)->m_objVector[current_foreach->idx_current++];
				 continue;
			 case BYTE_CODE_IT_CHK_END:
				 writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"TODOOOOO!",
									 SFI_GET_SYMBOL_NAME(calling_script_function,instruction)
									);
									RETURN_ERROR;
				 continue;
			//
			// END OPERATOR MANAGEMENT
			//
			//-----------------------------------------------------------------------------------------------------------------------
			}
			writeError(SFI_GET_FILE_LINE(calling_script_function,instruction),"operator type(%s) not implemented",ByteCodeToStr(instruction->byte_code));
			RETURN_ERROR;
		 }

	lbl_exit_function:

		//=========================
		// POP STACK
		if(calling_script_function->idx_script_function == 0){ // if main function only remove 0s and preserve variables!)
			removeEmptySharedPointers(idx_current_stack,NULL);
		}
		else{
			while(ptrStartScopeInfo<=(current_scope_info_ptr)){
				popScopeCall(idx_current_stack,callc_result.var_ref,0);
			}
		}

		idx_current_stack--;
		// POP STACK
		//=========================

		//popStack();
		return callc_result;
	}

}
