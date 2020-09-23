namespace zetscript{
	namespace eval{
		int n_anonymouse_function=0;

		//std::string * 	get_compiled_symbol(EvalData *eval_data,const std::string * symbol_name);
		std::string * get_compiled_symbol(EvalData *eval_data,const std::string & s);
		char 		*	eval_expression(
				EvalData *eval_data
				,const char *s
				, int & line
				, Scope *scope_info, std::vector<EvalInstruction *> 	* instructions
				, char expected_ending_char=0 // expecting ending char when expression finish (by default not check or 0)
				, int level=0
				);

		char * eval_object_function(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, TokenNode *token_node){
			// this function is not like keyword function, it ensures that is a function object (anonymouse function)...
			EvalInstruction *eval_instruction;
			std::vector<EvalInstruction *> 	* instructions=&token_node->instructions;
			char *aux_p = (char *)s;
			unsigned short scope=0; // global by default ...

			if(scope_info->scope_parent!=NULL){// is within function ?
				scope=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL;
				if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_CLASS_MAIN){ // is within function member ?
					scope=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS;
				}
			}

			instructions->push_back(eval_instruction=new EvalInstruction(
					BYTE_CODE_LOAD
					,scope & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS?LoadType::LOAD_TYPE_VARIABLE:LoadType::LOAD_TYPE_FUNCTION
					,ZS_IDX_UNDEFINED
					,scope
			));

			aux_p=eval_keyword_function(
				eval_data
				,aux_p
				,line
				,scope_info
				,true
				,&token_node->value
			);

			eval_instruction->link_symbol_first_access=LinkSymbolFirstAccess(
					eval_data->current_function->script_function->idx_script_function
					,scope_info
					,token_node->value
			);

			eval_instruction->instruction_source_info= InstructionSourceInfo(
				eval_data->current_parsing_file
				,line
				,get_compiled_symbol(eval_data,token_node->value)
			);


			return aux_p;
		}

		char * eval_object_dictionary(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions,int level){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			std::string symbol_value;
			int lineSymbol;
			std::string key_value;
			ConstantValue *constant_value;
			Keyword keyw;

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
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"dictionary: expected ',' for dictionary property");
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				}

				lineSymbol = line;

				if((keyw=eval::is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"dictionary: \"%s\" keyword is not allowed as property name",eval::eval_info_keywords[keyw].str);
				}

				aux_p=get_identifier_token(
						eval_data
						,aux_p
						,line
						,symbol_value
				);

				if(zs_strutils::is_empty(symbol_value)){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"dictionary: expected property name");
				}

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
					 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"dictionary: expected ':' after property name");
				 }

				 aux_p++;

				 // go to variable...
				 aux_p=eval_expression(
						 eval_data
						 ,aux_p
						 ,line
						 ,scope_info
						 ,instructions
						 ,0
						 ,level+1
				);

				 // push attr (push a element pair)
				 instructions->push_back(new EvalInstruction(BYTE_CODE_PUSH_ATTR));

				 v_elements++;
			}

			if( *aux_p != '}'){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"dictionary: expected ending '}'");
			}

			return aux_p+1;
		}

		char * eval_object_vector(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  std::vector<EvalInstruction *> *	instructions, int level){

			char * aux_p=NULL;
			IGNORE_BLANKS(aux_p,eval_data,s,line);

			if(*aux_p != '['){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"vector: expected '['");
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
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"vector: expected ',' before element");
					}
					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				}
				aux_p=eval_expression(
						eval_data
						,aux_p
						,line
						,scope_info
						,instructions
						,0
						,level+1);

				// vpush
				instructions->push_back(new EvalInstruction(BYTE_CODE_VPUSH));

				v_elements++;
			}

			if( *aux_p != ']'){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"vector: expected ending ']'");
			}

			return aux_p+1;
		}

		char * eval_object_new(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions, int level){

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
					EvalInstruction *eval_instruction = NULL;
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);
					// try get symbol ...++++
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

					instructions->push_back(eval_instruction=new EvalInstruction(BYTE_CODE_NEW));

					eval_instruction->vm_instruction.value_op1=sc->idx_class;
        			 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

        			 // call function if there's any constructor function
					 // get constructor function
					 constructor_function=sc->getSymbol(FUNCTION_MEMBER_CONSTRUCTOR_NAME);

					 // insert load function ...
					 if(constructor_function != NULL){
						 instructions->push_back(
							eval_instruction=new EvalInstruction(
								 BYTE_CODE_LOAD
								 ,LoadType::LOAD_TYPE_VARIABLE
								 ,ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR
								 ,MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS
							)
						 );

						 eval_instruction->instruction_source_info=InstructionSourceInfo(
							 eval_data->current_parsing_file
							 ,line
							 ,get_compiled_symbol(eval_data,FUNCTION_MEMBER_CONSTRUCTOR_NAME)
						);
					 }


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
									  ,0
									  ,level+1
							);
							  n_args++;
						  }

						if(*aux_p != ',' && *aux_p != ')'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Expected ',' or ')'");
						}

					 }while(*aux_p != ')');


					 // if constructor function found insert call function...
					 if(constructor_function != NULL){

						 instructions->push_back(
							 new EvalInstruction(
								 BYTE_CODE_CALL
								 ,n_args
								 ,ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR
								 ,MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS
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
