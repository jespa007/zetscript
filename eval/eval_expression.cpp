namespace zetscript{

	namespace eval{

		typedef struct{
			EvalInstruction *	instruction_symbol; // affected symbol
			Operator			assign_operator; // operator
			TokenNode			*token_node;
		}AssignTokenInformation;

		ByteCode convert_operator_to_byte_code(Operator op){
			switch(op){
			default:
				break;
			case Operator::OPERATOR_ASSIGN:
				return ByteCode::BYTE_CODE_STORE;

			// operators and assignments
			case Operator::OPERATOR_ADD:
			case Operator::OPERATOR_ASSIGN_ADD:
				return ByteCode::BYTE_CODE_ADD;

			case Operator::OPERATOR_SUB:
			case Operator::OPERATOR_ASSIGN_SUB:
				return ByteCode::BYTE_CODE_SUB;

			case Operator::OPERATOR_MUL:
			case Operator::OPERATOR_ASSIGN_MUL:
				return ByteCode::BYTE_CODE_MUL;

			case Operator::OPERATOR_ASSIGN_DIV:
			case Operator::OPERATOR_DIV:
				return ByteCode::BYTE_CODE_DIV;

			case Operator::OPERATOR_ASSIGN_MOD:
			case Operator::OPERATOR_MOD:
				return ByteCode::BYTE_CODE_MOD;

			case Operator::OPERATOR_ASSIGN_XOR:
			case Operator::OPERATOR_XOR:
				return ByteCode::BYTE_CODE_XOR;

			case Operator::OPERATOR_ASSIGN_BINARY_AND:
			case Operator::OPERATOR_BINARY_AND:
				return ByteCode::BYTE_CODE_AND;

			case Operator::OPERATOR_ASSIGN_SHIFT_LEFT:
			case Operator::OPERATOR_SHIFT_LEFT:
				return ByteCode::BYTE_CODE_SHL;

			case Operator::OPERATOR_ASSIGN_SHIFT_RIGHT:
			case Operator::OPERATOR_SHIFT_RIGHT:
				return ByteCode::BYTE_CODE_SHR;

			case Operator::OPERATOR_OR:
				return ByteCode::BYTE_CODE_OR;
			case Operator::OPERATOR_LOGIC_AND:
				return ByteCode::BYTE_CODE_LOGIC_AND;
			case Operator::OPERATOR_LOGIC_OR:
				return ByteCode::BYTE_CODE_LOGIC_OR;
			case Operator::OPERATOR_LOGIC_EQUAL:
				return ByteCode::BYTE_CODE_EQU;
			case Operator::OPERATOR_LOGIC_NOT_EQUAL:
				return ByteCode::BYTE_CODE_NOT_EQU;
			case Operator::OPERATOR_LOGIC_GT:
				return ByteCode::BYTE_CODE_GT;
			case Operator::OPERATOR_LOGIC_LT:
				return ByteCode::BYTE_CODE_LT;
			case Operator::OPERATOR_LOGIC_GTE:
				return ByteCode::BYTE_CODE_GTE;
			case Operator::OPERATOR_LOGIC_LTE:
				return ByteCode::BYTE_CODE_LTE;
			case Operator::OPERATOR_INSTANCEOF:
				return ByteCode::BYTE_CODE_INSTANCEOF;
			}

			THROW_RUNTIME_ERROR("Convert %i to byte code not implemented",op);
			return ByteCode::BYTE_CODE_END_FUNCTION;
		}

		std::string * get_compiled_symbol(EvalData *eval_data,const std::string & s){
			if(eval_data->compiled_symbol_name.count(s)==0){
				eval_data->compiled_symbol_name[s]=new std::string (s);
			}
			return eval_data->compiled_symbol_name[s];
		}

		// to std::string utils ...
		char * eval_symbol(EvalData *eval_data
				,const char *start_word
				, int line
				,TokenNode * token_node
				, PreOperator pre_operator
				, PrePostSelfOperation pre_self_operation
		){
			// PRE:
			unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT;
			//LoadType load_type=LOAD_TYPE_NOT_DEFINED;
			bool is_constant_number=false, is_constant_boolean=false;
			EvalInstruction *instruction=NULL;
			ByteCode byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_CONSTANT;
			token_node->token_type = TokenType::TOKEN_TYPE_LITERAL;


			//unsigned int scope_type=0;
			void *obj=NULL,*get_obj=NULL,*const_obj=NULL;
			char *aux=(char *)start_word;
			std::string str_value="";
			 bool error=false;
			 //token_node->token_type = TokenType::TOKEN_TYPE_UNKNOWN;
			 bool is_constant_string=false;
			 unsigned short instruction_properties=0;
			 bool is_is_symbol_super_method=false;
			 token_node->token_type = TokenType::TOKEN_TYPE_UNKNOWN;

			 if((aux=parse_literal_number(
					 eval_data
					 ,start_word
					 ,line
					 ,str_value
			))==NULL){ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

				 aux=(char *)start_word;
				 // try eval identifier, boolean, std::string ...

				char pre=0;
				if(*aux=='\"'){
					is_constant_string = true;
					do{
						aux++;
					}while(!((*aux=='\"' && *(aux-1)!= '\\')|| *aux==0 || *aux=='\n'));
				}else{ // identifier
					bool end=false;
					while(!end){
						pre=*aux;
						str_value += (*aux++);

						if(is_end_symbol_token(aux,pre)){
							end=true;
						}
					}
				}

				// verify constant string well format
				if(is_constant_string && *aux!='\"'){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Constant string not ends with '\"'");
				}

				if(*aux=='\"'){ // register constant string

					 if((start_word+1)<aux){ // copy string without double quotes...
						 zs_strutils::copy_from_ptr_diff(str_value,start_word+1,aux);
					 }
					 aux++;

					 type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING;

					 std::string key_value="\""+str_value+"\"";

					if((get_obj = eval_data->zs->getRegisteredConstantValue(key_value))!=NULL){
						obj = get_obj;
					}else{
						ScriptObjectString *s=new ScriptObjectString(eval_data->zs,str_value);
						obj=eval_data->zs->registerConstantValue(key_value,NULL,type);
						((StackElement *)obj)->stk_value=((void *)(s->str_value.c_str()));
						((StackElement *)obj)->var_ref=s;
					 }
				}
				 // add load std::string constant
				// compile constant ...
			 }

			 std::string str_number_value=str_value,str_boolean_value=str_value;

			 if(!is_constant_string){
				 // try parse value...
				if(str_value=="null"){ // null literal
					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;
					byte_code=BYTE_CODE_LOAD_TYPE_NULL;
					obj=NULL;//ScriptObject::NullSymbol;

				}else if(str_value=="undefined"){ // undefined literal
						type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
						byte_code=BYTE_CODE_LOAD_TYPE_UNDEFINED;
						obj=NULL;// ScriptObject::UndefinedSymbol;
				}else if((const_obj=zs_strutils::parse_int(str_number_value))!=NULL){ // int literal
					int value = *((int *)const_obj);
					if(pre_operator==PreOperator::PRE_OPERATOR_NEG){
						value=-value;
						str_number_value="-"+str_number_value;
					}

					delete (int *)const_obj;
					//load_type=LOAD_TYPE_CONSTANT;
					obj=eval_data->zs->registerConstantValue(str_number_value,value);
					is_constant_number=true;
				}
				else if((const_obj=zs_strutils::parse_float(str_number_value))!=NULL){ // float literal
					float value = *((float *)const_obj);

					if(pre_operator==PreOperator::PRE_OPERATOR_NEG){
						value=-value;
						str_number_value="-"+str_number_value;
					}

					delete (float *)const_obj;
					void *value_ptr;
					memcpy(&value_ptr,&value,sizeof(float));

					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
					//load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(str_number_value))!=NULL){
						obj = get_obj;
					}else{
						obj=eval_data->zs->registerConstantValue(str_number_value,value_ptr,type);
					}
					is_constant_number=true;
				}
				else if((const_obj=zs_strutils::parse_bool(str_boolean_value))!=NULL){ // bool literal

					bool value = *((bool *)const_obj);
					if(pre_operator==PreOperator::PRE_OPERATOR_NOT){
						value=!value;
						str_boolean_value="!"+str_boolean_value;
					}

					delete (bool *)const_obj;

					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;
					//load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(str_boolean_value))!=NULL){
						obj = get_obj;
					}else{
						obj=eval_data->zs->registerConstantValue(str_boolean_value,(void *)value,type);
					}
					is_constant_boolean=true;
				}else{ // it should be an identifier token  ...
					token_node->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
					byte_code = ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE;

					if(str_value == SYMBOL_VALUE_THIS || str_value == SYMBOL_VALUE_SUPER){
						if(str_value == SYMBOL_VALUE_SUPER){
							is_is_symbol_super_method=true;
						}

						instruction_properties=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;

					}else if((get_obj = eval_data->zs->getRegisteredConstantValue(str_value)) != NULL){  // check if symbol is constant ...
						byte_code = ByteCode::BYTE_CODE_LOAD_TYPE_CONSTANT;
						obj=get_obj;
						//load_type=LOAD_TYPE_CONSTANT;
					}else{
						// should be an identifier and should be find after eval function or at runtime...

						byte_code = ByteCode::BYTE_CODE_LOAD_TYPE_FIND;

						check_identifier_name_expression_ok(
							eval_data
							,str_value
							,line
						);
					}
				}
			 }

			 // check pre self operation (only allowed on indentifiers...
			 if(
					 pre_self_operation != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
					 && token_node->token_type != TokenType::TOKEN_TYPE_IDENTIFIER
			){
				 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"operation \"%s\" is only allowed on identifiers ",eval_info_pre_post_self_operations[ pre_self_operation].str);
			 }

			 // check pre operators...
			 if((   token_node->pre_operator == PreOperator::PRE_OPERATOR_NEG
			    || token_node->pre_operator == PreOperator::PRE_OPERATOR_POS)
					&& !(is_constant_number || (token_node->token_type == TokenType::TOKEN_TYPE_IDENTIFIER))
			     )
			 {
				 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"+/- pre operator not allowed before \"%s\". Only allowed on  numbers or identifiers",str_value.c_str(),eval_info_pre_operators[ token_node->pre_operator].str);
			 }

			 if(
					 (token_node->pre_operator == PreOperator::PRE_OPERATOR_NOT)
				  && !(is_constant_number || is_constant_boolean || (token_node->token_type == TokenType::TOKEN_TYPE_IDENTIFIER))
			     )
			 {
				 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"! pre operator not allowed before \"%s\". only allowed on constants booleans/numbers or identifiers ",str_value.c_str(),eval_info_pre_operators[ token_node->pre_operator].str);
			 }

			 if((token_node->pre_operator == PreOperator::PRE_OPERATOR_NEG) || (token_node->pre_operator == PreOperator::PRE_OPERATOR_NOT)){
				 instruction_properties|=MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT;
			 }

			token_node->value = str_value;
			token_node->instructions.push_back(
				instruction=new EvalInstruction(
						byte_code
						,ZS_IDX_UNDEFINED
						,(intptr_t)obj
						,instruction_properties
			));

			if(is_is_symbol_super_method){
				instruction->is_symbol_super_method=true;
			}

			return aux;
			// POST: token as literal or identifier
		}


		// eval operator expression only evaluates expression with normal operators (+,-,>>,<<,etc) respecting always its preference. Assign operators (=,+=,-=,etc) should be extracted
		void eval_operators_expression(
				  EvalData *eval_data
				, std::vector<TokenNode> * expression_tokens
				, std::vector<EvalInstruction *> *instructions
				, int idx_start
				, int idx_end
		){
			EvalInstruction *instruction;
			int 			idx_split=-1;
			TokenNode      *split_node = NULL;
			Operator 	op_split=Operator::OPERATOR_MAX;

			// trivial case (symbol node)
			if(idx_start>=idx_end){
				// concatenate instructions ...
				instructions->insert(
					  instructions->end()
					, expression_tokens->at(idx_start).instructions.begin()
					, expression_tokens->at(idx_start).instructions.end()
				);

				return;
			}


			// get the most preference operator (see eval_data::Operator)...
			for(int i=idx_end; i >= idx_start; i--){

				if( (expression_tokens->at(i).token_type == TokenType::TOKEN_TYPE_OPERATOR)
					&& (expression_tokens->at(i).operator_type < op_split))
				{
					op_split=expression_tokens->at(i).operator_type;
					idx_split=i;
				}
			}

			if(idx_split == -1){
				THROW_RUNTIME_ERROR("Unexpected error");
			}

			// split left/right by operator precedence...
			split_node=&expression_tokens->at(idx_split);

			// perform left side op (can have operators)...
			eval_operators_expression(
				eval_data
				,expression_tokens
				,instructions
				,idx_start
				,idx_split-1
			);

			// perform right side op...
			eval_operators_expression(
				eval_data
				,expression_tokens
				,instructions
				,idx_split+1
				,idx_end
			);


				// push operator byte code...
			instructions->push_back(instruction=new EvalInstruction(
					convert_operator_to_byte_code(split_node->operator_type))
			);


			instruction->instruction_source_info= InstructionSourceInfo(
					eval_data->current_parsing_file
					,split_node->line
					,NULL
			);

		}

		void eval_operators(
				EvalData *eval_data
				, std::vector<TokenNode> * expression_tokens
				, std::vector<EvalInstruction *> *instructions
			){

			std::vector<AssignTokenInformation> assing_tokens;
			int idx_start=0;
			int idx_end=expression_tokens->size()-1;
			std::vector<std::vector<EvalInstruction *>> assign_instructions_post_expression;
			TokenNode * token_ternary_if_else=NULL;

			for(int i=idx_end; i >= 0; i--){
				Operator token_operator = expression_tokens->at(i).operator_type;

				if(
					(OPERATOR_ASSIGN <= token_operator)
								&&
					(token_operator < OPERATOR_ARITHMETIC_ASSIGN_LAST)
				){ // ... save all assignables from operator split
						idx_start=i+1;
						break;
				}

			}

			//--------------------------------------------------------------
			// assign operators, load identifiers first
			for(int i=0; i < idx_start; i+=2){ // starting from assign operator if idx_start > 0
				EvalInstruction *instruction=NULL;
				TokenNode * token_node_symbol = &expression_tokens->at(i);
				TokenNode * token_node_operator = &expression_tokens->at(i+1);
				Operator operator_type=token_node_operator->operator_type;
				int end_idx=expression_tokens->size()-1;

				// should be assign operator...
				if(!(
						token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR
					&&	(operator_type == OPERATOR_ASSIGN
					|| (
						(OPERATOR_ARITHMETIC_ASSIGN_FIRST <= operator_type)
									&&
						(operator_type < OPERATOR_ARITHMETIC_ASSIGN_LAST)
					)))){ // ... save all assignables from operator split
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,token_node_operator->line,"Invalid left-hand side in assignment");
				}

				// should be identifier...
				if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,token_node_symbol->line,"Invalid left-hand side in assignment");
				}


				// load variable...
				instructions->push_back(token_node_symbol->instructions[0]);

				assign_instructions_post_expression.push_back({});

				// if is arithmetic with assign...
				if(operator_type>=OPERATOR_ARITHMETIC_ASSIGN_FIRST && operator_type<OPERATOR_ARITHMETIC_ASSIGN_LAST){
					// ... add arithmetic operator byte code
					assign_instructions_post_expression[i>>1].push_back(instruction=new EvalInstruction(
							convert_operator_to_byte_code(operator_type)
					));



					instruction->instruction_source_info= InstructionSourceInfo(
							eval_data->current_parsing_file
							,token_node_operator->line
							,NULL
					);

					// set pops one to do the operation but keeps the variable to store on the top of stack
					instruction->vm_instruction.properties |= MSK_INSTRUCTION_PROPERTY_POP_ONE;
				}

				//--------------------------------------------------------------
				// assign operators, add store byte code
				assign_instructions_post_expression[i>>1].push_back(instruction=new EvalInstruction(ByteCode::BYTE_CODE_STORE));

				instruction->instruction_source_info= InstructionSourceInfo(
						eval_data->current_parsing_file
						,token_node_operator->line
						,NULL
				);
			}

			//--------------------------------------------------------------

			// process all nodes except last one that is ternary
			if(expression_tokens->at(idx_end).operator_type == Operator::OPERATOR_TERNARY_IF){
				token_ternary_if_else=&expression_tokens->at(idx_end);
				idx_end=idx_end-1;

			}

			// eval expression ...
			eval_operators_expression(
					 eval_data
					, expression_tokens
					, instructions
					, idx_start
					, idx_end
			);

			if(token_ternary_if_else!=NULL){ // process expressions...
				instructions->push_back(new EvalInstruction(BYTE_CODE_JNT));
				instructions->insert(
						instructions->end()
						,token_ternary_if_else->instructions.begin()
						,token_ternary_if_else->instructions.end()
						);
				instructions->push_back(new EvalInstruction(BYTE_CODE_JMP));

				instructions->insert(
						instructions->end()
						,token_ternary_if_else->aux_node->instructions.begin()
						,token_ternary_if_else->aux_node->instructions.end()
						);

			}


			//--------------------------------------------------------------


			for(int i=assign_instructions_post_expression.size()-1; i >=0 ;i--){

				// insert assign instruction...
				for(unsigned j=0; j < assign_instructions_post_expression[i].size() ;j++){
					instructions->push_back(assign_instructions_post_expression[i][j]);
				}
			}
		}

		bool is_end_expression(const char *s){
			 return *s==')' || *s==','||  *s==']'  ||  *s==';' || *s == 0 || *s=='}';
		}

		bool    is_end_expression_or_keyword(const char * s){
			Keyword op=is_keyword(s);
			return is_end_expression(s) || !(op==Keyword::KEYWORD_UNKNOWN || op==Keyword::KEYWORD_NEW);
		}

		char * eval_expression(
				EvalData *eval_data
				,const char *s
				, int & line
				, Scope *scope_info
				, std::vector<EvalInstruction *> 	* instructions
				, std::vector<char> expected_ending_char
				, int level

			){
			// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
			// If this functions finds ';' then the function will generate ast.
			std::vector<TokenNode> expression_tokens;
			PreOperator pre_operator = PreOperator::PRE_OPERATOR_UNKNOWN;
			PrePostSelfOperation pre_self_operation_type=PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN;
			PrePostSelfOperation post_self_operation_type=PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN;
			Operator operator_type = Operator::OPERATOR_UNKNOWN;
			Keyword keyword_type;
			int last_line_ok=0;
			int last_accessor_line=0;
			std::string last_accessor_value="";
			std::string identifier_value="";
			const char *start_expression_str=NULL;
			int start_expression_line=-1;
			int instruction_identifier=ZS_IDX_UNDEFINED;
			char *aux_p=NULL,*test_s=NULL;
			bool new_line_break=false;
			IGNORE_BLANKS(aux_p,eval_data,s,line);

			start_expression_str=aux_p;
			start_expression_line=line;

			int idx_instruction_start_expression=eval_data->current_function->instructions.size();

			if(is_end_expression(aux_p) && *aux_p != ';'){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Unexpected '%c'",*aux_p);
			}

			if(!is_end_expression_or_keyword(aux_p)){

				for(;;){ // it eats identifier/constant operator, etc

					TokenNode 	symbol_token_node
								,operator_token_node;
					keyword_type=Keyword::KEYWORD_UNKNOWN;

					if((pre_self_operation_type=is_pre_post_self_operation(aux_p)) != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
						switch(pre_self_operation_type){
							default:
								break;
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_DEC:
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INC:
								aux_p+=strlen(eval_info_pre_post_self_operations[pre_self_operation_type].str);
								break;
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INVALID:
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Unknow pre-operation \"%.2s\"",aux_p);
								break;
						}
					}else{

						// check pre operator (-,+,!)...
						switch(pre_operator=is_pre_operator(aux_p)){
							default:
								break;
							case PreOperator::PRE_OPERATOR_NEG:
							case PreOperator::PRE_OPERATOR_POS:
							case PreOperator::PRE_OPERATOR_NOT:
								aux_p+=strlen(eval_info_pre_operators[pre_operator].str);
								break;
						}
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					keyword_type=is_keyword(aux_p);

					// parenthesis (evals another expression)
					if(*aux_p=='('){ // inner expression (priority)

						if(pre_self_operation_type != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"operation \"%s\" is only allowed on identifiers",eval_info_pre_post_self_operations[pre_self_operation_type].str);
						}

						aux_p=eval_expression(
								eval_data
								,aux_p+1
								, line
								, scope_info
								, &symbol_token_node.instructions
								,std::vector<char>{}
								,level+1);

						if(*aux_p != ')'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ')'");
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						if(pre_operator==PreOperator::PRE_OPERATOR_NEG || pre_operator==PreOperator::PRE_OPERATOR_NOT){
							symbol_token_node.instructions.push_back(new EvalInstruction(ByteCode::BYTE_CODE_NEG));
						}

						symbol_token_node.token_type=TokenType::TOKEN_TYPE_SUBEXPRESSION;
					}
					else{
						last_accessor_line=0;
						last_accessor_value="";
						if(*aux_p=='['){ // std::vector object...
							aux_p=eval_object_vector(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node.instructions
								,level
							);
							symbol_token_node.token_type = TokenType::TOKEN_TYPE_VECTOR;
						}else if(*aux_p=='{'){ // struct object ...

							aux_p=eval_object(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node.instructions
								,level
							);

							symbol_token_node.token_type = TokenType::TOKEN_TYPE_DICTIONARY;

						}else if(keyword_type == Keyword::KEYWORD_NEW){

							aux_p=eval_object_new(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node.instructions
								,level
							);

							symbol_token_node.token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

						}else if(keyword_type == Keyword::KEYWORD_FUNCTION){ // can be after instanceof or a function object..

							aux_p=eval_object_function(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node
							);

							symbol_token_node.token_type = TokenType::TOKEN_TYPE_FUNCTION_OBJECT;
						}
						else { // symbol
							symbol_token_node.pre_operator=pre_operator;
							last_accessor_line=line;
							aux_p = eval_symbol(eval_data
									,aux_p
									,line
									,&symbol_token_node
									,pre_operator
									,pre_self_operation_type
							);

							last_accessor_value=symbol_token_node.value;
						}

						last_line_ok=line;

						IGNORE_BLANKS(test_s,eval_data,aux_p,line);

						// check valid access punctuator...
						if(is_access_punctuator(test_s) || symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

							aux_p=test_s;

							char n_params=0;

							//is_first_access=true;
							LinkSymbolFirstAccess *link_symbol_first_access=NULL;

							// add info to solve symbols first access (we need to put here because we have to know n params if function related)
							symbol_token_node.instructions[0]->link_symbol_first_access=LinkSymbolFirstAccess(
									eval_data->current_function->script_function->idx_script_function
									,scope_info
									,symbol_token_node.value
									,NO_PARAMS_SYMBOL_ONLY // only if first access is a function...
							);

							link_symbol_first_access=&symbol_token_node.instructions[0]->link_symbol_first_access;

							// add info to add as symbol value ...
							symbol_token_node.instructions[0]->instruction_source_info = InstructionSourceInfo(
								eval_data->current_parsing_file
								,line
								,get_compiled_symbol(eval_data,symbol_token_node.value)
							);

							// eval accessor element (supose that was a preinsert a load instruction for identifier )...
							if(is_access_punctuator(aux_p)){
								do{
									ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
									std::string accessor_value="";
									bool this_symbol_access=false;
									bool vector_access=false;
									EvalInstruction *instruction_token=NULL;

									aux_p=test_s;

									switch(*aux_p){
									case '(': // is a function call

										n_params=0;

										if(link_symbol_first_access !=NULL){
											link_symbol_first_access->n_params=0;
										}

										IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

										while(*aux_p != ')'){
											//do{
												// check if ref identifier...
											if(n_params>0){
												if(*aux_p != ','){
													THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ','");
												}

												aux_p++;
											}

											aux_p = eval_expression(
													eval_data
													,aux_p
													,line
													,scope_info
													,&symbol_token_node.instructions
													,std::vector<char>{}
													,level+1
											);

											if(link_symbol_first_access != NULL){
												link_symbol_first_access->n_params++;
											}
											n_params++;
										}

										byte_code=ByteCode::BYTE_CODE_CALL;
										aux_p++;
										break;
									case '[': // std::vector access
										aux_p = eval_expression(
												eval_data
												,aux_p+1
												,line
												,scope_info
												,&symbol_token_node.instructions
												,std::vector<char>{}
												,level+1
										);

										if(*aux_p != ']'){
											THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ']'");
										}
										aux_p++;
										vector_access=true;
										byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE;
										break;
									case '.': // member access
										IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
										accessor_value="";
										last_accessor_line=line;
										while(!is_end_symbol_token(aux_p)){ // get name...
											accessor_value += (*aux_p++);
										}

										check_identifier_name_expression_ok(eval_data,accessor_value,line);

										if(accessor_value == SYMBOL_VALUE_THIS){
											THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"this cannot allowed as member name");
										}

										if(link_symbol_first_access !=NULL){ // check first symbol at first...
											if(symbol_token_node.value == SYMBOL_VALUE_THIS){ // if first symbol was this then the symbol

												instruction_token= symbol_token_node.instructions[0];// get the first instruction....
												// replace symbol
												//instruction_token->vm_instruction.value_op1=LoadType::LOAD_TYPE_VARIABLE;
												instruction_token->vm_instruction.value_op2=ZS_IDX_UNDEFINED;

												// override symbol "this" by symbol to search (in properties already has access scope)...
												symbol_token_node.instructions[0]->instruction_source_info.str_symbol =get_compiled_symbol(eval_data,accessor_value);
											}
										}

										byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE;
										break;
									}

									if(instruction_token==NULL){
										instruction_token=new EvalInstruction(byte_code);
										symbol_token_node.instructions.push_back(instruction_token);

										// generate source info in case accessor load...
										if(byte_code==ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE){

											// mark as accessor
											//instruction_token->vm_instruction.value_op1=LoadType::LOAD_TYPE_VARIABLE;

											if(vector_access){
												instruction_token->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR;
											}else{
												instruction_token->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD;
											}

											instruction_token->instruction_source_info= InstructionSourceInfo(
												eval_data->current_parsing_file
												,line
												,get_compiled_symbol(eval_data,accessor_value)
											);
										}

										if(byte_code == ByteCode::BYTE_CODE_CALL){
											// save total parameters on this call
											instruction_token->vm_instruction.value_op1=n_params;

											// also insert source file/line/symbol info to get info of this call...
											instruction_token->instruction_source_info= InstructionSourceInfo(
												eval_data->current_parsing_file
												,last_accessor_line
												,get_compiled_symbol(eval_data,last_accessor_value)
											);
										}
									}


									//is_first_access=false; // not first access anymore...
									link_symbol_first_access=NULL;
									last_accessor_value=accessor_value;

									test_s=aux_p;

								}while(is_access_punctuator(aux_p));
							}else{
								if(symbol_token_node.value==SYMBOL_VALUE_THIS){
									if(eval_data->current_function->script_function->idx_class == IDX_BUILTIN_TYPE_CLASS_MAIN){
										THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"\"this\" only can be used within a class");
									}

									symbol_token_node.instructions[0]->instruction_source_info= InstructionSourceInfo(
										eval_data->current_parsing_file
										,last_accessor_line
										,get_compiled_symbol(eval_data,SYMBOL_VALUE_THIS)
									);

									//symbol_token_node.instructions[0]->vm_instruction.value_op1=LoadType::LOAD_TYPE_VARIABLE;
									symbol_token_node.instructions[0]->vm_instruction.value_op2=ZS_IDX_INSTRUCTION_OP2_THIS;
									symbol_token_node.instructions[0]->vm_instruction.properties=0;
								}
							}

							if((post_self_operation_type=is_pre_post_self_operation(aux_p))!= PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
								aux_p+=strlen(eval_info_pre_post_self_operations[post_self_operation_type].str);
							}

							if(pre_self_operation_type != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
							&& post_self_operation_type != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Cannot perform post and pre operations on identifier at same time");
							}

							unsigned last_instruction=symbol_token_node.instructions.size()-1;

							// pre/post operator...
							switch(pre_self_operation_type){
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INC:
								symbol_token_node.instructions[last_instruction]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_PRE_INC;
								break;
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_DEC:
								symbol_token_node.instructions[last_instruction]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_PRE_DEC;
								break;
							}

							switch(post_self_operation_type){
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INC:
								symbol_token_node.instructions[last_instruction]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_POST_INC;
								break;
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_DEC:
								symbol_token_node.instructions[last_instruction]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_POST_DEC;
								break;
							}
						}
					}

					// push symbol
					expression_tokens.push_back(symbol_token_node);

					new_line_break=false;
					// cases that can ending and expression
					if(    *aux_p == '\n'
						|| *aux_p == ' '
						|| *aux_p == '\r' // compatible windows format
						|| *aux_p == '\t'
						|| is_comment_single_line(aux_p)
						|| is_comment_block_start(aux_p)){
						if(strchr(aux_p,'\n')!=NULL){ // new line is like ';'
							new_line_break=true;
						}

					}
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					operator_type=is_operator(aux_p);

					if(is_end_expression_or_keyword(aux_p) || ( new_line_break && operator_type==Operator::OPERATOR_UNKNOWN)){ // if not operator and carry return found is behaves as end expression
						break;
					}

					if(operator_type==Operator::OPERATOR_UNKNOWN){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected operator");
					}

					if(operator_type==Operator::OPERATOR_TERNARY_ELSE){ // it will processed on the parent expression
						break;
					}


					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_operators[operator_type].str),line);

					operator_token_node.line=line;
					operator_token_node.operator_type=operator_type;
					operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

					// push operator token
					expression_tokens.push_back(operator_token_node);

					if(operator_type == Operator::OPERATOR_TERNARY_IF){
						operator_token_node.aux_node = new TokenNode();
						//expression_tokens.push_back(operator_token_node);

						aux_p=eval_expression(
								eval_data
								,aux_p
								, line
								, scope_info
								, &operator_token_node.instructions
								,std::vector<char>{}
								,level+1);

						if(*aux_p != ':'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ':' on ternary expression");
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						operator_token_node.aux_node->line=line;
						operator_token_node.aux_node->operator_type=Operator::OPERATOR_TERNARY_ELSE;
						operator_token_node.aux_node->token_type=TokenType::TOKEN_TYPE_OPERATOR;


						aux_p=eval_expression(
							eval_data
							,aux_p
							, line
							, scope_info
							, &operator_token_node.aux_node->instructions
							,std::vector<char>{}
							,level+1
						);

						break; // ternary ends expression


					}
				}
			}

			if(expected_ending_char.size() > 0) { // throw error...
				std::string expected_ending_str="Expected ";
				bool found=false;

				for(unsigned i=0; i < expected_ending_char.size() && found==false; i++){

					if(*aux_p!=expected_ending_char[i]){
						if(i>0){
							expected_ending_str+=",";
						}
						expected_ending_str+=expected_ending_char[i];
					}
					else {
						found=true;
					}
				}

				if(found == false){
					size_t len=aux_p-start_expression_str;
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,start_expression_line,"%s at the end of expression %10s...",expected_ending_str.c_str(),zs_strutils::substring(start_expression_str,0,len).c_str());
				}
			}

			if(aux_p==0){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Unexpected end of file");
			}


			if(expression_tokens.size()>0){
				eval_operators(
					eval_data
					,&expression_tokens
					,instructions
				);

				if(level == 0 && (int)eval_data->current_function->instructions.size()>idx_instruction_start_expression){ // set instruction as start statment...
					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+idx_instruction_start_expression,
							new EvalInstruction(ByteCode::BYTE_CODE_RESET_STACK)
					);
					//eval_data->current_function->instructions[idx_instruction_start_expression]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_START_EXPRESSION;
				}
			}

			// last character is a separator so it return increments by 1
			return aux_p;
		}
	}
}
