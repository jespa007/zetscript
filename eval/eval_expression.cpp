namespace zetscript{

	namespace eval{

		ByteCode convert_operator_to_byte_code(Operator op){

			switch(op){
			default:
				break;
			case Operator::OPERATOR_ADD:
				return ByteCode::BYTE_CODE_ADD;
			case Operator::OPERATOR_MUL:
				return ByteCode::BYTE_CODE_MUL;
			case Operator::OPERATOR_DIV:
				return ByteCode::BYTE_CODE_DIV;
			case Operator::OPERATOR_MOD:
				return ByteCode::BYTE_CODE_MOD;
			case Operator::OPERATOR_ASSIGN:
				return ByteCode::BYTE_CODE_STORE;
			case Operator::OPERATOR_XOR:
				return ByteCode::BYTE_CODE_XOR;
			case Operator::OPERATOR_BINARY_AND:
				return ByteCode::BYTE_CODE_AND;
			case Operator::OPERATOR_OR:
				return ByteCode::BYTE_CODE_OR;
			case Operator::OPERATOR_SHIFT_LEFT:
				return ByteCode::BYTE_CODE_SHL;
			case Operator::OPERATOR_SHIFT_RIGHT:
				return ByteCode::BYTE_CODE_SHR;
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
		char * eval_symbol(EvalData *eval_data,const char *start_word, int line,TokenNode * token_node, PrePostSelfOperation pre_self_operation){
			// PRE:
			unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
			LoadType load_type=LOAD_TYPE_NOT_DEFINED;
			bool is_constant_number=false, is_constant_boolean=false;

			//unsigned int scope_type=0;
			void *obj=NULL,*get_obj=NULL,*const_obj=NULL;
			char *aux=(char *)start_word;
			std::string str_value="";
			 bool error=false;
			 token_node->token_type = TokenType::TOKEN_TYPE_UNKNOWN;
			 bool is_constant_string=false;
			 unsigned short instruction_properties=0;


			 if((aux=parse_literal_number(
					 eval_data
					 ,start_word
					 ,line
					 ,str_value
			))==NULL){ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

				 aux=(char *)start_word;
				 // try eval identifier, boolean, std::string ...
				 token_node->token_type = TokenType::TOKEN_TYPE_LITERAL;

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
					 load_type=LOAD_TYPE_CONSTANT;

					 std::string key_value="\""+str_value+"\"";

					if((get_obj = eval_data->zs->getRegisteredConstantValue(key_value))!=NULL){
						obj = get_obj;
					}else{
						ScriptVarString *s=new ScriptVarString(eval_data->zs,str_value);
						obj=eval_data->zs->registerConstantValue(key_value,NULL,type);
						((StackElement *)obj)->stk_value=((void *)(s->str_value.c_str()));
						((StackElement *)obj)->var_ref=s;
					 }
				}
				 // add load std::string constant
				// compile constant ...
			 }else{
				 token_node->token_type = TokenType::TOKEN_TYPE_LITERAL;
			 }

			 std::string str_number_value=str_value,str_boolean_value=str_value;

			// if preoperator type neg the value itself and reset token pre operator...
			/*if(token_node->pre_operator == PreOperator::PRE_OPERATOR_NEG){
				str_number_value="-"+str_value;
			}

			// if preoperator type neg the value itself and reset token pre operator...
			if(token_node->pre_operator == PreOperator::PRE_OPERATOR_NOT){
				str_boolean_value="!"+str_value;
				token_node->pre_operator =PreOperator::PRE_OPERATOR_UNKNOWN;
			}*/

			 if(!is_constant_string){
				 // try parse value...
				if(str_value=="null"){ // null literal
					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;
					load_type=LOAD_TYPE_NULL;
					obj=NULL;//ScriptVar::NullSymbol;

				}else if(str_value=="undefined"){ // undefined literal
						type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
						load_type=LOAD_TYPE_UNDEFINED;
						obj=NULL;// ScriptVar::UndefinedSymbol;
				}else if((const_obj=zs_strutils::parse_int(str_number_value))!=NULL){ // int literal
					int value = *((int *)const_obj);
					delete (int *)const_obj;
					load_type=LOAD_TYPE_CONSTANT;
					obj=eval_data->zs->registerConstantValue(str_number_value,value);
					is_constant_number=true;
				}
				else if((const_obj=zs_strutils::parse_float(str_number_value))!=NULL){ // float literal
					float value = *((float *)const_obj);
					delete (float *)const_obj;
					void *value_ptr;
					memcpy(&value_ptr,&value,sizeof(float));

					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
					load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(str_number_value))!=NULL){
						obj = get_obj;
					}else{
						obj=eval_data->zs->registerConstantValue(str_number_value,value_ptr,type);
					}
					is_constant_number=true;
				}
				else if((const_obj=zs_strutils::parse_bool(str_boolean_value))!=NULL){ // bool literal

					bool value = *((bool *)const_obj);
					delete (bool *)const_obj;

					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;
					load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(str_boolean_value))!=NULL){
						obj = get_obj;
					}else{
						obj=eval_data->zs->registerConstantValue(str_boolean_value,(void *)value,type);
					}
					is_constant_boolean=true;
				}else{ // it should be an identifier token  ...
					token_node->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
					intptr_t idx_local_var=	eval_data->current_function->script_function->existArgumentName(str_value);

					if(idx_local_var!=ZS_IDX_UNDEFINED){ // is arg...
						load_type=LOAD_TYPE_ARGUMENT;
						obj=(void *)idx_local_var;
					}else if(str_value == "super"){
						instruction_properties=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER;
					}else if(str_value == "this"){
						instruction_properties=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS;
					}else if((get_obj = eval_data->zs->getRegisteredConstantValue(str_value)) != NULL){  // check if symbol is constant ...
						obj=get_obj;
						load_type=LOAD_TYPE_CONSTANT;
					}else{
						// should be an identifier...
						check_identifier_name_expression_ok(
							eval_data
							,str_value
							,line
						);

						// add global var on vm if we are in main function
						/*if(eval_data->current_function->script_function->symbol.scope == MAIN_SCOPE(eval_data)){
							VirtualMachine *vm=eval_data->zs->getVirtualMachine();
							vm->addGlobalVar({});
						}*/

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
					new EvalInstruction(ByteCode::BYTE_CODE_LOAD
							,load_type
							,(intptr_t)obj
							,instruction_properties
					));

			return aux;
			// POST: token as literal or identifier
		}

		void make_operator_precedence(EvalData *eval_data,std::vector<TokenNode> * expression_tokens,std::vector<EvalInstruction *> *instructions, int idx_start,int idx_end){

			Operator 	op_split=Operator::OPERATOR_MAX;
			EvalInstruction *instruction;
			int 			idx_split=-1;
			TokenNode      *split_node;

			// trivial case (no operator left, append byte code )...
			if(idx_start>=idx_end){

				// concatenate instructions ...
				instructions->insert(
						  instructions->end()
						, expression_tokens->at(idx_start).instructions.begin()
						, expression_tokens->at(idx_start).instructions.end() );

				return;
			}

			// search for the most priority operator...
			for(int i=idx_start; i < idx_end; i++){

				if( (expression_tokens->at(i).token_type == TokenType::TOKEN_TYPE_OPERATOR)
					&& (expression_tokens->at(i).operator_type < op_split))
				{
					idx_split=i;
					op_split=expression_tokens->at(i).operator_type;
				}
			}

			if(idx_split == -1){
				THROW_RUNTIME_ERROR("Unexpected error");
			}

			// split left/right by operator precedence...
			split_node=&expression_tokens->at(idx_split);

			// perform left side op...
			make_operator_precedence(eval_data,expression_tokens,instructions,idx_start,idx_split-1);

			// perform right side op...
			make_operator_precedence(eval_data,expression_tokens,instructions,idx_split+1,idx_end);

			// push operator byte code...
			instructions->push_back(instruction=new EvalInstruction(convert_operator_to_byte_code(split_node->operator_type)));
			instruction->instruction_source_info= InstructionSourceInfo(
					eval_data->current_parsing_file
					,split_node->line
					,NULL
			);

		}

		char * eval_expression(EvalData *eval_data,const char *s, int & line, Scope *scope_info, std::vector<EvalInstruction *> 	* instructions){
			// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
			// If this functions finds ';' then the function will generate ast.

			std::vector<TokenNode> expression_tokens;
			PreOperator pre_operator = PreOperator::PRE_OPERATOR_UNKNOWN;
			PrePostSelfOperation pre_self_operation_type=PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN;
			PrePostSelfOperation post_self_operation_type=PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN;
			Operator operator_type = Operator::OPERATOR_UNKNOWN;
			Keyword keyword_type;
			int last_line_ok=0;

			bool is_first_access=false;
			//int instruction_first_access=-1;
			int instruction_identifier=ZS_IDX_UNDEFINED;
			unsigned char params=0;

			//PASTNode ast_node_to_be_evaluated=NULL;
			char *aux_p=NULL;
			IGNORE_BLANKS(aux_p,eval_data,s,line);

			while(!is_end_expression(aux_p)){

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
					}
				}else{

					// check pre operator (-,+,!)...
					switch(pre_operator=is_pre_operator_type(aux_p)){
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

					//std::vector<EvalInstruction *> 	instruction_inner;
					aux_p=eval_expression(eval_data,aux_p+1, line, scope_info, &symbol_token_node.instructions);

					if(*aux_p != ')'){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ')'");
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					// concatenate instruction ...
					/*symbol_token_node.instructions.insert(
							symbol_token_node.instructions.end()
							, instruction_inner.begin()
							, instruction_inner.end()
					);*/

					if(pre_operator==PreOperator::PRE_OPERATOR_NEG || pre_operator==PreOperator::PRE_OPERATOR_NOT){
						symbol_token_node.instructions.push_back(new EvalInstruction(ByteCode::BYTE_CODE_NEG));
					}
					
					symbol_token_node.token_type=TokenType::TOKEN_TYPE_SUBEXPRESSION;
				}
				else{
					if(*aux_p=='['){ // std::vector object...
						aux_p=eval_object_vector(
							eval_data
							,aux_p
							,line
							,scope_info
							,&symbol_token_node.instructions
						);
						symbol_token_node.token_type = TokenType::TOKEN_TYPE_VECTOR;
					}else if(*aux_p=='{'){ // struct object ...

						aux_p=eval_object_dictionary(
							eval_data
							,aux_p
							,line
							,scope_info
							,&symbol_token_node.instructions
						);

						symbol_token_node.token_type = TokenType::TOKEN_TYPE_DICTIONARY;
					}else if(keyword_type == Keyword::KEYWORD_NEW){

						aux_p=eval_object_new(
							eval_data
							,aux_p
							,line
							,scope_info
							,&symbol_token_node.instructions
						);

						symbol_token_node.token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

					}else if(keyword_type == Keyword::KEYWORD_FUNCTION){ // can be after instanceof or a function object..

						aux_p=eval_object_function(
							eval_data
							,aux_p
							,line
							,scope_info
							,&symbol_token_node.instructions
						);

						symbol_token_node.token_type = TokenType::TOKEN_TYPE_FUNCTION_OBJECT;
					}
					else {
						symbol_token_node.pre_operator=pre_operator;
						aux_p = eval_symbol(eval_data
								,aux_p
								,line
								,&symbol_token_node
								,pre_self_operation_type
							);
					}

					last_line_ok=line;

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if((keyword_type=is_keyword(aux_p))!=KEYWORD_UNKNOWN){ // unexpected key word... programmer forgot ;
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,last_line_ok ,"Expected ';'");
					}

					is_first_access=false;
					params=NO_PARAMS_IS_VARIABLE;

					// check valid access punctuator...
					if(is_access_punctuator(aux_p) || symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

						if(!(      symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER
							  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_FUNCTION_OBJECT)&& *aux_p == '(')// cannot be a number/boolean or std::string and then accessor like . / ( / [
							  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_VECTOR  )&& *aux_p == '[')// inline function object like this: 1+function(a,b){ return a+b;} + 0
							  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_DICTIONARY  )&& *aux_p == '.')// inline struct object like this: 1+{"a":0,"b":1}.a + 0

							))
						{
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Unexpected '%c' after literal",*aux_p);
						}

						is_first_access=true;

						// eval accessor element (supose that was a preinsert a load instruction for identifier )...
						while(is_access_punctuator(aux_p)){
							ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
							std::string accessor_value="";

							switch(*aux_p){
							case '(': // is a function call

								// set info that symbol value is function call (its existence is mandatory in vm)
								symbol_token_node.instructions[
								   symbol_token_node.instructions.size()-1
								]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_FUNCTION_CALL;

								if(is_first_access){
									params=0;
								}
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
								if(*aux_p != ')'){
									do{
										aux_p = eval_expression(
												eval_data
												,aux_p
												,line
												,scope_info
												,&symbol_token_node.instructions
										);

										if(*aux_p != ',' && *aux_p != ')'){
											THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ',' or ')'");
										}

										if(*aux_p == ','){
											IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
										}

										if(is_first_access){
											params++;
										}

									}while(*aux_p != ')');
								}
								//accessor_token.params=params;
								byte_code=ByteCode::BYTE_CODE_CALL;
								is_first_access=false;

								aux_p++;
								break;
							case '[': // std::vector access
								symbol_token_node.instructions[
								   symbol_token_node.instructions.size()-1
								]->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_VECTOR_ACCESS;

								aux_p = eval_expression(
										eval_data
										,aux_p+1
										,line
										,scope_info
										,&symbol_token_node.instructions
								);

								if(*aux_p != ']'){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ']'");
								}
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

								//symbol_token_node.instruction.push_back(Instruction(ByteCode::BYTE_CODE_VGET));
								byte_code=ByteCode::BYTE_CODE_VGET;
								break;
							case '.': // member access
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
								accessor_value="";
								while(!is_end_symbol_token(aux_p)){ // get name...
									accessor_value += (*aux_p++);
								}

								check_identifier_name_expression_ok(eval_data,accessor_value,line);
								byte_code=ByteCode::BYTE_CODE_LOAD;
								break;
							}

							EvalInstruction *instruction_token=new EvalInstruction(byte_code);
							symbol_token_node.instructions.push_back(instruction_token);

							// generate source info in case accessor load...
							if(byte_code==ByteCode::BYTE_CODE_LOAD){

								// mark as accessor
								instruction_token->vm_instruction.value_op1=LoadType::LOAD_TYPE_VARIABLE;
								instruction_token->vm_instruction.properties|=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS;

								symbol_token_node.instructions[symbol_token_node.instructions.size()-1]->instruction_source_info= InstructionSourceInfo(
									eval_data->current_parsing_file
									,line
									,get_compiled_symbol(eval_data,accessor_value)
								);
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
							is_first_access=false; // not first access anymore...
						}

						// add info to solve symbols first access (we need to put here because we have to know n params if function related)
						symbol_token_node.instructions[0]->link_symbol_first_access=LinkSymbolFirstAccess(
								eval_data->current_function->script_function->idx_script_function
								,scope_info
								,symbol_token_node.value
								,params // only if first access is a function...
						);


						// add info to add as symbol value ...
						symbol_token_node.instructions[0]->instruction_source_info = InstructionSourceInfo(
							eval_data->current_parsing_file
							,line
							,get_compiled_symbol(eval_data,symbol_token_node.value)
						);

						if((post_self_operation_type=is_pre_post_self_operation(aux_p))!= PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN){
							aux_p+=strlen(eval_info_pre_post_self_operations[post_self_operation_type].str);
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

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

				expression_tokens.push_back(symbol_token_node);

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(!is_end_expression(aux_p)){ // if not end expression then a operator is expected...
					operator_type=is_operator(aux_p);
					size_t inc_p=0;

					if(operator_type==Operator::OPERATOR_UNKNOWN){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected operator");
					}else {
						// sub operators in vm it has different operations and to have solve this issue we do add of negative values on next operation.
						if(operator_type==Operator::OPERATOR_SUB){
							operator_type=Operator::OPERATOR_ADD;
						}else{
							inc_p=strlen(eval_info_operators[operator_type].str);
						}
					}

					operator_token_node.line=line;
					operator_token_node.operator_type=operator_type;
					operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

					expression_tokens.push_back(operator_token_node);
					aux_p+=inc_p;
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
			}

			if(aux_p==0){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Unexpected end of file");
			}

			// make operator precedence from the AST built before...
			if(expression_tokens.size()>0){
				make_operator_precedence(
					eval_data
					,&expression_tokens
					,instructions
					,0
					,(int)(expression_tokens.size()-1)
				);
			}

			// last character is a separator so it return increments by 1
			return aux_p;
		}
	}
}
