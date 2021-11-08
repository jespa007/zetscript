/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	bool  is_access_punctuator(char *s){
		return *s=='.' || *s=='[' || *s=='(';
	}

	bool  is_access_punctuator_or_static_reference(char *s){
		return  is_access_punctuator(s) || (*s==':' && *(s+1)==':');
	}

	bool get_accessor_name(EvalData *eval_data, char **aux_p, int line, zs_string & accessor_name){
		//char *aux_p = s;
		accessor_name="";
		while(!is_end_symbol_token(*aux_p)){ // get name...
			accessor_name += *(*aux_p)++;
		}
		return check_identifier_name_expression_ok(eval_data,accessor_name,line) == TRUE;
	}

	bool eval_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,zs_string & static_error){
		bool ok=true;
		if(static_symbol->properties & SYMBOL_PROPERTY_STATIC){ // it should be constant type ...



			if(static_symbol->properties & SYMBOL_PROPERTY_FUNCTION){
				instruction->byte_code=BYTE_CODE_LOAD_FUNCTION;
				instruction->value_op2=(zs_int)static_symbol; // it's pointer (script function) or stack element id (const)
			}else if(static_symbol->properties & SYMBOL_PROPERTY_CONST){
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
			, Scope *scope_info
			, zs_vector *token_nodes
			,TokenNode *last_operator_token_node
			, uint16_t properties
			, int n_recursive_level
			, int n_return_values
		){
		char *aux_p = s,*test_aux_p;//, *test_s=NULL;
		int first_line=line,test_line=line;
		TokenNode *token_node_symbol=new TokenNode();
		zs_vector ei_arg_instruction;
		EvalInstruction *instruction_token=NULL;
		EvalInstruction *last_instruction_token=NULL;
		PreOperation pre_operation = PreOperation::PRE_OPERATION_UNKNOWN;
		PostOperation post_operation=PostOperation::POST_OPERATION_UNKNOWN;
		int last_accessor_line=line;
		int last_line_ok=line;
		zs_string static_error;
		ScriptClass *token_node_symbol_class=NULL;

		// check pre operator (-,+,!,-- or ++)
		switch(pre_operation=is_pre_operation(aux_p)){
			case PreOperation::PRE_OPERATION_UNKNOWN:
				break;
			default:
				aux_p+=strlen(eval_data_pre_operations[pre_operation].str);
				break;
			case PreOperation::PRE_OPERATION_DEC_INC_INVALID:
				EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"Unknow pre-operation \"%.2s\"",aux_p);
				break;
		}


		IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p,line);

		// parenthesis (evals another expression)
		if(*aux_p=='('){ // inner expression (priority)

			if(pre_operation == PreOperation::PRE_OPERATION_INC || pre_operation == PreOperation::PRE_OPERATION_DEC){
				EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"operation \"%s\" is only allowed on identifiers",eval_data_pre_operations[pre_operation].str);
			}

			if((aux_p=eval_sub_expression(
					eval_data
					, aux_p+1
					, line
					, scope_info
					, &token_node_symbol->eval_instructions
					, NULL
					, properties
					, n_recursive_level+1
					,n_return_values
				)
			)== NULL){
				goto error_expression_token_symbol;
			}

			if(*aux_p != ')'){
				EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"Expected ')'");
			}

			aux_p=aux_p+1;

			if(pre_operation==PreOperation::PRE_OPERATION_NEG){
				token_node_symbol->eval_instructions.push_back((zs_int)(new EvalInstruction(ByteCode::BYTE_CODE_NEG)));
				pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;
			}else if(pre_operation==PreOperation::PRE_OPERATION_NOT){
				token_node_symbol->eval_instructions.push_back((zs_int)(new EvalInstruction(ByteCode::BYTE_CODE_NOT)));
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
				token_node_symbol->token_type = TokenType::TOKEN_TYPE_OBJECT_VECTOR;
			}else if(*aux_p=='{'){ // object ...

				if((aux_p=eval_object(
					eval_data
					,aux_p
					,line
					,scope_info
					,&token_node_symbol->eval_instructions
				))==NULL){
					goto error_expression_token_symbol;
				}

				token_node_symbol->token_type = TokenType::TOKEN_TYPE_OBJECT_OBJECT;

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
					if(last_operator_token_node!=NULL && last_operator_token_node->operator_type!=OPERATOR_UNKNOWN){
						EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,
								eval_data->current_parsing_file
								,last_operator_token_node->line
								,"Expected literal or identifier after operator '%s'"
								,eval_data_operators[last_operator_token_node->operator_type].str
						);
					}else{
						EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,
								eval_data->current_parsing_file
								,last_operator_token_node->line
								,"Expected literal or identifier"
						);
					}

				}

				EvalInstruction *ei_first_token_node=(EvalInstruction *)token_node_symbol->eval_instructions.items[0];

				if(token_node_symbol->token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

					EvalInstruction *eval_instruction = ei_first_token_node;
					eval_instruction->symbol.scope=scope_info;


					if(*aux_p != 0 && *aux_p==':' && *(aux_p+1)==':'){ //  static access

						// mark symbol as static
						// the first item is the class
						zs_string static_access_value=token_node_symbol->value,class_element;
						Symbol *member_symbol=NULL;

						ScriptClass *sc=eval_data->zs->getScriptClassFactory()->getScriptClass(token_node_symbol->value);

						//do{
						IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p+2,line);

						last_accessor_line=line;
						if(get_accessor_name(eval_data, &aux_p, line,class_element) == false){
							EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line,"Expected identifier after '::'");
						}

						static_access_value+="::"+class_element;

						//}while(*aux_p != 0 && *aux_p==':' && *(aux_p+1)==':' && n_static_access < max_static_access);

						// override
						token_node_symbol->value=static_access_value;

						if(sc != NULL){ // if class exist ...

							member_symbol=sc->getSymbol(class_element); // ... and member as well we can define the instruction here

							if(member_symbol != NULL){
								if(member_symbol->properties & SYMBOL_PROPERTY_STATIC){
									if(!eval_set_instruction_static_symbol(&eval_instruction->vm_instruction,member_symbol,static_error)){
										EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,
												eval_data->current_parsing_file
												,line
												,"Symbol '%s' %s"
												,static_access_value.c_str()
												,static_error.c_str());
									}
								}else{
									EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line,"Symbol '%s' is not static",static_access_value.c_str());
								}
							}
						} // --> in eval::pop_function will be find

					}else{ // check if only gets the type

						token_node_symbol_class=eval_data->script_class_factory->getScriptClass(token_node_symbol->value);
						Instruction *instruction=&((EvalInstruction *)token_node_symbol->eval_instructions.items[0])->vm_instruction;
						if(token_node_symbol_class != NULL){ // byte code it will be a type
							instruction->byte_code= ByteCode::BYTE_CODE_LOAD_TYPE;
							instruction->value_op2=token_node_symbol_class->idx_class;
						}else{ // sc is null
							if((last_operator_token_node != NULL && last_operator_token_node->operator_type == Operator::OPERATOR_INSTANCEOF)){
								EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line,"expected a type after 'instanceof'");
							}

						}

					}
				}

				ei_first_token_node->symbol.name=token_node_symbol->value;

				// add info to add as symbol value ...
				ei_first_token_node->instruction_source_info = InstructionSourceInfo(
					eval_data->current_parsing_file
					,line
					,get_mapped_name(eval_data,token_node_symbol->value)
				);

			}
		}

		test_line=line;
		IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,test_aux_p,eval_data,aux_p,test_line);

		// eval accessor element (supose that was a preinsert a load instruction for identifier )...
		if(is_access_punctuator(test_aux_p) && (
				token_node_symbol->token_type==TokenType::TOKEN_TYPE_IDENTIFIER
				|| token_node_symbol->token_type==TokenType::TOKEN_TYPE_OBJECT_FUNCTION
				|| token_node_symbol->token_type==TokenType::TOKEN_TYPE_OBJECT_OBJECT
				|| token_node_symbol->token_type==TokenType::TOKEN_TYPE_OBJECT_VECTOR
		)){

			if(token_node_symbol_class!=NULL){

					EVAL_ERROR_FILE_LINE_AND_GOTO(
							error_expression_token_symbol
							,eval_data->current_parsing_file
							,line
							,"Unexpected  '%c' after type '%s'"
							,*aux_p
							,token_node_symbol->value.c_str()
					);
			}

			// set test
			aux_p=test_aux_p;
			line=test_line;

			zs_string accessor_name="",
						last_accessor_value=token_node_symbol->value;

			char n_params=0;
			int it_accessor_token=0;

			do{
				ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
				accessor_name="";
				zs_int instruction_value2=ZS_IDX_UNDEFINED;
				EvalInstruction *ei_first_token_node=(EvalInstruction *)token_node_symbol->eval_instructions.items[0];

				aux_p=test_aux_p;
				line=test_line;

				switch(*aux_p){
				case '(': // is a function call

					if(	it_accessor_token==0){
						if(
								token_node_symbol->value == SYMBOL_VALUE_SUPER
								&& scope_info->script_class->idx_class == IDX_SCRIPT_CLASS_MAIN){
							EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"\"super\" is not allowed here");
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
								EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,last_line_ok ,"Expected ',' or ')'");
							}

							IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p+1,line);

							if(*aux_p == ')'){ // unexpected ) but is ok anyway
								break;
								//EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,last_line_ok ,"Expected a value parameter after ','");
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
								,n_recursive_level+1
								,n_return_values
						))==NULL){
							error_arg =true;
						}


						// only one argument and is variable
						if(ei_arg_instruction.count==1){
							EvalInstruction *ei_arg=(EvalInstruction *)ei_arg_instruction.items[0];
							ByteCode byte_code=ei_arg->vm_instruction.byte_code;
							if(byte_code==BYTE_CODE_LOAD_REF){
								ei_arg->vm_instruction.byte_code=BYTE_CODE_LOAD_LOCAL;
							}
							else if(byte_code_is_load_var_type(byte_code)){
								ei_arg->vm_instruction.byte_code=byte_code_load_var_type_to_push_stk(byte_code);
								ei_arg->vm_instruction.properties |= INSTRUCTION_PROPERTY_USE_PUSH_STK;
							}
						}

						token_node_symbol->eval_instructions.concat(&ei_arg_instruction);

						if(error_arg){
							goto error_expression_token_symbol;
						}

						n_params++;
					}

					byte_code=ByteCode::BYTE_CODE_CALL;
					if(	it_accessor_token==0 ){ // direct or indirect call
						if(ei_first_token_node->vm_instruction.byte_code==BYTE_CODE_LOAD_THIS_FUNCTION){
							byte_code=ByteCode::BYTE_CODE_THIS_CALL;
						}else if(ei_first_token_node->vm_instruction.byte_code==ByteCode::BYTE_CODE_LOAD_LOCAL){
							byte_code= ByteCode::BYTE_CODE_INDIRECT_LOCAL_CALL;
						}
					}else{ // access token
						if(last_instruction_token->vm_instruction.byte_code == BYTE_CODE_LOAD_THIS_VARIABLE){
							byte_code=ByteCode::BYTE_CODE_THIS_CALL;
						}else{
							byte_code=ByteCode::BYTE_CODE_MEMBER_CALL;
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
							,n_recursive_level+1
							,n_return_values
					))==NULL){
						goto error_expression_token_symbol;
					}

					if(*aux_p != ']'){
						EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"Expected ']'");
					}

					aux_p++;
					byte_code=ByteCode::BYTE_CODE_LOAD_VECTOR_ITEM;
					break;
				case '.': // member/static access

					IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,aux_p,eval_data,aux_p+1,line);

					last_accessor_line=line;
					if(get_accessor_name(eval_data, &aux_p, line,accessor_name) == false){
						goto error_expression_token_symbol;
					}

					if(accessor_name == SYMBOL_VALUE_THIS){
						EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"\"this\" is not allowed as member name");
					}

					byte_code=ByteCode::BYTE_CODE_LOAD_OBJECT_ITEM;
					instruction_token=ei_first_token_node;

					if(it_accessor_token==0 && token_node_symbol->value == SYMBOL_VALUE_THIS){ // check first symbol at first...
						ScriptClass *sf_class = GET_SCRIPT_CLASS(eval_data,eval_data->current_function->script_function->idx_class);

						if(eval_data->current_function->script_function->properties & FUNCTION_PROPERTY_STATIC){
							EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"\"this\" cannot be used in static functions");
						}

						if(scope_info->script_class->idx_class == IDX_SCRIPT_CLASS_MAIN){
							EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"\"this\" is not allowed here");
						}

						// set symbol name
						ei_first_token_node->symbol.name=accessor_name;

						byte_code=ByteCode::BYTE_CODE_LOAD_THIS_VARIABLE;

						// search whether symbol is already in the object and set-it
						/*Symbol *symbol_member=sf_class->getSymbol(accessor_name);

						if(symbol_member != NULL){ // is member
							// functions always loads dynamically because we can have an override function
							// so we don't load as member in a fix position else is a variable ...
							if((symbol_member->properties & SYMBOL_PROPERTY_FUNCTION)==0){
								instruction_value2=symbol_member->idx_position;
								byte_code=ByteCode::BYTE_CODE_LOAD_THIS_VARIABLE;
							}else{
								byte_code=ByteCode::BYTE_CODE_LOAD_THIS_FUNCTION;
								instruction_value2=(zs_int)symbol_member;
							}
						}*/

					}
					break;
				}

				// if byte_code is type BYTE_CODE_LOAD_THIS_XXX it means that is Instruction BYTE_CODE_LOAD_THIS was muted into BYTE_CODE_LOAD_THIS_XXX. So
				// we only create new instructions from bytes code != BYTE_CODE_LOAD_THIS_XXX
				if((
						   byte_code==ByteCode::BYTE_CODE_LOAD_THIS_VARIABLE
						|| byte_code==ByteCode::BYTE_CODE_LOAD_THIS_FUNCTION
					)
					==false){

					// ... we create new instruction
					token_node_symbol->eval_instructions.push_back((zs_int)(instruction_token=new EvalInstruction(byte_code)));
				}

				//EvalInstruction *ei_first_instruction_token=(EvalInstruction *)token_node_symbol->eval_instructions.items[0];

				switch(byte_code){
				case BYTE_CODE_THIS_CALL:
				case BYTE_CODE_CALL:
				case BYTE_CODE_INDIRECT_LOCAL_CALL:
				case BYTE_CODE_INDIRECT_GLOBAL_CALL:
				case BYTE_CODE_UNRESOLVED_CALL:
				case BYTE_CODE_UNRESOLVED_THIS_CALL:
					instruction_token->vm_instruction=ei_first_token_node->vm_instruction;
					instruction_token->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_PARAMETER_COUNT(1,n_params); // by default always returns 1 value
					instruction_token->vm_instruction.byte_code=byte_code;
					instruction_token->symbol=ei_first_token_node->symbol;
					instruction_token->instruction_source_info= ei_first_token_node->instruction_source_info;
					delete ei_first_token_node;
					token_node_symbol->eval_instructions.erase(0);
					break;
				case BYTE_CODE_THIS_MEMBER_CALL:
				case BYTE_CODE_MEMBER_CALL:
				case BYTE_CODE_CONSTRUCTOR_CALL:
					instruction_token->vm_instruction.value_op1=n_params;

					// also insert source file/line/symbol info to get info of this call...
					instruction_token->instruction_source_info= InstructionSourceInfo(
						eval_data->current_parsing_file
						,last_accessor_line
						,get_mapped_name(eval_data,last_accessor_value) // only can get from last_accessor_value because accessor_name is empty on each iteration
					);
					break;
				default:
					instruction_token->vm_instruction.byte_code=byte_code;
					instruction_token->vm_instruction.value_op2=instruction_value2;
					instruction_token->instruction_source_info= InstructionSourceInfo(
						eval_data->current_parsing_file
						,line
						,get_mapped_name(eval_data,accessor_name)
					);
					break;
				}



				// not first access anymore...
				last_accessor_value=accessor_name;
				last_instruction_token=instruction_token;

				it_accessor_token++;

				IGNORE_BLANKS_AND_GOTO_ON_ERROR(error_expression_token_symbol,test_aux_p,eval_data,aux_p,test_line);

			}while(is_access_punctuator(aux_p));

		}else{

			EvalInstruction *ei_first_token_node=(EvalInstruction *)token_node_symbol->eval_instructions.items[0];

			if(token_node_symbol->value==SYMBOL_VALUE_THIS){ // only takes symbol this

				if(eval_data->current_function->script_function->idx_class == IDX_TYPE_MAIN){
					EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line,"\"this\" only can be used within a class");
				}

				ei_first_token_node->instruction_source_info= InstructionSourceInfo(
					eval_data->current_parsing_file
					,last_accessor_line
					,get_mapped_name(eval_data,SYMBOL_VALUE_THIS)
				);

				ei_first_token_node->vm_instruction.value_op2=ZS_IDX_INSTRUCTION_OP2_THIS;
				ei_first_token_node->vm_instruction.properties=0;
			}
		}


		post_operation=is_post_operation(aux_p);

		if(
				(post_operation == PostOperation::POST_OPERATION_INC)
			|| 	(post_operation == PostOperation::POST_OPERATION_DEC)
		){
			EvalInstruction *eval_instruction_post=NULL;
			ByteCode byte_code_post_operation= ByteCode::BYTE_CODE_INVALID;
			Instruction *last_load_instruction=&((EvalInstruction *)(token_node_symbol->eval_instructions.items[token_node_symbol->eval_instructions.count-1]))->vm_instruction;

			if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
				EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"expected identifier after post operation \"%s\"",eval_data_post_operations[ post_operation].str);
			}

			// advance pointer...
			aux_p+=strlen(eval_data_post_operations[post_operation].str);

			if(token_node_symbol->pre_operation != PreOperation::PRE_OPERATION_UNKNOWN){
			   if(token_node_symbol->pre_operation != PreOperation::PRE_OPERATION_NEG){
				   EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"Cannot combine pre-operation \"%s\" with post-operation \"%s\" on \"%s\""
						,eval_data_pre_operations[ pre_operation].str
						,eval_data_pre_operations[ post_operation].str
						,token_node_symbol->value.c_str()
					);
			   }
			}

		   if(post_operation == PostOperation::POST_OPERATION_INC){
			   byte_code_post_operation=ByteCode::BYTE_CODE_POST_INC;
			   if(token_node_symbol->pre_operation==PreOperation::PRE_OPERATION_NEG){
				   byte_code_post_operation=ByteCode::BYTE_CODE_NEG_POST_INC;
				   pre_operation=token_node_symbol->pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // the pre-operation neg was absorbed by -a++
			   }
		   }else {
			   byte_code_post_operation=ByteCode::BYTE_CODE_POST_DEC;
			   if(token_node_symbol->pre_operation==PreOperation::PRE_OPERATION_NEG){
				   byte_code_post_operation=ByteCode::BYTE_CODE_NEG_POST_DEC;
				   pre_operation=token_node_symbol->pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // the pre-operation neg was absorbed by -a--
			   }
		   }

			token_node_symbol->eval_instructions.push_back((zs_int)(
				eval_instruction_post=new EvalInstruction(
					byte_code_post_operation
				)
			));

			eval_instruction_post->instruction_source_info=InstructionSourceInfo(
				eval_data->current_parsing_file
				,line
			);

			// if post inc/dec hange load by push because is mutable
			if(byte_code_is_load_var_type(last_load_instruction->byte_code)){
				last_load_instruction->byte_code=byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
			}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
				last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}
		}

		if(		(pre_operation == PreOperation::PRE_OPERATION_NEG)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_NOT)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_INC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_DEC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_TYPEOF)
		){
			EvalInstruction *eval_instruction_pre=NULL;

			if(pre_operation != PreOperation::PRE_OPERATION_TYPEOF){
				if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"expected identifier before pre operation \"%s\"",eval_data_pre_operations[ pre_operation].str);
				}
			}else{
				// check is not type
				if(eval_data->script_class_factory->getScriptClass(token_node_symbol->value)!=NULL){
					EVAL_ERROR_FILE_LINE_AND_GOTO(error_expression_token_symbol,eval_data->current_parsing_file,line ,"expected variable after 'typeof' but it was a type");
				}
			}

			Instruction *last_load_instruction=&((EvalInstruction *)(token_node_symbol->eval_instructions.items[token_node_symbol->eval_instructions.count-1]))->vm_instruction;

			token_node_symbol->eval_instructions.push_back((zs_int)(
				eval_instruction_pre=new EvalInstruction(
					pre_operation == PreOperation::PRE_OPERATION_NEG ? ByteCode::BYTE_CODE_NEG:
					pre_operation == PreOperation::PRE_OPERATION_NOT ? ByteCode::BYTE_CODE_NOT:
					pre_operation == PreOperation::PRE_OPERATION_DEC ? ByteCode::BYTE_CODE_PRE_DEC:
					pre_operation == PreOperation::PRE_OPERATION_TYPEOF ? ByteCode::BYTE_CODE_TYPEOF:
					ByteCode::BYTE_CODE_PRE_INC
				)
			));

			eval_instruction_pre->instruction_source_info=InstructionSourceInfo(
				eval_data->current_parsing_file
				,line
			);


			// if pre inc/dec change load by push because is mutable
			if(
				(pre_operation == PreOperation::PRE_OPERATION_INC)
			|| 	(pre_operation == PreOperation::PRE_OPERATION_DEC)){

				if(byte_code_is_load_var_type(last_load_instruction->byte_code)){
					last_load_instruction->byte_code=byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
				}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
					last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
				}
			}

		}

		// finally push token node symbol
		token_nodes->push_back((zs_int)token_node_symbol);

		return aux_p;

error_expression_token_symbol:

		for(unsigned kk=0;kk<token_node_symbol->eval_instructions.count;kk++){
			delete (EvalInstruction *)token_node_symbol->eval_instructions.items[kk];
			token_node_symbol->eval_instructions.items[kk]=0;
		}

		delete token_node_symbol;

		return NULL;
	}
}

