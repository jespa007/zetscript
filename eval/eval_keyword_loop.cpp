#include "eval.h"


namespace zetscript{



	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// LOOPS
	//
	void link_loop_break_continues(EvalData *eval_data,int idx_start){

		int idx_end_instruction = eval_data->current_function->instructions.size();
		for(int i=idx_start; i < idx_end_instruction;i++){
			Instruction *ins=&eval_data->current_function->instructions[i]->vm_instruction;
			if(ins->value_op2 == ZS_IDX_INSTRUCTION_JMP_BREAK){
				ins->value_op2=idx_end_instruction-i;
			}else if(ins->value_op2 == ZS_IDX_INSTRUCTION_JMP_CONTINUE){
				ins->value_op2=i-idx_start;
			}
		}
	}

	char *eval_keyword_break(EvalData *eval_data,const char *s, int & line, Scope *scope_info){
		char *aux_p = (char *)s;
		Keyword key_w;

		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_BREAK){

			if(eval_data->current_function->parsing_loop <= 0 && eval_data->current_function->parsing_switch <= 0){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"break is only available on 'for', 'while' loops and switch");
			}

			int last_line_ok = line;

			//EvalInstruction *jmp_instruction;

			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

			// insert jmp instruction
			//eval_new_loop_jmp_break(eval_data);
			eval_data->current_function->instructions.push_back(
					new EvalInstruction(
						BYTE_CODE_JMP,
						ZS_IDX_UNDEFINED,
						ZS_IDX_INSTRUCTION_JMP_BREAK
					)
			);
		}
		return aux_p;
	}

	char *eval_keyword_continue(EvalData *eval_data,const char *s, int & line, Scope *scope_info){
		char *aux_p = (char *)s;
		Keyword key_w;

		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_CONTINUE){

			if(eval_data->current_function->parsing_loop <= 0){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"continue is only available on 'for' or 'while' loops");
			}

			int last_line_ok = line;
			//EvalInstruction *jmp_instruction;

			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

			eval_data->current_function->instructions.push_back(
				new EvalInstruction(
						BYTE_CODE_JMP,
						ZS_IDX_UNDEFINED,
						ZS_IDX_INSTRUCTION_JMP_CONTINUE
				)
			);

		}
		return aux_p;
	}

	char * eval_keyword_while(EvalData *eval_data,const char *s,int & line, Scope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		std::string start_symbol;
		Keyword key_w;
		int idx_instruction_conditional_while;
		EvalInstruction *ei_jnt; // conditional to end block

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_WHILE){

			aux_p += strlen(eval_data_keywords[key_w].str);
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// evaluate conditional line ...
			if(*aux_p != '('){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '(' while ");
			}

			// save current instruction to use later...
			idx_instruction_conditional_while=(int)(eval_data->current_function->instructions.size());

			end_expr = eval_expression(
					eval_data
					,aux_p+1
					,line
					,scope_info
					,&eval_data->current_function->instructions
					,std::vector<char>{')'}
			);

			// insert instruction if evaluated expression
			eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JNT));


			zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);

			IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected while-block open block ('{') ");
			}

			eval_data->current_function->parsing_loop++;

			if((aux_p=eval_block(
					eval_data
					,aux_p
					,line
					,scope_info)
			 	 ) == NULL){
				return NULL;
			}

			eval_data->current_function->parsing_loop--;

			// insert jmp instruction to begin condition while...
			eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_JMP,ZS_IDX_UNDEFINED,idx_instruction_conditional_while));

			// update jnt instruction to jmp after jmp instruction...
			ei_jnt->vm_instruction.value_op2=eval_data->current_function->instructions.size();

			// catch all breaks in the while...
			link_loop_break_continues(eval_data,idx_instruction_conditional_while);

			return aux_p;
		}
		return NULL;
	}

	char * eval_keyword_do_while(EvalData *eval_data,const char *s,int & line, Scope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		std::string start_symbol;
		Keyword key_w;
		int idx_do_while_start;
		int idx_do_while_conditional;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_DO_WHILE){

			aux_p += strlen(eval_data_keywords[key_w].str);

			//1st evaluate body ..
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// save current instruction to use later...
			idx_do_while_start=(int)(eval_data->current_function->instructions.size());


			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected open block ('{') in do-while expression");
			}

			eval_data->current_function->parsing_loop++;
			if((aux_p=eval_block(
					eval_data
					,aux_p
					,line
					,scope_info
			))==NULL){
				return NULL;
			}
			eval_data->current_function->parsing_loop--;

				// Finally evaluate conditional line ...
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = eval_is_keyword(aux_p);

			if(key_w!=KEYWORD_WHILE){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"expected while keyword");
			}

			aux_p += strlen(eval_data_keywords[key_w].str);

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p == '('){

				idx_do_while_conditional=(int)(eval_data->current_function->instructions.size());

				if((end_expr = eval_expression(
						eval_data
						,aux_p+1
						,line
						,scope_info
						,&eval_data->current_function->instructions
						,std::vector<char>{')'}
				)) != NULL){
					zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);
				}else{
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected ')' do-while expression");
				}
			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '(' do-while expression");
			}
			// insert jmp instruction to begin condition while...
			eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_JT,ZS_IDX_UNDEFINED,idx_do_while_start));

			// catch all breaks in the while...
			//link_breaks(eval_data);

			// catch all continues and evaluates bottom...
			link_loop_break_continues(eval_data,idx_do_while_start);


			return end_expr+1;
		}
		return NULL;
	}

	char * eval_keyword_for(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		Keyword key_w;
		int	 idx_instruction_for_start=ZS_IDX_UNDEFINED
			,idx_instruction_for_after_condition=ZS_IDX_UNDEFINED;

		EvalInstruction *ei_jnt=NULL; // conditional to end block
		std::vector<EvalInstruction *> post_operations;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_FOR){

			aux_p += strlen(eval_data_keywords[key_w].str);
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p == '('){ // ready ...

				idx_instruction_for_start=(int)(eval_data->current_function->instructions.size());

				// save scope pointer ...
				Scope *new_scope =eval_new_scope(eval_data,scope_info); // push current scope


				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				// 1. Iterator ...
				if(*aux_p != ';'){ // there's some var Init...
					// Init node ...
					Keyword key_w = eval_is_keyword(aux_p);

					if(key_w == KEYWORD_VAR){
						if((aux_p = eval_keyword_var(
								eval_data
								,aux_p
								,line
								,new_scope
						))==NULL){
							return NULL;
						}
					}
					else{
						if((aux_p = eval_expression(
								eval_data
								,aux_p
								,line
								,new_scope
								,&eval_data->current_function->instructions
								,{}
								,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_ON_MAIN_BLOCK
						))==NULL){
							return NULL;
						}
					}
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				key_w = eval_is_keyword(aux_p);
				if(key_w == Keyword::KEYWORD_IN){

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[Keyword::KEYWORD_IN].str),line);

					if((aux_p = eval_expression(
							eval_data
							,(const char *)aux_p
							,line
							,new_scope
							,&eval_data->current_function->instructions
					))==NULL){
						return NULL;
					}

					// init it and vector/object
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"CREATE ITERATOR TODOOO ");
					//eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_IT_INI));
				}
				else{ // expects conditional and post (i.e for(;;) )
					if(*aux_p != ';'){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected ';'");
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					if(*aux_p != ';'){ // conditional...
						char * end_p=NULL;
						IGNORE_BLANKS(end_p,eval_data,aux_p+1,line);

						if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

							idx_instruction_for_start=(int)(eval_data->current_function->instructions.size());

							if((aux_p = eval_expression(
									eval_data
									,(const char *)aux_p
									,line
									,new_scope
									,&eval_data->current_function->instructions
							))==NULL){
								return NULL;
							}

							eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JNT));

							idx_instruction_for_after_condition=(int)(eval_data->current_function->instructions.size());
						}
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p != ';'){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected ';'");

					}
					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					if(*aux_p != ')' ){ // finally do post op...
						if((aux_p = eval_expression(
							eval_data
							,(const char *)aux_p
							,line
							,new_scope
							,&post_operations
							,{}
							,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_ON_MAIN_BLOCK // it allows expression sequence and it does a reset stack in the end
						))==NULL){
							return NULL;
						}
					}
				}

				if(*aux_p != ')'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected ')'");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				if(*aux_p != '{'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '{' for begin block");
				}

				// eval block ...
				eval_data->current_function->parsing_loop++;
				if((aux_p=eval_block(
						eval_data
						,aux_p
						,line
						,new_scope
				))==NULL){
					return NULL;
				}
				eval_data->current_function->parsing_loop--;

				// insert post operations...
				eval_data->current_function->instructions.insert(
					eval_data->current_function->instructions.end()
					,post_operations.begin()
					,post_operations.end()
				);

				// insert jmp instruction to begin condition for...
				eval_data->current_function->instructions.push_back(
						new EvalInstruction(
							BYTE_CODE_JMP
							,ZS_IDX_UNDEFINED
							,-((int)(eval_data->current_function->instructions.size())-idx_instruction_for_start)
						)
				);

				// update jnt instruction to jmp after jmp instruction...
				if(ei_jnt != NULL){ // infinite loop
					ei_jnt->vm_instruction.value_op2=(eval_data->current_function->instructions.size()-idx_instruction_for_after_condition)+1;
				}

				// catch all continues and set all jmps after processing block but before post operation...
				link_loop_break_continues(eval_data,idx_instruction_for_start);

				// true: We treat declared variables into for as another scope.
				eval_check_scope(eval_data,new_scope);
				return aux_p;

			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '(' for");
			}
		}

		return NULL;
	}
}



