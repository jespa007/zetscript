#include "eval.h"

namespace zetscript{

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
				if_jnt=ei_aux;
				int idx_start_block=eval_data->current_function->instructions.size();
				//ei_jmps.push_back(ei_aux);

				IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);
				if(*aux_p != '{'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected if-block open block ('{')");

				}

				aux_p=eval_block(eval_data,aux_p
					,line
					,scope_info
				);

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
						aux_p=eval_block(
								eval_data
								,aux_p
								,line
								,scope_info
						);
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

	char * eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
		char *aux_p = (char *)s;
		Scope *scope_case=NULL;
		std::string val;
		Keyword key_w;//,key_w2;
		std::vector<EvalInstruction *> 	switch_expression; // stores all conditional instructions at begin
		std::vector<EvalInstruction *> 	ei_cases; // stores all conditional instructions at begin
		std::vector<EvalInstruction *>  ei_break_jmps; // breaks or if condition not satisfies nothing (there's no default)
		EvalInstruction *jmp_default = NULL;

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);


		if(key_w == Keyword::KEYWORD_SWITCH){

			aux_p += strlen(eval_data_keywords[key_w].str);
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p == '('){
					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
					// evaluate switch condition expression ...
					aux_p = eval_expression(
						eval_data
						,aux_p
						,line
						,scope_info
						,&switch_expression
						,std::vector<char>{')'}
					);

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					if(*aux_p == '{'){

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						jmp_default=NULL;
						int idx_start_instruction = (int)(eval_data->current_function->instructions.size());
						//int n_default=0;

						while(*aux_p != '}' && *aux_p != 0){

							bool is_default=false;
							// search for case or default...
							key_w = eval_is_keyword(aux_p);
							if(key_w == KEYWORD_CASE){

								TokenNode token_symbol;
								PreOperation pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;


								// ignore case
								IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);


								if((pre_operation = is_pre_operation(aux_p))!=PreOperation::PRE_OPERATION_UNKNOWN){
									IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_pre_operations[pre_operation].str),line);
								}

								// capture constant value (should be a constant -not a identifier in any case-)
								aux_p=eval_symbol(
									eval_data
									,aux_p
									,line
									,scope_info
									,&token_symbol
									,pre_operation
								);

								if(token_symbol.token_type != TOKEN_TYPE_LITERAL){
									EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"'case' only accepts literals");
								}

								// insert a pair of instructions...
								ei_cases.push_back(token_symbol.instructions[0]);
								ei_cases.push_back(new EvalInstruction(
										BYTE_CODE_JE
										,ZS_IDX_UNDEFINED
										,((int)(eval_data->current_function->instructions.size()))-idx_start_instruction	 // offset
								));

							}else if(key_w == KEYWORD_DEFAULT){
								if(jmp_default!=NULL){
									EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"there's an already 'default' case");
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
								EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"Expected 'case' or 'default' keyword");
							}

							if(*aux_p!=':'){
								EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"Expected ':' ");
							}

							// ignore :
							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if((eval_is_keyword(aux_p) == Keyword::KEYWORD_CASE)){ // another case ...
								continue;
							}

							aux_p=eval_parse_and_compile_recursive(
								eval_data
								,aux_p
								, line
								, scope_info
								, true // cancel eval when break or case is found...
							);

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
											,ZS_IDX_UNDEFINED
											,ZS_IDX_INSTRUCTION_JMP_BREAK
									);
									eval_data->current_function->instructions.push_back(ei_break_jmp);
									ei_break_jmps.push_back(ei_break_jmp);
								}


							}else if(is_default){
								EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"'default' needs a 'break' at the end");
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						}

						if(*aux_p != '}'){
							EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"Expected '}' switch");
						}

						// end instruction
						int offset_end_instruction=((int)(eval_data->current_function->instructions.size()))-idx_start_instruction;

						if(offset_end_instruction > 0 && ei_cases.size()> 0){ // there's conditions to manage

							//EvalInstruction *jmp_after_je_cases;

							int size_ei_cases=ei_cases.size();
							int size_ei_switch_expression = switch_expression.size();
							int size_ei_cases_and_switch_expression = size_ei_cases+size_ei_switch_expression+1; // +1 for jmp default if there's no default
							offset_end_instruction+=size_ei_cases_and_switch_expression;

							if(jmp_default == NULL){ // no default found so, we insert a default jmp to the end
								jmp_default=new EvalInstruction(
								BYTE_CODE_JMP
								,ZS_IDX_UNDEFINED
								,offset_end_instruction-size_ei_cases-size_ei_switch_expression
								);
							}

							// 1. Insert instruction condition cases
							eval_data->current_function->instructions.insert(
									eval_data->current_function->instructions.begin()+idx_start_instruction,
									switch_expression.begin(),
									switch_expression.end()
							);

							// 2. insert all cases found first from start + offset size instruction cases found
							eval_data->current_function->instructions.insert(
									eval_data->current_function->instructions.begin()+idx_start_instruction+size_ei_switch_expression,
									ei_cases.begin(),
									ei_cases.end()
							);

							// 3. insert last jmp to default or end switch
							eval_data->current_function->instructions.insert(
									eval_data->current_function->instructions.begin()+idx_start_instruction+size_ei_switch_expression+size_ei_cases,
									jmp_default
							);

							// update je-cases (multiple)and jmps
							for(unsigned i=0; i < ei_cases.size(); i+=2){ // to discard end jmp and += 2 because is x2 multiple
								Instruction *ins=&ei_cases[i+1]->vm_instruction; // load je
								ins->value_op2+=size_ei_cases-i; // --je minus distance
							}



							offset_end_instruction=eval_data->current_function->instructions.size();
							for(unsigned i=idx_start_instruction; i < eval_data->current_function->instructions.size();i++){
								Instruction *ins=&eval_data->current_function->instructions[i]->vm_instruction;
								if(ins->value_op2==ZS_IDX_INSTRUCTION_JMP_BREAK){
									ins->value_op2=offset_end_instruction-i;
								}
							}

						}else{

							// deallocate condition
							for(unsigned i=0; i < switch_expression.size(); i++){
								delete switch_expression[i];
							}
							switch_expression.clear();

							// deallocate all cases
							for(unsigned i=0; i < ei_cases.size(); i++){
								delete ei_cases[i];
							}
							ei_cases.clear();

							if(jmp_default != NULL){
								delete jmp_default;
							}
							jmp_default=NULL;
						}

						return aux_p+1;
					}
					else{
						EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"Expected '{' begin switch block");
					}
			}
			else{
				EVAL_ERROR_KEYWORD_SWITCH(eval_data->current_parsing_file,line,"Expected '(' switch ");
			}
		}

eval_keyword_switch_error:

		for(unsigned i=0; i < ei_cases.size(); i++){
			delete ei_cases[i];
		}
		ei_cases.clear();

		for(unsigned i=0; i < switch_expression.size(); i++){
			delete switch_expression[i];
		}
		switch_expression.clear();

		if(jmp_default!=NULL){
			delete jmp_default;
		}

		return NULL;
	}
}




