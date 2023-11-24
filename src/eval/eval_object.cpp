/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	//zs_string * 	get_mapped_name(EvalData *eval_data,const zs_string * symbol_name);
	zs_string * get_mapped_name(const zs_string & s);
	char 		*	eval_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, zs_vector<EvalInstruction *> 	* eval_instructions
			, const char *expected_ending_char=NULL // expecting ending char when expression finish (by default not check or 0)
			, uint16_t properties = 0
	);


	char * eval_sub_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, zs_vector<EvalInstruction *> 	* eval_instructions
			, const char *expected_ending_char=NULL
			, uint16_t properties=0 // uint16_t properties
			, zs_vector<Instruction *> *unique_call_instruction=NULL
	);

	//------------------------------------------------------------------------------------------------------------------------------

	char * eval_object_function(EvalData *eval_data,const char *s,int & line,  Scope *_scope_info, TokenNode *token_node){
		ZS_UNUSUED_PARAM(_scope_info);
		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...
		EvalInstruction *eval_instruction;
		zs_vector<EvalInstruction *> 	* eval_instructions=&token_node->eval_instructions;
		char *aux_p = (char *)s;
		unsigned short instruction_properties=0; // global by default ...
		Symbol *symbol_object=NULL;
		ByteCode byte_code = BYTE_CODE_LOAD_FUNCTION;
		Scope *scope_info=ZS_MAIN_SCOPE(eval_data);

		/*if(scope_info->scope_parent!=NULL){// is within function ?

			if(scope_info->script_type->id != ScriptTypeId::SCRIPT_TYPE_ID_CLASS_MAIN){ // function object as function member because it will use this inside
				byte_code=BYTE_CODE_LOAD_THIS_FUNCTION;
			}
		}*/

		eval_instructions->push_back(eval_instruction=new EvalInstruction(
				byte_code
				, INSTRUCTION_VALUE_OP1_NOT_DEFINED
				,ZS_UNDEFINED_IDX
				,instruction_properties
		));

		if((aux_p=eval_keyword_function(
			eval_data
			,aux_p
			,line
			,scope_info
			,ZS_EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
			,&symbol_object
		))==NULL){
			return NULL;
		}

		eval_instruction->vm_instruction.byte_code=byte_code;
		eval_instruction->vm_instruction.value_op2=(zs_int)symbol_object;//->idx_position;
		token_node->value=eval_instruction->symbol_name=symbol_object->name;
		eval_instruction->symbol_scope=scope_info;

		eval_instruction->instruction_source_info= eval_instruction_source_info(
			eval_data
			,eval_data->current_parsing_file
			,line
			,token_node->value
		);


		return aux_p;
	}

	char * eval_object_identifier(EvalData *eval_data,const char *s, int line, zs_string & symbol_value){
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
				EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line ,"string not ends with '\"'");
			}

			aux_p++;
		}
		else{ // without quotes

			aux_p=get_name_identifier_token(
					eval_data
					,aux_p
					,line
					,symbol_value
					,false
			);

		}

		return aux_p;
	}

	bool eval_object_test(EvalData *eval_data,const char *s){
		char *aux_p = (char *)s;
		int line=0;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		if(*aux_p == '{'){ // go for final ...
			zs_string symbol_value;
			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			if(*aux_p == '}'){ // Empty {} is a block
				return false;
			}

			aux_p=eval_object_identifier(eval_data,aux_p,line,symbol_value);


			if(eval_data->error == true){
				return false;
			}

			if(aux_p == NULL){
				return false;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			return (*aux_p == ':' && *(aux_p+1) != ':');
		}

		return false;
	}

	char * eval_object_object(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, zs_vector<EvalInstruction *>	*	eval_instructions){
		// Inline object: two possibles uses {a:1,b:2}["a"] or {a:1, b:2}.a
		char *aux_p = (char *)s;
		zs_string symbol_value;
		zs_string str_key;
		StackElement *stk_key_object;
		Keyword keyw;

		if(*aux_p != '{'){ // go for final ...
			EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: Expected '{'");
		}

		// instance object ...
		eval_instructions->push_back(new EvalInstruction(BYTE_CODE_NEW_OBJECT));

		// this solve problem void structs...
		IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
		unsigned v_elements=0;

		while (*aux_p != '}' && *aux_p != 0){

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: expected ',' for object property");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			}

			if((keyw=eval_is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: '%s' keyword is not allowed as property name",eval_data_keywords[keyw].str);
			}

			// get identifier with quotes...
			aux_p=eval_object_identifier(eval_data,aux_p,line,symbol_value);

			if(symbol_value.empty()){
				EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error Object: expected property name");
			}

			 // register constant...
			str_key=zs_string("\"")+symbol_value+"\"";
			if((stk_key_object = eval_data->zs->getStkConstantStringObject(str_key))==NULL){
				stk_key_object=eval_data->zs->registerStkConstantStringObject(str_key,symbol_value);
			 }

			// add instruction...
			eval_instructions->push_back(
					new EvalInstruction(
					BYTE_CODE_LOAD_STRING
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,(zs_int)stk_key_object
			));

			 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			 if(*aux_p != ':'){ // expected : ...
				 EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error Object: expected ':' after property name");
			 }

			 IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			 if(*aux_p == '}'){
				 EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error Object: unexpected '}' ");
			 }


			 // go to variable...
			 if((aux_p=eval_sub_expression(
					 eval_data
					 ,aux_p
					 ,line
					 ,scope_info
					 ,eval_instructions
			))==NULL){
				 return NULL;
			 }

			 // push attr (push a element pair)
			 eval_instructions->push_back(new EvalInstruction(BYTE_CODE_PUSH_OBJECT_ITEM));

			 v_elements++;
		}

		if( *aux_p != '}'){
			EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: expected ending '}'");
		}

		return aux_p+1;
	}

	char * eval_object_vector(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  zs_vector<EvalInstruction *> * eval_instructions){
		// Inline vector: [0,1,2,3][0]+23
		char * aux_p=NULL;
		IGNORE_BLANKS(aux_p,eval_data,s,line);

		if(*aux_p != '['){
			EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: expected '['");
		}

		// declare vector ...
		eval_instructions->push_back(new EvalInstruction(BYTE_CODE_NEW_ARRAY));

		IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
		unsigned v_elements=0;

		while(*aux_p!=0 && *aux_p != ']'){

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: expected ',' before element");
				}
				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			}
			else if(*aux_p == ','){
				EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: unexpected ','");
			}

			aux_p=eval_sub_expression(
					eval_data
					,aux_p
					,line
					,scope_info
					,eval_instructions
			);

			if(aux_p==NULL){
				return 0;
			}

			// vpush
			eval_instructions->push_back(new EvalInstruction(BYTE_CODE_PUSH_ARRAY_ITEM));

			v_elements++;
		}

		if( *aux_p != ']'){
			EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error: expected ending ']'");
		}

		return aux_p+1;
	}

	char * eval_object_new(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, zs_vector<EvalInstruction *>	*	eval_instructions){
		// Inline new : (new A(4+5)).toString()
		char *aux_p = (char *)s;
		zs_string symbol_name;
		ScriptType *script_type=NULL;
		int n_args=0;
		Symbol *constructor_function=NULL;
		int start_line=line;


		Keyword key_w;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w != Keyword::KEYWORD_UNKNOWN){

			if(key_w == Keyword::KEYWORD_NEW){
				zs_string expression="";
				bool is_native_type=false;
				Symbol *symbol_constructor_function_name=NULL;
				//bool end=false;

				EvalInstruction *ei_load_function_constructor = NULL,*eval_instruction=NULL;
				EvalInstruction *eval_instruction_new_object_by_value=NULL;
				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
				// try get symbol or access identifiers...++++

				for(;;){
					if((aux_p=get_name_identifier_token(
							eval_data
							,aux_p
							,line
							,symbol_name
					))==NULL){
						return NULL;
					}

					expression+=symbol_name;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p == '.'){
						expression+='.';
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
					}
					else if(*aux_p == '('){
						break;
					}else{
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected '%c' in 'new' expression",*aux_p);
					}
				};

				 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);


				script_type=GET_SCRIPT_TYPE(eval_data->script_type_factory,symbol_name);

				// parse expression
				if(script_type==NULL){
					char *test_str=NULL;
					if((test_str = eval_sub_expression(
							eval_data
							,expression.c_str()
							,line
							,scope_info
							,eval_instructions //eval_data->current_function->instructions
							,NULL
							,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
					))==NULL){
						return NULL;
					}

					eval_instructions->push_back(eval_instruction_new_object_by_value=new EvalInstruction(BYTE_CODE_NEW_OBJECT_BY_VALUE));
					eval_instruction_new_object_by_value->instruction_source_info=eval_instruction_source_info(
							  eval_data
							 ,eval_data->current_parsing_file
							 ,line
							 ,expression
						 );
				}else{ // known type
					is_native_type=script_type->isNativeType();
					symbol_constructor_function_name=script_type->getSymbolMemberFunction(ZS_CONSTRUCTOR_FUNCTION_NAME,0);

					if(!eval_data->script_type_factory->isScriptTypeInstanceable(script_type->id)){
						EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Cannot create object type '%s' because it has been defined as not instantiable. "
								"To solve this issue, register type '%s' as instantiable (i.e register type '%s' with new/delete functions)"
								,script_type->getTypeName()
								,script_type->getTypeName()
								,script_type->getTypeName()
								);
					}

					eval_instructions->push_back(eval_instruction=new EvalInstruction(BYTE_CODE_NEW_OBJECT_BY_TYPE));
					eval_instruction->vm_instruction.value_op1=script_type->id;
				}



				 IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				 if(((*aux_p==')') && is_native_type && (symbol_constructor_function_name==NULL))){
					 // no args set and is native type and there's no constructor with parameters --> set default contructor ==0
					return aux_p+1;
				 }

				 if(eval_instruction_new_object_by_value==NULL){
					 eval_instructions->push_back(
						ei_load_function_constructor=new EvalInstruction(
							 BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT
						)
					 );

					 ei_load_function_constructor->instruction_source_info=eval_instruction_source_info(
						 eval_data
						 ,eval_data->current_parsing_file
						 ,line
						 ,ZS_CONSTRUCTOR_FUNCTION_NAME
					 );
				 }




				 n_args=0;

				 // foreach constructor argument
				  do{


					  if(*aux_p!=')'){ // be sure that counts as argument for empty args
						  // eval expression
						  aux_p = eval_sub_expression(
								  eval_data
								  ,aux_p
								  ,line
								  ,scope_info
								  ,eval_instructions
								  ,",)"
						  );

						  if(aux_p == NULL){
							  return NULL;
						  }

						  if(*aux_p == ','){
							  IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						  }

						  n_args++;
					  }

				 }while(*aux_p != ')');

				 // if constructor function found insert call function...
				 eval_instructions->push_back(
							 eval_instruction=new EvalInstruction(
							  BYTE_CODE_CONSTRUCTOR_CALL

							 ,INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(
									 0   // <-- CONSTRUCTOR ALWAYS RETURNS 0! the object is created at begin of expression byte code
									 ,n_args
							)
						 )

				 );

				 if(eval_instruction_new_object_by_value==NULL){
					 // check constructor symbol
					 constructor_function=script_type->getSymbol(ZS_CONSTRUCTOR_FUNCTION_NAME);
					 int start_idx_function=script_type->scope->symbol_functions->size()-1;
					 if(constructor_function == NULL){ // find first constructor throught its function members
						 for(int i = start_idx_function; i >=0 && constructor_function==NULL; i--){
							Symbol *symbol_member = (Symbol *)script_type->scope->symbol_functions->get(i);
							ScriptFunction *sf_member=(ScriptFunction *)symbol_member->ref_ptr;
							if(sf_member->name== ZS_CONSTRUCTOR_FUNCTION_NAME){
								constructor_function = symbol_member;
							}
						}
					 }

					 if(constructor_function!=NULL){
						 // set idx function found
						 if((constructor_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0){  // is a script constructor so only set idx
							 ei_load_function_constructor->vm_instruction.value_op2=constructor_function->idx_position;
						 }else{// is a native constructor, find a constructor if it passes one or more args
							 if(n_args > 0){ // we have to find our custom function to call after object is created
								 constructor_function=script_type->getSymbol(symbol_name.c_str(),n_args+1); //GET FUNCTION_MEMBER_CONSTRUCTOR_NAME. +1 Is because we include _this paramaters always in the call (is memeber function)!
								 if(constructor_function == NULL){
									 EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Cannot find any constructor function '%s' with '%i' parameters",symbol_name.c_str(),n_args);
								 }
								 // override idx
								 ei_load_function_constructor->vm_instruction.value_op2=constructor_function->idx_position;
								 constructor_function->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME; //eval_instruction->vm_instruction.properties|=;
								 ei_load_function_constructor->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,n_args+1);
							 }
						 }

					 }
				 }else{
					 eval_instruction_new_object_by_value->vm_instruction.value_op1=INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,n_args);
				 }


				 eval_instruction->instruction_source_info.file=eval_data->current_parsing_file;
				 eval_instruction->instruction_source_info.line=start_line;
				//  }


				return aux_p+1; // ignore last )
			}
		}
		return NULL;
	}
}

