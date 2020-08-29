namespace zetscript{
	namespace eval{

		char *eval_expression(EvalData *eval_data,const char *s, int & line, Scope *scope_info, std::vector<EvalInstruction *> 	* instructions);

		char * eval_object_function(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions){
			// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

			return NULL;
		}

		char * eval_object_dictionary(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			std::string symbol_value;
			int lineSymbol;
			std::string key_value;
			ConstantValue *constant_value;

			if(*aux_p != '{'){ // go for final ...
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '{'");
			}

			// declare dictionary ...
			instructions->push_back(new EvalInstruction(BYTE_CODE_INST_DICT));

			// this solve problem void structs...
			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			unsigned v_elements=0;

			while (*aux_p != '}' && *aux_p != 0){

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// expression expected ...
				if(v_elements > 0){
					if(*aux_p != ','){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected , on dictionary attribute");
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				}

				lineSymbol = line;

				aux_p=get_identifier_token(
						eval_data
						,aux_p
						,line
						,symbol_value
				);


				 key_value="\""+symbol_value+"\"";

				 // register constant...
				if((constant_value = eval_data->zs->getRegisteredConstantValue(key_value))==NULL){
					ScriptVarString *s=new ScriptVarString(eval_data->zs,symbol_value);
					constant_value=eval_data->zs->registerConstantValue(key_value,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING);
					constant_value->stk_value=((void *)(s->str_value.c_str()));
					constant_value->var_ref=s;
				 }

				// add instruction...
				instructions->push_back(
						new EvalInstruction(ByteCode::BYTE_CODE_LOAD
						,LOAD_TYPE_CONSTANT
						,(intptr_t)constant_value
				));

				 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				 if(*aux_p != ':'){ // expected : ...
					 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ':'");
				 }

				 aux_p++;

				 // go to variable...
				 aux_p=eval_expression(
						 eval_data
						 ,aux_p
						 ,line
						 ,scope_info
						 ,instructions
				);

				 // push attr (push a element pair)
				 instructions->push_back(new EvalInstruction(BYTE_CODE_PUSH_ATTR));

				 v_elements++;
			}

			if( *aux_p != '}'){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected }");
			}


			return aux_p+1;
		}


		char * eval_object_vector(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  std::vector<EvalInstruction *> *	instructions){

			char * aux_p=NULL;
			IGNORE_BLANKS(aux_p,eval_data,s,line);

			if(*aux_p != '['){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '['");
			}

			// declare vector ...
			instructions->push_back(new EvalInstruction(BYTE_CODE_INST_VEC));

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			unsigned v_elements=0;

			while(*aux_p!=0 && *aux_p != ']'){

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// expression expected ...
				if(v_elements > 0){
					if(*aux_p != ','){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected , on vector element");
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				}
				aux_p=eval_expression(
						eval_data
						,aux_p
						,line
						,scope_info
						,instructions);

				// vpush
				instructions->push_back(new EvalInstruction(BYTE_CODE_VPUSH));

				v_elements++;
			}

			if( *aux_p != ']'){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected ]");
			}

			return aux_p+1;
		}

		char * eval_object_new(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions){

			char *aux_p = (char *)s;
			std::string symbol_value;
			ScriptClass *sc=NULL;
			int n_args=0;
			Symbol *constructor_function=NULL;

			Keyword key_w;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){

				if(key_w == Keyword::KEYWORD_NEW){
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);
					// try get symbol ...++++

					instructions->push_back(new EvalInstruction(BYTE_CODE_NEW));

					aux_p=get_identifier_token(
							eval_data
							,aux_p
							,line
							,symbol_value
					);

					sc=GET_SCRIPT_CLASS(eval_data,symbol_value);

					if(sc==NULL){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"class '%s' not defined",symbol_value.c_str());
					}

					 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					 if(*aux_p != '('){
						 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '(' after \'%s\'",eval_info_keywords[key_w].str);
					 }

					 n_args=0;

					 // foreach constructor argument
					  do{

						  IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						  if(*aux_p!=')'){ // be sure that counts as argument for empty args
							  // eval expression
							  aux_p = eval_expression(
									  eval_data
									  ,aux_p
									  ,line
									  ,scope_info
									  ,instructions
							);
							  n_args++;
						  }

						if(*aux_p != ',' && *aux_p != ')'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ',' or ')'");
						}

					 }while(*aux_p != ')');

					 // get constructor function
					 constructor_function=sc->getSymbol(FUNCTION_MEMBER_CONSTRUCTOR_NAME,n_args);

					 // if constructor function found insert call function...
					 if(constructor_function != NULL){

						 instructions->push_back(
							 new EvalInstruction(
									 BYTE_CODE_CALL
									 ,ZS_IDX_UNDEFINED
									 ,constructor_function->idx_position // idx function member
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
