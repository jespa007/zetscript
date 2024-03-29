/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define SYMBOL_VALUE_THIS				"this"
#define SYMBOL_VALUE_SUPER				"super"

namespace zetscript{

	// to String utils ...
	char * compiler_symbol(CompilerData *compiler_data
			,const char *start_word
			, int line
			,  ScriptScope *scope_info
			,TokenNode * token_node_symbol
			, PreOperation & pre_operation
		//	, PostOperation post_operation
	){
		// PRE:
		zs_int value = ZS_UNDEFINED_IDX;
		ByteCode byte_code=BYTE_CODE_INVALID;
		token_node_symbol->token_type = TokenType::TOKEN_TYPE_LITERAL;
		token_node_symbol->line=line;
		CompilerInstruction *compiler_instruction=NULL;
		void *const_obj=NULL;
		char *aux=(char *)start_word;
		String default_str_value="";

		 if((aux=parse_literal_number(
				 compiler_data
				 ,start_word
				 ,line
				 ,default_str_value
		))!=NULL){ // int/bool/float, etc

			if((const_obj=Integer::parse(default_str_value))!=NULL){ // int literal
				value = *((zs_int *)const_obj);
				if(pre_operation==PreOperation::PRE_OPERATION_NEG){
					pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
					value=-value;
				}else if(pre_operation==PreOperation::PRE_OPERATION_BWC){
					pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
					value=~value;
				}

				delete (zs_int *)const_obj;
				byte_code = BYTE_CODE_LOAD_INT;

			}else if((const_obj=Float::parse(default_str_value))!=NULL){ // float literal
				zs_float value_flt = *((zs_float *)const_obj);

				if(pre_operation==PreOperation::PRE_OPERATION_NEG){
					pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
					value_flt=-value_flt;
				}else if(pre_operation==PreOperation::PRE_OPERATION_BWC){
						EVAL_ERROR_FILE_LINE(
							compiler_data->current_parsing_file
							,line
							,"Bitwise complement (~) not allowed for constant float '%f'"
							,value_flt
						);
				}

				delete (zs_float *)const_obj;
				ZS_FLOAT_COPY(&value,&value_flt);
				byte_code = BYTE_CODE_LOAD_FLOAT;
			}
			else{
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Unable to parse literal '%s'",default_str_value.toConstChar());
			}

		}else{ // if not number,integer, hex, bit then is a literal String, boolean or identifier...

			 if(compiler_data->error){
				 return NULL;
			 }

			 aux=(char *)start_word;
			 // try compiler identifier, boolean, String ...
			char pre=0;
			if(*aux=='\"'){

				do{
					aux++;
				}while(!((*aux=='\"' && *(aux-1)!= '\\')|| *aux==0 || *aux=='\n'));

				if(*aux!='\"'){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"Constant string not ends with '\"'");
				}

				if((start_word+1)<aux){ // copy string without double quotes...
					default_str_value=compiler_copy_const_char_diff(start_word+1,aux);
				}
				aux++;
				value=(zs_int)compiler_data->script_engine->registerStkConstantStringScriptObject(String("\"")+default_str_value+"\"",default_str_value);
				byte_code = BYTE_CODE_LOAD_STRING;
			}else{ // is null,boolean or identifier
				bool end=false;
				while(!end){
					pre=*aux;
					default_str_value += (*aux++);
					//Operator token_operator=is_operator(aux);
					if(is_end_symbol_token(aux,pre)){
						// check if not instanceof and in operator
						end=true;
					}
				}

				if(default_str_value=="undefined"){ // undefined literal
					byte_code=BYTE_CODE_LOAD_UNDEFINED;
				}else if(default_str_value=="null"){ // null literal
						byte_code=BYTE_CODE_LOAD_NULL;
				}else if((const_obj=Boolean::parse(default_str_value))!=NULL){ // bool literal

					bool value_bool = *((bool *)const_obj);
					if(pre_operation==PreOperation::PRE_OPERATION_NOT){
						pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
						value_bool=!value_bool;
						//default_str_value="!"+default_str_value;
					}

					delete (bool *)const_obj;
					value = value_bool;
					byte_code=BYTE_CODE_LOAD_BOOL;
				}else{ // it's an identifier token  ...
					token_node_symbol->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
					byte_code = BYTE_CODE_FIND_VARIABLE;

					if(default_str_value == SYMBOL_VALUE_THIS || default_str_value == SYMBOL_VALUE_SUPER){

						if(default_str_value == SYMBOL_VALUE_SUPER){
							char *test=aux;
							IGNORE_BLANKS(test,compiler_data,aux,line);
							if(*test != '('){
								EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"'super' only allowed as function");
							}

							byte_code=BYTE_CODE_SUPER_CALL;
						}else{
							byte_code= BYTE_CODE_LOAD_THIS;// INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;
						}
					}else{
						Symbol *local_symbol=NULL;

						// should be an identifier and should be find after compiler function or at runtime...
						if(check_identifier_name_expression_ok(
							compiler_data
							,default_str_value
							,line
						)==FALSE){
							Operator op;
							if((op=is_operator(default_str_value.toConstChar()))!=Operator::ZS_OPERATOR_UNKNOWN){
								EVAL_ERROR_FILE_LINE(
									compiler_data->current_parsing_file
									,line
									,"unexpected operator '%s'"
									,compiler_data_operators[op].str
								);

							}else{

								EVAL_ERROR_FILE_LINE(
									compiler_data->current_parsing_file
									,line
									,"expected identifier after '%s'"
									,default_str_value.toConstChar()
								);
							}
						}

						if((local_symbol=compiler_find_local_symbol(compiler_data,scope_info,default_str_value)) != NULL){ // local sy

							if(local_symbol->properties & SYMBOL_PROPERTY_SCRIPT_FUNCTION){
								byte_code= BYTE_CODE_LOAD_FUNCTION;
								value=(zs_int)local_symbol;

							}else if(local_symbol->properties & SYMBOL_PROPERTY_SCRIPT_TYPE){
								byte_code= BYTE_CODE_LOAD_TYPE;
								value=-1;
							}else{

								byte_code= BYTE_CODE_LOAD_LOCAL;
								value=local_symbol->idx_position;

								if((local_symbol->properties & SYMBOL_PROPERTY_ARG_BY_REF) == SYMBOL_PROPERTY_ARG_BY_REF){
									byte_code= BYTE_CODE_LOAD_REF;
								}
							}
						}else{
							if(compiler_data->current_function->script_function->id==ZS_IDX_FUNCTION_MAIN){
								EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Symbol '%s' not defined",default_str_value.toConstChar());
							}
							// else it remains as variable to be found in the post compileruation
						}
					}
				}
			}
		 }

		token_node_symbol->value = default_str_value;
		token_node_symbol->compiler_instructions.push(
			compiler_instruction=new CompilerInstruction(
				byte_code
				, INSTRUCTION_VALUE_OP1_NOT_DEFINED
				,value
			)
		);

		return aux;
		// POST: token as literal or identifier
	}


}


