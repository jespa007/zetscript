/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	//String * 	get_mapped_name(CompilerData *compiler_data,const String * symbol_name);
	String * get_mapped_name(const String & s);
	char 		*	compiler_expression(
			CompilerData *compiler_data
			,const char *s
			, int & line
			, ScriptScope *scope_info
			, Vector<CompilerInstruction *> 	* compiler_instructions
			, const char *expected_ending_char=NULL // expecting ending char when expression finish (by default not check or 0)
			, uint16_t properties = 0
	);


	char * compiler_sub_expression(
			CompilerData *compiler_data
			,const char *s
			, int & line
			, ScriptScope *scope_info
			, Vector<CompilerInstruction *> 	* compiler_instructions
			, const char *expected_ending_char=NULL
			, uint16_t properties=0 // uint16_t properties
			, Vector<Instruction *> *unique_call_instruction=NULL
	);

	//------------------------------------------------------------------------------------------------------------------------------

	char * compiler_object_function(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *_scope_info, TokenNode *token_node){
		ZS_UNUSUED_PARAM(_scope_info);
		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...
		CompilerInstruction *compiler_instruction;
		Vector<CompilerInstruction *> 	* compiler_instructions=&token_node->compiler_instructions;
		char *aux_p = (char *)s;
		unsigned short instruction_properties=0; // global by default ...
		Symbol *symbol_object=NULL;
		ByteCode byte_code = BYTE_CODE_LOAD_FUNCTION;
		ScriptScope *scope_info=ZS_MAIN_SCOPE(compiler_data);

		/*if(scope_info->scope_parent!=NULL){// is within function ?

			if(scope_info->type->id != ScriptTypeId::SCRIPT_TYPE_ID_CLASS_MAIN){ // function object as function member because it will use this inside
				byte_code=BYTE_CODE_LOAD_THIS_FUNCTION;
			}
		}*/

		compiler_instructions->push(compiler_instruction=new CompilerInstruction(
				byte_code
				, INSTRUCTION_VALUE_OP1_NOT_DEFINED
				,ZS_UNDEFINED_IDX
				,instruction_properties
		));

		if((aux_p=compiler_keyword_function(
			compiler_data
			,aux_p
			,line
			,scope_info
			,EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
			,&symbol_object
		))==NULL){
			return NULL;
		}

		compiler_instruction->vm_instruction.byte_code=byte_code;
		compiler_instruction->vm_instruction.value_op2=(zs_int)symbol_object;//->idx_position;
		token_node->value=compiler_instruction->symbol_name=symbol_object->name;
		compiler_instruction->symbol_scope=scope_info;

		compiler_instruction->instruction_source_info= compiler_instruction_source_info(
			compiler_data
			,compiler_data->current_parsing_file
			,line
			,token_node->value
		);


		return aux_p;
	}

	char * compiler_object_identifier(CompilerData *compiler_data,const char *s, int line, String & symbol_value){
		char *aux_p = (char *)s;
		symbol_value="";
		// get identifier with quotes...
		if(*aux_p == '\"'){
			aux_p++;
			for(;;){
				if(((*aux_p=='\"' && *(aux_p-1)!= '\\')|| *aux_p==0 || *aux_p=='\n')){
					break;
				}else{
					symbol_value+=*aux_p;
				}
				aux_p++;
			}

			if(*aux_p!='\"'){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"string not ends with '\"'");
			}

			aux_p++;
		}
		else{ // without quotes

			aux_p=get_name_identifier_token(
					compiler_data
					,aux_p
					,line
					,symbol_value
					,false
			);

		}

		return aux_p;
	}

	bool compiler_object_test(CompilerData *compiler_data,const char *s){
		char *aux_p = (char *)s;
		int line=0;
		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		if(*aux_p == '{'){ // go for final ...
			String symbol_value;
			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

			if(*aux_p == '}'){ // Empty {} is a block
				return false;
			}

			aux_p=compiler_object_identifier(compiler_data,aux_p,line,symbol_value);


			if(compiler_data->error == true){
				return false;
			}

			if(aux_p == NULL){
				return false;
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			return (*aux_p == ':' && *(aux_p+1) != ':');
		}

		return false;
	}

	char * compiler_object_object(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info, Vector<CompilerInstruction *>	*	compiler_instructions){
		// Inline object: two possibles uses {a:1,b:2}["a"] or {a:1, b:2}.a
		char *aux_p = (char *)s;
		String symbol_value;
		String str_key;
		StackElement *stk_key_object;
		Keyword keyw;

		if(*aux_p != '{'){ // go for final ...
			EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: Expected '{'");
		}

		// instance object ...
		compiler_instructions->push(new CompilerInstruction(BYTE_CODE_NEW_OBJECT));

		// this solve problem void structs...
		IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
		unsigned v_elements=0;

		while (*aux_p != '}' && *aux_p != 0){

			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: expected ',' for object property");
				}

				IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
			}

			if((keyw=compiler_is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Syntax error: '%s' keyword is not allowed as property name",compiler_data_keywords[keyw].str);
			}

			// get identifier with quotes...
			aux_p=compiler_object_identifier(compiler_data,aux_p,line,symbol_value);

			if(symbol_value.isEmpty()){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error ScriptObject: expected property name");
			}

			 // register constant...
			str_key=String("\"")+symbol_value+"\"";
			if((stk_key_object = compiler_data->script_engine->getStkConstantStringScriptObject(str_key))==NULL){
				stk_key_object=compiler_data->script_engine->registerStkConstantStringScriptObject(str_key,symbol_value);
			 }

			// add instruction...
			compiler_instructions->push(
					new CompilerInstruction(
					BYTE_CODE_LOAD_STRING
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,(zs_int)stk_key_object
			));

			 IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			 if(*aux_p != ':'){ // expected : ...
				 EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error ScriptObject: expected ':' after property name");
			 }

			 IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

			 if(*aux_p == '}'){
				 EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error ScriptObject: unexpected '}' ");
			 }


			 // go to variable...
			 if((aux_p=compiler_sub_expression(
					 compiler_data
					 ,aux_p
					 ,line
					 ,scope_info
					 ,compiler_instructions
			))==NULL){
				 return NULL;
			 }

			 // push attr (push a element pair)
			 compiler_instructions->push(new CompilerInstruction(BYTE_CODE_PUSH_OBJECT_ITEM));

			 v_elements++;
		}

		if( *aux_p != '}'){
			EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: expected ending '}'");
		}

		return aux_p+1;
	}

	char * compiler_object_vector(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info,  Vector<CompilerInstruction *> * compiler_instructions){
		// Inline vector: [0,1,2,3][0]+23
		char * aux_p=NULL;
		IGNORE_BLANKS(aux_p,compiler_data,s,line);

		if(*aux_p != '['){
			EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: expected '['");
		}

		// declare vector ...
		compiler_instructions->push(new CompilerInstruction(BYTE_CODE_NEW_ARRAY));

		IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
		unsigned v_elements=0;

		while(*aux_p!=0 && *aux_p != ']'){

			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: expected ',' before element");
				}
				IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
			}
			else if(*aux_p == ','){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: unexpected ','");
			}

			aux_p=compiler_sub_expression(
					compiler_data
					,aux_p
					,line
					,scope_info
					,compiler_instructions
			);

			if(aux_p==NULL){
				return 0;
			}

			// vpush
			compiler_instructions->push(new CompilerInstruction(BYTE_CODE_PUSH_ARRAY_ITEM));

			v_elements++;
		}

		if( *aux_p != ']'){
			EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error: expected ending ']'");
		}

		return aux_p+1;
	}

	char * compiler_object_new(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info, Vector<CompilerInstruction *>	*	compiler_instructions){
		// Inline new : (new A(4+5)).toString()
		char *aux_p = (char *)s;
		String symbol_name;
		ScriptType *type=NULL;
		int n_args=0;
		Symbol *constructor_function=NULL;
		int start_line=line;


		Keyword key_w;

		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);

		if(key_w != Keyword::KEYWORD_UNKNOWN){

			if(key_w == Keyword::KEYWORD_NEW){
				String expression="";
				bool is_native_type=false;
				Symbol *symbol_constructor_function_name=NULL;
				//bool end=false;

				CompilerInstruction *ei_load_function_constructor = NULL,*compiler_instruction=NULL;
				CompilerInstruction *compiler_instruction_new_object_by_value=NULL;
				IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);
				// try get symbol or access identifiers...++++

				for(;;){
					if((aux_p=get_name_identifier_token(
							compiler_data
							,aux_p
							,line
							,symbol_name
					))==NULL){
						return NULL;
					}

					expression+=symbol_name;
					IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

					if(*aux_p == '.'){
						expression+='.';
						IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
					}
					else if(*aux_p == '('){
						break;
					}else{
						EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Unexpected '%c' in 'new' expression",*aux_p);
					}
				};

				 IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);


				type=ZS_GET_OBJECT_TYPE(compiler_data->script_types_factory,symbol_name);

				// parse expression
				if(type==NULL){
					char *test_str=NULL;
					if((test_str = compiler_sub_expression(
							compiler_data
							,expression.toConstChar()
							,line
							,scope_info
							,compiler_instructions //compiler_data->current_function->instructions
							,NULL
							,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
					))==NULL){
						return NULL;
					}

					compiler_instructions->push(compiler_instruction_new_object_by_value=new CompilerInstruction(BYTE_CODE_NEW_OBJECT_BY_VALUE));
					compiler_instruction_new_object_by_value->instruction_source_info=compiler_instruction_source_info(
							  compiler_data
							 ,compiler_data->current_parsing_file
							 ,line
							 ,expression
						 );
				}else{ // known type
					is_native_type=type->isNativeType();
					symbol_constructor_function_name=type->getSymbolMemberFunction(ZS_CONSTRUCTOR_FUNCTION_NAME,0);

					if(!compiler_data->script_types_factory->isTypeInstanceable(type->id)){
						EVAL_ERROR_FILE_LINE(
								compiler_data->current_parsing_file
								,line
								,"Cannot create object type '%s' because it has been defined as not instantiable. "
								"To solve this issue, register type '%s' as instantiable (i.e register type '%s' with new/delete functions)"
								,type->getScriptTypeName()
								,type->getScriptTypeName()
								,type->getScriptTypeName()
								);
					}

					compiler_instructions->push(compiler_instruction=new CompilerInstruction(BYTE_CODE_NEW_OBJECT_BY_TYPE));
					compiler_instruction->vm_instruction.value_op1=type->id;
				}



				 IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

				 if(((*aux_p==')') && is_native_type && (symbol_constructor_function_name==NULL))){
					 // no args set and is native type and there's no constructor with parameters --> set default contructor ==0
					return aux_p+1;
				 }

				 if(compiler_instruction_new_object_by_value==NULL){
					 compiler_instructions->push(
						ei_load_function_constructor=new CompilerInstruction(
							 BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT
						)
					 );

					 ei_load_function_constructor->instruction_source_info=compiler_instruction_source_info(
						 compiler_data
						 ,compiler_data->current_parsing_file
						 ,line
						 ,ZS_CONSTRUCTOR_FUNCTION_NAME
					 );
				 }




				 n_args=0;

				 // foreach constructor argument
				  do{


					  if(*aux_p!=')'){ // be sure that counts as argument for empty args
						  // compiler expression
						  aux_p = compiler_sub_expression(
								  compiler_data
								  ,aux_p
								  ,line
								  ,scope_info
								  ,compiler_instructions
								  ,",)"
						  );

						  if(aux_p == NULL){
							  return NULL;
						  }

						  if(*aux_p == ','){
							  IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
						  }

						  n_args++;
					  }

				 }while(*aux_p != ')');

				 // if constructor function found insert call function...
				 compiler_instructions->push(
							 compiler_instruction=new CompilerInstruction(
							  BYTE_CODE_CONSTRUCTOR_CALL

							 ,INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(
									 0   // <-- CONSTRUCTOR ALWAYS RETURNS 0! the object is created at begin of expression byte code
									 ,n_args
							)
						 )

				 );

				 if(compiler_instruction_new_object_by_value==NULL){
					 // check constructor symbol
					 constructor_function=type->getSymbol(ZS_CONSTRUCTOR_FUNCTION_NAME);
					 int start_idx_function=type->scope->symbol_functions->length()-1;
					 if(constructor_function == NULL){ // find first constructor throught its function members
						 for(int i = start_idx_function; i >=0 && constructor_function==NULL; i--){
							Symbol *symbol_member = (Symbol *)type->scope->symbol_functions->get(i);
							ScriptFunction *sf_member=(ScriptFunction *)symbol_member->ref_ptr;
							if(sf_member->name== ZS_CONSTRUCTOR_FUNCTION_NAME){
								constructor_function = symbol_member;
							}
						}
					 }

					 if(constructor_function!=NULL){
						 // set idx function found
						 if((constructor_function->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF)==0){  // is a script constructor so only set idx
							 ei_load_function_constructor->vm_instruction.value_op2=constructor_function->idx_position;
						 }else{// is a native constructor, find a constructor if it passes one or more args
							 if(n_args > 0){ // we have to find our custom function to call after object is created
								 constructor_function=type->getSymbol(symbol_name.toConstChar(),n_args+1); //GET FUNCTION_MEMBER_CONSTRUCTOR_NAME. +1 Is because we include _this paramaters always in the call (is memeber function)!
								 if(constructor_function == NULL){
									 EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Cannot find any constructor function '%s' with '%i' parameters",symbol_name.toConstChar(),n_args);
								 }
								 // override idx
								 ei_load_function_constructor->vm_instruction.value_op2=constructor_function->idx_position;
								 constructor_function->properties|=SCRIPT_FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME; //compiler_instruction->vm_instruction.properties|=;
								 ei_load_function_constructor->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,n_args+1);
							 }
						 }

					 }
				 }else{
					 compiler_instruction_new_object_by_value->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,n_args);
				 }


				 compiler_instruction->instruction_source_info.file=compiler_data->current_parsing_file;
				 compiler_instruction->instruction_source_info.line=start_line;
				//  }


				return aux_p+1; // ignore last )
			}
		}
		return NULL;
	}
}

