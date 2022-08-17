/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	bool is_end_expression(const char *_str_expression){
		 return *_str_expression==')' || *_str_expression==','||  *_str_expression==']' || *_str_expression==';' || *_str_expression == 0 || *_str_expression=='}';
	}

	int    is_end_expression_or_keyword(EvalData *eval_data,const char * _str_expression,int line){
		Keyword op=eval_is_keyword(_str_expression);
		return !is_anonymous_function(eval_data,_str_expression,line) && (is_end_expression(_str_expression) || (op<Keyword::KEYWORDS_WITHIN_EXPRESSIONS && op !=Keyword::KEYWORD_UNKNOWN));
	}

	void eval_deallocate_tokens(std::vector<TokenNode *> 	& token_nodes){
		for(unsigned i=0; i < token_nodes.size(); i++){
			delete (TokenNode *)token_nodes[i];
		}

		token_nodes.clear();
	}

	char * eval_sub_expression(
			EvalData *eval_data
			,const char *_str_expression
			, int & line
			, Scope *scope_info
			, std::vector<EvalInstruction *> 	* eval_instructions
			, const char *expected_ending_char
			, uint16_t _properties
			, std::vector<Instruction *> *unique_call_instruction
		){
		// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.
		std::vector<TokenNode *> token_nodes;
		Keyword keyword_type;
		//int last_line_ok=0;
		std::string identifier_value="";
		Operator operator_type = Operator::OPERATOR_UNKNOWN;
		TokenNode 	*operator_token_node=NULL, *last_operator_token_node=NULL;


		int last_line_ok;
		const char *start_expression_str=NULL;
		int start_expression_line=-1;
		char *aux_p=NULL;//,*test_s=NULL;
		bool new_line_break=false;
		char *test_ignore_blanks=NULL,*test_char_carry_return=NULL;

		std::string error_accessor_tokens;

		IGNORE_BLANKS(aux_p,eval_data,_str_expression,line);

		start_expression_str=aux_p;
		start_expression_line=line;

		if(
			   *_str_expression==')'
			|| *_str_expression==','
			|| *_str_expression==']'
			|| *_str_expression== 0
			|| *_str_expression=='}'
		){
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
				|| ((operator_type==Operator::OPERATOR_IN) && ((_properties & EVAL_EXPRESSION_FOR_IN_VARIABLES) || (_properties & EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR)))
				|| (_properties & EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL)
				|| ( new_line_break && (operator_type==Operator::OPERATOR_UNKNOWN ))){ // if not operator and carry return found is behaves as end expression
					break;
				}

				if(operator_type==Operator::OPERATOR_UNKNOWN){
					EVAL_ERROR_FILE_LINE_GOTO(
							eval_data->current_parsing_file
							,line
							,eval_error_sub_expression
							,"Expected operator after '%s'"
							,((TokenNode *)token_nodes[token_nodes.size()-1])->value.c_str());
				}

				operator_token_node=new TokenNode();
				operator_token_node->line=line;
				operator_token_node->operator_type=operator_type;
				operator_token_node->token_type=TokenType::TOKEN_TYPE_OPERATOR;

				last_operator_token_node=operator_token_node;

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_operators[operator_type].str),line);

				// push operator token
				token_nodes.push_back(
					operator_token_node
				);
			}
		}

		if(eval_data->error){
			goto eval_error_sub_expression;
		}

		if(expected_ending_char != NULL) { // throw error ...
			std::string expected_ending_str="Expected ";
			bool found=false;

			char *it=(char *)expected_ending_char;

			while(*it!=0 && found==false){

				if(*aux_p!=*it){
					if(it>expected_ending_char){
						*(it+1) == 0 ?
								expected_ending_str+=" or ":
								expected_ending_str+=" , ";


					}
					expected_ending_str+=std::string("\"")+*it+"\"";
				}
				else {
					found=true;
				}

				it++;
			}

			if(found == false){
				int len=aux_p-start_expression_str;
				EVAL_ERROR_FILE_LINE_GOTO(
						eval_data->current_parsing_file
						,start_expression_line
						,eval_error_sub_expression
						,"%s at the end of expression %10s..."
						,expected_ending_str.c_str(),std::string(start_expression_str).substr(0,len).c_str()
				);
			}
		}

		if(aux_p==0){
			EVAL_ERROR_FILE_LINE_GOTOF(
				eval_data->current_parsing_file
				,line
				,eval_error_sub_expression
				,"Unexpected end of file"
			);
		}

		// here convert each expression token to byte code
		if(token_nodes.size()>0){

			if((aux_p=eval_expression_to_byte_code(
				eval_data
				,aux_p
				,line
				,scope_info
				,eval_instructions
				,&token_nodes
				//,only_call_instructions
				,_properties
				,unique_call_instruction
			))==NULL){
				goto eval_error_sub_expression;
			}

		}

		eval_deallocate_tokens(token_nodes);

		// last character is a separator so it return increments by 1
		return aux_p;

