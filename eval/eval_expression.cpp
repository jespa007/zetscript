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

	char * eval_sub_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, std::vector<EvalInstruction *> 	* instructions
			, std::vector<char> expected_ending_char
			, uint16_t properties // uint16_t properties
			, int n_recursive_level
			//, std::vector<EvalInstruction *> 	*only_call_instructions


		){
		// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.
		std::vector<TokenNode> expression_tokens;
		Keyword keyword_type;
		//int last_line_ok=0;
		std::string identifier_value="";
		Operator operator_type = Operator::OPERATOR_UNKNOWN;
		TokenNode 	operator_token_node, last_operator_token_node;


		int last_line_ok;
		const char *start_expression_str=NULL;
		int start_expression_line=-1;
		int instruction_identifier=ZS_IDX_UNDEFINED;
		char *aux_p=NULL;//,*test_s=NULL;
		bool new_line_break=false;
		char *test_ignore_blanks=NULL,*test_char_carry_return=NULL;

		std::string error_accessor_tokens;


		IGNORE_BLANKS(aux_p,eval_data,s,line);

		start_expression_str=aux_p;
		start_expression_line=line;

		int idx_instruction_start_expression=(int)instructions->size();

		if(is_end_expression(aux_p) && *aux_p != ';'){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Unexpected '%c'",*aux_p);
		}

		if(!is_end_expression_or_keyword(eval_data,aux_p,line)){

			for(;;){ // it eats identifier/constant operator, etc

				if(last_operator_token_node.operator_type == Operator::OPERATOR_INSTANCEOF){ // retrieve a
					TokenNode token_node_type;
					std::string str_type;
					bool type_end=false;
					while(!type_end){
						char pre=*aux_p;
						str_type += (*aux_p++);

						if(is_end_symbol_token(aux_p,pre)){
							type_end=true;
						}
					}

					if(check_identifier_name_expression_ok(
						eval_data
						,str_type
						,line
					)==FALSE){
						goto eval_error_sub_expression;
					}

					token_node_type.instructions.push_back(
							new EvalInstruction(
									BYTE_CODE_LOAD_STRING,ZS_IDX_UNDEFINED,(zs_int)eval_data->zs->registerStkStringObject(str_type,str_type)
							)
					);

					expression_tokens.push_back(
							token_node_type
					);
					//last_operator_token_node->value=str_type;
					//EVAL_ERROR_EXPRESSION_TOKEN_SYMBOL(eval_data->current_parsing_file,line,"expected a class-type after 'instanceof' operator");
				}
				else if((aux_p = eval_expression_token_symbol(
						eval_data
						,aux_p
						,line
						,scope_info
						,&expression_tokens
						,&last_operator_token_node
						,properties
						,n_recursive_level))==NULL){
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

				if(	is_end_expression(aux_p)
				|| operator_type==Operator::OPERATOR_TERNARY_IF
				|| operator_type==Operator::OPERATOR_TERNARY_ELSE
				|| ((operator_type==Operator::OPERATOR_ASSIGN) && (properties & EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR))
				|| ((operator_type==Operator::OPERATOR_IN) && (properties & EVAL_EXPRESSION_FOR_IN_VARIABLES))
				|| ( new_line_break && (operator_type==Operator::OPERATOR_UNKNOWN ))){ // if not operator and carry return found is behaves as end expression
					break;
				}

				if(operator_type==Operator::OPERATOR_UNKNOWN){
					EVAL_ERROR_SUB_EXPRESSION(eval_data->current_parsing_file,line ,"Expected operator");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_operators[operator_type].str),line);

				operator_token_node.line=line;
				operator_token_node.operator_type=operator_type;
				operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

				last_operator_token_node=operator_token_node;

				// push operator token
				expression_tokens.push_back(operator_token_node);
			}
		}

		if(eval_data->error){
			goto eval_error_sub_expression;
		}

		if(expected_ending_char.size() > 0) { // throw error ...
			std::string expected_ending_str="Expected ";
			bool found=false;

			for(unsigned i=0; i < expected_ending_char.size() && found==false; i++){

				if(*aux_p!=expected_ending_char[i]){
					if(i>0){
						i == expected_ending_char.size()-1 ?
								expected_ending_str+=" or ":
								expected_ending_str+=" , ";


					}
					expected_ending_str+=std::string("\"")+expected_ending_char[i]+"\"";
				}
				else {
					found=true;
				}
			}

			if(found == false){
				size_t len=aux_p-start_expression_str;
				EVAL_ERROR_SUB_EXPRESSION(eval_data->current_parsing_file,start_expression_line,"%s at the end of expression %10s...",expected_ending_str.c_str(),zs_strutils::substring(start_expression_str,0,len).c_str());
			}
		}

		if(aux_p==0){
			EVAL_ERROR_SUB_EXPRESSION(eval_data->current_parsing_file,line ,"Unexpected end of file");
		}

		// here convert each expression token to byte code
		if(expression_tokens.size()>0){

			if((aux_p=eval_expression_to_byte_code(
				eval_data
				,aux_p
				,line
				,scope_info
				,instructions
				,&expression_tokens
				//,only_call_instructions
				,properties
				,n_recursive_level
			))==NULL){
				goto eval_error_sub_expression;
			}
		}

		// last character is a separator so it return increments by 1
		return aux_p;

eval_error_sub_expression:

		for(unsigned kk=0;kk<expression_tokens.size();kk++){
			if(expression_tokens[kk].are_instructions_moved == false){ // it means that instructions was not saved in instructions vector yet
				for(unsigned jj=0;jj<expression_tokens[kk].instructions.size();jj++){
					delete expression_tokens[kk].instructions[jj];
				}
			}
		}
		return NULL;

	}

	bool eval_check_all_instruction_only_load_op(std::vector<EvalInstruction *> * instructions){
		// is load or find variable
		for(unsigned i=0;i < instructions->size(); i++){
			Instruction *ei=&instructions->at(i)->vm_instruction;
			if(((byte_code_is_load_type(ei->byte_code) || ei->byte_code == BYTE_CODE_FIND_VARIABLE))==false){
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
			, std::vector<EvalInstruction *> 	* dst_instructions
			, std::vector<char> expected_ending_char
			, uint16_t properties

		){
		uint16_t additional_properties_first_recursive=properties&EVAL_EXPRESSION_FOR_IN_VARIABLES?EVAL_EXPRESSION_FOR_IN_VARIABLES:0;
		std::vector<EvalInstruction *>  ternary_end_jmp;
		std::vector<std::vector<EvalInstruction *> *> 	left_sub_expressions; // we will write all instructions here as aux, and later will assign to dst_instructions
		std::vector<std::vector<EvalInstruction *>*> 	right_sub_expressions; // right/left assigment
		//std::vector<EvalInstruction *> only_call_instructions;
		bool not_assignment=false;
		std::vector<EvalInstruction *> *first_sub_expression=new std::vector<EvalInstruction *>;

		left_sub_expressions.push_back(first_sub_expression);

		char *aux_p=eval_sub_expression(
			eval_data
			, s
			, line
			, scope_info
			, left_sub_expressions[0]
			, expected_ending_char
			, properties
		);

		if(aux_p == NULL){
			goto eval_error_expression_delete_left_right_sub_expressions;
		}

		// ok this is not the end...
		if(((properties & EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION)!=0) && (*aux_p == ','))
		{
			// preserve each set of instructions of each expressions
			std::vector<EvalInstruction *> *expression=NULL;//[2]; // right/left assigment

			int idx=0;
			bool only_load_left_expression=eval_check_all_instruction_only_load_op(left_sub_expressions[0]);

			do{

				if(idx==0) { // left expressions
					left_sub_expressions.push_back(expression=new std::vector<EvalInstruction *>);
				}else{ // right expressions
					right_sub_expressions.push_back(expression=new std::vector<EvalInstruction *>);
				}


				// starting performing expressions
				if((aux_p=eval_sub_expression(
					eval_data
					,aux_p+1 // +1 ignore ',' or '='
					, line
					, scope_info
					, expression // it's saving to instructions...
					,{}
					,properties | (idx==0?EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR|additional_properties_first_recursive:0)
				))==NULL){
					goto eval_error_expression_delete_left_right_sub_expressions;
				}

				// special case to break
				if((additional_properties_first_recursive & EVAL_EXPRESSION_FOR_IN_VARIABLES) && (is_operator(aux_p) == OPERATOR_IN)){
					break;
				}

				only_load_left_expression&=eval_check_all_instruction_only_load_op(left_sub_expressions[0]);

				if(aux_p != NULL && *aux_p != 0 && *aux_p=='=' && (only_load_left_expression==true)){ // assignment op, start left assigments
					idx++; //--> start next
				}

			}while(aux_p != NULL && *aux_p != 0 && (*aux_p==',' || *aux_p=='=') );

		}

		if(right_sub_expressions.size() > 0){ // multi-assignment
			int right_size=(int)right_sub_expressions.size();
			int left_size=(int)left_sub_expressions.size();
			int max_size=right_size>left_size?right_size:left_size;

			// write right expressions in reverse order and the right one < left one, we push an null element
			for(int r=max_size-1; r >=0;r--){
				if(r<right_size){
					dst_instructions->insert(
						dst_instructions->end(),
						right_sub_expressions[r]->begin(),
						right_sub_expressions[r]->end()
					);
				}else{
					dst_instructions->push_back(
						new EvalInstruction(
							BYTE_CODE_LOAD_NULL
						)
					);

				}
			}

			// write left assignments...
			for(int l=0; l < left_size;l++){

				Instruction *last_load_instruction=&left_sub_expressions[l]->at(left_sub_expressions[l]->size()-1)->vm_instruction;

				if(byte_code_is_load_type(last_load_instruction->byte_code)){
					last_load_instruction->byte_code=byte_code_load_to_push_stk(last_load_instruction->byte_code);
				}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
					last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
				}

				dst_instructions->insert(
						dst_instructions->end(),
						left_sub_expressions[l]->begin(),
						left_sub_expressions[l]->end()
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
				EvalInstruction *instruction = left_sub_expressions[l]->at(left_sub_expressions[l]->size()-1);

				if(IS_BYTE_CODE_PUSH_STK_VARIABLE_TYPE(instruction->vm_instruction.byte_code) == false){
					const char *str_symbol=instruction->instruction_source_info.ptr_str_symbol_name==NULL?"unknow":instruction->instruction_source_info.ptr_str_symbol_name->c_str();
					EVAL_ERROR_EXPRESSION(eval_data->current_parsing_file,instruction->instruction_source_info.line
						,"\"%s\" is not allowed on left assignment multiple because '%s' is not literal. Left assignments has to be literals  (i.e a,b.c,b[0]. etc)"
						,str_symbol
						,str_symbol
					);
				}
			}

		}else{ // make a reset stack in the end and write all instructions
			for(auto it=left_sub_expressions.begin();it!=left_sub_expressions.end();it++){
				// write all instructions to instructions pointer
				dst_instructions->insert(
					dst_instructions->end(),
					(*it)->begin(),
					(*it)->end()
				);

				// special case for catching vars for-in...
				if(properties & EVAL_EXPRESSION_FOR_IN_VARIABLES){
					dst_instructions->push_back(
						new EvalInstruction(
								BYTE_CODE_RESET_STACK
						)
					);
				}
			}
		}


		if(	((properties & (EVAL_EXPRESSION_ON_MAIN_BLOCK))!=0)
						&&
			// special case for catching vars for-in...
			((properties & (EVAL_EXPRESSION_FOR_IN_VARIABLES))==0)
		){ //
			dst_instructions->push_back(
				new EvalInstruction(
						BYTE_CODE_RESET_STACK
				)
			);
		}


//error_expression_delete_only_vectors:
		// erase all vectors ...
		for(auto it=left_sub_expressions.begin(); it!=left_sub_expressions.end(); it++){
			delete *it;
			*it=NULL;
		}

		for(auto it=right_sub_expressions.begin(); it!=right_sub_expressions.end(); it++){
			delete *it;
			*it=NULL;
		}


		return aux_p;

eval_error_expression_delete_left_right_sub_expressions:

		// we delete all instructions for left
		for(auto le=left_sub_expressions.begin(); le!=left_sub_expressions.end(); le++){ // delete left expressions and vectors
			for(auto e=(*le)->begin() //delete expressions
					; e!=(*le)->end()
					; e++){
					delete *e;
			}

			delete *le;
		}

		for(auto re=right_sub_expressions.begin(); re!=right_sub_expressions.end(); re++){ // delete right expressions and vectors
			for(auto e=(*re)->begin() //delete expressions
					; e!=(*re)->end()
					; e++){
					delete *e;
			}

			delete *re;
		}

		return NULL;

	};
}

