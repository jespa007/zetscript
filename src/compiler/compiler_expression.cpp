/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	bool is_end_expression(const char *_str_expression){
		 return *_str_expression==')' || *_str_expression==','||  *_str_expression==']' || *_str_expression==';' || *_str_expression == 0 || *_str_expression=='}';
	}

	int    is_end_expression_or_keyword(CompilerData *compiler_data,const char * _str_expression,int line){
		Keyword op=compiler_is_keyword(_str_expression);
		return !is_anonymous_function(compiler_data,_str_expression,line) && (is_end_expression(_str_expression) || (op<Keyword::KEYWORDS_WITHIN_EXPRESSIONS && op !=Keyword::KEYWORD_UNKNOWN));
	}

	void compiler_deallocate_tokens(Vector<TokenNode *> 	& token_nodes){
		for(int i=0; i < token_nodes.length(); i++){
			delete (TokenNode *)token_nodes.get(i);
		}

		token_nodes.clear();
	}

	char * compiler_sub_expression(
			CompilerData *compiler_data
			,const char *_str_expression
			, int & line
			, ScriptScope *scope_info
			, Vector<CompilerInstruction *> 	* compiler_instructions
			, const char *expected_ending_char
			, uint16_t _properties
			, Vector<Instruction *> *unique_call_instruction
		){
		// PRE: s is current String to compiler. This function tries to compiler an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.
		Vector<TokenNode *> token_nodes;
		String identifier_value="";
		Operator operator_type = Operator::ZS_OPERATOR_UNKNOWN;
		TokenNode 	*operator_token_node=NULL, *last_operator_token_node=NULL;


		int last_line_ok;
		const char *start_expression_str=NULL;
		int start_expression_line=-1;
		char *aux_p=NULL;//,*test_s=NULL;
		bool new_line_break=false;
		char *test_ignore_blanks=NULL,*test_char_carry_return=NULL;

		String error_accessor_tokens;

		IGNORE_BLANKS(aux_p,compiler_data,_str_expression,line);

		start_expression_str=aux_p;
		start_expression_line=line;

		if(
			   *_str_expression==')'
			|| *_str_expression==','
			|| *_str_expression==']'
			|| *_str_expression== 0
			|| *_str_expression=='}'
		){
			EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Unexpected '%c'",*aux_p);
		}

		if(!is_end_expression_or_keyword(compiler_data,aux_p,line)){

			for(;;){ // it eats identifier/constant operator, etc

				if((aux_p = compiler_expression_token_symbol(
						compiler_data
						,aux_p
						,line
						,scope_info
						,&token_nodes
						,last_operator_token_node
						,_properties
						))==NULL){
					goto compiler_error_sub_expression;
				}

				new_line_break=false;
				last_line_ok=line;
				IGNORE_BLANKS(test_ignore_blanks,compiler_data,aux_p,last_line_ok);

				if((test_char_carry_return=strchr(aux_p,'\n'))!=NULL){ // new line is like ';'
					new_line_break=test_char_carry_return<test_ignore_blanks;
				}

				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
				operator_type=is_operator(aux_p);

				if(	is_end_expression_or_keyword(compiler_data,aux_p,line)
				|| operator_type==Operator::ZS_OPERATOR_TERNARY_IF
				|| operator_type==Operator::ZS_OPERATOR_TERNARY_ELSE
				|| ((operator_type==Operator::ZS_OPERATOR_ASSIGN) && (_properties & EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR))
				|| ((operator_type==Operator::ZS_OPERATOR_IN) && ((_properties & EVAL_EXPRESSION_FOR_IN_VARIABLES) || (_properties & EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR)))
				|| (_properties & EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL)
				|| ( new_line_break && (operator_type==Operator::ZS_OPERATOR_UNKNOWN ))){ // if not operator and carry return found is behaves as end expression
					break;
				}

				if(operator_type==Operator::ZS_OPERATOR_UNKNOWN){
					char ch=*aux_p;
					if(is_special_char(&ch)){
						EVAL_ERROR_FILE_LINE_GOTO(
								compiler_data->current_parsing_file
								,line
								,compiler_error_sub_expression
								,"Unexpected '%c'"
								,ch);
					}else{

						EVAL_ERROR_FILE_LINE_GOTO(
								compiler_data->current_parsing_file
								,line
								,compiler_error_sub_expression
								,"Expected operator after '%s'"
								,((TokenNode *)token_nodes.get(token_nodes.length()-1))->value.toConstChar());
					}
				}

				operator_token_node=new TokenNode();
				operator_token_node->line=line;
				operator_token_node->operator_type=operator_type;
				operator_token_node->token_type=TokenType::TOKEN_TYPE_OPERATOR;

				last_operator_token_node=operator_token_node;

				IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_operators[operator_type].str),line);

				// push operator token
				token_nodes.push(
					operator_token_node
				);
			}
		}

		if(compiler_data->error){
			goto compiler_error_sub_expression;
		}

		if(expected_ending_char != NULL) { // throw error ...
			String expected_ending_str="Expected ";
			bool found=false;

			char *it=(char *)expected_ending_char;

			while(*it!=0 && found==false){

				if(*aux_p!=*it){
					if(it>expected_ending_char){
						*(it+1) == 0 ?
								expected_ending_str+=" or ":
								expected_ending_str+=" , ";


					}
					expected_ending_str+=String("\"")+*it+"\"";
				}
				else {
					found=true;
				}

				it++;
			}

			if(found == false){
				int len=aux_p-start_expression_str;
				EVAL_ERROR_FILE_LINE_GOTO(
						compiler_data->current_parsing_file
						,start_expression_line
						,compiler_error_sub_expression
						,"%s at the end of expression %10s..."
						,expected_ending_str.toConstChar(),String(start_expression_str).getSubstring(0,len).toConstChar()
				);
			}
		}

		if(aux_p==0){
			EVAL_ERROR_FILE_LINE_GOTOF(
				compiler_data->current_parsing_file
				,line
				,compiler_error_sub_expression
				,"Unexpected end of file"
			);
		}

		// here convert each expression token to byte code
		if(token_nodes.length()>0){

			if((aux_p=compiler_expression_to_byte_code(
				compiler_data
				,aux_p
				,line
				,scope_info
				,compiler_instructions
				,&token_nodes
				//,only_call_instructions
				,_properties
				,unique_call_instruction
			))==NULL){
				goto compiler_error_sub_expression;
			}

		}

		compiler_deallocate_tokens(token_nodes);

		// last character is a separator so it return increments by 1
		return aux_p;

