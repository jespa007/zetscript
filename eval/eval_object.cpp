namespace zetscript{
	namespace eval{

		char *evalExpression(EvalData *eval_data,const char *s, int & line, Scope *scope_info, std::vector<EvaluatedInstruction *> 	* instructions);

		char * evalFunctionObject(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvaluatedInstruction *> 		*	instructions){
			// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

			return NULL;
		}

		char * evalDictionaryObject(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvaluatedInstruction *> 		*	instructions){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				std::string symbol_value;
				int lineSymbol;

				if(*aux_p == '{'){ // go for final ...

					// this solve problem void structs...
					aux_p=ignoreBlanks(aux_p+1,line);

					while (*aux_p != '}' && *aux_p != 0){

						lineSymbol = line;

						if((aux_p=getIdentifierToken(
								eval_data
								,aux_p
								,symbol_value
						))==NULL){
							 writeError(eval_data->current_parsing_file,lineSymbol ,"Expected symbol");
							 return NULL;
						}

						 aux_p=ignoreBlanks(aux_p,line);

						 if(*aux_p != ':'){ // expected : ...
							 writeError(eval_data->current_parsing_file,line,"Expected ':'");
							 return NULL;
						 }

						 aux_p++;

						 // go to variable...
						 if((aux_p=evalExpression(
								 eval_data
								 ,aux_p
								 ,line
								 ,scope_info
								 ,NULL
						)) == NULL){  //ignoreBlanks(aux_p+1,line);
							 return NULL;
						 }

						 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...
						 aux_p=ignoreBlanks(aux_p,line);

						 if(*aux_p == ','){
							 aux_p=ignoreBlanks(aux_p+1,line);
						 }
						 else if(*aux_p != '}' ){
							 writeError(eval_data->current_parsing_file,line,"expected '}' or ','");
							 return NULL;
						 }
					}
				}
				else{
					writeError(eval_data->current_parsing_file,line,"Expected '{'");
				}
				return aux_p;
		}


		char * evalVectorObject(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  std::vector<EvaluatedInstruction *> *	instruction){

			char * aux_p=ignoreBlanks(s,line);

			if(*aux_p != '['){
				writeError(eval_data->current_parsing_file,line,"Expected '['");
				return NULL;
			}

			aux_p=ignoreBlanks(aux_p+1,line);
			unsigned v_elements=0;

			while(*aux_p!=0 && *aux_p != ']'){

				aux_p=ignoreBlanks(aux_p,line);

				// expression expected ...
				if(v_elements > 0){
					if(*aux_p != ','){
						writeError(eval_data->current_parsing_file,line,",");
						return NULL;
					}

					aux_p=ignoreBlanks(aux_p+1,line);
				}
				aux_p=evalExpression(eval_data,aux_p,line,scope_info,NULL);
				v_elements++;
			}

			return aux_p;
		}

		char * evalNewObject(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvaluatedInstruction *> 		*	instruction){

			char *aux_p = (char *)s;
			std::string symbol_value;
			ScriptClass *sc=NULL;
			int n_args=0;
			ScriptFunction *constructor_function=NULL;

			KeywordType key_w;

			aux_p=ignoreBlanks(aux_p,line);

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

				if(key_w == KeywordType::KEYWORD_TYPE_NEW){
					aux_p=ignoreBlanks(aux_p+strlen(eval_info_keywords[key_w].str),line);
					// try get symbol ...++++

					eval_data->current_evaluated_function->instructions.push_back(new EvaluatedInstruction(BYTE_CODE_NEW));

					if((aux_p=getIdentifierToken(
							eval_data
							,aux_p
							,symbol_value
					))==NULL){
						 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
						 return NULL;
					}

					sc=GET_SCRIPT_CLASS(eval_data,symbol_value);

					if(sc==NULL){
						 writeError(eval_data->current_parsing_file,line,"class '%s' not defined",symbol_value.c_str());
						 return NULL;
					}

					 aux_p=ignoreBlanks(aux_p,line);

					 if(*aux_p != '('){
						 writeError(eval_data->current_parsing_file,line,"Expected '(' after \'%s\'",eval_info_keywords[key_w].str);
						 return NULL;
					 }

					 n_args=0;

					 //aux_p=ignoreBlanks(aux_p+1,line);

					 // foreach constructor argument
					  do{

						  aux_p=ignoreBlanks(aux_p+1,line);
						  if(*aux_p!=')'){ // be sure that counts as argument for empty args
							  // eval expression
							  if((aux_p = evalExpression(eval_data,aux_p,line,scope_info,instruction))==NULL){
								  return NULL;
							  }
							  n_args++;
						  }

						if(*aux_p != ',' && *aux_p != ')'){
							writeError(eval_data->current_parsing_file,line ,"Expected ',' or ')'");
							return NULL;
						}

					 }while(*aux_p != ')');


					 //aux_p = evalExpressionArgs('(', ')',aux_p,line,scope_info,instruction);
					 if(aux_p == NULL){
						 return NULL;
					 }

					 // get constructor function
					 constructor_function=sc->getFunction("constructor",sc->symbol_info.symbol->idx_scope,n_args);

					 // if constructor function found insert call function...
					 if(constructor_function != NULL){

						 eval_data->current_evaluated_function->instructions.push_back(
								 new EvaluatedInstruction(
										 BYTE_CODE_CALL
										 ,ZS_IDX_UNDEFINED
										 ,constructor_function->symbol_info.idx_symbol // idx function member
										// ,MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL
								)
						 );
					 }

					return aux_p+1; // ignore last )
				}
			}
			return NULL;
		}
	}
}
