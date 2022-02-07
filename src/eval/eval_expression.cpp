/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	bool is_end_expression(const char *s){
		 return *s==')' || *s==','||  *s==']' || *s==';' || *s == 0 || *s=='}';
	}

	int    is_end_expression_or_keyword(EvalData *eval_data,const char * s,int line){
		Keyword op=eval_is_keyword(s);
		return !is_anonymous_function(eval_data,s,line) && (is_end_expression(s) || (op<Keyword::KEYWORDS_WITHIN_EXPRESSIONS && op !=Keyword::KEYWORD_UNKNOWN));
	}

	bool is_end_expression_with_cr(const char * s){
		return false;
	}

	void eval_deallocate_tokens(zs_vector 	& token_nodes){
		for(unsigned i=0; i < token_nodes.count; i++){
			delete (TokenNode *)token_nodes.items[i];
		}

		token_nodes.clear();
	}

	char * eval_sub_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, zs_vector 	* eval_instructions
			, const char *expected_ending_char
			, uint16_t _properties
			, int n_recursive_level
			, zs_vector *unique_call_instruction
		){
		// PRE: s is current zs_string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.
		zs_vector token_nodes;
		Keyword keyword_type;
		//int last_line_ok=0;
		zs_string identifier_value="";
		Operator operator_type = Operator::OPERATOR_UNKNOWN;
		TokenNode 	*operator_token_node=NULL, *last_operator_token_node=NULL;


		int last_line_ok;
		const char *start_expression_str=NULL;
		int start_expression_line=-1;
		int instruction_identifier=IDX_ZS_UNDEFINED;
		char *aux_p=NULL;//,*test_s=NULL;
		bool new_line_break=false;
		char *test_ignore_blanks=NULL,*test_char_carry_return=NULL;

		zs_string error_accessor_tokens;


		IGNORE_BLANKS(aux_p,eval_data,s,line);

		start_expression_str=aux_p;
		start_expression_line=line;

		int idx_instruction_start_expression=(int)eval_instructions->count;

		if(is_end_expression(aux_p) && *aux_p != ';'){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Unexpected '%c'",*aux_p);
		}

		if(!is_end_expression_or_keyword(eval_data,aux_p,line)){

			for(;;){ // it eats identifier/constant operator, etc

				if((aux_p = eval_expression_token_symbol(
						eval_data
						,aux_p
						,line
						,scope_info
						,&token_nodes
						,last_operator_token_node
						,_properties
						,n_recursive_level
						))==NULL){
					goto eval_error_sub_expression;
				}

				new_line_break=false;
				last_line_ok=line;
				IGNORE_BLANKS(test_ignore_blanks,eval_data,aux_p,last_line_ok);

				if((test_char_carry_return=strchr(aux_p,'\n'))!=NULL){ // new line is like ';'
					new_line_break=test_char_carry_return<test_ignore_blanks;
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				operator_type=is_operator(aux_p);

				keyword_type=eval_is_keyword(aux_p);

				if(	is_end_expression_or_keyword(eval_data,aux_p,line)
				|| operator_type==Operator::OPERATOR_TERNARY_IF
				|| operator_type==Operator::OPERATOR_TERNARY_ELSE
				|| ((operator_type==Operator::OPERATOR_ASSIGN) && (_properties & EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR))
				|| ((operator_type==Operator::OPERATOR_IN) && (_properties & EVAL_EXPRESSION_FOR_IN_VARIABLES))
				|| (_properties & EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL)
				|| ( new_line_break && (operator_type==Operator::OPERATOR_UNKNOWN ))){ // if not operator and carry return found is behaves as end expression
					break;
				}

				if(operator_type==Operator::OPERATOR_UNKNOWN){
					EVAL_ERROR_FILE_LINE_AND_GOTO(eval_error_sub_expression,eval_data->current_parsing_file,line ,"Expected operator after '%s'"
							,((TokenNode *)token_nodes.items[token_nodes.count-1])->value.c_str());
				}

				operator_token_node=new TokenNode();
				operator_token_node->line=line;
				operator_token_node->operator_type=operator_type;
				operator_token_node->token_type=TokenType::TOKEN_TYPE_OPERATOR;

				last_operator_token_node=operator_token_node;

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_operators[operator_type].str),line);

				// push operator token
				token_nodes.push_back((zs_int)(
					operator_token_node
				));
			}
		}

		if(eval_data->error){
			goto eval_error_sub_expression;
		}

		if(expected_ending_char != NULL) { // throw error ...
			zs_string expected_ending_str="Expected ";
			bool found=false;

			char *it=(char *)expected_ending_char;

			while(*it!=0 && found==false){

				if(*aux_p!=*it){
					if(it>expected_ending_char){
						*(it+1) == 0 ?
								expected_ending_str+=" or ":
								expected_ending_str+=" , ";


					}
					expected_ending_str+=zs_string("\"")+*it+"\"";
				}
				else {
					found=true;
				}

				it++;
			}

			if(found == false){
				size_t len=aux_p-start_expression_str;
				EVAL_ERROR_FILE_LINE_AND_GOTO(
						eval_error_sub_expression
						,eval_data->current_parsing_file
						,start_expression_line
						,"%s at the end of expression %10s..."
						,expected_ending_str.c_str(),zs_string(start_expression_str).substr(0,len).c_str()
				);
			}
		}

		if(aux_p==0){
			EVAL_ERROR_FILE_LINE_AND_GOTO(eval_error_sub_expression,eval_data->current_parsing_file,line ,"Unexpected end of file");
		}

		// here convert each expression token to byte code
		if(token_nodes.count>0){

			if((aux_p=eval_expression_to_byte_code(
				eval_data
				,aux_p
				,line
				,scope_info
				,eval_instructions
				,&token_nodes
				//,only_call_instructions
				,_properties
				,n_recursive_level
				,unique_call_instruction
			))==NULL){
				goto eval_error_sub_expression;
			}
		}

		eval_deallocate_tokens(token_nodes);

		// last character is a separator so it return increments by 1
		return aux_p;

