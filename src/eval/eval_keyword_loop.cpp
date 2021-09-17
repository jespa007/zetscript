#include "eval.h"


#define MAX_FOR_IN_VARIABLES	10

namespace zetscript{

	static int n_anonymous_iterator=0;

	zs_string eval_anonymous_iterator_name(){
		return "_@iter_"+zs_strutils::zs_int_to_str(n_anonymous_iterator++);
	}


	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// LOOPS
	//
	void link_loop_break_continues(EvalData *eval_data,int idx_start, int idx_post_instruction_for_start=ZS_IDX_UNDEFINED){

		int idx_end_instruction = eval_data->current_function->instructions.size();
		EvalInstruction **ei_it=&eval_data->current_function->instructions[idx_start];
		for(int i=idx_start; i < idx_end_instruction;i++,ei_it++){
			Instruction *ins=&(*ei_it)->vm_instruction;//&eval_data->current_function->instructions[i]->vm_instruction;
			if(ins->value_op1 == ZS_IDX_INSTRUCTION_JMP_BREAK){
				ins->value_op1=ZS_IDX_UNDEFINED;
				ins->value_op2=idx_end_instruction-i;
			}else if(ins->value_op1 == ZS_IDX_INSTRUCTION_JMP_CONTINUE){
				ins->value_op1=ZS_IDX_UNDEFINED;
				if(idx_post_instruction_for_start != ZS_IDX_UNDEFINED){
					ins->value_op2=idx_post_instruction_for_start-i;
				}
				else{
					ins->value_op2=idx_start-i;
				}
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
						ZS_IDX_INSTRUCTION_JMP_BREAK,
						0
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
						ZS_IDX_INSTRUCTION_JMP_CONTINUE,
						0
				)
			);

		}
		return aux_p;
	}

	char * eval_keyword_while(EvalData *eval_data,const char *s,int & line, Scope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		zs_string start_symbol;
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
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'while': Expected '(' ");
			}

			// save current instruction to use later...
			idx_instruction_conditional_while=(int)(eval_data->current_function->instructions.size());

			end_expr = eval_expression(
					eval_data
					,aux_p+1
					,line
					,scope_info
					,&eval_data->current_function->instructions
					,")"
			);

			// insert instruction if evaluated expression
			eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JNT));
			ei_jnt->instruction_source_info.file=eval_data->current_parsing_file;
			ei_jnt->instruction_source_info.line=line;



			zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);

			IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'while': Expected open block '{' ");
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
			eval_data->current_function->instructions.push_back(new EvalInstruction(
					BYTE_CODE_JMP
					,ZS_IDX_UNDEFINED
					,-((int)(eval_data->current_function->instructions.size())-idx_instruction_conditional_while)
				)
			);

			// update jnt instruction to jmp after jmp instruction...
			ei_jnt->vm_instruction.value_op2=eval_data->current_function->instructions.size()-idx_instruction_conditional_while-1; // +1 jmp

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
		zs_string start_symbol;
		Keyword key_w;
		int idx_do_while_start;
		int idx_do_while_conditional;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_DO_WHILE){
			EvalInstruction *ei_aux=NULL;

			aux_p += strlen(eval_data_keywords[key_w].str);

			//1st evaluate body ..
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// save current instruction to use later...
			idx_do_while_start=(int)(eval_data->current_function->instructions.size());


			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'do-while': Expected open block '{' ");
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
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'do-while': expected 'while' keyword");
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
						,")"
				)) != NULL){
					zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);
				}else{
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'do-while': Expected ')'");
				}
			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'do-while': Expected '('");
			}
			// insert jmp instruction to begin condition while...
			eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JT
					,ZS_IDX_UNDEFINED
					,-((int)(eval_data->current_function->instructions.size())-idx_do_while_start)
			));

			ei_aux->instruction_source_info.file=eval_data->current_parsing_file;
			ei_aux->instruction_source_info.line=line;

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
		bool is_for_in=false;
		int	 idx_instruction_for_start=ZS_IDX_UNDEFINED
			,idx_instruction_for_after_jnz_condition=ZS_IDX_UNDEFINED
			,idx_post_instruction_for_start=ZS_IDX_UNDEFINED;

		EvalInstruction *ei_jnt=NULL,*ei_jmp=NULL; // conditional to end block
		zs_vector<EvalInstruction *> post_operations;

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
					bool check_for_in=true;

					if(key_w == KEYWORD_VAR){
						check_for_in=false;
						char *aux_test=NULL;
						int line_test=line;
						if((aux_test = eval_keyword_var(
								eval_data
								,aux_p
								,line_test
								,new_scope
								, EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR
						))==NULL){
							return NULL;
						}

						// check if any instructions...
						check_for_in=
								(idx_instruction_for_start==((int)eval_data->current_function->instructions.size())
										&&
								(is_operator(aux_p))!=Operator::OPERATOR_IN);

						if(check_for_in  == true){ // restore
							aux_p+=strlen(eval_data_keywords[KEYWORD_VAR].str);
						}else{
							aux_p=aux_test;
							line=line_test;
						}
					}

					if(key_w != KEYWORD_VAR || check_for_in){
						zs_vector<EvalInstruction *> ei_init_vars_for;
						zs_vector<EvalInstruction> ei_init_vars_for_st[MAX_FOR_IN_VARIABLES];
						is_for_in=false;
						int n_for_in_vars=1;
						char *test_aux;
						int test_line=line;
						// eval expression and not optimize to convert load in op to load from iterator
						if((test_aux = eval_expression(
								eval_data
								,aux_p
								,test_line
								,new_scope
								,&ei_init_vars_for //eval_data->current_function->instructions
								,NULL
								,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_ON_MAIN_BLOCK | EVAL_EXPRESSION_FOR_IN_VARIABLES
						))==NULL){
							return NULL;
						}

						// copy per each variable detected...
						for(unsigned j=0; j<ei_init_vars_for.size();j++){
							if(n_for_in_vars<MAX_FOR_IN_VARIABLES){
								EvalInstruction *ins=ei_init_vars_for[j];
								if(ins->vm_instruction.byte_code == BYTE_CODE_RESET_STACK){
									if(j < (ei_init_vars_for.size()-1)){ // is not last variable
										n_for_in_vars++;
									}
								}else{
									ei_init_vars_for_st[n_for_in_vars-1].push_back(*ins);
								}
							}
							delete ei_init_vars_for[j];
						}

						if(n_for_in_vars>=MAX_FOR_IN_VARIABLES){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Error max for-in variables reached (Max: %i)",MAX_FOR_IN_VARIABLES);
						}

						ei_init_vars_for.clear();

						if(is_operator(test_aux)==Operator::OPERATOR_IN){
							EvalInstruction *ei_aux=NULL;
							is_for_in=true;
							// all check instructions
							for(int i=0; i < n_for_in_vars && is_for_in == true;i++){
								size_t end=ei_init_vars_for_st[i].size();// +2: expects operator in & reset stack at the end
								if(end == 0){
									is_for_in=false;
									break;
								}

								EvalInstruction *ptr_ei_init_vars_for_st=&ei_init_vars_for_st[i][0];

								for(unsigned j=0; j<end && is_for_in == true;j++){
									Instruction *ins=&ptr_ei_init_vars_for_st->vm_instruction;
									is_for_in&=(byte_code_is_load_type(ins->byte_code) || (ins->byte_code == BYTE_CODE_FIND_VARIABLE));
									ptr_ei_init_vars_for_st++;
								}

								if(is_for_in){
									Instruction *last_load_instruction=&ei_init_vars_for_st[i][end-1].vm_instruction;
									if(byte_code_is_load_type(last_load_instruction->byte_code)){
										last_load_instruction->byte_code=byte_code_load_to_push_stk(last_load_instruction->byte_code);
									}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
										last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
									}
								}



							}

							if(is_for_in){
								Symbol *symbol_iterator;
								Operator tst_op_aux;
								zs_vector<EvalInstruction *> ei_load_container_identifier;
								zs_vector<EvalInstruction> ei_load_container_identifier_st;
								EvalInstruction ei_iterator;
								// set aux_p as test_aux
								line=test_line;
								IGNORE_BLANKS(aux_p,eval_data,test_aux+strlen(eval_data_operators[OPERATOR_IN].str),line);



								if((tst_op_aux=is_operator(aux_p))!=OPERATOR_UNKNOWN){
									EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for-in': unexpected operator '%s' after 'in' ",eval_data_operators[tst_op_aux].str);
								}

								if((key_w=eval_is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
									EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for-in': unexpected '%s' after 'in' ",eval_data_keywords[key_w].str);
								}


								// eval iterator variable
								if((aux_p = eval_expression(
										eval_data
										,aux_p
										,line
										,new_scope
										,&ei_load_container_identifier //eval_data->current_function->instructions
										,{}
										,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
								))==NULL){
									// delete unusued vars_for
									return NULL;
								}

								for(unsigned j=0; j<ei_load_container_identifier.size();j++){
									ei_load_container_identifier_st.push_back(*ei_load_container_identifier[j]);
									delete ei_load_container_identifier[j];
								}

								if(*aux_p!=')'){
									if((tst_op_aux=is_operator(aux_p))!=OPERATOR_UNKNOWN){
										EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for-in': unexpected operator '%s' after container identifier ",eval_data_operators[tst_op_aux].str);
									}else{
										EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for-in': expected ')' after container identifier ");
									}
								}

								ei_init_vars_for.clear();

								// 1. create iterator symbol
								if((symbol_iterator=eval_data->current_function->script_function->registerLocalVariable(
										new_scope
										, eval_data->current_parsing_file
										, line
										, eval_anonymous_iterator_name()
									))==NULL){
									return NULL;
								};

								ei_iterator.vm_instruction.byte_code=BYTE_CODE_LOAD_LOCAL;
								ei_iterator.vm_instruction.value_op1=ZS_IDX_UNDEFINED;
								ei_iterator.vm_instruction.value_op2=symbol_iterator->idx_position;
								ei_iterator.symbol=*symbol_iterator;
								ei_iterator.instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data, symbol_iterator->name);


								// 2. emit iterator init
								for(unsigned i=0; i < ei_load_container_identifier_st.size(); i++){
									eval_data->current_function->instructions.push_back(
										new EvalInstruction(ei_load_container_identifier_st[i])
									);
								}

								eval_data->current_function->instructions.push_back(
										ei_aux=new EvalInstruction(ei_iterator)
								);

								// change load by push stk because we have to store iterator variable returned by iter()
								ei_aux->vm_instruction.byte_code=BYTE_CODE_PUSH_STK_LOCAL;

								eval_data->current_function->instructions.push_back(
										ei_aux=new EvalInstruction(
											BYTE_CODE_IT_INIT
									)
								);



								ei_aux->instruction_source_info=ei_load_container_identifier_st[ei_load_container_identifier_st.size()-1].instruction_source_info;


								idx_instruction_for_start=(int)(eval_data->current_function->instructions.size());

								// 3. emit iterator condition end
								eval_data->current_function->instructions.push_back(
										ei_aux=new EvalInstruction(ei_iterator)
								);


								// load object end symbol
								eval_data->current_function->instructions.push_back(
									ei_aux=new EvalInstruction(BYTE_CODE_LOAD_ELEMENT_OBJECT)
								);
								ei_aux->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data, "end");

								// call
								eval_data->current_function->instructions.push_back(
									new EvalInstruction(BYTE_CODE_CALL,0)
								);

								eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JT));
								ei_jnt->instruction_source_info.file=eval_data->current_parsing_file;
								ei_jnt->instruction_source_info.line=line;

								idx_instruction_for_after_jnz_condition=(int)(eval_data->current_function->instructions.size());

								// push as many null as n left vars -1
								for(int i=0; i < n_for_in_vars-1;i++){
									eval_data->current_function->instructions.push_back(
										ei_aux=new EvalInstruction(BYTE_CODE_LOAD_NULL)
									);
								}

								// load v
								eval_data->current_function->instructions.push_back(
										ei_aux=new EvalInstruction(ei_iterator)
								);

								// v._get
								eval_data->current_function->instructions.push_back(
									ei_aux=new EvalInstruction(BYTE_CODE_LOAD_ELEMENT_OBJECT)
								);

								ei_aux->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data, "get");

								// call, return all
								eval_data->current_function->instructions.push_back(
									new EvalInstruction(BYTE_CODE_CALL,0,0,INSTRUCTION_PROPERTY_RETURN_ALL_STACK)
								);


								// load k,v
								for(int i=n_for_in_vars-1; i >=0 ;i--){
									for(unsigned j=0; j<ei_init_vars_for_st[i].size();j++){
										eval_data->current_function->instructions.push_back(new EvalInstruction(ei_init_vars_for_st[i][j]));
									}
								}

								// store...
								eval_data->current_function->instructions.push_back(
									new EvalInstruction(BYTE_CODE_STORE,n_for_in_vars)
								);

								// 4. emit post operation
								post_operations.push_back(
										ei_aux=new EvalInstruction(ei_iterator)
								);

								// load object end symbol
								post_operations.push_back(
									ei_aux=new EvalInstruction(BYTE_CODE_LOAD_ELEMENT_OBJECT)
								);

								ei_aux->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data, "_post_inc");

								// call
								post_operations.push_back(
									new EvalInstruction(BYTE_CODE_CALL,0)
								);


							}else{ // is not for-in re eval for in with no in-break
								// delete
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
					}
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(is_for_in == false){
				  // expects conditional and post (i.e for(;;) )
					if(*aux_p != ';'){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for': Expected ';'");
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
							ei_jnt->instruction_source_info.file=eval_data->current_parsing_file;
							ei_jnt->instruction_source_info.line=line;


							idx_instruction_for_after_jnz_condition=(int)(eval_data->current_function->instructions.size());
						}
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p != ';'){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for': Expected ';'");

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
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for': Expected ')'");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				if(*aux_p != '{'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for': Expected '{' for begin block");
				}


				// increase the parsing_loop in order to take account for continues and breaks
				eval_data->current_function->parsing_loop++;

				// eval block ...
				if((aux_p=eval_block(
						eval_data
						,aux_p
						,line
						,new_scope
				))==NULL){
					return NULL;
				}

				// parsing_loop--
				eval_data->current_function->parsing_loop--;

				idx_post_instruction_for_start=eval_data->current_function->instructions.size();

				// insert post operations...
				eval_data->current_function->instructions.insert(
					eval_data->current_function->instructions.end()
					,post_operations.begin()
					,post_operations.end()
				);

				// insert jmp instruction to begin condition for...
				eval_data->current_function->instructions.push_back(
						ei_jmp=new EvalInstruction(
							BYTE_CODE_JMP
							,ZS_IDX_UNDEFINED
							,-((int)(eval_data->current_function->instructions.size())-idx_instruction_for_start)
						)
				);

				// update jnt instruction to jmp after jmp instruction...
				if(ei_jnt != NULL){ // infinite loop
					ei_jnt->vm_instruction.value_op2=(eval_data->current_function->instructions.size()-idx_instruction_for_after_jnz_condition)+1;
				}

				// catch all continues and set all jmps after processing block but before post operation...
				link_loop_break_continues(eval_data,idx_instruction_for_start,idx_post_instruction_for_start);

				// true: We treat declared variables into for as another scope.
				eval_check_scope(eval_data,new_scope);
				return aux_p;

			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'for': Expected '('");
			}
		}

		return NULL;
	}
}



