/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"

namespace zetscript{

	typedef struct{
		CompilerInstruction *ei_je_instruction;
		Vector<CompilerInstruction *> 		 ei_load_symbols;
	}CompilerInstructionCase;

	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONDITIONALS
	//
	char * compiler_keyword_if_else(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info){

		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		Keyword key_w;

		CompilerInstruction *ei_if_jnt;
		Vector<CompilerInstruction *>		ei_else_end_jmp;
		CompilerInstruction *ei_aux;
		bool end=true;

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_IF){
			do{
				end=true;
				//ei_jmps.clear();
				aux_p += strlen(compiler_data_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

				if(*aux_p != '('){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Expected '(' if");
				}

				// compiler conditional expression
				if((end_expr = compiler_sub_expression(
						compiler_data
						,aux_p+1
						,line
						,scope_info
						,&compiler_data->current_function->compiler_instructions
						,")"
				))==NULL){
					return 0;
				}


				// insert instruction if compileruated expression
				compiler_data->current_function->compiler_instructions.push(ei_aux=new CompilerInstruction(BYTE_CODE_JNT));
				ei_aux->instruction_source_info.file=compiler_data->current_parsing_file;
				ei_aux->instruction_source_info.line=line;

				ei_if_jnt=ei_aux;
				int idx_start_block=compiler_data->current_function->compiler_instructions.length();
				//ei_jmps.append(ei_aux);

				IGNORE_BLANKS(aux_p,compiler_data,end_expr+1,line);
				if(*aux_p != '{'){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Expected if-block open block ('{')");

				}

				if((aux_p=compiler_block_body(compiler_data,aux_p
					,line
					,scope_info
				  ))==NULL){
						return NULL;
				}

				int idx_end_block=compiler_data->current_function->compiler_instructions.length();

				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

				bool else_key = false;
				if((key_w = compiler_is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
					else_key = (key_w == Keyword::KEYWORD_ELSE);
				}

				ei_if_jnt->vm_instruction.value_op2=idx_end_block-idx_start_block+1;

				if(else_key){ // it finish if-else


					// we should insert jmp to end conditional chain if/else...
					compiler_data->current_function->compiler_instructions.push(ei_aux=new CompilerInstruction(
							BYTE_CODE_JMP
							, INSTRUCTION_VALUE_OP1_NOT_DEFINED
							,compiler_data->current_function->compiler_instructions.length()
							));
					ei_if_jnt->vm_instruction.value_op2+=1; // sum +1 because we inserted a jmp for else

					ei_else_end_jmp.push(ei_aux);

					aux_p += strlen(compiler_data_keywords[key_w].str);

					IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

					bool if_key = false;
					if((key_w = compiler_is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
						if_key = (key_w == Keyword::KEYWORD_IF);
					}

					if(if_key){ // else if
						end=false;
					}else{ // only else, expect {

						if(*aux_p != '{'){
							EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Expected else-block open block ('{')");
						}

						// compiler else block
						if((aux_p=compiler_block_body(
								compiler_data
								,aux_p
								,line
								,scope_info
						))==NULL){
							return NULL;
						}
					}
				}
			}while(!end); // loop

			for(int i=0; i < ei_else_end_jmp.length(); i++){
				Instruction *ins=&(ei_else_end_jmp.get(i))->vm_instruction;
				ins->value_op2=compiler_data->current_function->compiler_instructions.length()-ins->value_op2;
			}
			return aux_p;
		}
		return NULL;
	}


	void compiler_switch_deallocate(Vector<CompilerInstruction *> & ei_switch_condition,Vector<CompilerInstructionCase *> & _eic_cases,CompilerInstruction **_ei_jmp_default){

		// deallocate condition
		for(int i=0; i < ei_switch_condition.length(); i++){
			delete ei_switch_condition.get(i);
		}
		ei_switch_condition.clear();

		// deallocate all cases
		for(int i=0; i < _eic_cases.length(); i++){
			delete _eic_cases.get(i)->ei_je_instruction;
			CompilerInstructionCase * eic_current=_eic_cases.get(i);
			for(int j=0; j < eic_current->ei_load_symbols.length(); j++){
				delete eic_current->ei_load_symbols.get(j);
			}
			delete eic_current;
		}
		_eic_cases.clear();

		if(*_ei_jmp_default != NULL){
			delete *_ei_jmp_default;
		}
		*_ei_jmp_default=NULL;
	}

	char * compiler_keyword_switch(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info){

		char *aux_p = (char *)s;
		String val;
		Keyword key_w;//,key_w2;
		Vector<CompilerInstruction *> 		ei_switch_condition; // switch condition
		Vector<CompilerInstructionCase *>	eic_cases; // stores all conditional instructions at begin
		Vector<CompilerInstruction *>  		ei_break_jmps; // breaks or if condition not satisfies nothing (there's no default)
		CompilerInstruction *ei_jmp_default = NULL;

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);


		if(key_w == Keyword::KEYWORD_SWITCH){
			int idx_start_instruction = (int)(compiler_data->current_function->compiler_instructions.length());;
			int size_ei_cases=0;

			aux_p += strlen(compiler_data_keywords[key_w].str);
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			if(*aux_p != '('){
				EVAL_ERROR_FILE_LINE_GOTOF(
					compiler_data->current_parsing_file
					,line
					,compiler_keyword_switch_error
					,"Expected '(' conditional switch "
				);
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);


			// compileruate switch condition expression ...
			aux_p = compiler_sub_expression(
				compiler_data
				,aux_p
				,line
				,scope_info
				,&ei_switch_condition
				,")"
			);

			if(aux_p==NULL){
				goto compiler_keyword_switch_error;
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					compiler_data->current_parsing_file
					,line
					,compiler_keyword_switch_error
					,"Expected '{' begin switch block"
				);
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
			ei_jmp_default=NULL;

			//int n_default=0;

			while(*aux_p != '}' && *aux_p != 0){

				bool is_default=false;
				// search for case or default...
				key_w = compiler_is_keyword(aux_p);
				if(key_w == KEYWORD_CASE){
					int line_case=line;
					CompilerInstructionCase *compiler_instruction_case=new CompilerInstructionCase();
					eic_cases.push(compiler_instruction_case);

					// ignore case
					IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

					// get token symbol
					if((aux_p = compiler_expression(
							compiler_data
							,aux_p
							,line
							,scope_info
							,&compiler_instruction_case->ei_load_symbols
							,NULL
							,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
					))==NULL){
						// delete unusued vars_for
						goto compiler_keyword_switch_error;
					}

					/*if(*aux_p != ':'){
						EVAL_ERROR_FILE_LINE_GOTOF(
							compiler_data->current_parsing_file
							,line
							,compiler_keyword_switch_error
							,"Expected ':' after expression"
						);
					}

					IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);*/

					// insert a pair of instructions...
					compiler_instruction_case->ei_je_instruction=new CompilerInstruction(
							BYTE_CODE_JE_CASE
							, INSTRUCTION_VALUE_OP1_NOT_DEFINED
							,((int)(compiler_data->current_function->compiler_instructions.length()))-idx_start_instruction	 // offset
					);

					compiler_instruction_case->ei_je_instruction->instruction_source_info.line=line_case;
					compiler_instruction_case->ei_je_instruction->instruction_source_info.file=compiler_data->current_parsing_file;




					// update size cases
					size_ei_cases+=(compiler_instruction_case->ei_load_symbols.length()+1); // +1 is for je (i.e je_instruction)

				}else if(key_w == KEYWORD_DEFAULT){
					if(ei_jmp_default!=NULL){
						EVAL_ERROR_FILE_LINE_GOTO(
								compiler_data->current_parsing_file
								,line
								,compiler_keyword_switch_error
								,"There's an already 'default' case"
								, NULL
						);
					}

					ei_jmp_default=new CompilerInstruction(
							BYTE_CODE_JMP_CASE
							, INSTRUCTION_VALUE_OP1_NOT_DEFINED
							,((int)(compiler_data->current_function->compiler_instructions.length()))-idx_start_instruction+1	 // offset
							,INSTRUCTION_PROPERTY_RESET_STACK
					);

					is_default=true;
					//n_default++;
					IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

				}else{
					EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,line
						,compiler_keyword_switch_error
						,(key_w == KEYWORD_DEFAULT)?"Expected ':' after 'default'"
								:"Expected ':' after case-value"
					);
				}

				if(*aux_p!=':'){
					EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,line
						,compiler_keyword_switch_error
						,"Expected ':' after case-value"
					);
				}

				// ignore :
				IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

				if((compiler_is_keyword(aux_p) == Keyword::KEYWORD_CASE)){ // another case ...
					continue;
				}

				// compile the body
				compiler_data->current_function->parsing_switch++;
				aux_p=compiler_compile_recursive(
					compiler_data
					,aux_p
					, line
					, scope_info
					, true // cancel compiler when break or case is found...
				);
				compiler_data->current_function->parsing_switch--;

				if(aux_p == NULL){
					goto compiler_keyword_switch_error;
				}

				if((compiler_is_keyword(aux_p) == Keyword::KEYWORD_BREAK)){ // it cuts current expression to link breaks...

					IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[Keyword::KEYWORD_BREAK].str),line);

					// ignore all ;
					while(*aux_p==';' && *aux_p != 0){
						IGNORE_BLANKS(aux_p,compiler_data,aux_p+1, line);
					}

					if(*aux_p != '}'){ // not end insert jmp

						CompilerInstruction *ei_break_jmp=new CompilerInstruction(
								BYTE_CODE_JMP
								,INSTRUCTION_VALUE_OP1_JMP_BREAK
								,0
						);
						compiler_data->current_function->compiler_instructions.push(ei_break_jmp);
						ei_break_jmps.push(ei_break_jmp);
					}


				}else if(is_default){
					//CompilerInstruction *ie_last=(CompilerInstruction *)compiler_data->current_function->compiler_instructions.items[compiler_data->current_function->compiler_instructions.length()-1];
					EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,line
						,compiler_keyword_switch_error
						,"'default' needs a 'break' at the end"
					);
				}

				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			}

			if(*aux_p != '}'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					compiler_data->current_parsing_file
					,line
					,compiler_keyword_switch_error
					,"Syntax error switch: Expected '}' end switch"
				);
			}

			// end instruction
			int offset_end_instruction=((int)(compiler_data->current_function->compiler_instructions.length()))-idx_start_instruction;

			if(offset_end_instruction > 0 && eic_cases.length() > 0){ // there's conditions to manage

				//CompilerInstruction *jmp_after_je_cases;

				//int size_ei_cases=ei_cases.length();
				unsigned offset_ei_case=0;
				int size_ei_switch_condition = ei_switch_condition.length();
				int size_ei_cases_and_switch_expression = size_ei_cases+size_ei_switch_condition+1; // +1 for jmp default if there's no default
				int total_instructions_switch=offset_end_instruction+size_ei_cases_and_switch_expression; // total instructions switch

				if(ei_jmp_default == NULL){ // no default found so, we insert a default jmp to the end
					ei_jmp_default=new CompilerInstruction(
					BYTE_CODE_JMP
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,total_instructions_switch-size_ei_cases-size_ei_switch_condition
					);
				}

				// 1. Insert compiler instructions for switch condition
				compiler_data->current_function->compiler_instructions.insert(
						idx_start_instruction
						,ei_switch_condition
				);

				// 2. insert all cases found first from start + offset size instruction cases found
				for(int i=0; i < eic_cases.length(); i++){
					CompilerInstructionCase *eic_case=(CompilerInstructionCase *) eic_cases.get(i);
					Instruction *ins=&eic_case->ei_je_instruction->vm_instruction; // load je

					compiler_data->current_function->compiler_instructions.insert(
							idx_start_instruction+size_ei_switch_condition+offset_ei_case,
							eic_case->ei_load_symbols
					);

					offset_ei_case+=(eic_case->ei_load_symbols.length()); // load instruction token

					// insert je...
					compiler_data->current_function->compiler_instructions.insert(
							idx_start_instruction+size_ei_switch_condition+offset_ei_case,
							eic_case->ei_je_instruction
					);

					offset_ei_case++; // je + 1

					ins->value_op2=ins->value_op2+(size_ei_cases+1)-(offset_ei_case)+1; // +1 jmp after ei cases
				}

				// 3. insert last jmp to default or end switch
				compiler_data->current_function->compiler_instructions.insert(
						idx_start_instruction+size_ei_switch_condition+size_ei_cases,
						ei_jmp_default
				);

				offset_end_instruction=compiler_data->current_function->compiler_instructions.length();
				for(int i=idx_start_instruction; i < compiler_data->current_function->compiler_instructions.length();i++){
					Instruction *ins=&((CompilerInstruction *)compiler_data->current_function->compiler_instructions.get(i))->vm_instruction;
					if(ins->value_op1==INSTRUCTION_VALUE_OP1_JMP_BREAK){
						ins->value_op1= INSTRUCTION_VALUE_OP1_NOT_DEFINED;
						ins->value_op2=offset_end_instruction-i;
					}
				}

				// deallocate all cases
				for(int i=0; i < eic_cases.length(); i++){
					CompilerInstructionCase * eic_current=eic_cases.get(i);
					if(eic_current != NULL){
						delete eic_current;
					}
				}

			}else{

				compiler_switch_deallocate(ei_switch_condition,eic_cases,&ei_jmp_default);

			}

			return aux_p+1;


		}

compiler_keyword_switch_error:

		compiler_switch_deallocate(ei_switch_condition,eic_cases,&ei_jmp_default);

		return NULL;
	}
}




