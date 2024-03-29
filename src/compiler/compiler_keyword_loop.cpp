/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"

namespace zetscript{

	static int k_n_anonymous_iterator=0;

	String compiler_anonymous_iterator_name(){
		return "@_iter_"+Integer::toString(k_n_anonymous_iterator++);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// LOOPS
	//
	void link_loop_break_continues(CompilerData *compiler_data,int idx_start, int idx_post_instruction_for_start=ZS_UNDEFINED_IDX){

		int idx_end_instruction = compiler_data->current_function->compiler_instructions.length();

		for(int i=idx_start; i < idx_end_instruction;i++){
			Instruction *ins=&(compiler_data->current_function->compiler_instructions.get(i)->vm_instruction);//&compiler_data->current_function->instructions[i]->vm_instruction;
			if(ins->value_op1 == INSTRUCTION_VALUE_OP1_JMP_BREAK){
				ins->value_op1= INSTRUCTION_VALUE_OP1_NOT_DEFINED;
				ins->value_op2=idx_end_instruction-i-1; // -1 is for jmp to extra POP SCOPE, due it does PUSH SCOPE ALWAYS in the loops
			}else if(ins->value_op1 == INSTRUCTION_VALUE_OP1_JMP_CONTINUE){
				ins->value_op1= INSTRUCTION_VALUE_OP1_NOT_DEFINED;
				if(idx_post_instruction_for_start != ZS_UNDEFINED_IDX){
					ins->value_op2=idx_post_instruction_for_start-i;
				}
				else{
					ins->value_op2=idx_start-i;
				}
			}
		}
	}

	char *compiler_keyword_break(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info){
		ZS_UNUSUED_PARAM(scope_info);


		char *aux_p = (char *)s;
		Keyword key_w;

		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_BREAK){

			if(compiler_data->current_function->parsing_loop <= 0 && compiler_data->current_function->parsing_switch <= 0){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"break is only available on 'for', 'while' loops and switch");
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

			// insert jmp instruction
			compiler_data->current_function->compiler_instructions.push(
					new CompilerInstruction(
						BYTE_CODE_JMP,
						INSTRUCTION_VALUE_OP1_JMP_BREAK,
						0
					)
			);
		}
		return aux_p;
	}

	char *compiler_keyword_continue(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info){
		ZS_UNUSUED_PARAM(scope_info);
		char *aux_p = (char *)s;
		Keyword key_w;

		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_CONTINUE){

			if(compiler_data->current_function->parsing_loop <= 0){
				EVAL_ERROR_FILE_LINEF(
					compiler_data->current_parsing_file
					,line
					,"continue is only available on 'for' or 'while' loops"
				);
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

			compiler_data->current_function->compiler_instructions.push(
				new CompilerInstruction(
					BYTE_CODE_JMP,
					INSTRUCTION_VALUE_OP1_JMP_CONTINUE,
					0
				)
			);

		}
		return aux_p;
	}

