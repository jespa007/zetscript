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

		bool  is_access_punctuator(char *s){
			return *s=='.' || *s=='[' || *s=='(';
		}

		bool  is_access_punctuator_or_static_reference(char *s){
			return  is_access_punctuator(s) || (*s==':' && *(s+1)==':');
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
			bool is_constant_number=false, is_constant_boolean=false;
			EvalInstruction *instruction=NULL;
			ByteCode byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_CONSTANT;
			token_node->token_type = TokenType::TOKEN_TYPE_LITERAL;
			intptr_t obj=ZS_IDX_UNDEFINED;
			void *get_obj=NULL,*const_obj=NULL;
			char *aux=(char *)start_word;
			std::string str_value="";
			 bool error=false;
			 bool is_constant_string=false;
			 unsigned short instruction_properties=0;
			 token_node->token_type = TokenType::TOKEN_TYPE_UNKNOWN;

			 if((aux=parse_literal_number(
					 eval_data
					 ,start_word
					 ,line
					 ,str_value
			))!=NULL){ // int/bool/float, etc

				if((const_obj=zs_strutils::parse_int(str_value))!=NULL){ // int literal
					zs_int value = *((zs_int *)const_obj);
					if(pre_operator==PreOperator::PRE_OPERATOR_NEG){
						value=-value;
						str_value="-"+str_value;
					}

					delete (zs_int *)const_obj;
					obj=(zs_int)eval_data->zs->registerConstantValue(str_value,value);
					is_constant_number=true;
				}else if((const_obj=zs_strutils::parse_float(str_value))!=NULL){ // float literal
					float value = *((float *)const_obj);

					if(pre_operator==PreOperator::PRE_OPERATOR_NEG){
						value=-value;
						str_value="-"+str_value;
					}

					delete (float *)const_obj;
					void *value_ptr;
					memcpy(&value_ptr,&value,sizeof(float));

					if((get_obj = eval_data->zs->getRegisteredConstantValue(str_value))!=NULL){
						obj = (zs_int)get_obj;
					}else{
						obj=(zs_int)eval_data->zs->registerConstantValue(str_value,value_ptr,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT);
					}
					is_constant_number=true;
				}
				else{
					EVAL_ERROR(eval_data->current_parsing_file,line ,"Unable to parse literal \"%s\"",str_value.c_str());
				}

			}else{ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

				 if(eval_data->error){
					 return NULL;
				 }

				 aux=(char *)start_word;
				 // try eval identifier, boolean, std::string ...
				char pre=0;
				if(*aux=='\"'){

					do{
						aux++;
					}while(!((*aux=='\"' && *(aux-1)!= '\\')|| *aux==0 || *aux=='\n'));

					if(*aux!='\"'){
						EVAL_ERROR(eval_data->current_parsing_file,line ,"Constant string not ends with '\"'");
					}

					if((start_word+1)<aux){ // copy string without double quotes...
						 zs_strutils::copy_from_ptr_diff(str_value,start_word+1,aux);
					}
					aux++;
					std::string key_value="\""+str_value+"\"";

					if((get_obj = eval_data->zs->getRegisteredConstantValue(key_value))!=NULL){
						obj = (zs_int)get_obj;
					}else{
						ScriptObjectString *s=new ScriptObjectString(eval_data->zs);
						s->str_value=str_value;
						obj=(zs_int)eval_data->zs->registerConstantValue(key_value,s);
					 }

				}else{ // is speacial word or identifier
					bool end=false;
					while(!end){
						pre=*aux;
						str_value += (*aux++);

						if(is_end_symbol_token(aux,pre)){
							end=true;
						}
					}

					if(str_value=="null"){ // null literal
						byte_code=BYTE_CODE_LOAD_TYPE_NULL;
					}else if(str_value=="undefined"){ // undefined literal
						byte_code=BYTE_CODE_LOAD_TYPE_UNDEFINED;
					}else if((const_obj=zs_strutils::parse_bool(str_value))!=NULL){ // bool literal

						bool value = *((bool *)const_obj);
						if(pre_operator==PreOperator::PRE_OPERATOR_NOT){
							value=!value;
							str_value="!"+str_value;
						}

						delete (bool *)const_obj;

						if((get_obj = eval_data->zs->getRegisteredConstantValue(str_value))!=NULL){
							obj = (zs_int)get_obj;
						}else{
							obj=(zs_int)eval_data->zs->registerConstantValue(str_value,(void *)value,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL);
						}
						is_constant_boolean=true;
					}else{ // it should be an identifier token  ...
						token_node->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
						byte_code = ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE;

						if(str_value == SYMBOL_VALUE_THIS || str_value == SYMBOL_VALUE_SUPER){

							if(str_value == SYMBOL_VALUE_SUPER){
								char *test=aux;
								IGNORE_BLANKS(test,eval_data,aux,line);
								if(*test != '('){
									EVAL_ERROR(eval_data->current_parsing_file,line ,"\"super\" only allowed as function");
								}
							}

							instruction_properties=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;

						}else if((get_obj = eval_data->zs->getRegisteredConstantValue(str_value)) != NULL){  // check if symbol is constant ...
							byte_code = ByteCode::BYTE_CODE_LOAD_TYPE_CONSTANT;
							obj=(zs_int)get_obj;
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
			 }
			 // check pre self operation (only allowed on indentifiers...
			 if(
					 pre_self_operation != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
					 && token_node->token_type != TokenType::TOKEN_TYPE_IDENTIFIER
			){
				 EVAL_ERROR(eval_data->current_parsing_file,line ,"operation \"%s\" is only allowed on identifiers ",eval_data_pre_post_self_operations[ pre_self_operation].str);
			 }

			 // check pre operators...
			 if((   token_node->pre_operator == PreOperator::PRE_OPERATOR_NEG
			    || token_node->pre_operator == PreOperator::PRE_OPERATOR_POS)
					&& !(is_constant_number || (token_node->token_type == TokenType::TOKEN_TYPE_IDENTIFIER))
			     )
			 {
				 EVAL_ERROR(eval_data->current_parsing_file,line ,"+/- pre operator not allowed before \"%s\". Only allowed on  numbers or identifiers",str_value.c_str(),eval_data_pre_operators[ token_node->pre_operator].str);
			 }

			 if(
					 (token_node->pre_operator == PreOperator::PRE_OPERATOR_NOT)
				  && !(is_constant_number || is_constant_boolean || (token_node->token_type == TokenType::TOKEN_TYPE_IDENTIFIER))
			     )
			 {
				 EVAL_ERROR(eval_data->current_parsing_file,line ,"! pre operator not allowed before \"%s\". only allowed on constants booleans/numbers or identifiers ",str_value.c_str(),eval_data_pre_operators[ token_node->pre_operator].str);
			 }

			 if((token_node->pre_operator == PreOperator::PRE_OPERATOR_NEG) || (token_node->pre_operator == PreOperator::PRE_OPERATOR_NOT)){
				 instruction_properties|=MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT;
			 }

			token_node->value = str_value;
			token_node->instructions.push_back(
				instruction=new EvalInstruction(
						byte_code
						,ZS_IDX_UNDEFINED
						,obj
						,instruction_properties
			));

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

		char * eval_operators(
				EvalData *eval_data
				,const char *s
				, int & line
				, Scope *scope_info
				, std::vector<EvalInstruction *> *instructions
				, int level
				, std::vector<TokenNode> * expression_tokens

			){

			char *aux_p=(char *)s;
			std::vector<AssignTokenInformation> assing_tokens;
			int idx_start=0;
			int idx_end=(int)(expression_tokens->size()-1);
			std::vector<std::vector<EvalInstruction *>> assign_instructions_post_expression;

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
				int end_idx=(int)(expression_tokens->size()-1);

				// should be assign operator...
				if(!(
						token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR
					&&	(operator_type == OPERATOR_ASSIGN
					|| (
						(OPERATOR_ARITHMETIC_ASSIGN_FIRST <= operator_type)
									&&
						(operator_type < OPERATOR_ARITHMETIC_ASSIGN_LAST)
					)))){ // ... save all assignables from operator split
					EVAL_ERROR(eval_data->current_parsing_file,token_node_operator->line,"Operation \"%s\" in assignment is not allowed",eval_data_operators[operator_type].str);
				}

				// should be identifier...
				if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					EVAL_ERROR(eval_data->current_parsing_file,token_node_symbol->line,"Assign a literal \"%s\" is not allowed",token_node_symbol->value.c_str());
				}

				// load variable and accessors...
				for(unsigned i=0;i<token_node_symbol->instructions.size();i++){
					EvalInstruction *ei_load_assign_instruction=token_node_symbol->instructions[i];
					if(ei_load_assign_instruction->vm_instruction.byte_code ==  BYTE_CODE_CALL){
						EVAL_ERROR(
								eval_data->current_parsing_file
								,ei_load_assign_instruction->instruction_source_info.line
								,"Calling a function in an assignment is not allowed");
					}
					instructions->push_back(token_node_symbol->instructions[i]);
				}

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
			eval_operators_expression(
				 eval_data
				, expression_tokens
				, instructions
				, idx_start
				, idx_end
			);

			if(*aux_p == '?'){ // ternary
				EvalInstruction *ei_ternary_if_jnt=NULL;
				EvalInstruction *ei_ternary_else_jmp=NULL;

				// insert JNT
				instructions->push_back(ei_ternary_if_jnt=new EvalInstruction(BYTE_CODE_JNT));

				aux_p=eval_expression(
						eval_data
						,aux_p+1
						, line
						, scope_info
						, instructions
						,std::vector<char>{}
						,level+1);

				if(*aux_p != ':'){
					EVAL_ERROR(eval_data->current_parsing_file,line ,"Expected ':' on ternary expression");
				}

				instructions->push_back(ei_ternary_else_jmp=new EvalInstruction(BYTE_CODE_JMP));

				ei_ternary_if_jnt->vm_instruction.value_op2=instructions->size();

				aux_p=eval_expression(
					eval_data
					,aux_p+1
					, line
					, scope_info
					, instructions
					,std::vector<char>{}
					,level+1
				);

				ei_ternary_else_jmp->vm_instruction.value_op2=instructions->size()+(level>=1?2:0); // adds +2

			}
			//--------------------------------------------------------------

			// ... finally save store operators
			for(int i=(int)(assign_instructions_post_expression.size()-1); i >=0 ;i--){

				// insert assign instruction...
				for(unsigned j=0; j < assign_instructions_post_expression[i].size() ;j++){
					instructions->push_back(assign_instructions_post_expression[i][j]);
				}
			}
			return aux_p;
		}

		bool is_end_expression(const char *s){
			 return *s==')' || *s==','||  *s==']'  ||  *s==';' || *s == 0 || *s=='}';
		}

		int    is_end_expression_or_keyword(const char * s,EvalData *eval_data,int line){
			Keyword op=is_keyword(s);
			bool is_anonymouse_function=false;

			if(op==Keyword::KEYWORD_FUNCTION){
				char *check=NULL;
				IGNORE_BLANKS(check,eval_data,s+strlen(eval_data_keywords[op].str),line);

				is_anonymouse_function=*check=='(';
			}

			return !is_anonymouse_function && (is_end_expression(s) || (op<Keyword::KEYWORDS_WITHIN_EXPRESSIONS && op !=Keyword::KEYWORD_UNKNOWN));
		}

		bool is_end_expression_with_cr(const char * s){
			return false;
		}

		char * eval_expression(
				EvalData *eval_data
				,const char *s
				, int & line
				, Scope *scope_info
				, std::vector<EvalInstruction *> 	* instructions
				, std::vector<char> expected_ending_char
				, int level
				, bool reset_stack

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
			std::string identifier_value="",
						accessor_value="",
						last_accessor_value="";
			std::vector<std::string >static_access;
			std::string static_access_name;

			const char *start_expression_str=NULL;
			int start_expression_line=-1;
			int instruction_identifier=ZS_IDX_UNDEFINED;
			char *aux_p=NULL,*test_s=NULL;
			bool new_line_break=false;
			char *test_ignore_blanks=NULL,*test_char_carry_return=NULL;
			TokenNode symbol_token_node;

			IGNORE_BLANKS(aux_p,eval_data,s,line);

			start_expression_str=aux_p;
			start_expression_line=line;

			int idx_instruction_start_expression=(int)(eval_data->current_function->instructions.size());

			if(is_end_expression(aux_p) && *aux_p != ';'){
				EVAL_ERROR(eval_data->current_parsing_file,line ,"Unexpected '%c'",*aux_p);
			}

			if(!is_end_expression_or_keyword(aux_p,eval_data,line)){

				for(;;){ // it eats identifier/constant operator, etc

					TokenNode 	operator_token_node;

					keyword_type=Keyword::KEYWORD_UNKNOWN;

					if((pre_self_operation_type=is_pre_post_self_operation(aux_p)) != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
						switch(pre_self_operation_type){
							default:
								break;
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_DEC:
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INC:
								aux_p+=strlen(eval_data_pre_post_self_operations[pre_self_operation_type].str);
								break;
							case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INVALID:
								EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Unknow pre-operation \"%.2s\"",aux_p);
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
								aux_p+=strlen(eval_data_pre_operators[pre_operator].str);
								break;
						}
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					keyword_type=is_keyword(aux_p);

					// parenthesis (evals another expression)
					if(*aux_p=='('){ // inner expression (priority)

						if(pre_self_operation_type != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
							EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"operation \"%s\" is only allowed on identifiers",eval_data_pre_post_self_operations[pre_self_operation_type].str);
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
							EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Expected ')'");
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
							if((aux_p=eval_object_vector(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node.instructions
								,level
							))==NULL){
								goto error_expression;
							}
							symbol_token_node.token_type = TokenType::TOKEN_TYPE_VECTOR;
						}else if(*aux_p=='{'){ // struct object ...

							if((aux_p=eval_object(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node.instructions
								,level
							))==NULL){
								goto error_expression;
							}

							symbol_token_node.token_type = TokenType::TOKEN_TYPE_OBJECT;

						}else if(keyword_type == Keyword::KEYWORD_NEW){

							if((aux_p=eval_object_new(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node.instructions
								,level
							))==NULL){
								goto error_expression;
							}

							symbol_token_node.token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

						}else if(keyword_type == Keyword::KEYWORD_FUNCTION){ // can be after instanceof or a function object..

							if((aux_p=eval_object_function(
								eval_data
								,aux_p
								,line
								,scope_info
								,&symbol_token_node
							))==NULL){
								goto error_expression;
							}

							symbol_token_node.token_type = TokenType::TOKEN_TYPE_FUNCTION_OBJECT;
						}
						else { // symbol
							symbol_token_node.pre_operator=pre_operator;

							last_accessor_line=line;
							if((aux_p = eval_symbol(eval_data
									,aux_p
									,line
									,&symbol_token_node
									,pre_operator
									,pre_self_operation_type
							)) == NULL){
								goto error_expression;
							}

							last_accessor_value=symbol_token_node.value;

							if(symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){
								symbol_token_node.instructions[0]->symbol.scope=scope_info;
								symbol_token_node.instructions[0]->symbol.name=symbol_token_node.value;

								// add info to add as symbol value ...
								symbol_token_node.instructions[0]->instruction_source_info = InstructionSourceInfo(
									eval_data->current_parsing_file
									,line
									,get_mapped_name(eval_data,symbol_token_node.value)
								);
							}
						}
					}

					last_line_ok=line;

					// ignore blanks and test
					IGNORE_BLANKS(test_s,eval_data,aux_p,last_line_ok);

					// eval accessor element (supose that was a preinsert a load instruction for identifier )...
					if(is_access_punctuator_or_static_reference(aux_p)){

						line=last_line_ok;
						aux_p=test_s;
						char n_params=0;
						bool first_access=true;
						//bool last_access_static=false;
						static_access.clear();
						bool is_static_access=false;
						bool is_next_static=false;

						do{
							ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
							accessor_value="";
							bool this_symbol_access=false;
							bool vector_access=false;
							EvalInstruction *instruction_token=NULL;


							aux_p=test_s;

							switch(*aux_p){
							case '(': // is a function call

								n_params=0;
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

								while(*aux_p != ')'){
									// check if ref identifier...
									if(n_params>0){
										if(*aux_p != ','){
											EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Expected ','");
										}
										aux_p++;
									}

									if((aux_p = eval_expression(
											eval_data
											,aux_p
											,line
											,scope_info
											,&symbol_token_node.instructions
											,std::vector<char>{}
											,level+1
									))==NULL){
										goto error_expression;
									}
									n_params++;
								}
								byte_code=ByteCode::BYTE_CODE_CALL;
								is_static_access=false;
								aux_p++;
								break;
							case '[': // std::vector access
								if((aux_p = eval_expression(
										eval_data
										,aux_p+1
										,line
										,scope_info
										,&symbol_token_node.instructions
										,std::vector<char>{}
										,level+1
								))==NULL){
									goto error_expression;
								}

								if(*aux_p != ']'){
									EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Expected ']'");
								}

								aux_p++;
								vector_access=true;
								is_static_access=false;
								byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE;
								break;
							case ':':
							case '.': // member/static access
								if(*aux_p != '.'){
									if( *(aux_p+1) == ':'){ // static access
										// check if first access, else check if last name was also static...
										if(first_access==true){
											// push first access name
											static_access.push_back(symbol_token_node.value);
											is_static_access=true;
											aux_p++;
										}
										else{
											EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Invalid static access");
										}
									}else{
										EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Unexpected char ':'");
									}
								}

								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
								accessor_value="";
								last_accessor_line=line;
								while(!is_end_symbol_token(aux_p)){ // get name...
									accessor_value += (*aux_p++);
								}

								check_identifier_name_expression_ok(eval_data,accessor_value,line);

								if(is_static_access){
									IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
									// check static const var...
									is_static_access = is_access_punctuator_or_static_reference(aux_p) && *aux_p != '.'; // . property ends static
									if(is_static_access == false){ // supose byte code as VARIABLE
										byte_code=BYTE_CODE_LOAD_TYPE_VARIABLE;
									}

									static_access.push_back(accessor_value);
								}else{

									if(accessor_value == SYMBOL_VALUE_THIS){
										EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"\"this\" is not allowed as member name");
									}

									if(first_access){ // check first symbol at first...
										if(symbol_token_node.value == SYMBOL_VALUE_THIS){

											if(eval_data->current_function->script_function->symbol.properties & SYMBOL_PROPERTY_STATIC){
												EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"\"this\" is not allowed in static functions");
											}

											// set symbol name
											instruction_token=symbol_token_node.instructions[0];
											instruction_token->symbol.name=accessor_value;

											// set variable accessor instead of "this"
											instruction_token->instruction_source_info= InstructionSourceInfo(
												eval_data->current_parsing_file
												,line
												,get_mapped_name(eval_data,accessor_value)
											);
										}
									}

									byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE;
								}
								break;
							}

							if(instruction_token==NULL && (is_static_access==false)){


								ScriptClass *script_class_access=NULL;
								Symbol *symbol_static = NULL;//script_class_access->getSymbol();

								if(static_access.size() == 0
									|| byte_code != ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE // the other types it requires an extra op to do call or access vector
								){ // static access
									instruction_token=new EvalInstruction(byte_code);
									symbol_token_node.instructions.push_back(instruction_token);
								}

								if(static_access.size() > 0){

									EvalInstruction *first_instruction_token=symbol_token_node.instructions[0]; // override first instruction

									if(byte_code == ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE){
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
									symbol_token_node.value=static_access_name;
									first_instruction_token->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data,static_access_name);

									if((script_class_access=eval_data->script_class_factory->getScriptClass(static_access[0]))==NULL){
										EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"static access error: class \"%s\" not exist",static_access[0].c_str());
									}

									symbol_static=script_class_access->getSymbol(static_access[1]);

									if(symbol_static != NULL){

										if(byte_code == ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE){ // it should be constant type ...

											// check symbol is native and not function
											if((symbol_static->properties & SYMBOL_PROPERTY_CONST) == 0){
												EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"Symbol \"%s\" is not constant",static_access_name.c_str());
											}

											first_instruction_token->vm_instruction.byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_CONSTANT;
											first_instruction_token->vm_instruction.value_op2=symbol_static->ref_ptr; // global stack element
										}
										else if(byte_code == ByteCode::BYTE_CODE_CALL){

											bool is_static_symbol= symbol_static->properties & SYMBOL_PROPERTY_STATIC;

											if( symbol_static->n_params >= 0){ // is a function
												ScriptFunction *static_script_function = (ScriptFunction *)symbol_static->ref_ptr;
												if(static_script_function->symbol.properties & SYMBOL_PROPERTY_STATIC){
													first_instruction_token->vm_instruction.byte_code=ByteCode::BYTE_CODE_LOAD_TYPE_FUNCTION;
													first_instruction_token->vm_instruction.value_op2=(zs_int)static_script_function;
												}else{
													EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"static access error: \"%s\" is not static. Make sure that there's no this/super defined in body",static_access_name.c_str());
												}
											}else{
												EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"static access error: \"%s\" is not a function",static_access_name.c_str());
											}
										}else{
											EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"static access: Unexpected byte code");
										}
									}else{
										EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"Symbol %s not exist",static_access_name.c_str());
									}

									// clear static symbols to mark end static
									static_access.clear();
								}

								// generate source info in case accessor load...
								if(byte_code==ByteCode::BYTE_CODE_LOAD_TYPE_VARIABLE){
									// mark as accessor
									if(vector_access){
										instruction_token->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR;
									}else if(symbol_static == NULL){
										instruction_token->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD;
									}

									instruction_token->instruction_source_info= InstructionSourceInfo(
										eval_data->current_parsing_file
										,line
										,get_mapped_name(eval_data,symbol_static!=NULL?static_access_name:accessor_value)
									);
								}
								else if(byte_code == ByteCode::BYTE_CODE_CALL){
									// save total parameters on this call
									instruction_token->vm_instruction.value_op1=n_params;

									// also insert source file/line/symbol info to get info of this call...
									instruction_token->instruction_source_info= InstructionSourceInfo(
										eval_data->current_parsing_file
										,last_accessor_line
										,get_mapped_name(eval_data,symbol_static != NULL?static_access_name:accessor_value)
									);
								}

							}
							// not first access anymore...
							first_access=false;
							//last_access_static=is_static_access;
							last_accessor_value=accessor_value;

							test_s=aux_p;
							last_line_ok=line;

						}while(is_access_punctuator_or_static_reference(aux_p));
					}else{

						if(symbol_token_node.value==SYMBOL_VALUE_THIS){
							if(eval_data->current_function->script_function->idx_class == IDX_BUILTIN_TYPE_CLASS_MAIN){
								EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line,"\"this\" only can be used within a class");
							}

							symbol_token_node.instructions[0]->instruction_source_info= InstructionSourceInfo(
								eval_data->current_parsing_file
								,last_accessor_line
								,get_mapped_name(eval_data,SYMBOL_VALUE_THIS)
							);

							symbol_token_node.instructions[0]->vm_instruction.value_op2=ZS_IDX_INSTRUCTION_OP2_THIS;
							symbol_token_node.instructions[0]->vm_instruction.properties=0;
						}
					}

					if(
						((post_self_operation_type=is_pre_post_self_operation(aux_p))!= PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN)
						&& ((post_self_operation_type=is_pre_post_self_operation(aux_p))!= PrePostSelfOperation::PRE_POST_SELF_OPERATION_INVALID)
					){
						aux_p+=strlen(eval_data_pre_post_self_operations[post_self_operation_type].str);
					}

					if(pre_self_operation_type != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
					&& post_self_operation_type != PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
						EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Cannot perform post and pre operations on identifier at same time");
					}

					unsigned last_instruction=(int)(symbol_token_node.instructions.size()-1);

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

					// push symbol
					expression_tokens.push_back(symbol_token_node);
					symbol_token_node.reset();

					new_line_break=false;
					last_line_ok=line;
					IGNORE_BLANKS(test_ignore_blanks,eval_data,aux_p,last_line_ok);

					if((test_char_carry_return=strchr(aux_p,'\n'))!=NULL){ // new line is like ';'
						new_line_break=test_char_carry_return<test_ignore_blanks;
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					operator_type=is_operator(aux_p);

					keyword_type=is_keyword(aux_p);

					if(	is_end_expression(aux_p)
					|| operator_type==Operator::OPERATOR_TERNARY_IF
					|| operator_type==Operator::OPERATOR_TERNARY_ELSE
					|| ( new_line_break && (operator_type==Operator::OPERATOR_UNKNOWN ))){ // if not operator and carry return found is behaves as end expression
						break;
					}

					if(operator_type==Operator::OPERATOR_UNKNOWN){
						switch(post_self_operation_type){
						case PrePostSelfOperation::PRE_POST_SELF_OPERATION_INC:
							EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"invalid post increment");
							break;
						case PrePostSelfOperation::PRE_POST_SELF_OPERATION_DEC:
							EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"invalid post decrement");
							break;
						default:
							EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Expected operator");
							break;
						}

					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_operators[operator_type].str),line);

					operator_token_node.line=line;
					operator_token_node.operator_type=operator_type;
					operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

					// push operator token
					expression_tokens.push_back(operator_token_node);
				}
			}

			if(eval_data->error){
				goto error_expression;
			}

			if(expected_ending_char.size() > 0) { // throw error...
				std::string expected_ending_str="Expected ";
				bool found=false;

				for(unsigned i=0; i < expected_ending_char.size() && found==false; i++){

					if(*aux_p!=expected_ending_char[i]){
						if(i>0){
							i == expected_ending_char.size()-1 ?
									expected_ending_str+=" or ":
									expected_ending_str+=" , ";


						}
						expected_ending_str+=std::string("\"")+expected_ending_char[i]+"\"";
					}
					else {
						found=true;
					}
				}

				if(found == false){
					size_t len=aux_p-start_expression_str;
					EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,start_expression_line,"%s at the end of expression %10s...",expected_ending_str.c_str(),zs_strutils::substring(start_expression_str,0,len).c_str());
				}
			}

			if(aux_p==0){
				EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,line ,"Unexpected end of file");
			}

			// there's an expression
			if(expression_tokens.size()>0){

				if(level == 0 && reset_stack == true){ // set instruction as start statment...
					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+idx_instruction_start_expression,
							new EvalInstruction(ByteCode::BYTE_CODE_RESET_STACK)
					);
				}

				aux_p=eval_operators(
					eval_data
					,aux_p
					,line
					,scope_info
					,instructions
					,level
					,&expression_tokens

				);
			}

			// last character is a separator so it return increments by 1
			return aux_p;

error_expression:

			for(unsigned kk=0;kk<symbol_token_node.instructions.size();kk++){
				delete symbol_token_node.instructions[kk];
			}

			for(unsigned kk=0;kk<expression_tokens.size();kk++){
				for(unsigned jj=0;jj<expression_tokens[kk].instructions.size();jj++){
					delete expression_tokens[kk].instructions[jj];
				}
			}

			return NULL;

		}
	}
}
