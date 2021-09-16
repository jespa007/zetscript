#include "eval.h"

namespace zetscript{

	typedef struct{
		EvalInstruction *je_instruction;
		std::vector<EvalInstruction *> load_symbol_instructions;
	}EvalInstructionCase;

	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONDITIONALS
	//
	char * eval_keyword_if_else(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr;//,*start_symbol;
		Keyword key_w;
		//std::vector<EvalInstruction *> ei_jmps;
		EvalInstruction *if_jnt;
		std::vector<EvalInstruction *> else_end_jmp;
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
				end_expr = eval_expression(
						eval_data
						,aux_p+1
						,line
						,scope_info
						,&eval_data->current_function->instructions
						,std::vector<char>{')'}
				);

				// insert instruction if evaluated expression
				eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JNT));
				ei_aux->instruction_source_info.file=eval_data->current_parsing_file;
				ei_aux->instruction_source_info.line=line;

				if_jnt=ei_aux;
				int idx_start_block=eval_data->current_function->instructions.size();
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

				int idx_end_block=eval_data->current_function->instructions.size();

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				bool else_key = false;
				if((key_w = eval_is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
					else_key = (key_w == Keyword::KEYWORD_ELSE);
				}

				if_jnt->vm_instruction.value_op2=idx_end_block-idx_start_block+1;

				if(else_key){ // it finish if-else


					// we should insert jmp to end conditional chain if/else...
					eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(
							BYTE_CODE_JMP
							,ZS_IDX_UNDEFINED
							,eval_data->current_function->instructions.size()
							));
					if_jnt->vm_instruction.value_op2+=1; // sum +1 because we inserted a jmp for else

					else_end_jmp.push_back(ei_aux);

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

			for(unsigned i=0; i < else_end_jmp.size(); i++){
				Instruction *ins=&else_end_jmp[i]->vm_instruction;
				ins->value_op2=eval_data->current_function->instructions.size()-ins->value_op2;
			}
			return aux_p;
		}
		return NULL;
	}


	void eval_switch_deallocate(std::vector<EvalInstruction *> & ei_switch_condition,std::vector<EvalInstructionCase> & _ei_cases,EvalInstruction **_jmp_default){

		// deallocate condition
		for(unsigned i=0; i < ei_switch_condition.size(); i++){
			delete ei_switch_condition[i];
		}
		ei_switch_condition.clear();

		// deallocate all cases
		for(unsigned i=0; i < _ei_cases.size(); i++){
			delete _ei_cases[i].je_instruction;
			for(unsigned j=0; j < _ei_cases[i].load_symbol_instructions.size(); j++){
				delete _ei_cases[i].load_symbol_instructions[j];
			}
		}
		_ei_cases.clear();

		if(*_jmp_default != NULL){
			delete *_jmp_default;
		}
		*_jmp_default=NULL;
	}

	char * eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){



		char *aux_p = (char *)s;
		Scope *scope_case=NULL;
		zs_string val;
		Keyword key_w;//,key_w2;
		std::vector<EvalInstruction *> 		ei_switch_condition; // switch condition
		std::vector<EvalInstructionCase> 	ei_cases; // stores all conditional instructions at begin
		std::vector<EvalInstruction *>  	ei_break_jmps; // breaks or if condition not satisfies nothing (there's no default)
		EvalInstruction *jmp_default = NULL;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);


		if(key_w == Keyword::KEYWORD_SWITCH){
			int idx_start_instruction = (int)(eval_data->current_function->instructions.size());;
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
				,std::vector<char>{')'}
			);

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: Expected '{' begin switch block");
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			jmp_default=NULL;

			//int n_default=0;

			while(*aux_p != '}' && *aux_p != 0){

				bool is_default=false;
				// search for case or default...
				key_w = eval_is_keyword(aux_p);
				if(key_w == KEYWORD_CASE){
					int line_case=line;
					EvalInstructionCase eval_instruction_case;
					//std::vector<EvalInstruction *> aux_instructions;
					//TokenNode token_symbol;
					//PreOperation pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;


					// ignore case
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);


					/*if((pre_operation = is_pre_operation(aux_p))!=PreOperation::PRE_OPERATION_UNKNOWN){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_pre_operations[pre_operation].str),line);
					}*/

					// capture constant value (should be a constant -not a identifier in any case-)
					/*if((aux_p=eval_symbol(
						eval_data
						,aux_p
						,line
						,scope_info
						,&token_symbol
						,pre_operation
					))==NULL){
						EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line," expected constant or literal after 'case'");
					}

					if(token_symbol.token_type != TOKEN_TYPE_LITERAL){
						EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line," expected constant or literal after 'case'");
					}*/


					// get token symbol
					if((aux_p = eval_expression(
							eval_data
							,aux_p
							,line
							,scope_info
							,&eval_instruction_case.load_symbol_instructions
							,{':'}
							,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
					))==NULL){
						// delete unusued vars_for
						return NULL;
					}

					// insert a pair of instructions...
					//token_symbol.instructions[0]);

					eval_instruction_case.je_instruction=new EvalInstruction(
							BYTE_CODE_JE_CASE
							,ZS_IDX_UNDEFINED
							,((int)(eval_data->current_function->instructions.size()))-idx_start_instruction	 // offset
					);

					eval_instruction_case.je_instruction->instruction_source_info.line=line_case;
					eval_instruction_case.je_instruction->instruction_source_info.file=eval_data->current_parsing_file;


					ei_cases.push_back(eval_instruction_case);

					// update size cases
					size_ei_cases+=(eval_instruction_case.load_symbol_instructions.size()+1); // +1 is for je (i.e je_instruction)

				}else if(key_w == KEYWORD_DEFAULT){
					if(jmp_default!=NULL){
						EVAL_ERROR_FILE_LINE_AND_GOTO(eval_keyword_switch_error,eval_data->current_parsing_file,line,"Syntax error switch: there's an already 'default' case");
					}

					jmp_default=new EvalInstruction(
							BYTE_CODE_JMP
							,ZS_IDX_UNDEFINED
							,((int)(eval_data->current_function->instructions.size()))-idx_start_instruction+1	 // offset
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
						eval_data->current_function->instructions.push_back(ei_break_jmp);
						ei_break_jmps.push_back(ei_break_jmp);
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
			int offset_end_instruction=((int)(eval_data->current_function->instructions.size()))-idx_start_instruction;

			if(offset_end_instruction > 0 && ei_cases.size()> 0){ // there's conditions to manage

				//EvalInstruction *jmp_after_je_cases;

				//int size_ei_cases=ei_cases.size();
				unsigned offset_ei_case=0;
				int size_ei_switch_condition = ei_switch_condition.size();
				int size_ei_cases_and_switch_expression = size_ei_cases+size_ei_switch_condition+1; // +1 for jmp default if there's no default
				int total_instructions_switch=offset_end_instruction+size_ei_cases_and_switch_expression; // total instructions switch

				if(jmp_default == NULL){ // no default found so, we insert a default jmp to the end
					jmp_default=new EvalInstruction(
					BYTE_CODE_JMP
					,ZS_IDX_UNDEFINED
					,total_instructions_switch-size_ei_cases-size_ei_switch_condition
					);
				}

				// 1. Insert eval instructions for switch condition
				eval_data->current_function->instructions.insert(
						eval_data->current_function->instructions.begin()+idx_start_instruction,
						ei_switch_condition.begin(),
						ei_switch_condition.end()
				);

				// 2. insert all cases found first from start + offset size instruction cases found
				for(unsigned i=0; i < ei_cases.size(); i++){
					Instruction *ins=&ei_cases[i].je_instruction->vm_instruction; // load je

					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+idx_start_instruction+size_ei_switch_condition+offset_ei_case,
							ei_cases[i].load_symbol_instructions.begin(),
							ei_cases[i].load_symbol_instructions.end()
					);

					offset_ei_case+=(ei_cases[i].load_symbol_instructions.size()); // load instruction token

					// insert je...
					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+idx_start_instruction+size_ei_switch_condition+offset_ei_case,
							ei_cases[i].je_instruction
					);

					offset_ei_case++; // je + 1

					ins->value_op2=ins->value_op2+(size_ei_cases+1)-(offset_ei_case)+1; // +1 jmp after ei cases
				}

				// 3. insert last jmp to default or end switch
				eval_data->current_function->instructions.insert(
						eval_data->current_function->instructions.begin()+idx_start_instruction+size_ei_switch_condition+size_ei_cases,
						jmp_default
				);

				offset_end_instruction=eval_data->current_function->instructions.size();
				for(unsigned i=idx_start_instruction; i < eval_data->current_function->instructions.size();i++){
					Instruction *ins=&eval_data->current_function->instructions[i]->vm_instruction;
					if(ins->value_op1==ZS_IDX_INSTRUCTION_JMP_BREAK){
						ins->value_op1=ZS_IDX_UNDEFINED;
						ins->value_op2=offset_end_instruction-i;
					}
				}

			}else{

				eval_switch_deallocate(ei_switch_condition,ei_cases,&jmp_default);

			}

			return aux_p+1;


		}

eval_keyword_switch_error:

		eval_switch_deallocate(ei_switch_condition,ei_cases,&jmp_default);

		return NULL;
	}
}