	char * compiler_keyword_while(CompilerData *compiler_data,const char *s,int & line, ScriptScope *scope_info){

		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		String start_symbol;
		Keyword key_w;
		int idx_instruction_conditional_while=0;
		int idx_instruction_jmp_while=0;
		CompilerInstruction *ei_jnt; // conditional to end block

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_WHILE){

			aux_p += strlen(compiler_data_keywords[key_w].str);
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			// compileruate conditional line ...
			if(*aux_p != '('){
				EVAL_ERROR_FILE_LINEF(
					compiler_data->current_parsing_file
					,line
					,"Syntax error 'while': Expected '(' "
				);
			}

			// save current instruction to use later...
			idx_instruction_jmp_while=(int)(compiler_data->current_function->compiler_instructions.length());

			end_expr = compiler_sub_expression(
					compiler_data
					,aux_p+1
					,line
					,scope_info
					,&compiler_data->current_function->compiler_instructions
					,")"
			);

			idx_instruction_conditional_while=(int)(compiler_data->current_function->compiler_instructions.length());

			// insert instruction if compileruated expression
			compiler_data->current_function->compiler_instructions.push(ei_jnt=new CompilerInstruction(BYTE_CODE_JNT));
			ei_jnt->instruction_source_info.file=compiler_data->current_parsing_file;
			ei_jnt->instruction_source_info.line=line;


			start_symbol=compiler_copy_const_char_diff(aux_p+1, end_expr);

			IGNORE_BLANKS(aux_p,compiler_data,end_expr+1,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error 'while': Expected open block '{' ");
			}

			compiler_data->current_function->parsing_loop++;

			if((aux_p=compiler_block_body(
					compiler_data
					,aux_p
					,line
					,scope_info
					,NULL
					,true
					)
			 	 ) == NULL){
				return NULL;
			}

			compiler_data->current_function->parsing_loop--;

			// insert jmp instruction to begin condition while...
			compiler_data->current_function->compiler_instructions.push(new CompilerInstruction(
					BYTE_CODE_JMP
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,-((int)(compiler_data->current_function->compiler_instructions.length())-idx_instruction_jmp_while)
				)
			);

			// add extra pop scope at the end in case there's a break and it has to pop for local variables
			compiler_data->current_function->compiler_instructions.push(
					new CompilerInstruction(
						BYTE_CODE_POP_SCOPE
					)
			);

			// update jnt instruction to jmp after jmp instruction...
			ei_jnt->vm_instruction.value_op2=compiler_data->current_function->compiler_instructions.length()-idx_instruction_conditional_while;

			// catch all breaks in the while...
			link_loop_break_continues(compiler_data,idx_instruction_jmp_while,(int)compiler_data->current_function->compiler_instructions.length()-3);

			return aux_p;
		}
		return NULL;
	}

	char * compiler_keyword_do_while(CompilerData *compiler_data,const char *s,int & line, ScriptScope *scope_info){

		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		String start_symbol;
		Keyword key_w;
		int idx_do_while_start;
		int idx_do_while_conditional;

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_DO_WHILE){
			CompilerInstruction *ei_aux=NULL;

			aux_p += strlen(compiler_data_keywords[key_w].str);

			//1st compileruate body ..
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			// save current instruction to use later...
			idx_do_while_start=(int)(compiler_data->current_function->compiler_instructions.length());


			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error 'do-while': Expected open block '{' ");
			}

			compiler_data->current_function->parsing_loop++;
			if((aux_p=compiler_block_body(
					compiler_data
					,aux_p
					,line
					,scope_info
					,NULL
					,true
			))==NULL){
				return NULL;
			}

			compiler_data->current_function->parsing_loop--;

				// Finally compileruate conditional line ...
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			// check for keyword ...
			key_w = compiler_is_keyword(aux_p);

			if(key_w!=KEYWORD_WHILE){
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error 'do-while': expected 'while' keyword");
			}

			aux_p += strlen(compiler_data_keywords[key_w].str);

			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			if(*aux_p == '('){

				idx_do_while_conditional=(int)(compiler_data->current_function->compiler_instructions.length());

				// compiler conditional
				if((end_expr = compiler_sub_expression(
						compiler_data
						,aux_p+1
						,line
						,scope_info
						,&compiler_data->current_function->compiler_instructions
						,")"
				)) != NULL){
					start_symbol=compiler_copy_const_char_diff(aux_p+1, end_expr);
				}else{
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error 'do-while': Expected ')'");
				}
			}else{
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error 'do-while': Expected '('");
			}
			// insert jmp instruction to begin condition while...
			compiler_data->current_function->compiler_instructions.push(
				ei_aux=new CompilerInstruction(
					BYTE_CODE_JT
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,-((int)(compiler_data->current_function->compiler_instructions.length())-idx_do_while_start)
			));

			// insert jmp instruction to skip pop ...
			compiler_data->current_function->compiler_instructions.push(
					new CompilerInstruction(
						BYTE_CODE_JMP
						,INSTRUCTION_VALUE_OP1_NOT_DEFINED
						,2
					)
			);

			// Insert pop scope declared variables in the do-while in case break instruction is found
			compiler_data->current_function->compiler_instructions.push(
					new CompilerInstruction(
						BYTE_CODE_POP_SCOPE
					)
			);

			ei_aux->instruction_source_info.file=compiler_data->current_parsing_file;
			ei_aux->instruction_source_info.line=line;

			// catch all breaks in the while...
			//link_breaks(compiler_data);

			// catch all continues and compileruates bottom...
			link_loop_break_continues(compiler_data,idx_do_while_start,idx_do_while_conditional-1); // -1 POP_SCOPE


			return end_expr+1;
		}
		return NULL;
	}

	char * compiler_keyword_for(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info){

		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		Keyword key_w;
		bool is_for_in=false;
		int idx_instruction_for_start= ZS_UNDEFINED_IDX
			  ,idx_instruction_for_after_jnz_condition= ZS_UNDEFINED_IDX
			,idx_post_instruction_for_start= ZS_UNDEFINED_IDX;

		CompilerInstruction *ei_jnt=NULL; // conditional to end block
		Vector<CompilerInstruction *> ei_post_operations
				,ei_load_container_identifier
				,ei_init_vars_for;


		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);

		if(key_w != Keyword::KEYWORD_FOR){
			return NULL;
		}

		aux_p += strlen(compiler_data_keywords[key_w].str);
		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		if(*aux_p != '('){ // ready ...
			EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error 'for': Expected '('");
		}

		idx_instruction_for_start=compiler_data->current_function->compiler_instructions.length();

		// save scope pointer ...
		ScriptScope *new_scope_body=NULL;
		ScriptScope *new_scope_init =compiler_new_scope_block(compiler_data,scope_info); // push current scope

		IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);


		//------------------------------------------------------------------------------------------
		// BEGIN FOR-VAR IN PROCESS
		// 1. var init ...
		// there's some var Init...
		bool check_for_in=true;

		if(compiler_is_keyword(aux_p) == KEYWORD_VAR){
			char *aux_test=aux_p;
			int line_test=line;
			if((aux_p = compiler_keyword_var(
					compiler_data
					,aux_p
					,line
					,new_scope_init
					, EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR
			))==NULL){
				return NULL;
			}

			if(is_operator(aux_p)!=Operator::ZS_OPERATOR_IN){
				check_for_in=false;
			}
			else{ // in

				// check if no instructions (i.e v,k=1+2) and aux_test is operator in...
				if(idx_instruction_for_start!=compiler_data->current_function->compiler_instructions.length()){
					EVAL_ERROR_FILE_LINEF(
							compiler_data->current_parsing_file
							,line
							,"malformed for-in loop"
					);
				}

				// restore current ptr + line in order to recompileruate load vars
				line=line_test;
				aux_p=aux_test+strlen(compiler_data_keywords[KEYWORD_VAR].str);
			}
		}

		// no keyword or the previus parsing was successful
		if(check_for_in){
			// we do another compiler to capture load byte code instructions from variables variables
			is_for_in=false;
			char *test_aux;
			char name[1024]={0};
			bool error_some_instruction_not_load_local=false;
			int test_line=line;

			// compiler expression and not optimize to convert load in op to load from iterator
			if((test_aux = compiler_expression(
					compiler_data
					,aux_p
					,test_line
					,new_scope_init
					,&ei_init_vars_for //compiler_data->current_function->instructions
					,NULL
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_FOR_IN_VARIABLES
			))==NULL){
				goto label_exit_for;
			}

			if(is_operator(test_aux)==Operator::ZS_OPERATOR_IN){
				is_for_in=true;

				aux_p=test_aux;
				line=test_line;

				// check each byte code is load...
				for(int j=0; j<ei_init_vars_for.length();j++){
					CompilerInstruction *ins=ei_init_vars_for.get(j);
					if(ins->vm_instruction.byte_code!=BYTE_CODE_LOAD_LOCAL){
						error_some_instruction_not_load_local=true;
						break;
					}

					// switch to push
					ins->vm_instruction.byte_code=BYTE_CODE_PUSH_STK_LOCAL;

				}

				// Some symbol is not local
				if(error_some_instruction_not_load_local == true){
					EVAL_ERROR_FILE_LINE_GOTOF(
							compiler_data->current_parsing_file
							,line
							,label_exit_for
							,"malformed for-in loop"
					);
				}

				CompilerInstruction *ei_aux=NULL;
				Symbol *symbol_iterator=NULL;
				Operator tst_op_aux;
				CompilerInstruction ei_iterator;
				// set aux_p as test_aux
				line=test_line;

				// all check instructions
				IGNORE_BLANKS(aux_p,compiler_data,test_aux+strlen(compiler_data_operators[ZS_OPERATOR_IN].str),line);

				if((tst_op_aux=is_operator(aux_p))!=ZS_OPERATOR_UNKNOWN){
					EVAL_ERROR_FILE_LINE_GOTO(
							compiler_data->current_parsing_file
							,test_line
							,label_exit_for
							,"Syntax error 'for-in': unexpected operator '%s' after 'in' "
							,compiler_data_operators[tst_op_aux].str
					);
				}

				// container expression
				key_w=compiler_is_keyword(aux_p);
				if((key_w!=Keyword::KEYWORD_UNKNOWN) && (key_w!=Keyword::KEYWORD_THIS)){
					EVAL_ERROR_FILE_LINE_GOTO(
							compiler_data->current_parsing_file
							,test_line
							,label_exit_for
							,"Syntax error 'for-in': unexpected '%s' after 'in' "
							,compiler_data_keywords[key_w].str
					);
				}

				// compiler iterator variable
				if((test_aux = compiler_sub_expression(
						compiler_data
						,aux_p
						,line
						,new_scope_init
						,&ei_load_container_identifier //compiler_data->current_function->instructions
						,NULL
						,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
				))==NULL){
					goto label_exit_for;
				}

				strncpy(name,aux_p,test_aux-aux_p);

				switch(((CompilerInstruction *)ei_load_container_identifier.get(0))->vm_instruction.byte_code){
				case BYTE_CODE_LOAD_UNDEFINED:
				case BYTE_CODE_LOAD_NULL:
				case BYTE_CODE_LOAD_FLOAT:
				case BYTE_CODE_LOAD_BOOL:
				case BYTE_CODE_LOAD_INT:
					EVAL_ERROR_FILE_LINE_GOTO(
							compiler_data->current_parsing_file
							,test_line
							,label_exit_for
							,"Expected container as variable but it was constant value '%s'"
							,name
					);
					break;
				default:
					break;
				}

				aux_p=test_aux;

				if(*aux_p!=')'){
					if((tst_op_aux=is_operator(aux_p))!=ZS_OPERATOR_UNKNOWN){
						EVAL_ERROR_FILE_LINE_GOTO(
							compiler_data->current_parsing_file
							,line
							,label_exit_for
							,"Syntax error 'for-in': unexpected operator '%s' after container identifier "
							,compiler_data_operators[tst_op_aux].str
						);
					}else{
						EVAL_ERROR_FILE_LINE_GOTOF(
							compiler_data->current_parsing_file
							,line
							,label_exit_for
							,"Syntax error 'for-in': expected ')' after container identifier "
						);
					}
				}

				// 1. create iterator symbol
				if((symbol_iterator=compiler_data->current_function->script_function->registerLocalVariable(
						new_scope_init
						, compiler_data->current_parsing_file
						, line
						, compiler_anonymous_iterator_name()
					))==NULL){
					goto label_exit_for;
				};

				ei_iterator.vm_instruction.byte_code=BYTE_CODE_LOAD_LOCAL;
				ei_iterator.vm_instruction.value_op1= INSTRUCTION_VALUE_OP1_NOT_DEFINED;
				ei_iterator.vm_instruction.value_op2=symbol_iterator->idx_position;
				ei_iterator.symbol_name=symbol_iterator->name;
				ei_iterator.symbol_scope=symbol_iterator->scope;
				ei_iterator.instruction_source_info.ptr_str_symbol_name=get_mapped_name(compiler_data, symbol_iterator->name);


				// 2. emit iterator init
				for(int i=0; i < ei_load_container_identifier.length(); i++){
					compiler_data->current_function->compiler_instructions.push(
						new CompilerInstruction(*((CompilerInstruction *)ei_load_container_identifier.get(i)))
					);
				}

				compiler_data->current_function->compiler_instructions.push(
						ei_aux=new CompilerInstruction(ei_iterator)
				);

				// change load by push stk because we have to store iterator variable returned by iter()
				ei_aux->vm_instruction.byte_code=BYTE_CODE_PUSH_STK_LOCAL;

				compiler_data->current_function->compiler_instructions.push(
						ei_aux=new CompilerInstruction(
							BYTE_CODE_IT_INIT
							, (uint8_t)(ZS_UNDEFINED_IDX)
							,ZS_UNDEFINED_IDX
							,INSTRUCTION_PROPERTY_RESET_STACK
					)
				);

				ei_aux->instruction_source_info=((CompilerInstruction *)ei_load_container_identifier.get(ei_load_container_identifier.length()-1))->instruction_source_info;

				idx_instruction_for_start=(int)(compiler_data->current_function->compiler_instructions.length());

				// 3. emit iterator condition end
				compiler_data->current_function->compiler_instructions.push(
						ei_aux=new CompilerInstruction(ei_iterator)
				);


				// load object end symbol
				compiler_data->current_function->compiler_instructions.push(
					ei_aux=new CompilerInstruction(BYTE_CODE_LOAD_OBJECT_ITEM,(uint8_t)ZS_UNDEFINED_IDX,ZS_UNDEFINED_IDX,INSTRUCTION_PROPERTY_CALLING_FUNCTION)
				);
				ei_aux->instruction_source_info.ptr_str_symbol_name=get_mapped_name(compiler_data, "_end");

				// call
				compiler_data->current_function->compiler_instructions.push(
					new CompilerInstruction(
							BYTE_CODE_MEMBER_CALL
							,INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(1,0)
					)
				);

				compiler_data->current_function->compiler_instructions.push(
						ei_jnt=new CompilerInstruction(BYTE_CODE_JT)
				);
				ei_jnt->instruction_source_info.file=compiler_data->current_parsing_file;
				ei_jnt->instruction_source_info.line=line;

				idx_instruction_for_after_jnz_condition=(int)(compiler_data->current_function->compiler_instructions.length());

				// PUSH SCOPE
				/*compiler_data->current_function->compiler_instructions.append(
						new CompilerInstruction(BYTE_CODE_PUSH_SCOPE)
				);*/
				new_scope_body =compiler_new_scope_block(compiler_data,new_scope_init);

				// load container
				compiler_data->current_function->compiler_instructions.push(
						ei_aux=new CompilerInstruction(ei_iterator)
				);

				// load _get member function
				compiler_data->current_function->compiler_instructions.push(
					ei_aux=new CompilerInstruction(BYTE_CODE_LOAD_OBJECT_ITEM, (uint8_t)ZS_UNDEFINED_IDX,ZS_UNDEFINED_IDX,INSTRUCTION_PROPERTY_CALLING_FUNCTION)
				);

				ei_aux->instruction_source_info.ptr_str_symbol_name=get_mapped_name(compiler_data, "_get");

				// call, return all
				compiler_data->current_function->compiler_instructions.push(
					// insert byte code call and set return count as 2, due in in this case we have prepared 2 vars on the left (k,v)
					new CompilerInstruction(
							BYTE_CODE_MEMBER_CALL
							,INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(ei_init_vars_for.length(),0)
					)
				);

				// load k,v
				for(int i=0; i <ei_init_vars_for.length() ;i++){
					compiler_data->current_function->compiler_instructions.push(
							new CompilerInstruction(*(CompilerInstruction *)ei_init_vars_for.get(i))
					);
				}

				// store...
				compiler_data->current_function->compiler_instructions.push(
					new CompilerInstruction(
							BYTE_CODE_STORE
							,(uint8_t)ei_init_vars_for.length()
							,ZS_UNDEFINED_IDX
							,INSTRUCTION_PROPERTY_RESET_STACK
					)
				);

				// 4. emit post operation
				ei_post_operations.push(
						ei_aux=new CompilerInstruction(ei_iterator)
				);

				// load object end symbol
				ei_post_operations.push(
					ei_aux=new CompilerInstruction(BYTE_CODE_LOAD_OBJECT_ITEM, (uint8_t)ZS_UNDEFINED_IDX,ZS_UNDEFINED_IDX,INSTRUCTION_PROPERTY_CALLING_FUNCTION)
				);

				ei_aux->instruction_source_info.ptr_str_symbol_name=get_mapped_name(compiler_data, "_next");

				// call
				ei_post_operations.push(
					new CompilerInstruction(BYTE_CODE_MEMBER_CALL,0,0,INSTRUCTION_PROPERTY_RESET_STACK)
				);

				// reset zero pointers
				/*ei_post_operations.append(
					new CompilerInstruction(BYTE_CODE_CLEAR_ZERO_POINTERS)
				);*/
			}else{ // is not for-in re compiler for in with no in-break

				// copy var initialization
				for(int i=0; i <ei_init_vars_for.length() ;i++){
					compiler_data->current_function->compiler_instructions.push(
							new CompilerInstruction(*(CompilerInstruction *)ei_init_vars_for.get(i))
					);
				}

				// restore string iterator and current linep and test_aux
				aux_p=test_aux;
				line=test_line;
			}
		}
		// END FOR-VAR IN PROCESS
		//------------------------------------------------------------------------------------------


		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		if(is_for_in == false){
		  // expects conditional and post (i.e for(;;) )
			if(*aux_p != ';'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					compiler_data->current_parsing_file
					,line
					,label_exit_for
					,"Syntax error 'for': Expected ';'"
				);
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

			if(*aux_p != ';'){ // conditional...
				char * end_p=NULL;
				IGNORE_BLANKS(end_p,compiler_data,aux_p+1,line);

				if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

					idx_instruction_for_start=(int)(compiler_data->current_function->compiler_instructions.length());

					// compiler conditional
					if((aux_p = compiler_sub_expression(
							compiler_data
							,(const char *)aux_p
							,line
							,new_scope_init
							,&compiler_data->current_function->compiler_instructions
					))==NULL){
						goto label_exit_for;
					}

					compiler_data->current_function->compiler_instructions.push(
							ei_jnt=new CompilerInstruction(BYTE_CODE_JNT)
					);
					ei_jnt->instruction_source_info.file=compiler_data->current_parsing_file;
					ei_jnt->instruction_source_info.line=line;


					idx_instruction_for_after_jnz_condition=(int)(compiler_data->current_function->compiler_instructions.length());
				}
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			if(*aux_p != ';'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					compiler_data->current_parsing_file
					,line
					,label_exit_for
					,"Syntax error 'for': Expected ';'"
				);

			}
			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

			if(*aux_p != ')' ){ // finally do post op...
				if((aux_p = compiler_expression(
					compiler_data
					,(const char *)aux_p
					,line
					,new_scope_init
					,&ei_post_operations
					,{}
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION // it allows expression sequence and it does a reset stack in the end
				))==NULL){
					goto label_exit_for;
				}
			}
		}

		if(*aux_p != ')'){
			EVAL_ERROR_FILE_LINE_GOTOF(
				compiler_data->current_parsing_file
				,line
				,label_exit_for
				,"Syntax error 'for': Expected ')'"
			);
		}

		IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
		if(*aux_p != '{'){
			EVAL_ERROR_FILE_LINE_GOTOF(
				compiler_data->current_parsing_file
				,line
				,label_exit_for
				,"Syntax error 'for': Expected '{' for begin block"
			);
		}

		// increase the parsing_loop in order to take account for continues and breaks
		compiler_data->current_function->parsing_loop++;

		// compiler block and write push/pop scope...
		if((aux_p=compiler_block_body(
				compiler_data
				,aux_p
				,line
				,new_scope_init
				,new_scope_body
				,true
		))==NULL){
			// deallocate post operations
			goto label_exit_for;
		}

		// parsing_loop--
		compiler_data->current_function->parsing_loop--;

		idx_post_instruction_for_start=compiler_data->current_function->compiler_instructions.length();

		// insert post operations...
		compiler_data->current_function->compiler_instructions.concat(ei_post_operations);

		// insert jmp instruction to begin condition for...
		compiler_data->current_function->compiler_instructions.push(
				new CompilerInstruction(
					BYTE_CODE_JMP
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,-((int)(compiler_data->current_function->compiler_instructions.length())-(int)idx_instruction_for_start)
				)
		);

		// insert extra pop_scope for 'break'
		compiler_data->current_function->compiler_instructions.push(
				new CompilerInstruction(
					BYTE_CODE_POP_SCOPE
				)
		);

		// all opeartions done so clean ei_post_operations vector in order to not deallocate already compileruated instructions
		ei_post_operations.clear();

		// update jnt instruction to jmp after jmp instruction...
		if(ei_jnt != NULL){ // infinite loop
			ei_jnt->vm_instruction.value_op2=(compiler_data->current_function->compiler_instructions.length()-idx_instruction_for_after_jnz_condition)+1;
		}

		// catch all continues and set all jmps after processing block but before post operation...
		link_loop_break_continues(compiler_data,idx_instruction_for_start,idx_post_instruction_for_start-1);

		// true: We treat declared variables into for as another scope.
		compiler_check_scope(compiler_data,new_scope_init);

label_exit_for:

		for(int i=0; i<ei_post_operations.length();i++){
			CompilerInstruction *ei=(CompilerInstruction *)ei_post_operations.get(i);
			delete ei;
		}
		ei_post_operations.clear();


		for(int j=0; j<ei_init_vars_for.length();j++){
			CompilerInstruction *ei= (CompilerInstruction *)ei_init_vars_for.get(j);
			delete ei;
		}
		ei_init_vars_for.clear();

		for(int j=0; j<ei_load_container_identifier.length();j++){
			CompilerInstruction *ei= (CompilerInstruction *)ei_load_container_identifier.get(j);
			delete ei;
		}
		ei_load_container_identifier.clear();

		return aux_p;
	}
}



