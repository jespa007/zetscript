namespace zetscript{

	int n_anonymouse_function=0;

	//std::string * 	get_mapped_name(EvalData *eval_data,const std::string * symbol_name);
	std::string * get_mapped_name(EvalData *eval_data,const std::string & s);
	char 		*	eval_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info, std::vector<EvalInstruction *> 	* instructions
			, std::vector<char>expected_ending_char={} // expecting ending char when expression finish (by default not check or 0)
			, uint16_t properties = 0
	);


	//------------------------------------------------------------------------------------------------------------------------------

	char * eval_object_function(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, TokenNode *token_node){
		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...
		EvalInstruction *eval_instruction;
		std::vector<EvalInstruction *> 	* instructions=&token_node->instructions;
		char *aux_p = (char *)s;
		unsigned short instruction_properties=0; // global by default ...
		Symbol *symbol_object=NULL;
		ByteCode byte_code = ByteCode::BYTE_CODE_LOAD_LOCAL; // not found by default

		if(scope_info->scope_parent!=NULL){// is within function ?

			if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_MAIN){ // function object as function member because it will use this inside
				byte_code=ByteCode::BYTE_CODE_LOAD_ELEMENT_THIS;
			}
		}

		instructions->push_back(eval_instruction=new EvalInstruction(
				byte_code
				,ZS_IDX_UNDEFINED
				,ZS_IDX_UNDEFINED
				,instruction_properties
		));

		if((aux_p=eval_keyword_function(
			eval_data
			,aux_p
			,line
			,scope_info
			,EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
			,&symbol_object
		))==NULL){
			return NULL;
		}

		eval_instruction->vm_instruction.byte_code=byte_code;
		eval_instruction->vm_instruction.value_op2=symbol_object->idx_position;
		token_node->value=eval_instruction->symbol.name=symbol_object->name;
		eval_instruction->symbol.scope=scope_info;

		/*eval_instruction->link_symbol_first_access=LinkSymbolFirstAccess(
				eval_data->current_function->script_function->idx_script_function
				,scope_info
				,token_node->value
		);*/

		eval_instruction->instruction_source_info= InstructionSourceInfo(
			eval_data->current_parsing_file
			,line
			,get_mapped_name(eval_data,token_node->value
			)
		);


		return aux_p;
	}

	char * eval_object_identifier(EvalData *eval_data,const char *s, int line, std::string & symbol_value){
		char *aux_p = (char *)s;
		symbol_value="";
		// get identifier with quotes...
		if(*aux_p == '\"'){
			aux_p++;
			bool end=false;
			for(;;){
				if(((*aux_p=='\"' && *(aux_p-1)!= '\\')|| *aux_p==0 || *aux_p=='\n')){
					break;
				}else{
					symbol_value+=*aux_p;
				}
				aux_p++;
			}

			if(*aux_p!='\"'){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"string not ends with '\"'");
			}

			aux_p++;
		}
		else{ // without quotes

			aux_p=get_name_identifier_token(
					eval_data
					,aux_p
					,line
					,symbol_value
			);

		}

		return aux_p;
	}

	bool eval_object_test(EvalData *eval_data,const char *s){
		char *aux_p = (char *)s;
		int line=0;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		if(*aux_p == '{'){ // go for final ...
			std::string symbol_value;
			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			if(*aux_p == '}')
				return true;

			aux_p=eval_object_identifier(eval_data,aux_p,line,symbol_value);


			if(eval_data->error == true){
				return false;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			return (*aux_p == ':' && *(aux_p+1) != ':');
		}

		return false;
	}

	char * eval_object(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions){
		// Inline object: two possibles uses {a:1,b:2}["a"] or {a:1, b:2}.a
		char *aux_p = (char *)s;
		std::string symbol_value;
		std::string str_key;
		int lineSymbol;
		//std::string key_value;
		StackElement *stk_key_object;
		Keyword keyw;

		if(*aux_p != '{'){ // go for final ...
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: Expected '{'");
		}

		// instance object ...
		instructions->push_back(new EvalInstruction(BYTE_CODE_NEW_OBJECT));

		// this solve problem void structs...
		IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
		unsigned v_elements=0;

		while (*aux_p != '}' && *aux_p != 0){

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected ',' for object property");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			}

			lineSymbol = line;

			if((keyw=eval_is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: \"%s\" keyword is not allowed as property name",eval_data_keywords[keyw].str);
			}

			// get identifier with quotes...
			aux_p=eval_object_identifier(eval_data,aux_p,line,symbol_value);

			if(zs_strutils::is_empty(symbol_value)){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error Object: expected property name");
			}

			 // register constant...
			str_key=std::string("\"")+symbol_value+"\"";
			if((stk_key_object = eval_data->zs->getStkStringObject(str_key))==NULL){
				stk_key_object=eval_data->zs->registerStkStringObject(str_key,symbol_value);
			 }

			// add instruction...
			instructions->push_back(
					new EvalInstruction(ByteCode::BYTE_CODE_LOAD_STRING
					,ZS_IDX_UNDEFINED
					,(zs_int)stk_key_object
			));

			 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			 if(*aux_p != ':'){ // expected : ...
				 EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error Object: expected ':' after property name");
			 }

			 IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			 if(*aux_p == '}'){
				 EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error Object: unexpected '}' ");
			 }


			 // go to variable...
			 if((aux_p=eval_expression(
					 eval_data
					 ,aux_p
					 ,line
					 ,scope_info
					 ,instructions
					 ,std::vector<char>{}
			))==NULL){
				 return NULL;
			 }

			 // push attr (push a element pair)
			 instructions->push_back(new EvalInstruction(BYTE_CODE_PUSH_OBJECT_ELEMENT));

			 v_elements++;
		}

		if( *aux_p != '}'){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected ending '}'");
		}

		return aux_p+1;
	}

	char * eval_object_vector(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  std::vector<EvalInstruction *> *	instructions){
		// Inline vector: [0,1,2,3][0]+23
		char * aux_p=NULL;
		IGNORE_BLANKS(aux_p,eval_data,s,line);

		if(*aux_p != '['){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected '['");
		}

		// declare vector ...
		instructions->push_back(new EvalInstruction(BYTE_CODE_NEW_VECTOR));

		IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
		unsigned v_elements=0;

		while(*aux_p!=0 && *aux_p != ']'){

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected ',' before element");
				}
				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			}
			else if(*aux_p == ','){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: unexpected ','");
			}

			aux_p=eval_expression(
					eval_data
					,aux_p
					,line
					,scope_info
					,instructions
					,std::vector<char>{}
					);

			// vpush
			instructions->push_back(new EvalInstruction(BYTE_CODE_PUSH_VECTOR_ELEMENT));

			v_elements++;
		}

		if( *aux_p != ']'){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected ending ']'");
		}

		return aux_p+1;
	}

	char * eval_object_new(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions){
		// Inline new : (new A(4+5)).toString()
		char *aux_p = (char *)s;
		std::string class_name;
		ScriptClass *sc=NULL;
		int n_args=0;
		Symbol *constructor_function=NULL;
		int start_line=line;

		Keyword key_w;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w != Keyword::KEYWORD_UNKNOWN){

			if(key_w == Keyword::KEYWORD_NEW){
				EvalInstruction *eval_instruction = NULL;
				int start_line_new=line;
				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
				// try get symbol ...++++
				aux_p=get_name_identifier_token(
						eval_data
						,aux_p
						,line
						,class_name
				);

				sc=GET_SCRIPT_CLASS(eval_data,class_name);

				if(sc==NULL){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"class '%s' not defined",class_name.c_str());
				}

				instructions->push_back(eval_instruction=new EvalInstruction(BYTE_CODE_NEW_CLASS));

				eval_instruction->vm_instruction.value_op1=sc->idx_class;
				 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				 // call function if there's any constructor function
				 // get constructor function
				 constructor_function=sc->getSymbol(class_name); // FUNCTION_MEMBER_CONSTRUCTOR_NAME

				 if(constructor_function != NULL){
					 // insert load function ...
					 instructions->push_back(
						eval_instruction=new EvalInstruction(
							 ByteCode::BYTE_CODE_LOAD_CONSTRUCTOR
							 ,ZS_IDX_UNDEFINED
							 ,constructor_function->idx_position
						)
					 );
				 }

				 eval_instruction->instruction_source_info=InstructionSourceInfo(
					 eval_data->current_parsing_file
					 ,line
					 ,get_mapped_name(eval_data,class_name)//FUNCTION_MEMBER_CONSTRUCTOR_NAME)
				 );

				 if(*aux_p != '('){
					 EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '(' after \'%s\'",eval_data_keywords[key_w].str);
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
								  ,std::vector<char>{',',')'}
						  );

						  if(aux_p == NULL){
							  return NULL;
						  }

						  n_args++;
					  }


				 }while(*aux_p != ')');

				 // if constructor function found insert call function...
				  if(constructor_function != NULL){
					 instructions->push_back(
							 eval_instruction=new EvalInstruction(
							  BYTE_CODE_CALL_CONSTRUCTOR
							 ,n_args
						)
					 );

					 eval_instruction->instruction_source_info.file=eval_data->current_parsing_file;
					 eval_instruction->instruction_source_info.line=start_line;
				  }


				return aux_p+1; // ignore last )
			}
		}
		return NULL;
	}
}

