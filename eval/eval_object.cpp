namespace zetscript{
	namespace eval{

		typedef enum{
			EVAL_EXPRESSION_PROPERTY_NO_RESET_STACK=0x1<<0,
			EVAL_EXPRESSION_PROPERTY_ALLOW_EXPRESSION_SEQUENCE=0x1<<1,
			EVAL_EXPRESSION_PROPERTY_NO_ALLOW_EXPRESSION_SEQUENCE_ASSIGNMENT=0x1<<2,
			EVAL_EXPRESSION_PROPERTY_BREAK_ON_ASSIGNMENT_OPERATOR=0x1<<3
		}EvalExpressionProperty;

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

			if(scope_info->scope_parent!=NULL){// is within function ?
				instruction_properties=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL;
				if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_CLASS_MAIN){ // is within function member ?
					instruction_properties=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;
				}
			}

			instructions->push_back(eval_instruction=new EvalInstruction(
					instruction_properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS?BYTE_CODE_LOAD_TYPE_VARIABLE:BYTE_CODE_LOAD_TYPE_FIND
					,ZS_IDX_UNDEFINED
					,ZS_IDX_UNDEFINED
					,instruction_properties
			));

			if((aux_p=eval_keyword_function(
				eval_data
				,aux_p
				,line
				,scope_info
				,true
				,&token_node->value
			))==NULL){
				return NULL;
			}

			eval_instruction->symbol.name=token_node->value;
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

		char * eval_object(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, std::vector<EvalInstruction *> 		*	instructions){
			// Inline object: two possibles uses {a:1,b:2}["a"] or {a:1, b:2}.a
			char *aux_p = (char *)s;
			std::string symbol_value;
			int lineSymbol;
			std::string key_value;
			ConstantValue *constant_value;
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

				if((keyw=eval::is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: \"%s\" keyword is not allowed as property name",eval::eval_data_keywords[keyw].str);
				}

				aux_p=get_name_identifier_token(
						eval_data
						,aux_p
						,line
						,symbol_value
				);

				if(zs_strutils::is_empty(symbol_value)){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected property name");
				}

				 key_value="\""+symbol_value+"\"";

				 // register constant...
				if((constant_value = eval_data->zs->getRegisteredConstantValue(key_value))==NULL){
					ScriptObjectString *s=new ScriptObjectString(eval_data->zs);
					s->str_value=symbol_value;
					constant_value=eval_data->zs->registerConstantValue(key_value,NULL,MSK_STK_PROPERTY_STRING);
					constant_value->stk_value=((void *)(s->str_value.c_str()));
					constant_value->var_ref=s;
				 }

				// add instruction...
				instructions->push_back(
						new EvalInstruction(ByteCode::BYTE_CODE_LOAD_TYPE_CONSTANT
						,ZS_IDX_UNDEFINED
						,(zs_int)constant_value
				));

				 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				 if(*aux_p != ':'){ // expected : ...
					 EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected ':' after property name");
				 }

				 aux_p++;

				 // go to variable...
				 aux_p=eval_expression(
						 eval_data
						 ,aux_p
						 ,line
						 ,scope_info
						 ,instructions
						 ,std::vector<char>{}
				 	 	 ,EVAL_EXPRESSION_PROPERTY_NO_RESET_STACK // we want to preserve all values current expression evaluation
				);

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
				aux_p=eval_expression(
						eval_data
						,aux_p
						,line
						,scope_info
						,instructions
						,std::vector<char>{}
						,EVAL_EXPRESSION_PROPERTY_NO_RESET_STACK // we want to preserve all values current expression evaluation
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
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
					// try get symbol ...++++
					aux_p=get_name_identifier_token(
							eval_data
							,aux_p
							,line
							,symbol_value
					);

					sc=GET_SCRIPT_CLASS(eval_data,symbol_value);

					if(sc==NULL){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"class '%s' not defined",symbol_value.c_str());
					}

					instructions->push_back(eval_instruction=new EvalInstruction(BYTE_CODE_NEW));

					eval_instruction->vm_instruction.value_op1=sc->idx_class;
        			 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

        			 // call function if there's any constructor function
					 // get constructor function
					 constructor_function=sc->getSymbol(scope_info->script_class->symbol_class.name); // FUNCTION_MEMBER_CONSTRUCTOR_NAME

					 // insert load function ...
					 instructions->push_back(
						eval_instruction=new EvalInstruction(
							 BYTE_CODE_LOAD_TYPE_VARIABLE
							 ,ZS_IDX_UNDEFINED
							 ,constructor_function!=NULL?ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR:ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR_NOT_FOUND
							 ,MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD
						)
					 );

					 eval_instruction->instruction_source_info=InstructionSourceInfo(
						 eval_data->current_parsing_file
						 ,line
						 ,get_mapped_name(eval_data,scope_info->script_class->symbol_class.name)//FUNCTION_MEMBER_CONSTRUCTOR_NAME)
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
							  	  	  ,EVAL_EXPRESSION_PROPERTY_NO_RESET_STACK // we want to preserve all values current expression evaluation
							  );

							  if(aux_p == NULL){
								  return NULL;
							  }

							  n_args++;
						  }


					 }while(*aux_p != ')');

					 // if constructor function found insert call function...
					 instructions->push_back(
						 new EvalInstruction(
							 BYTE_CODE_CALL
							 ,n_args
							 ,constructor_function!=NULL?ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR:ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR_NOT_FOUND
							 ,MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD
						)
					 );


					return aux_p+1; // ignore last )
				}
			}
			return NULL;
		}
	}
}
