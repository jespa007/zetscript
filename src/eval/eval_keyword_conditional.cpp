/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{

	typedef struct{
		EvalInstruction *ei_je_instruction;
		zs_vector 		 ei_load_symbols;
	}EvalInstructionCase;

	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONDITIONALS
	//
	char * eval_keyword_if_else(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;
		Keyword key_w;

		EvalInstruction *ei_if_jnt;
		zs_vector		ei_else_end_jmp;
		EvalInstruction *ei_aux;
		bool end=true;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_IF){
			do{
				end=true;
				//ei_jmps.clear();
				aux_p += strlen(eval_data_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p != '('){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '(' if");
				}

				// eval conditional expression
				if((end_expr = eval_expression(
						eval_data
						,aux_p+1
						,line
						,scope_info
						,&eval_data->current_function->eval_instructions
						,")"
						,EVAL_EXPRESSION_DO_NOT_RESET_STACK_LAST_CALL
				))==NULL){
					return 0;
				}


				// insert instruction if evaluated expression
				eval_data->current_function->eval_instructions.push_back((zs_int)(ei_aux=new EvalInstruction(BYTE_CODE_JNT)));
				ei_aux->instruction_source_info.file=eval_data->current_parsing_file;
				ei_aux->instruction_source_info.line=line;

				ei_if_jnt=ei_aux;
				int idx_start_block=eval_data->current_function->eval_instructions.count;
				//ei_jmps.push_back(ei_aux);

				IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);
				if(*aux_p != '{'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected if-block open block ('{')");

				}

				if((aux_p=eval_block(eval_data,aux_p
					,line
					,scope_info
				  ))==NULL){
						return NULL;
				}

				int idx_end_block=eval_data->current_function->eval_instructions.count;

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				bool else_key = false;
				if((key_w = eval_is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
					else_key = (key_w == Keyword::KEYWORD_ELSE);
				}

				ei_if_jnt->vm_instruction.value_op2=idx_end_block-idx_start_block+1;

				if(else_key){ // it finish if-else


					// we should insert jmp to end conditional chain if/else...
					eval_data->current_function->eval_instructions.push_back((zs_int)(ei_aux=new EvalInstruction(
							BYTE_CODE_JMP
							,IDX_ZS_UNDEFINED
							,eval_data->current_function->eval_instructions.count
							)));
					ei_if_jnt->vm_instruction.value_op2+=1; // sum +1 because we inserted a jmp for else

					ei_else_end_jmp.push_back((zs_int)ei_aux);

					aux_p += strlen(eval_data_keywords[key_w].str);

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					bool if_key = false;
					if((key_w = eval_is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
						if_key = (key_w == Keyword::KEYWORD_IF);
					}

					if(if_key){ // else if
						end=false;
					}else{ // only else, expect {

						if(*aux_p != '{'){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected else-block open block ('{')");
						}

						// eval else block
						if((aux_p=eval_block(
								eval_data
								,aux_p
								,line
								,scope_info
						))==NULL){
							return NULL;
						}
					}
				}
			}while(!end); // loop

			for(unsigned i=0; i < ei_else_end_jmp.count; i++){
				Instruction *ins=&((EvalInstruction *)ei_else_end_jmp.items[i])->vm_instruction;
				ins->value_op2=eval_data->current_function->eval_instructions.count-ins->value_op2;
			}
			return aux_p;
		}
		return NULL;
	}


	void eval_switch_deallocate(zs_vector & ei_switch_condition,zs_vector & _eic_cases,EvalInstruction **_ei_jmp_default){

		// deallocate condition
		for(unsigned i=0; i < ei_switch_condition.count; i++){
			delete (EvalInstruction *)ei_switch_condition.items[i];
		}
		ei_switch_condition.clear();

		// deallocate all cases
		for(unsigned i=0; i < _eic_cases.count; i++){
			delete ((EvalInstructionCase *)_eic_cases.items[i])->ei_je_instruction;
			EvalInstructionCase * eic_current=((EvalInstructionCase *)_eic_cases.items[i]);
			for(unsigned j=0; j < eic_current->ei_load_symbols.count; j++){
				delete (EvalInstruction *)(eic_current->ei_load_symbols.items[j]);
			}
			delete eic_current;
		}
		_eic_cases.clear();

		if(*_ei_jmp_default != NULL){
			delete *_ei_jmp_default;
		}
		*_ei_jmp_default=NULL;
	}

	char * eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){



		char *aux_p = (char *)s;
		Scope *scope_case=NULL;
		zs_string val;
		Keyword key_w;//,key_w2;
		zs_vector 		ei_switch_condition; // switch condition
		zs_vector 		eic_cases; // stores all conditional instructions at begin
		zs_vector  		ei_break_jmps; // breaks or if condition not satisfies nothing (there's no default)
		EvalInstruction *ei_jmp_default = NULL;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);


		if(key_w == Keyword::KEYWORD_SWITCH){
			int idx_start_instruction = (int)(eval_data->current_function->eval_instructions.count);;
			int size_ei_cases=0;

			aux_p += strlen(eval_data_keywords[key_w].str);
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p != '('){
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: Expected '(' conditional switch ");
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);


			// evaluate switch condition expression ...
			aux_p = eval_expression(
				eval_data
				,aux_p
				,line
				,scope_info
				,&ei_switch_condition
				,")"
				,EVAL_EXPRESSION_DO_NOT_RESET_STACK_LAST_CALL
			);

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: Expected '{' begin switch block");
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			ei_jmp_default=NULL;

			//int n_default=0;

			while(*aux_p != '}' && *aux_p != 0){

				bool is_default=false;
				// search for case or default...
				key_w = eval_is_keyword(aux_p);
				if(key_w == KEYWORD_CASE){
					int line_case=line;
					EvalInstructionCase *eval_instruction_case=new EvalInstructionCase();

					// ignore case
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

					// get token symbol
					if((aux_p = eval_expression(
							eval_data
							,aux_p
							,line
							,scope_info
							,&eval_instruction_case->ei_load_symbols
							,":"
							,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
					))==NULL){
						// delete unusued vars_for
						return NULL;
					}

					// insert a pair of instructions...
					eval_instruction_case->ei_je_instruction=new EvalInstruction(
							BYTE_CODE_JE_CASE
							,IDX_ZS_UNDEFINED
							,((int)(eval_data->current_function->eval_instructions.count))-idx_start_instruction	 // offset
					);

					eval_instruction_case->ei_je_instruction->instruction_source_info.line=line_case;
					eval_instruction_case->ei_je_instruction->instruction_source_info.file=eval_data->current_parsing_file;


					eic_cases.push_back((zs_int)eval_instruction_case);

					// update size cases
					size_ei_cases+=(eval_instruction_case->ei_load_symbols.count+1); // +1 is for je (i.e je_instruction)

				}else if(key_w == KEYWORD_DEFAULT){
					if(ei_jmp_default!=NULL){
						EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: there's an already 'default' case");
					}

					ei_jmp_default=new EvalInstruction(
							BYTE_CODE_JMP
							,IDX_ZS_UNDEFINED
							,((int)(eval_data->current_function->eval_instructions.count))-idx_start_instruction+1	 // offset
					);

					is_default=true;
					//n_default++;
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

				}else{
					EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: Expected 'case' or 'default' keyword");
				}

				if(*aux_p!=':'){
					EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: Expected ':' ");
				}

				// ignore :
				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				if((eval_is_keyword(aux_p) == Keyword::KEYWORD_CASE)){ // another case ...
					continue;
				}

				// compile the body
				eval_data->current_function->parsing_switch++;
				aux_p=eval_parse_and_compile_recursive(
					eval_data
					,aux_p
					, line
					, scope_info
					, true // cancel eval when break or case is found...
				);
				eval_data->current_function->parsing_switch--;

				if(aux_p == NULL){
					goto eval_keyword_switch_error;
				}

				if((eval_is_keyword(aux_p) == Keyword::KEYWORD_BREAK)){ // it cuts current expression to link breaks...

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[Keyword::KEYWORD_BREAK].str),line);

					// ignore all ;
					while(*aux_p==';' && *aux_p != 0){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1, line);
					}

					if(*aux_p != '}'){ // not end insert jmp

						EvalInstruction *ei_break_jmp=new EvalInstruction(
								BYTE_CODE_JMP
								,ZS_IDX_INSTRUCTION_JMP_BREAK
								,0
						);
						eval_data->current_function->eval_instructions.push_back((zs_int)ei_break_jmp);
						ei_break_jmps.push_back((zs_int)ei_break_jmp);
					}


				}else if(is_default){
					EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch:'default' needs a 'break' at the end");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			}

			if(*aux_p != '}'){
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: Expected '}' end switch");
			}

			// end instruction
			int offset_end_instruction=((int)(eval_data->current_function->eval_instructions.count))-idx_start_instruction;

			if(offset_end_instruction > 0 && eic_cases.count > 0){ // there's conditions to manage

				//EvalInstruction *jmp_after_je_cases;

				//int size_ei_cases=ei_cases.size();
				unsigned offset_ei_case=0;
				int size_ei_switch_condition = ei_switch_condition.count;
				int size_ei_cases_and_switch_expression = size_ei_cases+size_ei_switch_condition+1; // +1 for jmp default if there's no default
				int total_instructions_switch=offset_end_instruction+size_ei_cases_and_switch_expression; // total instructions switch

				if(ei_jmp_default == NULL){ // no default found so, we insert a default jmp to the end
					ei_jmp_default=new EvalInstruction(
					BYTE_CODE_JMP
					,IDX_ZS_UNDEFINED
					,total_instructions_switch-size_ei_cases-size_ei_switch_condition
					);
				}

				// 1. Insert eval instructions for switch condition
				eval_data->current_function->eval_instructions.insert(
						idx_start_instruction
						,&ei_switch_condition
				);

				// 2. insert all cases found first from start + offset size instruction cases found
				for(unsigned i=0; i < eic_cases.count; i++){
					EvalInstructionCase *eic_case=(EvalInstructionCase *) eic_cases.items[i];
					Instruction *ins=&eic_case->ei_je_instruction->vm_instruction; // load je

					eval_data->current_function->eval_instructions.insert(
							idx_start_instruction+size_ei_switch_condition+offset_ei_case,
							&eic_case->ei_load_symbols
					);

					offset_ei_case+=(eic_case->ei_load_symbols.count); // load instruction token

					// insert je...
					eval_data->current_function->eval_instructions.insert(
							idx_start_instruction+size_ei_switch_condition+offset_ei_case,
							(zs_int)eic_case->ei_je_instruction
					);

					offset_ei_case++; // je + 1

					ins->value_op2=ins->value_op2+(size_ei_cases+1)-(offset_ei_case)+1; // +1 jmp after ei cases
				}

				// 3. insert last jmp to default or end switch
				eval_data->current_function->eval_instructions.insert(
						idx_start_instruction+size_ei_switch_condition+size_ei_cases,
						(zs_int)ei_jmp_default
				);

				offset_end_instruction=eval_data->current_function->eval_instructions.count;
				for(unsigned i=idx_start_instruction; i < eval_data->current_function->eval_instructions.count;i++){
					Instruction *ins=&((EvalInstruction *)eval_data->current_function->eval_instructions.items[i])->vm_instruction;
					if(ins->value_op1==ZS_IDX_INSTRUCTION_JMP_BREAK){
						ins->value_op1=IDX_ZS_UNDEFINED;
						ins->value_op2=offset_end_instruction-i;
					}
				}

				// deallocate all cases
				for(unsigned i=0; i < eic_cases.count; i++){
					EvalInstructionCase * eic_current=((EvalInstructionCase *)eic_cases.items[i]);
					if(eic_current != NULL){
						delete eic_current;
					}
				}

			}else{

				eval_switch_deallocate(ei_switch_condition,eic_cases,&ei_jmp_default);

			}

			return aux_p+1;


		}

eval_keyword_switch_error:

		eval_switch_deallocate(ei_switch_condition,eic_cases,&ei_jmp_default);

		return NULL;
	}
}