eval_error_sub_expression:

		for(unsigned kk=0;kk<token_nodes.count;kk++){
			TokenNode *node=(TokenNode *)token_nodes.items[kk];
			if(node->are_instructions_moved == false){ // it means that instructions was not saved in instructions vector yet

				for(unsigned jj=0;jj<node->eval_instructions.count;jj++){
					delete (EvalInstruction *)node->eval_instructions.items[jj];
				}
			}
		}

		eval_deallocate_tokens(token_nodes);

		return NULL;

	}

	bool eval_check_all_instruction_only_load_op(zs_vector * eval_instructions){
		// is load or find variable
		for(unsigned i=0;i < eval_instructions->count; i++){
			Instruction *ei=&((EvalInstruction *)eval_instructions->items[i])->vm_instruction;
			if(((byte_code_is_load_var_type(ei->byte_code) || ei->byte_code == BYTE_CODE_FIND_VARIABLE))==false){
				return false;
			}
		}
		return true;
	}

	char * eval_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, zs_vector 	* dst_instructions
			, const char *expected_ending_char
			, uint16_t _properties

		){
		uint16_t additional_properties_first_recursive=_properties&EVAL_EXPRESSION_FOR_IN_VARIABLES?EVAL_EXPRESSION_FOR_IN_VARIABLES:0;
		zs_vector  ei_ternary_end_jmp;
		zs_vector 	zs_ei_left_sub_expressions; // we will write all instructions here as aux, and later will assign to dst_instructions
		zs_vector 	zs_ei_right_sub_expressions; // right/left assigment

		EvalInstruction *ei_last=NULL;
		bool not_assignment=false;
		zs_vector *ei_first_sub_expression=new zs_vector;

		zs_ei_left_sub_expressions.push_back((zs_int)(
			ei_first_sub_expression
		));

		char *aux_p=eval_sub_expression(
			eval_data
			, s
			, line
			, scope_info
			, (zs_vector *)zs_ei_left_sub_expressions.items[0]
			, expected_ending_char
			, _properties
		);

		if(aux_p == NULL){
			goto eval_error_expression_delete_left_right_sub_expressions;
		}

		// ok this is not the end...
		if(((_properties & EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION)!=0) && (*aux_p == ',')){
			// preserve each set of instructions of each expressions
			zs_vector *expression=NULL;

			int idx=0;
			bool only_load_left_expression=eval_check_all_instruction_only_load_op((zs_vector *)zs_ei_left_sub_expressions.items[0]);
			zs_vector unique_call_instruction;

			do{
				uint16_t properties_multi_expression=_properties;
				zs_vector *ptr_unique_call_instruction=NULL;

				if(idx==0) { // left expressions
					zs_ei_left_sub_expressions.push_back((zs_int)(
							expression=new zs_vector()
					));
					properties_multi_expression|=additional_properties_first_recursive;

					if(only_load_left_expression == true){
						properties_multi_expression|=EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR;
					}

				}else{ // right expressions
					zs_ei_right_sub_expressions.push_back((zs_int)(
							expression=new zs_vector()
					));

					if(zs_ei_right_sub_expressions.count==1 && only_load_left_expression==true){ // valid multiassingment
						ptr_unique_call_instruction=&unique_call_instruction;
					}
				}


				// starting performing expressions
				if((aux_p=eval_sub_expression(
					eval_data
					,aux_p+1 // +1 ignore ',' or '='
					, line
					, scope_info
					, expression // it's saving to instructions...
					,NULL
					,properties_multi_expression
					,0
					,ptr_unique_call_instruction // max assignments left
				))==NULL){
					goto eval_error_expression_delete_left_right_sub_expressions;
				}

				// special case to break
				if((additional_properties_first_recursive & EVAL_EXPRESSION_FOR_IN_VARIABLES) && (is_operator(aux_p) == OPERATOR_IN)){
					break;
				}

				only_load_left_expression&=eval_check_all_instruction_only_load_op((zs_vector *)zs_ei_left_sub_expressions.items[0]);

				if(aux_p != NULL && *aux_p != 0 && *aux_p=='=' && (only_load_left_expression==true)){ // assignment op, start left assigments
					if(zs_ei_left_sub_expressions.count >= FUNCTION_RETURN_COUNT_MAX){
						EVAL_ERROR_FILE_LINE_AND_GOTO(
								eval_error_expression_delete_left_right_sub_expressions
								,eval_data->current_parsing_file
								,line
							,"Reached max assigment count (max: %i)"
							,FUNCTION_RETURN_COUNT_MAX
						);
					}
					idx++; //--> start next
				}

			}while(aux_p != NULL && *aux_p != 0 && (*aux_p==',' || *aux_p=='=') );

			if(zs_ei_right_sub_expressions.count==1 && unique_call_instruction.count > 0){ // assign maximum return for this calls
				for(unsigned i=0; i < unique_call_instruction.count; i++){
					EvalInstruction *ei_call=(EvalInstruction *)unique_call_instruction.items[i];
					INSTRUCTION_SET_RETURN_COUNT(&ei_call->vm_instruction,zs_ei_left_sub_expressions.count);
				}
			}

		}

		if(zs_ei_right_sub_expressions.count > 0){ // multi-assignment detected
			int right_size=(int)zs_ei_right_sub_expressions.count;
			int left_size=(int)zs_ei_left_sub_expressions.count;
			int max_size=right_size>left_size?right_size:left_size;

			// write right expressions in reverse order and the right one < left one, we push an null element
			for(int r=max_size-1; r >=0;r--){
				if(r<right_size){
					dst_instructions->concat(
						(zs_vector *)zs_ei_right_sub_expressions.items[r]

					);
				}else{
					dst_instructions->push_back((zs_int)(
						new EvalInstruction(
							BYTE_CODE_LOAD_NULL
						)
					));

				}
			}

			// write left assignments...
			for(int l=0; l < left_size;l++){
				zs_vector *ei_left_sub_expressions=(zs_vector *)zs_ei_left_sub_expressions.items[l];
				Instruction *last_load_instruction=&((EvalInstruction *)ei_left_sub_expressions->items[ei_left_sub_expressions->count-1])->vm_instruction;

				if(byte_code_is_load_var_type(last_load_instruction->byte_code)){
					last_load_instruction->byte_code=byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
				}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
					last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
				}

				dst_instructions->concat(
						(zs_vector *)zs_ei_left_sub_expressions.items[l]
				);
			}

			// add final store instruction...
			dst_instructions->push_back((zs_int)(
				new EvalInstruction(
					BYTE_CODE_STORE
					,left_size
				)
			));

			// check if any left assignment is not literal ...
			for(int l=0; l < left_size;l++){
				zs_vector *left_sub_expression=(zs_vector *)zs_ei_left_sub_expressions.items[l];
				EvalInstruction *instruction = (EvalInstruction *)left_sub_expression->items[left_sub_expression->count-1];

				if(IS_BYTE_CODE_PUSH_STK_VARIABLE_TYPE(instruction->vm_instruction.byte_code) == false){
					const char *str_symbol=instruction->instruction_source_info.ptr_str_symbol_name==NULL?"unknow":instruction->instruction_source_info.ptr_str_symbol_name->c_str();
					EVAL_ERROR_FILE_LINE_AND_GOTO(eval_error_expression_delete_left_right_sub_expressions,eval_data->current_parsing_file,instruction->instruction_source_info.line
						,"'%s' is not allowed on left assignment multiple because '%s' is not literal. Left assignments has to be literals  (i.e a,b.c,b[0]. etc)"
						,str_symbol
						,str_symbol
					);
				}
			}

		}else{ // there's no assignment on the left --> make a reset stack in the end and write all instructions

			for(unsigned it=0;it<zs_ei_left_sub_expressions.count;it++){
				zs_vector *left_instructions=(zs_vector *)zs_ei_left_sub_expressions.items[it];

				if(left_instructions->count>0){
					// read first instruction
					EvalInstruction *ei=(EvalInstruction *)left_instructions->items[0];

					dst_instructions->concat(
						left_instructions
					);

				}
			}
		}

		// special case for catching vars for-in...
		if(dst_instructions->count>0){
			ei_last=(EvalInstruction *)dst_instructions->items[dst_instructions->count-1];
			if(
					IS_BYTE_CODE_CALL(ei_last->vm_instruction.byte_code)
						&&
				((_properties & EVAL_EXPRESSION_DO_NOT_RESET_STACK_LAST_CALL)==0)
			){
				ei_last->vm_instruction.properties|=INSTRUCTION_PROPERTY_RESET_STACK;
				//ei_last->vm_instruction.value_op1&=0xf; // if last op, no return parameters needed
			}else if(  IS_BYTE_CODE_STORE(ei_last->vm_instruction.byte_code)
					|| IS_BYTE_CODE_PRE_POST(ei_last->vm_instruction.byte_code)
			){
				ei_last->vm_instruction.properties|=INSTRUCTION_PROPERTY_RESET_STACK;
			}else if(IS_BYTE_CODE_LOAD(ei_last->vm_instruction.byte_code) && (_properties &EVAL_EXPRESSION_RESET_STACK_LAST_LOAD)){
				dst_instructions->push_back((zs_int)(
					new EvalInstruction(
						BYTE_CODE_RESET_STACK
					)
				));
			}
		}

		// erase all vectors ...
		for(unsigned it=0; it<zs_ei_left_sub_expressions.count; it++){
			delete (zs_vector *)zs_ei_left_sub_expressions.items[it];
		}

		for(unsigned it=0; it<zs_ei_right_sub_expressions.count; it++){
			delete (zs_vector *)zs_ei_right_sub_expressions.items[it];
		}


		return aux_p;

eval_error_expression_delete_left_right_sub_expressions:

		// we delete all instructions for left
		for(unsigned le=0; le<zs_ei_left_sub_expressions.count; le++){ // delete left expressions and vectors
			zs_vector *ie_left_sub_expression=(zs_vector *)zs_ei_left_sub_expressions.items[le];
			for(unsigned e=0 //delete expressions
					; e<ie_left_sub_expression->count
					; e++){
				EvalInstruction *ei=(EvalInstruction *)ie_left_sub_expression->items[e];
				delete ei;
			}

			delete ie_left_sub_expression;
		}

		for(auto re=0; re<zs_ei_right_sub_expressions.count; re++){ // delete right expressions and vectors
			zs_vector *ie_right_sub_expression=(zs_vector *)zs_ei_right_sub_expressions.items[re];
			for(auto e=0 //delete expressions
					; e!=ie_right_sub_expression->count
					; e++){
					delete (EvalInstruction *)ie_right_sub_expression->items[e];
			}

			delete ie_right_sub_expression;
		}

		return NULL;

	}
}

