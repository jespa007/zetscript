


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

		bool get_accessor_name(EvalData *eval_data, char *s, int line, std::string & accessor_name){
			char *aux_p = s;
			accessor_name="";
			while(!is_end_symbol_token(aux_p)){ // get name...
				accessor_name += (*aux_p++);
			}
			return check_identifier_name_expression_ok(eval_data,accessor_name,line) == TRUE;

		}

		char *eval_expression_token_symbol(EvalData *eval_data, char *s, int & current_line, Scope *scope_info, std::vector<TokenNode> *expression_tokens){
			char *aux_p = NULL, *test_s=NULL;
			int first_line=current_line;
			int line=current_line;
			TokenNode token_node_symbol;
			PreOperation pre_operation = PreOperation::PRE_OPERATION_UNKNOWN;
			PostOperation post_operation=PostOperation::POST_OPERATION_UNKNOWN;
			int last_accessor_line=0;
			std::string last_accessor_value="";
			std::string static_access_name="";
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
				last_accessor_line=0;
				last_accessor_value="";
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
//									,pre_operation
					)) == NULL){
						goto error_expression_token_symbol;
					}

					last_accessor_value=token_node_symbol.value;

					if(token_node_symbol.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){
						token_node_symbol.instructions[0]->symbol.scope=scope_info;
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

			last_accessor_value=token_node_symbol.value;

			IGNORE_BLANKS(aux_p,eval_data,s,line);

			// eval accessor element (supose that was a preinsert a load instruction for identifier )...
			if(is_access_punctuator_or_static_reference(aux_p)){
				std::string accessor_name="",
							last_accessor_value="";

				std::vector<std::string >static_access;
				line=current_line;
				test_s=s;
				char n_params=0;
				bool is_static_access=false, is_next_static=false;
				int iteration=0;

				static_access.clear();

				do{
					ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
					accessor_name="";
					bool this_symbol_access=false;
					//bool vector_access=false;
					EvalInstruction *instruction_token=NULL;


					aux_p=test_s;

					switch(*aux_p){
					case '(': // is a function call

						if(	iteration==0
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
						is_static_access=false;
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
						//vector_access=true;
						is_static_access=false;
						byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_VECTOR;
						break;
					case ':':
						if( *(aux_p+1) == ':'){ // static access
							// check if first access, else check if last name was also static...
							if(iteration==0){
								// push first access name
								static_access.push_back(token_node_symbol.value);
								is_static_access=true;
								aux_p++;

								last_accessor_line=line;
								if(get_accessor_name(eval_data, s, line,accessor_name) == false){
									goto error_expression_token_symbol;
								}

								IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
								// check static const var...
								is_static_access = is_access_punctuator_or_static_reference(aux_p) && *aux_p != '.'; // . property ends static
								if(is_static_access == false){ // end static access and load variable
									byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_OBJECT;
								}

								static_access.push_back(accessor_name);
							}
							else{
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"Invalid static access");
							}
						}else{
							EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"Unexpected char ':'");
						}
						break;
					case '.': // member/static access

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						last_accessor_line=line;
						if(get_accessor_name(eval_data, s, line,accessor_name) == false){
							goto error_expression_token_symbol;
						}


						if(accessor_name == SYMBOL_VALUE_THIS){
							EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"this\" is not allowed as member name");
						}

						if(iteration==0){ // check first symbol at first...

							if(token_node_symbol.value == SYMBOL_VALUE_THIS){

								if(eval_data->current_function->script_function->symbol.properties & SYMBOL_PROPERTY_STATIC){
									EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"this\" cannot be used in static functions");
								}

								if(scope_info == MAIN_SCOPE(eval_data)){
									EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line ,"\"this\" is not allowed here");
								}

								// set symbol name
								instruction_token=token_node_symbol.instructions[0];
								instruction_token->symbol.name=accessor_name;

								// set variable accessor instead of "this"
								instruction_token->instruction_source_info= InstructionSourceInfo(
									eval_data->current_parsing_file
									,line
									,get_mapped_name(eval_data,accessor_name)
								);
							}


							byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_OBJECT;
						}
						break;
					}

					if(instruction_token==NULL && (is_static_access==false)){


						ScriptClass *script_class_access=NULL;
						Symbol *symbol_static = NULL;//script_class_access->getSymbol();
						//EvalInstruction *last_instruction_token=NULL;

						if(   static_access.size() == 0 // is not static access
							|| byte_code == ByteCode::BYTE_CODE_CALL // it requires an extra op to do call
						){

							instruction_token=new EvalInstruction(byte_code);
							token_node_symbol.instructions.push_back(instruction_token);
						}

						if(static_access.size() > 0){

							EvalInstruction *first_instruction_token=token_node_symbol.instructions[0]; // override first instruction

							if(IS_BYTE_CODE_LOAD_VARIABLE_TYPE(byte_code)){
								instruction_token=first_instruction_token;
							}

							//std::vectorzs_strutils::split('|');
							static_access_name="";
							for(unsigned i=0; i < static_access.size(); i++){
								if(i>0){
									static_access_name+="::";
								}
								static_access_name+=static_access[i];
							}

							// override symbol name
							token_node_symbol.value=static_access_name;
							first_instruction_token->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data,static_access_name);

							if((script_class_access=eval_data->script_class_factory->getScriptClass(static_access[0]))==NULL){
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"static access error: class \"%s\" not exist",static_access[0].c_str());
							}

							symbol_static=script_class_access->getSymbol(static_access[1]);

							if(symbol_static != NULL){

								if(IS_BYTE_CODE_LOAD_VARIABLE_TYPE(byte_code)){ // it should be constant type ...

									// check symbol is native and not function
									if((symbol_static->properties & SYMBOL_PROPERTY_CONST) == 0){
										EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"Symbol \"%s\" is not constant",static_access_name.c_str());
									}

									first_instruction_token->vm_instruction.byte_code=ByteCode::BYTE_CODE_LOAD_STACK_ELEMENT;
									first_instruction_token->vm_instruction.value_op2=symbol_static->ref_ptr; // global stack element
								}
								else if(byte_code == ByteCode::BYTE_CODE_CALL){

									bool is_static_symbol= symbol_static->properties & SYMBOL_PROPERTY_STATIC;

									if( symbol_static->n_params >= 0){ // is a function
										ScriptFunction *static_script_function = (ScriptFunction *)symbol_static->ref_ptr;
										if(static_script_function->symbol.properties & SYMBOL_PROPERTY_STATIC){
											first_instruction_token->vm_instruction.byte_code=ByteCode::BYTE_CODE_LOAD_FUNCTION;
											first_instruction_token->vm_instruction.value_op2=(zs_int)static_script_function;
										}else{
											EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"static access error: \"%s\" is not static. Make sure that there's no this/super defined in body",static_access_name.c_str());
										}
									}else{
										EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"static access error: \"%s\" is not a function",static_access_name.c_str());
									}
								}else{
									EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"static access: Unexpected byte code");
								}
							}else{
								EVAL_ERROR_EXPRESSION_TOKEN(eval_data->current_parsing_file,line,"Symbol %s not exist",static_access_name.c_str());
							}

							// clear static symbols to mark end static
							static_access.clear();
						}

						// generate source info in case accessor load...
						if(IS_BYTE_CODE_LOAD_VARIABLE_TYPE(byte_code)){
							// override load byte code if accessor
							if(byte_code == BYTE_CODE_LOAD_ELEMENT_VECTOR){
								instruction_token->vm_instruction.byte_code=BYTE_CODE_LOAD_ELEMENT_VECTOR; //.properties|=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR;
							}else if(symbol_static == NULL){
								instruction_token->vm_instruction.byte_code=BYTE_CODE_LOAD_ELEMENT_OBJECT;//.properties|=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD;
							}

							instruction_token->instruction_source_info= InstructionSourceInfo(
								eval_data->current_parsing_file
								,line
								,get_mapped_name(eval_data,symbol_static!=NULL?static_access_name:accessor_name)
							);
						}
						else if(byte_code == ByteCode::BYTE_CODE_CALL){
							// save total parameters on this call
							instruction_token->vm_instruction.value_op1=n_params;

							// also insert source file/line/symbol info to get info of this call...
							instruction_token->instruction_source_info= InstructionSourceInfo(
								eval_data->current_parsing_file
								,last_accessor_line
								,get_mapped_name(eval_data,symbol_static != NULL?static_access_name:last_accessor_value) // only can get from last_accessor_value because accessor_name is empty on each iteration
							);
						}

					}
					// not first access anymore...

					//last_access_static=is_static_access;
					last_accessor_value=accessor_name;

					test_s=aux_p;
					//last_line_ok=line;
					iteration++;

				}while(is_access_punctuator_or_static_reference(aux_p));


			}else{

				if(token_node_symbol.value==SYMBOL_VALUE_THIS){
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
