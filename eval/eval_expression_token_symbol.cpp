
#define EVAL_ERROR_EXPRESSION_TOKEN(file,line,s,...)		eval_data->error=true;\
														eval_data->error_str=ZS_LOG_FILE_LINE_STR(file,line)+zetscript::zs_strutils::format(s, ##__VA_ARGS__);\
														goto error_expression_token_symbol;\

namespace zetscript{
	namespace eval{

		bool  is_access_punctuator(char *s){
			return *s=='.' || *s=='[' || *s=='(';
		}

		bool  is_access_punctuator_or_static_reference(char *s){
			return  is_access_punctuator(s) || (*s==':' && *(s+1)==':');
		}

		bool get_accessor_name(EvalData *eval_data, char **aux_p, int line, std::string & accessor_name){
			//char *aux_p = s;
			accessor_name="";
			while(!is_end_symbol_token(*aux_p)){ // get name...
				accessor_name += *(*aux_p)++;
			}
			return check_identifier_name_expression_ok(eval_data,accessor_name,line) == TRUE;
		}

		char *eval_expression_token_symbol(EvalData *eval_data, char *s, int & current_line, Scope *scope_info, std::vector<TokenNode> *expression_tokens){
			char *aux_p = s;//, *test_s=NULL;
			int first_line=current_line;
			int line=current_line;
			TokenNode token_node_symbol;
			EvalInstruction *instruction_token=NULL;
			PreOperation pre_operation = PreOperation::PRE_OPERATION_UNKNOWN;
			PostOperation post_operation=PostOperation::POST_OPERATION_UNKNOWN;
			int last_accessor_line=0;
			Keyword keyword_type=Keyword::KEYWORD_UNKNOWN;

			// check pre operator (-,+,!,-- or ++)
			switch(pre_operation=is_pre_operation(aux_p)){
				case PreOperation::PRE_OPERATION_UNKNOWN:
					break;
				default:
					aux_p+=strlen(eval_data_pre_operations[pre_operation].str);
					break;
				case PreOperation::PRE_OPERATION_DEC_INC_INVALID:
					EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"Unknow pre-operation \"%.2s\"",aux_p);
					break;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
			keyword_type=is_keyword(aux_p);

			// parenthesis (evals another expression)
			if(*aux_p=='('){ // inner expression (priority)

				if(pre_operation == PreOperation::PRE_OPERATION_INC || pre_operation == PreOperation::PRE_OPERATION_DEC){
					EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"operation \"%s\" is only allowed on identifiers",eval_data_pre_operations[pre_operation].str);
				}

				if((aux_p=eval_expression_main(
						eval_data
						,aux_p+1
						, line
						, scope_info
						, &token_node_symbol.instructions
						,std::vector<char>{}
					)
				)== NULL){
					goto error_expression_token_symbol;
				}

				if(*aux_p != ')'){
					EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"Expected ')'");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				if(pre_operation==PreOperation::PRE_OPERATION_NEG){
					token_node_symbol.instructions.push_back(new EvalInstruction(ByteCode::BYTE_CODE_NEG));
				}else if(pre_operation==PreOperation::PRE_OPERATION_NOT){
					token_node_symbol.instructions.push_back(new EvalInstruction(ByteCode::BYTE_CODE_NOT));
				}

				token_node_symbol.token_type=TokenType::TOKEN_TYPE_SUBEXPRESSION;
			}
			else{ // symbol token
				if(*aux_p=='['){ // std::vector object...
					if((aux_p=eval_object_vector(
						eval_data
						,aux_p
						,line
						,scope_info
						,&token_node_symbol.instructions
					))==NULL){
						goto error_expression_token_symbol;
					}
					token_node_symbol.token_type = TokenType::TOKEN_TYPE_VECTOR;
				}else if(*aux_p=='{'){ // struct object ...

					if((aux_p=eval_object(
						eval_data
						,aux_p
						,line
						,scope_info
						,&token_node_symbol.instructions
					))==NULL){
						goto error_expression_token_symbol;
					}

					token_node_symbol.token_type = TokenType::TOKEN_TYPE_OBJECT;

				}else if(keyword_type == Keyword::KEYWORD_NEW){

					if((aux_p=eval_object_new(
						eval_data
						,aux_p
						,line
						,scope_info
						,&token_node_symbol.instructions
					))==NULL){
						goto error_expression_token_symbol;
					}

					token_node_symbol.token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

				}else if(keyword_type == Keyword::KEYWORD_FUNCTION){ // can be after instanceof or a function object..

					if((aux_p=eval_object_function(
						eval_data
						,aux_p
						,line
						,scope_info
						,&token_node_symbol
					))==NULL){
						goto error_expression_token_symbol;
					}

					token_node_symbol.token_type = TokenType::TOKEN_TYPE_FUNCTION_OBJECT;
				}
				else { // symbol
					token_node_symbol.pre_operation=pre_operation;

					last_accessor_line=line;
					if((aux_p = eval_symbol(eval_data
							,aux_p
							,line
							,&token_node_symbol
							,pre_operation
					)) == NULL){
						goto error_expression_token_symbol;
					}

					if(token_node_symbol.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

						token_node_symbol.instructions[0]->symbol.scope=scope_info;
						EvalInstruction *instruction = token_node_symbol.instructions[0];

						if(*aux_p != 0 && *aux_p==':' && *(aux_p+1)==':'){ // class static

							// the first item is the class
							std::string static_access_value=token_node_symbol.value,class_element;
							Symbol *member_symbol=NULL;

							ScriptClass *sc=eval_data->zs->getScriptClassFactory()->getScriptClass(token_node_symbol.value);

							if(sc == NULL){
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"static access error: class \"%s\" not exist",token_node_symbol.value.c_str());
							}

							//do{
							IGNORE_BLANKS(aux_p,eval_data,aux_p+2,line);

							last_accessor_line=line;
							if(get_accessor_name(eval_data, &aux_p, line,class_element) == false){
								goto error_expression_token_symbol;
							}

							static_access_value+="::"+class_element;

							//}while(*aux_p != 0 && *aux_p==':' && *(aux_p+1)==':' && n_static_access < max_static_access);

							// override
							token_node_symbol.value=static_access_value;

							member_symbol=sc->getSymbol(class_element);

							if(member_symbol == NULL){
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"Symbol %s not exist",static_access_value.c_str());
							}

							if(member_symbol->properties & SYMBOL_PROPERTY_STATIC){ // it should be constant type ...

								instruction->vm_instruction.value_op2=member_symbol->ref_ptr; // it's pointer (script function) or stack element id (const)

								if(member_symbol->properties & SYMBOL_PROPERTY_FUNCTION){
									instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_FUNCTION;
								}else if(member_symbol->properties & SYMBOL_PROPERTY_CONST){
									instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_GLOBAL;
								}else{
									EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"Static symbol \"%s\" expected to be function or const",static_access_value.c_str());
								}
							}
							else{
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"Symbol \"%s\" is not static",static_access_value.c_str());
							}
						}

						token_node_symbol.instructions[0]->symbol.name=token_node_symbol.value;

						// add info to add as symbol value ...
						token_node_symbol.instructions[0]->instruction_source_info = InstructionSourceInfo(
							eval_data->current_parsing_file
							,line
							,get_mapped_name(eval_data,token_node_symbol.value)
						);
					}
				}
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// eval accessor element (supose that was a preinsert a load instruction for identifier )...
			if(is_access_punctuator(aux_p)){
				std::string accessor_name="",
							last_accessor_value=token_node_symbol.value;

				line=current_line;
				char n_params=0;
				int it_accessor_token=0;

				do{
					ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
					accessor_name="";
					zs_int instruction_value2=ZS_IDX_UNDEFINED;

					switch(*aux_p){
					case '(': // is a function call

						if(	it_accessor_token==0
								&& token_node_symbol.value == SYMBOL_VALUE_SUPER
								&& scope_info == MAIN_SCOPE(eval_data)
						){
							EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"super\" is not allowed here");

						}

						n_params=0;
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						while(*aux_p != ')'){
							// check if ref identifier...
							if(n_params>0){
								if(*aux_p != ','){
									EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"Expected ','");
								}
								aux_p++;
							}

							if((aux_p = eval_expression_main(
									eval_data
									,aux_p
									,line
									,scope_info
									,&token_node_symbol.instructions
									,std::vector<char>{}
							))==NULL){
								goto error_expression_token_symbol;
							}
							n_params++;
						}

						byte_code=ByteCode::BYTE_CODE_CALL;
						aux_p++;
						break;
					case '[': // std::vector access
						if((aux_p = eval_expression_main(
								eval_data
								,aux_p+1
								,line
								,scope_info
								,&token_node_symbol.instructions
								,std::vector<char>{}
						))==NULL){
							goto error_expression_token_symbol;
						}

						if(*aux_p != ']'){
							EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"Expected ']'");
						}

						aux_p++;
						byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_VECTOR;
						break;
					case '.': // member/static access

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						last_accessor_line=line;
						if(get_accessor_name(eval_data, &aux_p, line,accessor_name) == false){
							goto error_expression_token_symbol;
						}

						if(accessor_name == SYMBOL_VALUE_THIS){
							EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"this\" is not allowed as member name");
						}

						byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_OBJECT;

						if(it_accessor_token==0 && token_node_symbol.value == SYMBOL_VALUE_THIS){ // check first symbol at first...
							ScriptClass *sf_class = GET_SCRIPT_CLASS(eval_data,eval_data->current_function->script_function->idx_class);

							if(eval_data->current_function->script_function->symbol.properties & SYMBOL_PROPERTY_STATIC){
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"this\" cannot be used in static functions");
							}

							if(scope_info == MAIN_SCOPE(eval_data)){
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"this\" is not allowed here");
							}

							// set symbol name
							token_node_symbol.instructions[0]->symbol.name=accessor_name;

							byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_THIS;

							// search whether symbol is already in the object and set-it
							Symbol *symbol_member=sf_class->getSymbol(accessor_name);

							if(symbol_member != NULL){ // is member
								instruction_value2=symbol_member->idx_position;
								byte_code=ByteCode::BYTE_CODE_LOAD_THIS_MEMBER;
							}


						}
						break;
					}


					token_node_symbol.instructions.push_back(instruction_token=new EvalInstruction(byte_code));

					switch(byte_code){
					case BYTE_CODE_CALL:
						instruction_token->vm_instruction.value_op1=n_params;

						// also insert source file/line/symbol info to get info of this call...
						instruction_token->instruction_source_info= InstructionSourceInfo(
							eval_data->current_parsing_file
							,last_accessor_line
							,get_mapped_name(eval_data,last_accessor_value) // only can get from last_accessor_value because accessor_name is empty on each iteration
						);
						break;
					default:
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

					it_accessor_token++;

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				}while(is_access_punctuator(aux_p));

			}else{

				if(token_node_symbol.value==SYMBOL_VALUE_THIS){ // only takes symbol this
					if(eval_data->current_function->script_function->idx_class == IDX_BUILTIN_TYPE_CLASS_MAIN){
						EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"\"this\" only can be used within a class");
					}

					token_node_symbol.instructions[0]->instruction_source_info= InstructionSourceInfo(
						eval_data->current_parsing_file
						,last_accessor_line
						,get_mapped_name(eval_data,SYMBOL_VALUE_THIS)
					);

					token_node_symbol.instructions[0]->vm_instruction.value_op2=ZS_IDX_INSTRUCTION_OP2_THIS;
					token_node_symbol.instructions[0]->vm_instruction.properties=0;
				}
			}

			if(		(pre_operation == PreOperation::PRE_OPERATION_NEG)
				|| 	(pre_operation == PreOperation::PRE_OPERATION_NOT)
				|| 	(pre_operation == PreOperation::PRE_OPERATION_INC)
				|| 	(pre_operation == PreOperation::PRE_OPERATION_DEC)
			){
				if(token_node_symbol.token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"expected identifier before pre operation \"%s\"",eval_data_pre_operations[ pre_operation].str);
				}

				token_node_symbol.instructions.push_back(
					new EvalInstruction(
						pre_operation == PreOperation::PRE_OPERATION_NEG ? ByteCode::BYTE_CODE_NEG:
						pre_operation == PreOperation::PRE_OPERATION_NOT ? ByteCode::BYTE_CODE_NOT:
						pre_operation == PreOperation::PRE_OPERATION_DEC ? ByteCode::BYTE_CODE_PRE_DEC:
						ByteCode::BYTE_CODE_PRE_INC
					)
				);
			}

			post_operation=is_post_operation(aux_p);

			if(
					(post_operation == PostOperation::POST_OPERATION_INC)
				|| 	(post_operation == PostOperation::POST_OPERATION_DEC)
			){
				ByteCode byte_code_post_operation= ByteCode::BYTE_CODE_INVALID;

				if(token_node_symbol.token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"expected identifier after post operation \"%s\"",eval_data_post_operations[ post_operation].str);
				}

				if(token_node_symbol.pre_operation != PreOperation::PRE_OPERATION_UNKNOWN){
				   if(token_node_symbol.pre_operation != PreOperation::PRE_OPERATION_NEG){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Cannot combine pre-operation \"%s\" with post-operation \"%s\" on \"%s\""
							,eval_data_pre_operations[ pre_operation].str
							,eval_data_pre_operations[ post_operation].str
							,token_node_symbol.value.c_str()
						);
				   }
				}

			   if(post_operation == PostOperation::POST_OPERATION_INC){
				   byte_code_post_operation=ByteCode::BYTE_CODE_POST_INC;
				   if(token_node_symbol.pre_operation==PreOperation::PRE_OPERATION_NEG){
					   byte_code_post_operation=ByteCode::BYTE_CODE_NEG_POST_INC;
				   }
			   }else {
				   byte_code_post_operation=ByteCode::BYTE_CODE_POST_DEC;
				   if(token_node_symbol.pre_operation==PreOperation::PRE_OPERATION_NEG){
					   byte_code_post_operation=ByteCode::BYTE_CODE_NEG_POST_DEC;
				   }
			   }

				token_node_symbol.instructions.push_back(
					new EvalInstruction(
						byte_code_post_operation
					)
				);
			}

			// finally push token node symbol
			expression_tokens->push_back(token_node_symbol);

			return aux_p;

error_expression_token_symbol:

			for(unsigned kk=0;kk<token_node_symbol.instructions.size();kk++){
				delete token_node_symbol.instructions[kk];
			}

			return NULL;
		}
	}
}