eval_error_sub_expression:

		for(unsigned kk=0;kk<token_nodes.size();kk++){
			TokenNode *node=(TokenNode *)token_nodes[kk];
			if(node->are_instructions_moved == false){ // it means that instructions was not saved in instructions vector yet

				for(unsigned jj=0;jj<node->eval_instructions.size();jj++){
					delete (EvalInstruction *)node->eval_instructions[jj];
				}
			}
		}

		eval_deallocate_tokens(token_nodes);

		return NULL;

	}

	bool eval_check_all_instruction_only_load_op(std::vector<EvalInstruction *> * eval_instructions){
		// is load or find variable
		for(unsigned i=0;i < eval_instructions->size(); i++){
			Instruction *ei=&((EvalInstruction *)eval_instructions->at(i))->vm_instruction;
			if(((byte_code_is_load_var_type(ei->byte_code) || ei->byte_code == BYTE_CODE_FIND_VARIABLE))==false){
				return false;
			}
		}
		return true;
	}

	char * eval_expression(
			EvalData *eval_data
			,const char *_str_expression
			, int & line
			, Scope *scope_info
			, std::vector<EvalInstruction *> 	* dst_instructions
			, const char *expected_ending_char
			, uint16_t _properties

		){
		uint16_t additional_properties_first_recursive=_properties&EVAL_EXPRESSION_FOR_IN_VARIABLES?EVAL_EXPRESSION_FOR_IN_VARIABLES:0;
		std::vector<EvalInstruction *>  ei_ternary_end_jmp;
		std::vector<std::vector<EvalInstruction *> *> 	zs_ei_left_sub_expressions; // we will write all instructions here as aux, and later will assign to dst_instructions
		std::vector<std::vector<EvalInstruction *> *> 	zs_ei_right_sub_expressions; // right/left assigment

		EvalInstruction *ei_last=NULL;
		std::vector<EvalInstruction *> *ei_first_sub_expression=new std::vector<EvalInstruction *>;

		zs_ei_left_sub_expressions.push_back(
			ei_first_sub_expression
		);

		char *aux_p=eval_sub_expression(
			eval_data
			, _str_expression
			, line
			, scope_info
			, zs_ei_left_sub_expressions[0]
			, expected_ending_char
			, _properties
		);

		if(aux_p == NULL){
			goto eval_error_expression_delete_left_right_sub_expressions;
		}

		// ok this is not the end...
		if(((_properties & EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION)!=0) && (*aux_p == ',')){
			// preserve each set of instructions of each expressions
			std::vector<EvalInstruction *> *expression=NULL;

			int idx=0;
			bool only_load_left_expression=eval_check_all_instruction_only_load_op(zs_ei_left_sub_expressions[0]);
			std::vector<Instruction *> unique_call_instruction;

			do{
				uint16_t properties_multi_expression=_properties;
				std::vector<Instruction *> *ptr_unique_call_instruction=NULL;

				if(idx==0) { // left expressions
					zs_ei_left_sub_expressions.push_back(
							expression=new std::vector<EvalInstruction *>()
					);
					properties_multi_expression|=additional_properties_first_recursive;

					if(only_load_left_expression == true){
						properties_multi_expression|=EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR;
					}

				}else{ // right expressions
					zs_ei_right_sub_expressions.push_back(
							expression=new std::vector<EvalInstruction *>()
					);

					if(zs_ei_right_sub_expressions.size()==1 && only_load_left_expression==true){ // valid multiassingment
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
					,ptr_unique_call_instruction // max assignments left
				))==NULL){
					goto eval_error_expression_delete_left_right_sub_expressions;
				}

				// special case to break
				if((additional_properties_first_recursive & EVAL_EXPRESSION_FOR_IN_VARIABLES) && (is_operator(aux_p) == OPERATOR_IN)){
					break;
				}

				only_load_left_expression&=eval_check_all_instruction_only_load_op((std::vector<EvalInstruction *> *)zs_ei_left_sub_expressions[0]);

				if(aux_p != NULL && *aux_p != 0 && *aux_p=='=' && (only_load_left_expression==true)){ // assignment op, start left assigments
					if(zs_ei_left_sub_expressions.size() >= FUNCTION_RETURN_COUNT_MAX){
						EVAL_ERROR_FILE_LINE_GOTO(
							eval_data->current_parsing_file
							,line
							,eval_error_expression_delete_left_right_sub_expressions
							,"Reached max assigment count (max: %i)"
							,FUNCTION_RETURN_COUNT_MAX
						);
					}
					idx++; //--> start next
				}

			}while(aux_p != NULL && *aux_p != 0 && (*aux_p==',' || *aux_p=='=') );

			if(zs_ei_right_sub_expressions.size()==1 && unique_call_instruction.size() > 0){ // assign maximum return for this calls
				for(unsigned i=0; i < unique_call_instruction.size(); i++){
					EvalInstruction *ei_call=(EvalInstruction *)unique_call_instruction[i];
					INSTRUCTION_SET_RETURN_COUNT(&ei_call->vm_instruction,zs_ei_left_sub_expressions.size());
				}
			}

		}

		if(zs_ei_right_sub_expressions.size() > 0){ // multi-assignment detected
			int right_size=(int)zs_ei_right_sub_expressions.size();
			int left_size=(int)zs_ei_left_sub_expressions.size();
			int max_size=right_size>left_size?right_size:left_size;

			// LOADS
			// write right expressions in reverse order and the right one < left one, we push an null element
			for(int r=max_size-1; r >=0;r--){
				if(r<right_size){
					dst_instructions->insert(
						dst_instructions->end()
						,zs_ei_right_sub_expressions[r]->begin()
						,zs_ei_right_sub_expressions[r]->end()

					);
				}else{
					dst_instructions->push_back(
						new EvalInstruction(
							BYTE_CODE_LOAD_UNDEFINED
						)
					);

				}
			}

			// ASSIGMENTS
			// write left assignments...
			for(int l=0; l < left_size;l++){
				std::vector<EvalInstruction *> *ei_left_sub_expressions=(std::vector<EvalInstruction *> *)zs_ei_left_sub_expressions[l];
				Instruction *last_load_instruction=&((EvalInstruction *)ei_left_sub_expressions->at(ei_left_sub_expressions->size()-1))->vm_instruction;

				if(byte_code_is_load_var_type(last_load_instruction->byte_code)){
					last_load_instruction->byte_code=byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);

					if(
							last_load_instruction->byte_code==BYTE_CODE_PUSH_STK_VECTOR_ITEM
						|| last_load_instruction->byte_code==BYTE_CODE_PUSH_STK_THIS_VARIABLE
						|| last_load_instruction->byte_code==BYTE_CODE_PUSH_STK_OBJECT_ITEM

					){
						last_load_instruction->properties|=INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE;
					}

				}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
					last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
				}

				dst_instructions->insert(
						dst_instructions->end()
						,ei_left_sub_expressions->begin()
						,ei_left_sub_expressions->end()
				);

			}

			// add final store instruction...
			dst_instructions->push_back(
				new EvalInstruction(
					BYTE_CODE_STORE
					,left_size
				)
			);

			// check if any left assignment is not literal ...
			for(int l=0; l < left_size;l++){
				std::vector<EvalInstruction *> *left_sub_expression=(std::vector<EvalInstruction *> *)zs_ei_left_sub_expressions[l];
				EvalInstruction *instruction = (EvalInstruction *)left_sub_expression->at(left_sub_expression->size()-1);

				if(IS_BYTE_CODE_PUSH_STK_VARIABLE_TYPE(instruction->vm_instruction.byte_code) == false){
					const char *str_symbol=instruction->instruction_source_info.ptr_str_symbol_name==NULL?"unknow":instruction->instruction_source_info.ptr_str_symbol_name->c_str();
					EVAL_ERROR_FILE_LINE_GOTO(
						eval_data->current_parsing_file
						,instruction->instruction_source_info.line
						,eval_error_expression_delete_left_right_sub_expressions
						,"'%s' is not allowed on left assignment multiple because '%s' is not literal. Left assignments has to be literals  (i.e a,b.c,b[0]. etc)"
						,str_symbol
						,str_symbol
					);
				}
			}

		}else{ // there's no assignment on the left --> make a reset stack in the end and write all instructions

			for(unsigned it=0;it<zs_ei_left_sub_expressions.size();it++){
				std::vector<EvalInstruction *> *left_instructions=(std::vector<EvalInstruction *> *)zs_ei_left_sub_expressions[it];

				if(left_instructions->size()>0){
					// read first instruction
					dst_instructions->insert(
						dst_instructions->end()
						,left_instructions->begin()
						,left_instructions->end()
					);

				}

				ei_last=(EvalInstruction *)left_instructions->at(left_instructions->size()-1);

				if(ei_last->vm_instruction.byte_code==BYTE_CODE_STORE){
					EvalInstruction *eval_store_target=(EvalInstruction *)left_instructions->at(left_instructions->size()-1-1);
					if(
						eval_store_target->vm_instruction.byte_code==BYTE_CODE_PUSH_STK_VECTOR_ITEM
						|| eval_store_target->vm_instruction.byte_code==BYTE_CODE_PUSH_STK_THIS_VARIABLE
						|| eval_store_target->vm_instruction.byte_code==BYTE_CODE_PUSH_STK_OBJECT_ITEM

					){
						eval_store_target->vm_instruction.properties|=INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE;
					}
				}
			}
		}

		// special case for catching vars for-in...
		if(dst_instructions->size()>0){
			ei_last=(EvalInstruction *)dst_instructions->at(dst_instructions->size()-1);
			if(
					IS_BYTE_CODE_CALL(ei_last->vm_instruction.byte_code)
			){
				ei_last->vm_instruction.properties|=INSTRUCTION_PROPERTY_RESET_STACK;
				//ei_last->vm_instruction.value_op1&=0xf; // if last op, no return parameters needed
			}else if(  IS_BYTE_CODE_STORE(ei_last->vm_instruction.byte_code)
					|| IS_BYTE_CODE_PRE_POST(ei_last->vm_instruction.byte_code)
			){
				ei_last->vm_instruction.properties|=INSTRUCTION_PROPERTY_RESET_STACK;
			}else if(/*IS_BYTE_CODE_LOAD(ei_last->vm_instruction.byte_code) &&*/ (_properties &EVAL_EXPRESSION_RESET_STACK_LAST_LOAD)){
				dst_instructions->push_back(
					new EvalInstruction(
						BYTE_CODE_RESET_STACK
					)
				);
			}
		}

		// erase all vectors ...
		for(unsigned it=0; it<zs_ei_left_sub_expressions.size(); it++){
			delete (std::vector<EvalInstruction *> *)zs_ei_left_sub_expressions[it];
		}

		for(unsigned it=0; it<zs_ei_right_sub_expressions.size(); it++){
			delete (std::vector<EvalInstruction *> *)zs_ei_right_sub_expressions[it];
		}


		return aux_p;

eval_error_expression_delete_left_right_sub_expressions:

		// we delete all instructions for left
		for(unsigned le=0; le<zs_ei_left_sub_expressions.size(); le++){ // delete left expressions and vectors
			std::vector<EvalInstruction *> *ie_left_sub_expression=(std::vector<EvalInstruction *> *)zs_ei_left_sub_expressions[le];
			for(unsigned e=0 //delete expressions
					; e<ie_left_sub_expression->size()
					; e++){
				EvalInstruction *ei=(EvalInstruction *)ie_left_sub_expression->at(e);
				delete ei;
			}

			delete ie_left_sub_expression;
		}

		for(unsigned re=0; re<zs_ei_right_sub_expressions.size(); re++){ // delete right expressions and vectors
			std::vector<EvalInstruction *> *ie_right_sub_expression=(std::vector<EvalInstruction *> *)zs_ei_right_sub_expressions[re];
			for(unsigned e=0 //delete expressions
					; e!=ie_right_sub_expression->size()
					; e++){
					delete (EvalInstruction *)ie_right_sub_expression->at(e);
			}

			delete ie_right_sub_expression;
		}

		return NULL;

	}
}

