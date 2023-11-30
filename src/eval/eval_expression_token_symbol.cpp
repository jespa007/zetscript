/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	bool eval_is_access_punctuator(char *s){
		return *s=='.' || *s=='[' || *s=='(';
	}

	bool eval_is_access_punctuator_or_static_reference(char *s){
		return  eval_is_access_punctuator(s) || (*s==':' && *(s+1)==':');
	}

	bool eval_get_accessor_name(EvalData *eval_data, char **aux_p, int line, String & accessor_name){
		accessor_name="";
		while(!is_end_symbol_token(*aux_p)){ // get name...
			accessor_name += *(*aux_p)++;
		}
		return check_identifier_name_expression_ok(eval_data,accessor_name,line) == TRUE;
	}

	bool eval_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,String & static_error){
		bool ok=true;
		if(static_symbol->properties & SYMBOL_PROPERTY_STATIC){ // it should be constant type ...

			if((static_symbol->properties & SYMBOL_PROPERTY_FUNCTION)){
				instruction->byte_code=BYTE_CODE_LOAD_FUNCTION;
				instruction->value_op2=(zs_int)static_symbol; // it's pointer (script function) or stack element id (const)
			}else if(static_symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
				MemberProperty *metamethod_member=(MemberProperty *)(static_symbol->ref_ptr);
				if(metamethod_member->metamethod_members.getter!=NULL){
					instruction->byte_code=BYTE_CODE_CALL;
					instruction->value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,0);
					instruction->value_op2=(zs_int)metamethod_member->metamethod_members.getter;
				}
				else{
					static_error="internal error: symbol property member null";
					ok=false;
				}
			}else if((static_symbol->properties & SYMBOL_PROPERTY_CONST)){
				instruction->byte_code=BYTE_CODE_LOAD_GLOBAL;
				instruction->value_op2=static_symbol->ref_ptr; // it's pointer (script function) or stack element id (const)
			}else{
				static_error="symbol expected to be function or const";
				ok=false;
			}
		}
		else{
			static_error="is not static";
			ok=false;
		}
		return ok;
	}

	char *eval_expression_token_symbol(
			EvalData *eval_data
			, char *s
			, int & line
			, ScriptScope *scope_info
			, Vector<TokenNode *> *token_nodes
			,TokenNode *last_operator_token_node
			, uint16_t properties
		){
		char *aux_p = s,*test_aux_p;//, *test_s=NULL;
		int test_line=line;
		TokenNode *token_node_symbol=new TokenNode();
		Vector<EvalInstruction *> ei_arg_instruction;
		EvalInstruction *instruction_token=NULL;
		EvalInstruction *last_instruction_token=NULL;
		PreOperation pre_operation = PreOperation::PRE_OPERATION_UNKNOWN;
		PostOperation post_operation=PostOperation::POST_OPERATION_UNKNOWN;
		int last_accessor_line=line;
		int last_line_ok=line;
		String static_error;
		ScriptType *token_node_symbol_type=NULL;

		// check pre operator (-,+,!,-- or ++)
		switch(pre_operation=is_pre_operation(aux_p)){
			case PreOperation::PRE_OPERATION_UNKNOWN:
				break;
			default:
				aux_p+=strlen(eval_data_pre_operations[pre_operation].str);
				break;
			case PreOperation::PRE_OPERATION_DEC_INC_INVALID:
				EVAL_ERROR_FILE_LINE_GOTO(eval_data->current_parsing_file,line ,error_expression_token_symbol,"Unknow pre-operation '%.2s'",aux_p);
				break;
		}


		IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p,line);

		// parenthesis (evals another expression)
		if(*aux_p=='('){ // inner expression (priority)

			if(pre_operation == PreOperation::PRE_OPERATION_INC || pre_operation == PreOperation::PRE_OPERATION_DEC){
				EVAL_ERROR_FILE_LINE_GOTO(eval_data->current_parsing_file,line ,error_expression_token_symbol,"operation '%s' is only allowed on identifiers",eval_data_pre_operations[pre_operation].str);
			}

			if((aux_p=eval_sub_expression(
					eval_data
					, aux_p+1
					, line
					, scope_info
					, &token_node_symbol->eval_instructions
					, NULL
					, properties
				)
			)== NULL){
				goto error_expression_token_symbol;
			}

			if(*aux_p != ')'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					eval_data->current_parsing_file
					,line 
					,error_expression_token_symbol,"Expected ')'"
				);
			}

			aux_p=aux_p+1;

			if(pre_operation==PreOperation::PRE_OPERATION_NEG){
				token_node_symbol->eval_instructions.push(new EvalInstruction(BYTE_CODE_NEG));
				pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;
			}else if(pre_operation==PreOperation::PRE_OPERATION_BWC){
				token_node_symbol->eval_instructions.push(new EvalInstruction(BYTE_CODE_BWC));
				pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;
			}else if(pre_operation==PreOperation::PRE_OPERATION_NOT){
				token_node_symbol->eval_instructions.push(new EvalInstruction(BYTE_CODE_NOT));
				pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;
			}

			token_node_symbol->token_type=TokenType::TOKEN_TYPE_SUBEXPRESSION;
		}
		else{ // symbol token

			Keyword keyword_type=eval_is_keyword(aux_p);

			if(*aux_p=='['){ // vector ...
				if((aux_p=eval_object_vector(
					eval_data
					,aux_p
					,line
					,scope_info
					,&token_node_symbol->eval_instructions
				))==NULL){
					goto error_expression_token_symbol;
				}
				token_node_symbol->token_type = TokenType::TOKEN_TYPE_OBJECT_ARRAY;
			}else if(*aux_p=='{'){ // object ...

				if((aux_p=eval_object_object(
					eval_data
					,aux_p
					,line
					,scope_info
					,&token_node_symbol->eval_instructions
				))==NULL){
					goto error_expression_token_symbol;
				}

				token_node_symbol->token_type = TokenType::TOKEN_TYPE_DICTIONARY_SCRIPT_OBJECT;

			}else if(keyword_type == Keyword::KEYWORD_NEW){

				if((aux_p=eval_object_new(
					eval_data
					,aux_p
					,line
					,scope_info
					,&token_node_symbol->eval_instructions
				))==NULL){
					goto error_expression_token_symbol;
				}

				token_node_symbol->token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

			}else if(keyword_type == Keyword::KEYWORD_FUNCTION){ // can be after instanceof or a function object..

				if((aux_p=eval_object_function(
					eval_data
					,aux_p
					,line
					,scope_info
					,token_node_symbol
				))==NULL){
					goto error_expression_token_symbol;
				}

				token_node_symbol->token_type = TokenType::TOKEN_TYPE_OBJECT_FUNCTION;
			}else{ // symbol
				token_node_symbol->pre_operation=pre_operation;

				last_accessor_line=line;
				if((aux_p = eval_symbol(eval_data
						,aux_p
						,line
						,scope_info
						,token_node_symbol
						,pre_operation
				)) == NULL){
					goto error_expression_token_symbol;
				}

				EvalInstruction *ei_first_token_node=(EvalInstruction *)token_node_symbol->eval_instructions.get(0);

				if(token_node_symbol->token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

					EvalInstruction *eval_instruction = ei_first_token_node;
					eval_instruction->symbol_scope=scope_info;


					if(*aux_p != 0 && *aux_p==':' && *(aux_p+1)==':'){ //  static access

						// mark symbol as static
						// the first item is the type
						//String static_access_value=token_node_symbol->value
						String class_element;
						Symbol *member_symbol=NULL;

						ScriptType *sc=eval_data->zs->getScriptTypesFactory()->getScriptType(token_node_symbol->value);

						//do{
						IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p+2,line);

						last_accessor_line=line;
						if(eval_get_accessor_name(eval_data, &aux_p, line,class_element) == false){
							EVAL_ERROR_FILE_LINE_GOTOF(
								eval_data->current_parsing_file,line
								,error_expression_token_symbol
								,"Expected identifier after '::'"
							);
						}

						token_node_symbol->value+="::"+class_element;



						//}while(*aux_p != 0 && *aux_p==':' && *(aux_p+1)==':' && n_static_access < max_static_access);

						// override
						//token_node_symbol->value=static_access_value;

						/*if(sc==NULL ){
							EVAL_ERROR_FILE_LINE_GOTO(
								eval_data->current_parsing_file
								,line
								,error_expression_token_symbol
								,"Cannot perform static access %s::%s'. '%s' is not a type"
								,static_access_value.toConstChar()
								,class_element.toConstChar()
								,static_access_value.toConstChar()
							);
						}*/


						if(sc != NULL){ // if type exist ...

							ei_first_token_node->vm_instruction.byte_code = BYTE_CODE_FIND_VARIABLE;

							member_symbol=sc->getSymbol(class_element.toConstChar()); // ... and member as well we can define the instruction here

							if(member_symbol != NULL){
								if(member_symbol->properties & SYMBOL_PROPERTY_STATIC){
									if(!eval_set_instruction_static_symbol(&eval_instruction->vm_instruction,member_symbol,static_error)){
										EVAL_ERROR_FILE_LINE_GOTO(
												eval_data->current_parsing_file
												,line
												,error_expression_token_symbol
												,"Symbol '%s' %s"
												,token_node_symbol->value.toConstChar()
												,static_error.toConstChar());
									}
								}else{
									EVAL_ERROR_FILE_LINE_GOTO(
											eval_data->current_parsing_file
											,line
											,error_expression_token_symbol
											,"Symbol '%s' is not static",token_node_symbol->value.toConstChar());
								}
							}
						} // --> in eval::pop_function will be find

					}else{ // check if only gets the type

						token_node_symbol_type=eval_data->script_types_factory->getScriptType(token_node_symbol->value);
						EvalInstruction *ei_instruction=(EvalInstruction *)token_node_symbol->eval_instructions.get(0);
						if(token_node_symbol_type != NULL){ // byte code it will be a type
							ei_instruction->vm_instruction.byte_code= BYTE_CODE_LOAD_TYPE;
							ei_instruction->vm_instruction.value_op2=token_node_symbol_type->id;
						}

						// set operator symbol
						if((last_operator_token_node != NULL && last_operator_token_node->operator_type == Operator::ZS_OPERATOR_INSTANCEOF)){
							last_operator_token_node->value=token_node_symbol->value;
						}
						/*else{ // sc is null
							if((last_operator_token_node != NULL && last_operator_token_node->operator_type == Operator::ZS_OPERATOR_INSTANCEOF)){
								//last_operator_token_node->value=token_node_symbol->value;
								//EvalInstruction *ei_last_operator=last_operator_token_node->eval_instructions.items[0];
								ei_last_operator->instruction_source_info= InstructionSourceInfo(
										eval_data->current_parsing_file
										,line
										,get_mapped_name(eval_data,token_node_symbol->value)
									);
								//int kk=0;
								//kk++;

								EVAL_ERROR_FILE_LINE_GOTOF(
									eval_data->current_parsing_file
									,line
									,error_expression_token_symbol
									,"expected a type after 'instanceof'"
								);
							}

						}*/

					}
				}

				ei_first_token_node->symbol_name=token_node_symbol->value;

				// add info to add as symbol value ...
				ei_first_token_node->instruction_source_info = eval_instruction_source_info(
					eval_data
					,eval_data->current_parsing_file
					,line
					,token_node_symbol->value
				);

			}
		}

		test_line=line;
		IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,test_aux_p,eval_data,aux_p,test_line);


		if(eval_is_access_punctuator(test_aux_p)){
			if((
				   token_node_symbol->token_type==TokenType::TOKEN_TYPE_IDENTIFIER
				|| token_node_symbol->token_type==TokenType::TOKEN_TYPE_OBJECT_FUNCTION
				|| token_node_symbol->token_type==TokenType::TOKEN_TYPE_DICTIONARY_SCRIPT_OBJECT
				|| token_node_symbol->token_type==TokenType::TOKEN_TYPE_OBJECT_ARRAY
				|| ((token_node_symbol->token_type==TokenType::TOKEN_TYPE_LITERAL) && (((EvalInstruction *)token_node_symbol->eval_instructions.get(0))->vm_instruction.byte_code==BYTE_CODE_LOAD_STRING) && *test_aux_p=='.')
				|| ((token_node_symbol->token_type==TokenType::TOKEN_TYPE_SUBEXPRESSION) && (((EvalInstruction *)token_node_symbol->eval_instructions.get(0))->vm_instruction.byte_code==BYTE_CODE_NEW_OBJECT_BY_TYPE) && *test_aux_p=='.')
			)==false){
				EVAL_ERROR_FILE_LINE_GOTO(
						eval_data->current_parsing_file
						,line
						,error_expression_token_symbol
						,"Unexpected '%c'"
						,*test_aux_p);
			}
		// eval accessor element (supose that was a preinsert a load instruction for identifier )...

			if(token_node_symbol_type!=NULL){

				EVAL_ERROR_FILE_LINE_GOTO(
						eval_data->current_parsing_file
						,line
						,error_expression_token_symbol
						,"Unexpected '%c' after type '%s' %s"
						,*test_aux_p
						,token_node_symbol->value.toConstChar()
						,*test_aux_p=='.' ? " on trying access a static member. Static members should be access through '::'":"."
				);
			}

			// set test
			aux_p=test_aux_p;
			line=test_line;

			String accessor_name="",
						last_accessor_value=token_node_symbol->value;

			int8_t n_params=0;
			int it_accessor_token=0;

			do{
				ByteCode byte_code=BYTE_CODE_INVALID;
				accessor_name="";
				zs_int instruction_value2=ZS_UNDEFINED_IDX;
				EvalInstruction *ei_first_token_node=(EvalInstruction *)token_node_symbol->eval_instructions.get(0);

				aux_p=test_aux_p;
				line=test_line;

				switch(*aux_p){
				case '(': // is a function call

					if(	it_accessor_token==0){
						if(
								token_node_symbol->value == SYMBOL_VALUE_SUPER
								&& scope_info->owner_type->id == SCRIPT_TYPE_ID_CLASS_MAIN){
							EVAL_ERROR_FILE_LINE_GOTOF(
									eval_data->current_parsing_file
									,line
									,error_expression_token_symbol
									,"'super' keyword only can be used within member function"
							);
						}

					}

					n_params=0;
					last_line_ok=line;
					IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p+1,line);

					// eval all calling arguments
					while(*aux_p != ')'){
						bool error_arg=false;
						ei_arg_instruction.clear();

						// check if ref identifier...
						if(n_params>0){
							if(*aux_p != ','){
								EVAL_ERROR_FILE_LINE_GOTOF(
										eval_data->current_parsing_file
										,last_line_ok
										,error_expression_token_symbol
										,"Expected ',' or ')'"
								);
							}

							IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p+1,line);

							if(*aux_p == ')'){ // unexpected ) but is ok anyway
								break;
							}


						}

						last_line_ok=line;

						if((aux_p = eval_sub_expression(
								eval_data
								,aux_p
								,line
								,scope_info
								,&ei_arg_instruction//token_node_symbol->instructions
								,NULL
								,properties // avoid load ref, to avoid pass 2 reference
						))==NULL){
							error_arg =true;
						}

						// only one argument and is variable
						if(ei_arg_instruction.length()==1){
							EvalInstruction *ei_arg=(EvalInstruction *)ei_arg_instruction.get(0);
							ByteCode byte_code_aux=ei_arg->vm_instruction.byte_code;

							// if not LOAD THIS set STK
							if(byte_code_aux !=BYTE_CODE_LOAD_THIS_VARIABLE){
								if(byte_code_aux ==BYTE_CODE_LOAD_REF){
									ei_arg->vm_instruction.byte_code=BYTE_CODE_LOAD_LOCAL;
								}
								else if(eval_is_byte_code_load_var_type(byte_code_aux)){
									ei_arg->vm_instruction.byte_code=eval_byte_code_load_var_type_to_push_stk(byte_code_aux);
									ei_arg->vm_instruction.properties |= INSTRUCTION_PROPERTY_USE_PUSH_STK;
								}
							}
						}

						token_node_symbol->eval_instructions.concat(ei_arg_instruction);

						if(error_arg){
							goto error_expression_token_symbol;
						}

						n_params++;
					}

					if(token_node_symbol->value == SYMBOL_VALUE_SUPER){
						byte_code=BYTE_CODE_SUPER_CALL;
					}
					else{
						byte_code=BYTE_CODE_CALL;

						if(it_accessor_token==0){ // direct or indirect call
							if( ei_first_token_node->vm_instruction.byte_code == BYTE_CODE_LOAD_THIS_FUNCTION ){
								byte_code=BYTE_CODE_THIS_CALL;
							}else if( ei_first_token_node->vm_instruction.byte_code == BYTE_CODE_LOAD_LOCAL ){
								byte_code= BYTE_CODE_INDIRECT_LOCAL_CALL;
							}
						}else if(last_accessor_value==""){ // last access value was empty so is a stack call (a call from returning function)
							byte_code=BYTE_CODE_STACK_CALL;
						}else{
							if( last_instruction_token->vm_instruction.byte_code == BYTE_CODE_LOAD_THIS_VARIABLE ){
								byte_code=BYTE_CODE_THIS_CALL;
							}else{
								byte_code=BYTE_CODE_MEMBER_CALL;
								last_instruction_token->vm_instruction.properties|=INSTRUCTION_PROPERTY_CALLING_FUNCTION;
							}
						}
					}

					aux_p++;
					break;
				case '[': // vector access
					if((aux_p = eval_sub_expression(
							eval_data
							,aux_p+1
							,line
							,scope_info
							,&token_node_symbol->eval_instructions
							,NULL
							,properties
					))==NULL){
						goto error_expression_token_symbol;
					}

					if(*aux_p != ']'){
						EVAL_ERROR_FILE_LINE_GOTOF(
							eval_data->current_parsing_file
							,line
							,error_expression_token_symbol
							,"Expected ']'"
						);
					}

					aux_p++;
					byte_code=BYTE_CODE_LOAD_ARRAY_ITEM;
					break;
				case '.': // member/static access

					IGNORE_BLANKS_AND_GOTO_ON_ERROR(
							error_expression_token_symbol
							,aux_p
							,eval_data
							,aux_p+1
							,line
					);

					last_accessor_line=line;
					if(eval_get_accessor_name(eval_data, &aux_p, line,accessor_name) == false){
						EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,line
						,error_expression_token_symbol
						,"Expected identifier after '.'"
						);
					}

					if(accessor_name == SYMBOL_VALUE_THIS){
						EVAL_ERROR_FILE_LINE_GOTOF(
							eval_data->current_parsing_file
							,line
							,error_expression_token_symbol
							,"'this' is not allowed as member name"
						);
					}

					byte_code=BYTE_CODE_LOAD_OBJECT_ITEM;

					if(it_accessor_token==0 && token_node_symbol->value == SYMBOL_VALUE_THIS){ // check first symbol at first...
						instruction_token=ei_first_token_node;

						if(eval_data->current_function->script_function->properties & SCRIPT_FUNCTION_PROPERTY_STATIC){
							EVAL_ERROR_FILE_LINE_GOTOF(
								eval_data->current_parsing_file
								,line
								,error_expression_token_symbol
								,"'this' cannot be used in static functions"
							);
						}

						if(scope_info->owner_type->id == SCRIPT_TYPE_ID_CLASS_MAIN){
							EVAL_ERROR_FILE_LINE_GOTOF(
								eval_data->current_parsing_file
								,line
								,error_expression_token_symbol
								,"'this' is only allowed within member functions"
							);
						}

						// set symbol name
						ei_first_token_node->symbol_name=accessor_name;
						Symbol *symbol_access_this=scope_info->owner_type->getSymbolVariableMember(accessor_name.toConstChar());
						if(symbol_access_this!=NULL){
							instruction_value2=symbol_access_this->idx_position;
						}
						byte_code=BYTE_CODE_LOAD_THIS_VARIABLE;

					}
					break;
				}

				// if byte_code is type BYTE_CODE_LOAD_THIS_XXX it means that is Instruction BYTE_CODE_LOAD_THIS was muted into BYTE_CODE_LOAD_THIS_XXX. So
				// we only create new instructions from bytes code != BYTE_CODE_LOAD_THIS_XXX
				if((
						   byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
						|| byte_code==BYTE_CODE_LOAD_THIS_FUNCTION
					)
					==false){

					// ... we create new instruction
					token_node_symbol->eval_instructions.push(instruction_token=new EvalInstruction(byte_code));
				}

				//EvalInstruction *ei_first_instruction_token=(EvalInstruction *)token_node_symbol->eval_instructions.items[0];

				switch(byte_code){
				case BYTE_CODE_THIS_CALL:
				case BYTE_CODE_SUPER_CALL:
				case BYTE_CODE_CALL:
				case BYTE_CODE_INDIRECT_LOCAL_CALL:
				case BYTE_CODE_INDIRECT_GLOBAL_CALL:
				case BYTE_CODE_UNRESOLVED_CALL:
					instruction_token->vm_instruction=ei_first_token_node->vm_instruction;
					instruction_token->vm_instruction.byte_code=byte_code;
					instruction_token->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,n_params); // by default always returns 1 value
					instruction_token->symbol_name=ei_first_token_node->symbol_name;
					instruction_token->symbol_scope=ei_first_token_node->symbol_scope;
					instruction_token->instruction_source_info= ei_first_token_node->instruction_source_info;

					// The last instruction was BYTE_CODE_LOAD_THIS_VARIABLE and the evaluation found its idx_position
					// Because the instruction to be replaced is a call_this and it could have inheritance, leave as undefined
					// to allow locate functions in the top most inherited type
					if(byte_code==BYTE_CODE_THIS_CALL || byte_code==BYTE_CODE_SUPER_CALL){
						instruction_token->vm_instruction.value_op2=ZS_UNDEFINED_IDX;
					}


					// erase first token node because last this.xxx is gona to be replaced to a single instruction
					// last_instruction_token will be erased!!!
					delete ei_first_token_node;
					token_node_symbol->eval_instructions.erase(0);
					last_instruction_token=NULL;

					break;
				case BYTE_CODE_STACK_CALL:
				case BYTE_CODE_MEMBER_CALL:
				case BYTE_CODE_CONSTRUCTOR_CALL:
					instruction_token->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,n_params);

					// also insert source file/line/symbol info to get info of this call...
					instruction_token->instruction_source_info= eval_instruction_source_info(
						eval_data
						,eval_data->current_parsing_file
						,last_accessor_line
						,last_accessor_value // only can get from last_accessor_value because accessor_name is empty on each iteration
					);
					break;
				default:
					instruction_token->vm_instruction.byte_code=byte_code;
					instruction_token->vm_instruction.value_op2=instruction_value2;
					instruction_token->instruction_source_info= eval_instruction_source_info(
						eval_data
						,eval_data->current_parsing_file
						,line
						,accessor_name
					);
					break;
				}



				// not first access anymore...
				last_accessor_value=accessor_name;
				last_instruction_token=instruction_token;

				it_accessor_token++;

				// set test
				test_aux_p=aux_p;
				test_line=line;

				IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,test_aux_p,eval_data,aux_p,test_line);

			}while(eval_is_access_punctuator(test_aux_p) );

		}else{

			EvalInstruction *ei_first_token_node=(EvalInstruction *)token_node_symbol->eval_instructions.get(0);

			if(token_node_symbol->value==SYMBOL_VALUE_THIS){ // only takes symbol this

				if(eval_data->current_function->script_function->owner_script_type_id == SCRIPT_TYPE_ID_CLASS_MAIN){
					EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,line
						,error_expression_token_symbol
						,"'this' only can be used within a type"
					);
				}

				ei_first_token_node->instruction_source_info= eval_instruction_source_info(
					eval_data
					,eval_data->current_parsing_file
					,last_accessor_line
					,SYMBOL_VALUE_THIS
				);

				ei_first_token_node->vm_instruction.value_op2=INSTRUCTION_VALUE_OP2_THIS;
				ei_first_token_node->vm_instruction.properties=0;
			}
		}


		post_operation=is_post_operation(aux_p);

		if(
				(post_operation == PostOperation::POST_OPERATION_INC)
			|| 	(post_operation == PostOperation::POST_OPERATION_DEC)
		){
			EvalInstruction *eval_instruction_post=NULL;
			//EvalInstruction *eval_instruction_pre_op=NULL;

			ByteCode byte_code_post_operation= BYTE_CODE_INVALID;
			//ByteCode byte_code_pre_operation= BYTE_CODE_INVALID;

			Instruction *last_load_instruction=&((EvalInstruction *)(token_node_symbol->eval_instructions.get(token_node_symbol->eval_instructions.length()-1)))->vm_instruction;

			if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
				EVAL_ERROR_FILE_LINE_GOTO(
						eval_data->current_parsing_file
						,line
						,error_expression_token_symbol
						,"expected identifier after post operation '%s'"
						,eval_data_post_operations[ post_operation].str
				);
			}

			// advance pointer...
			aux_p+=strlen(eval_data_post_operations[post_operation].str);

			if(token_node_symbol->pre_operation != PreOperation::PRE_OPERATION_UNKNOWN){
			   if(
					  (token_node_symbol->pre_operation != PreOperation::PRE_OPERATION_NEG)
					  &&
					  (token_node_symbol->pre_operation != PreOperation::PRE_OPERATION_BWC)
			   ){
				   EVAL_ERROR_FILE_LINE_GOTO(
					   eval_data->current_parsing_file
					   ,line
					   ,error_expression_token_symbol
					   ,"Cannot combine pre-operation '%s' with post-operation '%s' on '%s'"
						,eval_data_pre_operations[ pre_operation].str
						,eval_data_pre_operations[ post_operation].str
						,token_node_symbol->value.toConstChar()
					);
			   }
			}

		   if(post_operation == PostOperation::POST_OPERATION_INC){
			   byte_code_post_operation=BYTE_CODE_POST_INC;
		   }else {
			   byte_code_post_operation=BYTE_CODE_POST_DEC;
		   }

			token_node_symbol->eval_instructions.push(
				eval_instruction_post=new EvalInstruction(
					byte_code_post_operation
				)
			);

			// post op instruction
			EvalInstruction *eval_instruction_last_access=(EvalInstruction *)token_node_symbol->eval_instructions.get(token_node_symbol->eval_instructions.length()-1);

			eval_instruction_post->instruction_source_info=eval_instruction_source_info(
				eval_data
				,eval_data->current_parsing_file
				,line
				,eval_instruction_last_access->instruction_source_info.ptr_str_symbol_name
			);

			// pre op instruction
		   /*if(token_node_symbol->pre_operation==PreOperation::PRE_OPERATION_NEG){
			   byte_code_pre_operation=BYTE_CODE_NEG_POST_INC;
			   pre_operation=token_node_symbol->pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // the pre-operation neg was absorbed by -a++
		   }else if(token_node_symbol->pre_operation==PreOperation::PRE_OPERATION_BWC){
			   byte_code_pre_operation=BYTE_CODE_BWC_POST_INC;
			   pre_operation=token_node_symbol->pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // the pre-operation neg was absorbed by -a++
		   }

		   if(byte_code_pre_operation != BYTE_CODE_INVALID){
				token_node_symbol->eval_instructions.append(
					eval_instruction_pre_op=new EvalInstruction(
							byte_code_pre_operation
					)
				);
			   // insert instruction pre
				eval_instruction_pre_op->instruction_source_info=eval_instruction_source_info(
					eval_data
					,eval_data->current_parsing_file
					,line
					,eval_instruction_last_access->instruction_source_info.ptr_str_symbol_name
				);
		   }*/


			// if post inc/dec hange load by push because is mutable
			if(eval_is_byte_code_load_var_type(last_load_instruction->byte_code)){
				last_load_instruction->byte_code=eval_byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
			}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
				last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}
		}

		if(		(pre_operation == PreOperation::PRE_OPERATION_NEG)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_BWC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_NOT)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_INC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_DEC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_TYPEOF)
		){
			EvalInstruction *eval_instruction_pre=NULL;

			if(pre_operation != PreOperation::PRE_OPERATION_TYPEOF){
				if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					EVAL_ERROR_FILE_LINE_GOTO(
							eval_data->current_parsing_file
							,line
							,error_expression_token_symbol
							,"expected identifier before pre operation '%s'"
							,eval_data_pre_operations[ pre_operation].str
					);
				}
			}else{
				// check is not type
				if(eval_data->script_types_factory->getScriptType(token_node_symbol->value)!=NULL){
					EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,line
						,error_expression_token_symbol
						,"expected variable after 'typeof' but it was a type"
					);
				}
			}

			EvalInstruction *eval_instruction_last_access=(EvalInstruction *)token_node_symbol->eval_instructions.get(token_node_symbol->eval_instructions.length()-1);
			Instruction *last_load_instruction=&((EvalInstruction *)(token_node_symbol->eval_instructions.get(token_node_symbol->eval_instructions.length()-1)))->vm_instruction;

			token_node_symbol->eval_instructions.push(
				eval_instruction_pre=new EvalInstruction(
					pre_operation == PreOperation::PRE_OPERATION_NEG ? BYTE_CODE_NEG:
					pre_operation == PreOperation::PRE_OPERATION_BWC ? BYTE_CODE_BWC:
					pre_operation == PreOperation::PRE_OPERATION_NOT ? BYTE_CODE_NOT:
					pre_operation == PreOperation::PRE_OPERATION_DEC ? BYTE_CODE_PRE_DEC:
					pre_operation == PreOperation::PRE_OPERATION_TYPEOF ? BYTE_CODE_TYPEOF:
					BYTE_CODE_PRE_INC
				)
			);

			eval_instruction_pre->instruction_source_info=eval_instruction_source_info(
				eval_data
				,eval_data->current_parsing_file
				,line
				,eval_instruction_last_access->instruction_source_info.ptr_str_symbol_name
			);


			// if pre inc/dec change load by push because is mutable
			if(
				(pre_operation == PreOperation::PRE_OPERATION_INC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_DEC)){

				if(eval_is_byte_code_load_var_type(last_load_instruction->byte_code)){
					last_load_instruction->byte_code=eval_byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
				}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
					last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
				}
			}

		}

		// finally push token node symbol
		token_nodes->push(token_node_symbol);

		return aux_p;

error_expression_token_symbol:

		for(int kk=0;kk<token_node_symbol->eval_instructions.length();kk++){
			delete token_node_symbol->eval_instructions.get(kk);
			token_node_symbol->eval_instructions.set(kk,0);
		}

		delete token_node_symbol;

		return NULL;
	}
}