compiler_error_sub_expression:

		for(int kk=0;kk<token_nodes.length();kk++){
			TokenNode *node=(TokenNode *)token_nodes.get(kk);
			if(node->are_instructions_moved == false){ // it means that instructions was not saved in instructions vector yet

				for(int jj=0;jj<node->compiler_instructions.length();jj++){
					delete (CompilerInstruction *)node->compiler_instructions.get(jj);
				}
			}
		}

		compiler_deallocate_tokens(token_nodes);

		return NULL;

	}

	bool compiler_check_all_instruction_only_load_op(Vector<CompilerInstruction *> * compiler_instructions){
		// is load or find variable
		for(int i=0;i < compiler_instructions->length(); i++){
			Instruction *ei=&((CompilerInstruction *)compiler_instructions->get(i))->vm_instruction;
			if(((compiler_is_byte_code_load_var_type(ei->byte_code) || ei->byte_code == BYTE_CODE_FIND_VARIABLE))==false){
				return false;
			}
		}
		return true;
	}

	char * compiler_expression(
			CompilerData *compiler_data
			,const char *_str_expression
			, int & line
			, ScriptScope *scope_info
			, Vector<CompilerInstruction *> 	* dst_instructions
			, const char *expected_ending_char
			, uint16_t _properties

		){
		uint16_t additional_properties_first_recursive=_properties&EVAL_EXPRESSION_FOR_IN_VARIABLES?EVAL_EXPRESSION_FOR_IN_VARIABLES:0;
		Vector<CompilerInstruction *>  ei_ternary_end_jmp;
		Vector<Vector<CompilerInstruction *> *> 	zs_ei_left_sub_expressions; // we will write all instructions here as aux, and later will assign to dst_instructions
		Vector<Vector<CompilerInstruction *> *> 	zs_ei_right_sub_expressions; // right/left assigment

		CompilerInstruction *ei_last=NULL;
		Vector<CompilerInstruction *> *ei_first_sub_expression=new Vector<CompilerInstruction *>;

		zs_ei_left_sub_expressions.push(
			ei_first_sub_expression
		);

		char *aux_p=compiler_sub_expression(
			compiler_data
			, _str_expression
			, line
			, scope_info
			, zs_ei_left_sub_expressions.get(0)
			, expected_ending_char
			, _properties
		);

		if(aux_p == NULL){
			goto compiler_error_expression_delete_left_right_sub_expressions;
		}

		// ok this is not the end...
		if(((_properties & EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION)!=0) && (*aux_p == ',')){
			// preserve each set of instructions of each expressions
			Vector<CompilerInstruction *> *expression=NULL;

			int idx=0;
			bool only_load_left_expression=compiler_check_all_instruction_only_load_op(zs_ei_left_sub_expressions.get(0));
			Vector<Instruction *> unique_call_instruction;

			do{
				uint16_t properties_multi_expression=_properties;
				Vector<Instruction *> *ptr_unique_call_instruction=NULL;

				if(idx==0) { // left expressions
					zs_ei_left_sub_expressions.push(
							expression=new Vector<CompilerInstruction *>()
					);
					properties_multi_expression|=additional_properties_first_recursive;

					if(only_load_left_expression == true){
						properties_multi_expression|=EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR;
					}

				}else{ // right expressions
					zs_ei_right_sub_expressions.push(
							expression=new Vector<CompilerInstruction *>()
					);

					if(zs_ei_right_sub_expressions.length()==1 && only_load_left_expression==true){ // valid multiassingment
						ptr_unique_call_instruction=&unique_call_instruction;
					}
				}


				// starting performing expressions
				if((aux_p=compiler_sub_expression(
					compiler_data
					,aux_p+1 // +1 ignore ',' or '='
					, line
					, scope_info
					, expression // it's saving to instructions...
					,NULL
					,properties_multi_expression
					,ptr_unique_call_instruction // max assignments left
				))==NULL){
					goto compiler_error_expression_delete_left_right_sub_expressions;
				}

				// special case to break
				if((additional_properties_first_recursive & EVAL_EXPRESSION_FOR_IN_VARIABLES) && (is_operator(aux_p) == ZS_OPERATOR_IN)){
					break;
				}

				only_load_left_expression&=compiler_check_all_instruction_only_load_op((Vector<CompilerInstruction *> *)zs_ei_left_sub_expressions.get(0));

				if(aux_p != NULL && *aux_p != 0 && *aux_p=='=' && (only_load_left_expression==true)){ // assignment op, start left assigments
					if(zs_ei_left_sub_expressions.length() >= ZS_SCRIPT_FUNCTION_RETURN_COUNT_MAX){
						EVAL_ERROR_FILE_LINE_GOTO(
							compiler_data->current_parsing_file
							,line
							,compiler_error_expression_delete_left_right_sub_expressions
							,"Reached max assigment count (max: %i)"
							,ZS_SCRIPT_FUNCTION_RETURN_COUNT_MAX
						);
					}
					idx++; //--> start next
				}

			}while(aux_p != NULL && *aux_p != 0 && (*aux_p==',' || *aux_p=='=') );

			if(zs_ei_right_sub_expressions.length()==1 && unique_call_instruction.length() > 0){ // assign maximum return for this calls
				for(int i=0; i < unique_call_instruction.length(); i++){
					CompilerInstruction *ei_call=(CompilerInstruction *)unique_call_instruction.get(i);
					INSTRUCTION_SET_RETURN_COUNT(&ei_call->vm_instruction,zs_ei_left_sub_expressions.length());
				}
			}

		}

		if(zs_ei_right_sub_expressions.length() > 0){ // multi-assignment detected
			int right_size=(int)zs_ei_right_sub_expressions.length();
			int left_size=(int)zs_ei_left_sub_expressions.length();
			int max_size=right_size>left_size?right_size:left_size;

			// LOADS
			// write right expressions in reverse order and the right one < left one, we push an null element
			for(int r=max_size-1; r >=0;r--){
				if(r<right_size){
					dst_instructions->concat(
						*zs_ei_right_sub_expressions.get(r)

					);
				}else{
					dst_instructions->push(
						new CompilerInstruction(
							BYTE_CODE_LOAD_UNDEFINED
						)
					);

				}
			}

			// ASSIGMENTS
			// write left assignments...
			for(int l=0; l < left_size;l++){
				Vector<CompilerInstruction *> *ei_left_sub_expressions=(Vector<CompilerInstruction *> *)zs_ei_left_sub_expressions.get(l);
				Instruction *last_load_instruction=&((CompilerInstruction *)ei_left_sub_expressions->get(ei_left_sub_expressions->length()-1))->vm_instruction;

				if(compiler_is_byte_code_load_var_type(last_load_instruction->byte_code)){
					last_load_instruction->byte_code=compiler_byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);

					if(
							last_load_instruction->byte_code==BYTE_CODE_PUSH_STK_ARRAY_ITEM
						|| last_load_instruction->byte_code==BYTE_CODE_PUSH_STK_THIS_VARIABLE
						|| last_load_instruction->byte_code==BYTE_CODE_PUSH_STK_OBJECT_ITEM

					){
						last_load_instruction->properties|=INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT;
					}

				}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
					last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
				}

				dst_instructions->concat(
						*ei_left_sub_expressions
				);

			}

			// add final store instruction...
			dst_instructions->push(
				new CompilerInstruction(
					BYTE_CODE_STORE
					,left_size
				)
			);

			// check if any left assignment is not literal ...
			for(int l=0; l < left_size;l++){
				Vector<CompilerInstruction *> *left_sub_expression=(Vector<CompilerInstruction *> *)zs_ei_left_sub_expressions.get(l);
				CompilerInstruction *instruction = (CompilerInstruction *)left_sub_expression->get(left_sub_expression->length()-1);

				if(BYTE_CODE_IS_PUSH_STK_VARIABLE_TYPE(instruction->vm_instruction.byte_code) == false){
					const char *str_symbol=instruction->instruction_source_info.ptr_str_symbol_name==NULL?"unknow":instruction->instruction_source_info.ptr_str_symbol_name;
					EVAL_ERROR_FILE_LINE_GOTO(
						compiler_data->current_parsing_file
						,instruction->instruction_source_info.line
						,compiler_error_expression_delete_left_right_sub_expressions
						,"'%s' is not allowed on left assignment multiple because '%s' is not literal. Left assignments has to be literals  (i.e a,b.c,b[0]. etc)"
						,str_symbol
						,str_symbol
					);
				}
			}

		}else{ // there's no assignment on the left --> make a reset stack in the end and write all instructions

			for(int it=0;it<zs_ei_left_sub_expressions.length();it++){
				Vector<CompilerInstruction *> *left_instructions=(Vector<CompilerInstruction *> *)zs_ei_left_sub_expressions.get(it);

				if(left_instructions->length()>0){
					// read first instruction
					dst_instructions->concat(
						*left_instructions
					);

				}

				if(left_instructions->length()>0){
					ei_last=(CompilerInstruction *)left_instructions->get(left_instructions->length()-1);

					if(ei_last->vm_instruction.byte_code==BYTE_CODE_STORE){
						CompilerInstruction *compiler_store_target=((CompilerInstruction *)left_instructions->get(left_instructions->length()-1-1));
						if(
							compiler_store_target->vm_instruction.byte_code==BYTE_CODE_PUSH_STK_ARRAY_ITEM
							|| compiler_store_target->vm_instruction.byte_code==BYTE_CODE_PUSH_STK_THIS_VARIABLE
							|| compiler_store_target->vm_instruction.byte_code==BYTE_CODE_PUSH_STK_OBJECT_ITEM

						){
							compiler_store_target->vm_instruction.properties|=INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT;
						}
					}
				}
			}
		}

		// special case for catching vars for-in...
		if(dst_instructions->length()>0){
			ei_last=(CompilerInstruction *)dst_instructions->get(dst_instructions->length()-1);
			if(
					BYTE_CODE_IS_CALL(ei_last->vm_instruction.byte_code)
			){
				ei_last->vm_instruction.properties|=INSTRUCTION_PROPERTY_RESET_STACK;
				//ei_last->vm_instruction.value_op1&=0xf; // if last op, no return parameters needed
			}else if(  BYTE_CODE_IS_STORE(ei_last->vm_instruction.byte_code)
					|| BYTE_CODE_IS_PRE_POST(ei_last->vm_instruction.byte_code)
			){
				ei_last->vm_instruction.properties|=INSTRUCTION_PROPERTY_RESET_STACK;
			}else if(/*BYTE_CODE_IS_LOAD_TYPE(ei_last->vm_instruction.byte_code) &&*/ (_properties &EVAL_EXPRESSION_RESET_STACK_LAST_LOAD)){
				dst_instructions->push(
					new CompilerInstruction(
						BYTE_CODE_RESET_STACK
					)
				);
			}
		}

		// erase all vectors ...
		for(int it=0; it<zs_ei_left_sub_expressions.length(); it++){
			delete (Vector<CompilerInstruction *> *)zs_ei_left_sub_expressions.get(it);
		}

		for(int it=0; it<zs_ei_right_sub_expressions.length(); it++){
			delete (Vector<CompilerInstruction *> *)zs_ei_right_sub_expressions.get(it);
		}


		return aux_p;

compiler_error_expression_delete_left_right_sub_expressions:

		// we delete all instructions for left
		for(int le=0; le<zs_ei_left_sub_expressions.length(); le++){ // delete left expressions and vectors
			Vector<CompilerInstruction *> *ie_left_sub_expression=(Vector<CompilerInstruction *> *)zs_ei_left_sub_expressions.get(le);
			for(int e=0 //delete expressions
					; e<ie_left_sub_expression->length()
					; e++){
				CompilerInstruction *ei=(CompilerInstruction *)ie_left_sub_expression->get(e);
				delete ei;
			}

			delete ie_left_sub_expression;
		}

		for(int re=0; re<zs_ei_right_sub_expressions.length(); re++){ // delete right expressions and vectors
			Vector<CompilerInstruction *> *ie_right_sub_expression=(Vector<CompilerInstruction *> *)zs_ei_right_sub_expressions.get(re);
			for(int e=0 //delete expressions
					; e!=ie_right_sub_expression->length()
					; e++){
					delete (CompilerInstruction *)ie_right_sub_expression->get(e);
			}

			delete ie_right_sub_expression;
		}

		return NULL;

	}
}

