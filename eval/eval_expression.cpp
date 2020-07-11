namespace zetscript{

	namespace eval{


		ByteCode convertOperatorTypeToByteCode(OperatorType op){

			switch(op){
			default:
				break;
			case OperatorType::OPERATOR_TYPE_SUB:
				return ByteCode::BYTE_CODE_NEG;
			case OperatorType::OPERATOR_TYPE_ADD:
				return ByteCode::BYTE_CODE_ADD;
			case OperatorType::OPERATOR_TYPE_MUL:
				return ByteCode::BYTE_CODE_MUL;
			case OperatorType::OPERATOR_TYPE_DIV:
				return ByteCode::BYTE_CODE_DIV;
			case OperatorType::OPERATOR_TYPE_MOD:
				return ByteCode::BYTE_CODE_MOD;
			case OperatorType::OPERATOR_TYPE_ASSIGN:
				return ByteCode::BYTE_CODE_STORE;
			case OperatorType::OPERATOR_TYPE_XOR:
				return ByteCode::BYTE_CODE_XOR;
			case OperatorType::OPERATOR_TYPE_BINARY_AND:
				return ByteCode::BYTE_CODE_AND;
			case OperatorType::OPERATOR_TYPE_OR:
				return ByteCode::BYTE_CODE_OR;
			case OperatorType::OPERATOR_TYPE_SHIFT_LEFT:
				return ByteCode::BYTE_CODE_SHL;
			case OperatorType::OPERATOR_TYPE_SHIFT_RIGHT:
				return ByteCode::BYTE_CODE_SHR;
			case OperatorType::OPERATOR_TYPE_LOGIC_AND:
				return ByteCode::BYTE_CODE_LOGIC_AND;
			case OperatorType::OPERATOR_TYPE_LOGIC_OR:
				return ByteCode::BYTE_CODE_LOGIC_OR;
			case OperatorType::OPERATOR_TYPE_LOGIC_EQUAL:
				return ByteCode::BYTE_CODE_EQU;
			case OperatorType::OPERATOR_TYPE_LOGIC_NOT_EQUAL:
				return ByteCode::BYTE_CODE_NOT_EQU;
			case OperatorType::OPERATOR_TYPE_LOGIC_GT:
				return ByteCode::BYTE_CODE_GT;
			case OperatorType::OPERATOR_TYPE_LOGIC_LT:
				return ByteCode::BYTE_CODE_LT;
			case OperatorType::OPERATOR_TYPE_LOGIC_GTE:
				return ByteCode::BYTE_CODE_GTE;
			case OperatorType::OPERATOR_TYPE_LOGIC_LTE:
				return ByteCode::BYTE_CODE_LTE;
			case OperatorType::OPERATOR_TYPE_INSTANCEOF:
				return ByteCode::BYTE_CODE_INSTANCEOF;
			}

			THROW_RUNTIME_ERROR(zs_strutils::format("operator %i not implemented",op));
			return ByteCode::BYTE_CODE_END_FUNCTION;
		}

		std::string * getCompiledSymbol(EvalData *eval_data,const std::string & s){
			if(eval_data->compiled_symbol_name.count(s)==0){
				eval_data->compiled_symbol_name[s]=new std::string (s);
			}
			return eval_data->compiled_symbol_name[s];
		}

		// to std::string utils ...
		char * evalSymbol(EvalData *eval_data,const char *start_word, int line,TokenNode * token_node){
			// PRE:
			unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
			LoadType load_type=LOAD_TYPE_NOT_DEFINED;

			unsigned int scope_type=0;
			void *obj=NULL,*get_obj=NULL,*const_obj=NULL;
			//char *start_str=(char *)start_word;
			char *aux=(char *)start_word;
			std::string v="";
			 //bool is_possible_number=false;
			 //int i=0;
			 bool error=false;
			 //bool start_digit = false;
			 token_node->token_type = TokenType::TOKEN_TYPE_UNKNOWN;
			 //KeywordType kw=isKeywordType(aux);
			 //int start_line = line;
			 bool is_constant_string=false;

			 if((aux=parseLiteralNumber(
					 eval_data
					 ,start_word
					 ,line
					 ,v
					 ,error
			))==NULL){ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

				 if(error){
					 return NULL;
				 }

				 aux=(char *)start_word;
				 // try eval identifier, boolean, std::string ...
				 token_node->token_type = TokenType::TOKEN_TYPE_LITERAL;

				char pre=0;
				if(*aux=='\"'){
					is_constant_string = true;
					aux++;
				}
				while(!isEndSymbolToken(aux,pre)){
					pre=*aux;
					v += (*aux++);
				}

				if(*aux==0){
					return NULL;
				}

				if(*aux=='\"'){
					 if(!is_constant_string){
						 writeError(eval_data->current_parsing_file,line ,"Unexpected '\"'");
						 return NULL;
					 }
					 aux++;

					 type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING;
					 load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(v))!=NULL){
						obj = get_obj;
					}else{

						ScriptVarString *s=new ScriptVarString(eval_data->zs,v);
						obj=eval_data->zs->registerConstantValue(v,NULL,type);
						((StackElement *)obj)->stk_value=((void *)(s->str_value.c_str()));
						((StackElement *)obj)->var_ref=s;
					 }
				}
				 // add load std::string constant
				// compile constant ...
			 }

			 if(!is_constant_string){
				 // try parse value...
				if(v=="null"){
					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;
					load_type=LOAD_TYPE_NULL;
					obj=NULL;//ScriptVar::NullSymbol;

				}else if(v=="undefined"){
						type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
						load_type=LOAD_TYPE_UNDEFINED;
						obj=NULL;// ScriptVar::UndefinedSymbol;
				}else if((const_obj=zs_strutils::parseInteger(v))!=NULL){
					int value = *((int *)const_obj);
					delete (int *)const_obj;
					load_type=LOAD_TYPE_CONSTANT;
					obj=eval_data->zs->registerConstantIntValue(v,value);
				}
				else if((const_obj=zs_strutils::parseFloat(v))!=NULL){
					float value = *((float *)const_obj);
					delete (float *)const_obj;
					void *value_ptr;
					memcpy(&value_ptr,&value,sizeof(float));

					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
					load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(v))!=NULL){
						obj = get_obj;
					}else{
						obj=eval_data->zs->registerConstantValue(v,value_ptr,type);
					}
				}
				else if((const_obj=zs_strutils::parseBoolean(v))!=NULL){

					bool value = *((bool *)const_obj);
					delete (bool *)const_obj;

					type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;
					load_type=LOAD_TYPE_CONSTANT;

					if((get_obj = eval_data->zs->getRegisteredConstantValue(v))!=NULL){
						obj = get_obj;
					}else{
						obj=eval_data->zs->registerConstantValue(v,(void *)value,type);
					}
				}else{ // it should be an identifier token  ...

					token_node->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
					intptr_t idx_local_var=ZS_UNDEFINED_IDX;

					for(unsigned i = 0; i < eval_data->evaluated_function_current->script_function->arg_info.size() && idx_local_var == ZS_UNDEFINED_IDX; i++){

						if(eval_data->evaluated_function_current->script_function->arg_info[i].arg_name == v){
							idx_local_var=i;
						}
					}

					if(idx_local_var!=ZS_UNDEFINED_IDX){ // is arg...
						load_type=LOAD_TYPE_ARGUMENT;
						obj=(void *)idx_local_var;
					}
					else if(v == "super"){
						scope_type=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER;
					}else if(v == "this"){
						scope_type=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS;
					}else if((get_obj = eval_data->zs->getRegisteredConstantValue(v)) != NULL){  // check if symbol is constant ...
						obj=get_obj;
						load_type=LOAD_TYPE_CONSTANT;
					}else{
						// should be an identifier...
						if(!isIdentifierNameExpressionOk(
							eval_data
							,v
							,line
						)){
							return NULL;
						}
					}
				}
			 }
			token_node->value = v;
			token_node->instruction.push_back(EvaluatedInstruction(ByteCode::BYTE_CODE_LOAD,load_type,(intptr_t)obj,scope_type));

			return aux;
			// POST: token as literal or identifier
		}

		bool makeOperatorPrecedence(EvalData *eval_data,std::vector<TokenNode> * expression_tokens,std::vector<EvaluatedInstruction> *instruction, int idx_start,int idx_end, bool & error){

			OperatorType 	op_split=OperatorType::OPERATOR_TYPE_MAX;
			int 			idx_split=-1;
			TokenNode      *split_node;

			// trivial case (no operator left, append byte code )...
			if(idx_start>=idx_end){

				// concatenate instructions ...
				instruction->insert(
						  instruction->end()
						, expression_tokens->at(idx_start).instruction.begin()
						, expression_tokens->at(idx_start).instruction.end() );

				return true;
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
				writeError(eval_data->current_parsing_file,-1,"Unexpected error");
				error =true;
				return false;
			}

			// split left/right by operator precedence...
			split_node=&expression_tokens->at(idx_split);

			bool result= makeOperatorPrecedence(eval_data,expression_tokens,instruction,idx_start,idx_split-1, error) // left branches...
										&&
						makeOperatorPrecedence(eval_data,expression_tokens,instruction,idx_split+1,idx_end,error); // right branches...

			if(result){ // if result was ok, push byte_code...
				instruction->push_back(EvaluatedInstruction(convertOperatorTypeToByteCode(split_node->operator_type)));
			}

			return result;
		}

		char * evalExpression(EvalData *eval_data,const char *s, int & line, Scope *scope_info, std::vector<EvaluatedInstruction> 	* instruction){
			// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
			// If this functions finds ';' then the function will generate ast.

			std::vector<TokenNode> expression_tokens;
			PreOperatorType pre_op = PreOperatorType::PRE_OPERATOR_TYPE_UNKNOWN;
			OperatorType op = OperatorType::OPERATOR_TYPE_UNKNOWN;
			KeywordType kw;
			bool error=false;

			bool is_first_access=false;
			//int instruction_first_access=-1;
			int instruction_identifier=-1;
			unsigned char params=0;

			//PASTNode ast_node_to_be_evaluated=NULL;
			char *aux_p=ignoreBlanks(s,line);

			while(!isEndExpression(aux_p)){

				TokenNode 	symbol_token_node
							,operator_token_node;
				kw=KeywordType::KEYWORD_TYPE_UNKNOWN;

				// check pre operator (-,+,!)...
				switch(pre_op=isPreOperatorType(aux_p)){

					default:
						break;
					case PreOperatorType::PRE_OPERATOR_TYPE_SUB:
					case PreOperatorType::PRE_OPERATOR_TYPE_ADD:
					case PreOperatorType::PRE_OPERATOR_TYPE_NOT:
						aux_p+=strlen(eval_info_operators[pre_op].str);
						break;
				}

				aux_p=ignoreBlanks(aux_p,line);
				kw=isKeywordType(aux_p);

				// parenthesis (evals another expression)
				if(*aux_p=='('){ // inner expression (priority)
					std::vector<EvaluatedInstruction> 	instruction_inner;
					if((aux_p=evalExpression(eval_data,aux_p+1, line, scope_info, &instruction_inner))==NULL){
						return NULL;
					}

					// concatenate instruction ...
					instruction->insert(
										  instruction->end()
										, instruction_inner.begin()
										, instruction_inner.end() );

					if(*aux_p != ')'){
						writeError(eval_data->current_parsing_file,line ,"Expected ')'");
						return NULL;
					}

					aux_p=ignoreBlanks(aux_p+1,line);

					operator_token_node.token_type=TokenType::TOKEN_TYPE_SUBEXPRESSION;

				}
				else{

					IdentityOperatorType identity_pre_operator_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
					IdentityOperatorType identity_post_operator_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;

					if(isOperatorTypeInc(aux_p)){
						identity_pre_operator_type=IDENTITY_OPERATOR_TYPE_INC;
						aux_p=ignoreBlanks(aux_p+2,line);
					}
					else if(isOperatorTypeDec(aux_p)){
						identity_pre_operator_type=IDENTITY_OPERATOR_TYPE_DEC;
						aux_p=ignoreBlanks(aux_p+2,line);
					}

					// pre operator identifier ...

					// first call..
					if(*aux_p=='['){ // std::vector object...

						if((aux_p=evalVectorObject(eval_data,aux_p,line,scope_info,instruction)) == NULL){
							return NULL;
						}
						symbol_token_node.token_type = TokenType::TOKEN_TYPE_VECTOR;

					}else if(*aux_p=='{'){ // struct object ...

						if((aux_p=evalDictionaryObject(eval_data,aux_p,line,scope_info,instruction)) == NULL){
							return NULL;
						}
						symbol_token_node.token_type = TokenType::TOKEN_TYPE_DICTIONARY;
					}else if(kw == KeywordType::KEYWORD_TYPE_NEW){

						if((aux_p=evalNewObject(eval_data,aux_p,line,scope_info,instruction)) == NULL){
							return NULL;
						}
						symbol_token_node.token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

					}else if(kw == KeywordType::KEYWORD_TYPE_FUNCTION){ // can be after instanceof or a function object..
						if((aux_p=evalFunctionObject(eval_data,aux_p,line,scope_info,instruction)) == NULL){
							return NULL;
						}
						symbol_token_node.token_type = TokenType::TOKEN_TYPE_FUNCTION_OBJECT;
					}
					else if((aux_p = evalSymbol(eval_data,aux_p,line,&symbol_token_node))==NULL){ // finally try EvalSymbol (it should be an identifier or literal)
							return NULL;
					}

					aux_p=ignoreBlanks(aux_p,line);
					is_first_access=false;
					params=NO_PARAMS_IS_VARIABLE;

					// check valid access punctuator...
					if(isAccessPunctuator(aux_p) || symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

						if(!(      symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER
							  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_FUNCTION_OBJECT)&& *aux_p == '(')// cannot be a number/boolean or std::string and then accessor like . / ( / [
							  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_VECTOR  )&& *aux_p == '[')// inline function object like this: 1+function(a,b){ return a+b;} + 0
							  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_DICTIONARY  )&& *aux_p == '.')// inline struct object like this: 1+{"a":0,"b":1}.a + 0

							))
						{
								writeError(eval_data->current_parsing_file,line ,"Unexpected '%c' after literal",*aux_p);
								return NULL;
						}

						is_first_access=true;

						//instruction_first_access=0;//symbol_token_node.instruction.size()-1;
						//instruction_identifier=instruction_first_access;

						// eval accessor element (supose that was a preinsert a load instruction for identifier )...
						while(isAccessPunctuator(aux_p)){
							ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
							std::string accessor_value="";

							switch(*aux_p){
							case '(': // function call
								if(is_first_access){
									params=0;
								}
								aux_p = ignoreBlanks(aux_p+1,line);
								if(*aux_p != ')'){
									do{


										if((aux_p = evalExpression(eval_data,aux_p,line,scope_info,&symbol_token_node.instruction))==NULL){
											return NULL;
										}

										if(*aux_p != ',' && *aux_p != ')'){
											writeError(eval_data->current_parsing_file,line ,"Expected ',' or ')'");
											return NULL;
										}

										if(*aux_p == ','){
											aux_p=ignoreBlanks(aux_p+1,line);
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
								if((aux_p = evalExpression(eval_data,ignoreBlanks(aux_p+1,line),line,scope_info,&symbol_token_node.instruction))==NULL){
									return NULL;
								}
								if(*aux_p != ']'){
									writeError(eval_data->current_parsing_file,line ,"Expected ']'");
									return NULL;
								}
								aux_p=ignoreBlanks(aux_p+1,line);

								//symbol_token_node.instruction.push_back(Instruction(ByteCode::BYTE_CODE_VGET));
								byte_code=ByteCode::BYTE_CODE_VGET;
								break;
							case '.': // member access
								aux_p=ignoreBlanks(aux_p+1,line);
								accessor_value="";
								while(!isEndSymbolToken(aux_p)){ // get name...
									accessor_value += (*aux_p++);
								}

								if(!isIdentifierNameExpressionOk(eval_data,accessor_value,line)){
									return NULL;
								}

								//save_str_symbol_value=true;
								//evaluated_function_current->script_function->instruction_source_info[symbol_token_node.instruction.size()]=ScriptFunction::InstructionSourceInfo(accessor_value,current_parsing_file,line);
								//symbol_token_node.instruction.push_back(Instruction(ByteCode::BYTE_CODE_LOAD)); //--> must add symbol value instruction...
								byte_code=ByteCode::BYTE_CODE_LOAD;
								break;
							}

							if(byte_code==ByteCode::BYTE_CODE_LOAD){
								//instruction_identifier=symbol_token_node.instruction.size();
								symbol_token_node.instruction[instruction_identifier].instruction_source_info= InstructionSourceInfo(
									eval_data->current_parsing_file
									,line
									,getCompiledSymbol(eval_data,symbol_token_node.value)
								);
							}

							EvaluatedInstruction instruction_token=EvaluatedInstruction(byte_code);
							symbol_token_node.instruction.push_back(instruction_token);

							aux_p=ignoreBlanks(aux_p,line);
						}

						// add info to solve symbols first access (we need to put here because we have to know n params if function related)
						symbol_token_node.instruction[0].link_symbol_first_access=LinkSymbolFirstAccess(
								eval_data->evaluated_function_current->script_function->idx_script_function
								,scope_info->idx_scope
								,symbol_token_node.value
								,params // only if first access is a function...
						);

						// add info to add as symbol value ...
						symbol_token_node.instruction[0].instruction_source_info = InstructionSourceInfo(
							eval_data->current_parsing_file
							,line
							,getCompiledSymbol(eval_data,symbol_token_node.value)
						);


						if(isOperatorTypeInc(aux_p)){
							identity_post_operator_type=IDENTITY_OPERATOR_TYPE_INC;
							aux_p=ignoreBlanks(aux_p+2,line);
						}
						else if(isOperatorTypeDec(aux_p)){
							identity_post_operator_type=IDENTITY_OPERATOR_TYPE_DEC;
							aux_p=ignoreBlanks(aux_p+2,line);
						}

						// pre/post operator...
						symbol_token_node.identity_pre_operator_type = identity_pre_operator_type;
						symbol_token_node.identity_post_operator_type = identity_post_operator_type;
					}
				}

				expression_tokens.push_back(symbol_token_node);

				aux_p=ignoreBlanks(aux_p,line);

				if(!isEndExpression(aux_p)){ // if not end expression then a operator is expected...
					op=isOperatorType(aux_p);

					if(op==OperatorType::OPERATOR_TYPE_UNKNOWN){
						writeError(eval_data->current_parsing_file,line ,"Expected operator");
						return NULL;
					}

					operator_token_node.operator_type=op;
					operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

					expression_tokens.push_back(operator_token_node);
					aux_p+=strlen(eval_info_operators[op].str);
				}

				aux_p=ignoreBlanks(aux_p,line);
			}

			if(aux_p==0){
				writeError(eval_data->current_parsing_file,line ,"Unexpected end of file");
				return NULL;
			}


			// make operator precedence from the AST built before...
			if(!makeOperatorPrecedence(eval_data,&expression_tokens,instruction,0,expression_tokens.size()-1,error)){
				return NULL;
			}

			// last character is a separator so it return increments by 1
			return aux_p;
		}
	}
}
