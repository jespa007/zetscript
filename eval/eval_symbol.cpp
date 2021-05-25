namespace zetscript{


	// to std::string utils ...
	char * eval_symbol(EvalData *eval_data
			,const char *start_word
			, int line
			,  Scope *scope_info
			,TokenNode * token_node_symbol
			, PreOperation & pre_operation
		//	, PostOperation post_operation
	){
		// PRE:
		bool is_constant_number=false, is_constant_boolean=false;
		EvalInstruction *instruction=NULL;
		zs_int value = ZS_IDX_UNDEFINED;
		ByteCode byte_code=ByteCode::BYTE_CODE_INVALID;
		token_node_symbol->token_type = TokenType::TOKEN_TYPE_LITERAL;
		token_node_symbol->line=line;
		void *get_obj=NULL,*const_obj=NULL;
		char *aux=(char *)start_word;
		std::string default_str_value="";
		 bool error=false;
//			 unsigned short instruction_properties=0;

		 if((aux=parse_literal_number(
				 eval_data
				 ,start_word
				 ,line
				 ,default_str_value
		))!=NULL){ // int/bool/float, etc

			if((const_obj=zs_strutils::parse_int(default_str_value))!=NULL){ // int literal
				value = *((zs_int *)const_obj);
				if(pre_operation==PreOperation::PRE_OPERATION_NEG){
					pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
					value=-value;
				}

				delete (zs_int *)const_obj;
				byte_code = ByteCode::BYTE_CODE_LOAD_ZS_INT;

			}else if((const_obj=zs_strutils::parse_float(default_str_value))!=NULL){ // float literal
				zs_float value_flt = *((zs_float *)const_obj);

				if(pre_operation==PreOperation::PRE_OPERATION_NEG){
					pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
					value_flt=-value_flt;
				}

				delete (zs_float *)const_obj;
				ZS_FLOAT_COPY(&value,&value_flt);
				byte_code = ByteCode::BYTE_CODE_LOAD_ZS_FLOAT;
			}
			else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Unable to parse literal \"%s\"",default_str_value.c_str());
			}

		}else{ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

			 if(eval_data->error){
				 return NULL;
			 }

			 aux=(char *)start_word;
			 // try eval identifier, boolean, std::string ...
			char pre=0;
			if(*aux=='\"'){

				do{
					aux++;
				}while(!((*aux=='\"' && *(aux-1)!= '\\')|| *aux==0 || *aux=='\n'));

				if(*aux!='\"'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Constant string not ends with '\"'");
				}

				if((start_word+1)<aux){ // copy string without double quotes...
					 zs_strutils::copy_from_ptr_diff(default_str_value,start_word+1,aux);
				}
				aux++;
				value=(zs_int)eval_data->zs->registerConstantScriptObjectString(default_str_value);
				byte_code = ByteCode::BYTE_CODE_LOAD_STRING;
			}else{ // is null,boolean or identifier
				bool end=false;
				while(!end){
					pre=*aux;
					default_str_value += (*aux++);

					if(is_end_symbol_token(aux,pre)){
						end=true;
					}
				}

				if(default_str_value=="null"){ // null literal
					byte_code=ByteCode::BYTE_CODE_LOAD_NULL;
				}else if((const_obj=zs_strutils::parse_bool(default_str_value))!=NULL){ // bool literal

					bool value_bool = *((bool *)const_obj);
					if(pre_operation==PreOperation::PRE_OPERATION_NOT){
						pre_operation=PreOperation::PRE_OPERATION_UNKNOWN; // --> already consumed
						value_bool=!value_bool;
						//default_str_value="!"+default_str_value;
					}

					delete (bool *)const_obj;
					value = value_bool;
					byte_code=ByteCode::BYTE_CODE_LOAD_BOOL;
				}else{ // it's an identifier token  ...
					token_node_symbol->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
					byte_code = ByteCode::BYTE_CODE_FIND_VARIABLE;

					if(default_str_value == SYMBOL_VALUE_THIS || default_str_value == SYMBOL_VALUE_SUPER){

						if(default_str_value == SYMBOL_VALUE_SUPER){
							char *test=aux;
							IGNORE_BLANKS(test,eval_data,aux,line);
							if(*test != '('){
								EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"\"super\" only allowed as function");
							}

							byte_code=BYTE_CODE_LOAD_ELEMENT_THIS;
						}else{
							byte_code= ByteCode::BYTE_CODE_LOAD_THIS;// INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;
						}
					}else{
						Symbol *vis=NULL;

						// should be an identifier and should be find after eval function or at runtime...
						if(check_identifier_name_expression_ok(
							eval_data
							,default_str_value
							,line
						)==FALSE){
							return NULL;
						}

						// try to find local or global
						/*if(scope_info == MAIN_SCOPE(eval_data)){ // symbol in global scope
							if( (vis=eval_find_global_symbol(eval_data,default_str_value) )!= NULL){ // global symbol found
								byte_code= ByteCode::BYTE_CODE_LOAD_GLOBAL;
								value=vis->idx_position;
							}
						}else*/ if((vis=eval_find_local_symbol(eval_data,scope_info,default_str_value)) != NULL){ // local sy
							byte_code= ByteCode::BYTE_CODE_LOAD_LOCAL;
							value=vis->idx_position;

							if((vis->properties & SYMBOL_PROPERTY_ARG_BY_REF) == SYMBOL_PROPERTY_ARG_BY_REF){
								byte_code= ByteCode::BYTE_CODE_LOAD_REF;
							}
						}

					}
				}
			}
		 }

		token_node_symbol->value = default_str_value;
		token_node_symbol->instructions.push_back(
			new EvalInstruction(
				byte_code
				,ZS_IDX_UNDEFINED
				,value
			)
		);

		return aux;
		// POST: token as literal or identifier
	}


}


