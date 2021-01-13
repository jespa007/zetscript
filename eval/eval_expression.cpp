namespace zetscript{

	namespace eval{




		bool is_end_expression(const char *s){
			 return *s==')' || *s==','||  *s==']' || *s==';' || *s == 0 || *s=='}';
		}

		int    is_end_expression_or_keyword(const char * s,EvalData *eval_data,int line){
			Keyword op=is_keyword(s);
			bool is_anonymouse_function=false;

			if(op==Keyword::KEYWORD_FUNCTION){
				char *check=NULL;
				IGNORE_BLANKS(check,eval_data,s+strlen(eval_data_keywords[op].str),line);

				is_anonymouse_function=*check=='(';
			}

			return !is_anonymouse_function && (is_end_expression(s) || (op<Keyword::KEYWORDS_WITHIN_EXPRESSIONS && op !=Keyword::KEYWORD_UNKNOWN));
		}

		bool is_end_expression_with_cr(const char * s){
			return false;
		}

		char * eval_expression_main(
				EvalData *eval_data
				,const char *s
				, int & line
				, Scope *scope_info
				, std::vector<EvalInstruction *> 	* instructions
				, std::vector<char> expected_ending_char
				, uint16_t properties // uint16_t properties
				, std::vector<EvalInstruction *> 	*only_call_instructions


			){
			// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
			// If this functions finds ';' then the function will generate ast.
			std::vector<TokenNode> expression_tokens;
			Keyword keyword_type;
			//int last_line_ok=0;
			std::string identifier_value="";
			Operator operator_type = Operator::OPERATOR_UNKNOWN;
			TokenNode 	operator_token_node;


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

			if(!is_end_expression_or_keyword(aux_p,eval_data,line)){

				for(;;){ // it eats identifier/constant operator, etc


					if((aux_p = eval_expression_token_symbol(eval_data,aux_p,line,scope_info,&expression_tokens))==NULL){
						goto error_expression;
					}

					new_line_break=false;
					last_line_ok=line;
					IGNORE_BLANKS(test_ignore_blanks,eval_data,aux_p,last_line_ok);

					if((test_char_carry_return=strchr(aux_p,'\n'))!=NULL){ // new line is like ';'
						new_line_break=test_char_carry_return<test_ignore_blanks;
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					operator_type=is_operator(aux_p);

					keyword_type=is_keyword(aux_p);

					if(	is_end_expression(aux_p)
					|| operator_type==Operator::OPERATOR_TERNARY_IF
					|| operator_type==Operator::OPERATOR_TERNARY_ELSE
					|| ((operator_type==Operator::OPERATOR_ASSIGN) && ((properties & EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR) == true))
					|| ( new_line_break && (operator_type==Operator::OPERATOR_UNKNOWN ))){ // if not operator and carry return found is behaves as end expression
						break;
					}

					if(operator_type==Operator::OPERATOR_UNKNOWN){
						/*switch(post_operation){
						case PostOperation::POST_OPERATION_INC:
							EVAL_ERROR_EXPRESSION_MAIN(eval_data->current_parsing_file,line ,"invalid post increment");
							break;
						case PostOperation::POST_OPERATION_DEC:
							EVAL_ERROR_EXPRESSION_MAIN(eval_data->current_parsing_file,line ,"invalid post decrement");
							break;
						default:*/
							EVAL_ERROR_EXPRESSION_MAIN(eval_data->current_parsing_file,line ,"Expected operator");
						/*	break;
						}*/
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_operators[operator_type].str),line);

					operator_token_node.line=line;
					operator_token_node.operator_type=operator_type;
					operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

					// push operator token
					expression_tokens.push_back(operator_token_node);
				}
			}

			if(eval_data->error){
				goto error_expression;
			}

			if(expected_ending_char.size() > 0) { // throw error...
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
					EVAL_ERROR_EXPRESSION_MAIN(eval_data->current_parsing_file,start_expression_line,"%s at the end of expression %10s...",expected_ending_str.c_str(),zs_strutils::substring(start_expression_str,0,len).c_str());
				}
			}

			if(aux_p==0){
				EVAL_ERROR_EXPRESSION_MAIN(eval_data->current_parsing_file,line ,"Unexpected end of file");
			}

			// there's an expression
			if(expression_tokens.size()>0){

				if((aux_p=eval_expression_to_byte_code(
					eval_data
					,aux_p
					,line
					,scope_info
					,instructions
					,&expression_tokens
					,only_call_instructions
				))==NULL){
					goto error_expression;
				}
			}

			// last character is a separator so it return increments by 1
			return aux_p;

error_expression:

			for(unsigned kk=0;kk<expression_tokens.size();kk++){
				if(expression_tokens[kk].are_instructions_moved == false){ // it means that instructions was not saved in instructions vector yet
					for(unsigned jj=0;jj<expression_tokens[kk].instructions.size();jj++){
						delete expression_tokens[kk].instructions[jj];
					}
				}
			}

			// erase all instructions in vector...
			for(unsigned i=0; i < instructions->size(); i++){
				delete instructions->at(i);
			}

			return NULL;

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
			std::vector<EvalInstruction *>  ternary_end_jmp;
			std::vector<std::vector<EvalInstruction *> *> 	left_expressions; // we will write all instructions here as aux, and later will assign to dst_instructions
			std::vector<std::vector<EvalInstruction *>*> 	expressions; // right/left assigment
			std::vector<EvalInstruction *> only_call_instructions;
			bool not_assignment=false;

			expressions.push_back(new std::vector<EvalInstruction *>);

			char *aux_p=eval_expression_main(
				eval_data
				, s
				, line
				, scope_info
				, expressions[0]
				, expected_ending_char
				, properties
			);

			if(aux_p == NULL){
				return NULL;
			}

			// ok this is not the end...
			if(((properties & EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION)!=0) && (*aux_p == ','))
			{
				// preserve each set of instructions of each expressions
				std::vector<EvalInstruction *> *expression;//[2]; // right/left assigment

				//aux_p++; // ignore ,

				int idx=0;
				int n_expression=1;

				do{

					if(idx==0) { // left expressions
						expressions.push_back(expression=new std::vector<EvalInstruction *>);
					}else{ // right expressions
						left_expressions.push_back(expression=new std::vector<EvalInstruction *>);
					}


					// starting performing expressions
					aux_p=eval_expression_main(
						eval_data
						,aux_p+1 // +1 ignore ,
						, line
						, scope_info
						, expression // it's saving to instructions...
						,{}
						,EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR
						, idx==1 &&  n_expression==0? &only_call_instructions:NULL
					);

					if(aux_p != NULL && *aux_p != 0 && *aux_p=='='){ // assignment op, start left assigments
						idx++; //--> start next
						n_expression=0;
					}

					n_expression++;

				}while(aux_p != NULL && *aux_p != 0 && *aux_p==',' );

			}

			if(left_expressions.size() > 0){ // multi-assignment
				int right_size=(int)expressions.size();
				int left_size=(int)left_expressions.size();

				for(int l=right_size; l < left_size;l++){ // push an expression with only undefiend byte code
					expressions.push_back(new std::vector<EvalInstruction *>({
						new EvalInstruction(
							BYTE_CODE_LOAD_UNDEFINED
						)
					}));
				}

				// right and left are balanced (for each left expression write right
				for(int i=0; i < left_size;i++){
					EvalInstruction *instruction = left_expressions[i]->at(left_expressions[i]->size()-1);

					if(IS_BYTE_CODE_LOAD_VARIABLE_TYPE(instruction->vm_instruction.byte_code) == false){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,instruction->instruction_source_info.line
							,"\"%s\" is not allowed on left assignment multiple because is %s. Left assignments has to be literals  (i.e a,b.c,b[0]. etc)"
							,eval_data->current_parsing_file,instruction->instruction_source_info.ptr_str_symbol_name->c_str()
							,"unknown"
						);
					}

					dst_instructions->insert(
							dst_instructions->end(),
							expressions[i]->begin(),
							expressions[i]->end()
					);


					dst_instructions->insert(
							dst_instructions->end(),
							left_expressions[i]->begin(),
							left_expressions[i]->end()
					);

					// add final store instruction...
					dst_instructions->push_back(
						new EvalInstruction(
							BYTE_CODE_STORE
						)
					);
					dst_instructions->push_back(
						new EvalInstruction(
							BYTE_CODE_POP_ONE
						)
					);


				}

				// write the rest right expressions,
				for(int r=left_size; r < right_size;r++){ // push an expression with only undefiend byte code
					if(properties & (EVAL_EXPRESSION_POP_ONE_ON_END_EXPRESSION)){ //
						expressions[r]->push_back(
							new EvalInstruction(
								BYTE_CODE_POP_ONE
							)
						);
					}

					dst_instructions->insert(
							dst_instructions->end(),
							expressions[r]->begin(),
							expressions[r]->end()
					);
				}

				// finally check special condition where there's function on the right and n vars on the left
				if(expressions.size() == 1){
					for(auto it=only_call_instructions.begin();it!=only_call_instructions.end();it++){
						(*it)->vm_instruction.value_op2=left_expressions.size(); // the number of assigments expect on the left
					}
				}

			}else{ // make a reset stack in the end and write all instructions
				for(auto it=expressions.begin();it!=expressions.end();it++){
					EvalInstruction *ei=(*it)->at((*it)->size()-1);
					if(properties & (EVAL_EXPRESSION_POP_ONE_ON_END_EXPRESSION)){ //
						(*it)->push_back(
							new EvalInstruction(
								BYTE_CODE_POP_ONE
							)
						);
					}
					// write all instructions to instructions pointer
					dst_instructions->insert(
						dst_instructions->end(),
						(*it)->begin(),
						(*it)->end()
					);
					//delete *it;
				}
			}

			// write all instructions to instructions pointer
			/*dst_instructions->insert(
					dst_instructions->end(),
					src_instructions.begin(),
					src_instructions.end()
			);*/

			// for each right expression, if not store then do pop one

			/*if(properties & (EVAL_EXPRESSION_POP_ONE_IF_NO_STORE)){ // reset stack at the end
				dst_instructions->insert(
						new EvalInstruction(BYTE_CODE_POP_ONE)
				);
			}*/

			// erase all vectors ...
			for(auto it=expressions.begin(); it!=expressions.end(); it++){
				delete *it;
				*it=NULL;
			}

			for(auto it=left_expressions.begin(); it!=left_expressions.end(); it++){
				delete *it;
				*it=NULL;
			}


			return aux_p;

		};
	}
}
