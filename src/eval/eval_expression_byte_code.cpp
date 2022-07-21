/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{

	typedef struct{
		EvalInstruction *	instruction_symbol; // affected symbol
		Operator			assign_operator; // operator
		TokenNode			*token_node;
	}AssignTokenInformation;

	char * eval_sub_expression(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, zs_vector 	* eval_instructions
			, const char *expected_ending_char
			, uint16_t properties // uint16_t properties
			, zs_vector *unique_call_instruction
	);


	bool eval_two_last_instruction_are_constants(zs_vector *_eval_instructions){

		size_t size_instructions=_eval_instructions->count;

		if(size_instructions >= 2){

			EvalInstruction *i1=(EvalInstruction *)_eval_instructions->items[size_instructions-2];
			EvalInstruction *i2=(EvalInstruction *)_eval_instructions->items[size_instructions-1];

			return i1->vm_instruction.isConstant() && i2->vm_instruction.isConstant();
		}

		return false;
	}


	// eval operator expression only evaluates expression with normal operators (+,-,>>,<<,etc) respecting always its preference. Assign operators (=,+=,-=,etc) should be extracted
	void eval_expression_tokens_to_byte_code(
			  EvalData *eval_data
			, Scope *scope
			, zs_vector 	* 	token_nodes
			, zs_vector 	*	eval_instructions
			, zs_vector     *   logical_and_jnt
			, zs_vector     *  logical_or_jt
			, int idx_start_eval_instructions
			, int idx_start
			, int idx_end
			, uint16_t properties

	){
		EvalInstruction 	*eval_instruction=NULL;
		int 				idx_split=-1;
		TokenNode      		*split_node = NULL;
		unsigned char 		idx_group_split=OPERATOR_GROUP_MAX;

		// trivial case (symbol node)
		if(idx_start>=idx_end){
			// concatenate instructions ...
			TokenNode *end_node=(TokenNode *)token_nodes->items[idx_start];
			end_node->are_instructions_moved=true; // mark as processed

			eval_instructions->concat(&end_node->eval_instructions);
			return;
		}

		// get the most preference operator by group(see eval_data::Operator)...
		for(int i=idx_start; i<= idx_end; i++){

			// get split preference
			if(((TokenNode *)(token_nodes->items[i]))->token_type == TokenType::TOKEN_TYPE_OPERATOR){
				unsigned char idx_group=get_operator_type_group(((TokenNode *)(token_nodes->items[i]))->operator_type);

				if(idx_group <= idx_group_split){
					idx_group_split=idx_group;
					idx_split=i;
				}
			}
		}

		if(idx_split == -1){
			THROW_RUNTIME_ERRORF("Cannot fint operator");
		}

		// split left/right by operator precedence...
		split_node=(TokenNode *)token_nodes->items[idx_split];

		// perform left side op (can have operators)...
		eval_expression_tokens_to_byte_code(
			eval_data
			,scope
			,token_nodes
			,eval_instructions
			, logical_and_jnt
			,logical_or_jt
			, idx_start_eval_instructions
			,idx_start
			,idx_split-1
			,properties
		);

		// perform right side op...
		eval_expression_tokens_to_byte_code(
			eval_data
			,scope
			,token_nodes
			,eval_instructions
			,logical_and_jnt
			,logical_or_jt
			,idx_start_eval_instructions
			,idx_split+1
			,idx_end
			,properties
		);

		//------------------------------------------------------------------------------------
		// OPTIMIZATION PART: Try to simplify 2 ops into one op
		//byte_code=convert_operator_to_byte_code(split_node->operator_type);
		bool two_last_instructions_are_constants=eval_two_last_instruction_are_constants(eval_instructions);
		if((split_node->operator_type == OPERATOR_LOGIC_AND) && (two_last_instructions_are_constants==false)){

			int insert_at=eval_instructions->count-1;
			int consecutive_logical_operators=0;
			for(int i=eval_instructions->count-1; i >= idx_start_eval_instructions; i--){
				EvalInstruction *current_instruction=(EvalInstruction *)eval_instructions->items[i];
				if(IS_BYTE_BOOLEAN_OP(current_instruction->vm_instruction.byte_code)){

					if(consecutive_logical_operators==1){
						insert_at=i+1;
						break;
					}

					consecutive_logical_operators++;
				}
			}

			// insert JT/acording type of JNT
			eval_instructions->insert(insert_at,
				(zs_int)(eval_instruction=new EvalInstruction(
					BYTE_CODE_JNT
					,ZS_IDX_INSTRUCTION_JNT_LOGIC_NEXT_OR
					,eval_instructions->count
				))
			);



			logical_and_jnt->push_back((zs_int)eval_instruction);

		}else if((split_node->operator_type == OPERATOR_LOGIC_OR) && (two_last_instructions_are_constants==false)){

			// insert jt in the last logical jnt
			if(logical_and_jnt->count>0){

				//search two consecutives compare byte codes from current
				int consecutive_logical_operators=0;

				for(int i=eval_instructions->count-1; i >= idx_start_eval_instructions && consecutive_logical_operators<2; i--){
					EvalInstruction *current_instruction=(EvalInstruction *)eval_instructions->items[i];
					// reset consecutive each JNT NEXT OR found
					if((current_instruction->vm_instruction.byte_code == BYTE_CODE_JNT) && (current_instruction->vm_instruction.value_op1== ZS_IDX_INSTRUCTION_JNT_LOGIC_NEXT_OR)){
						consecutive_logical_operators=0;
					}else if(IS_BYTE_BOOLEAN_OP(current_instruction->vm_instruction.byte_code)){
						consecutive_logical_operators++;

						if(consecutive_logical_operators==2){ // found the gap, insert JT
							eval_instructions->insert(
								i+1
								,(zs_int)(eval_instruction=new EvalInstruction(
										BYTE_CODE_JT
										,ZS_IDX_INSTRUCTION_JT_LOGIC_OK
								))
							);

							logical_or_jt->push_back((zs_int)eval_instruction);
						}
					}

				}


			}

		}else{
			if((eval_instruction=eval_expression_optimize(eval_data,scope,split_node, eval_instructions))==NULL){
				// cannot be simplified...
				// push operator byte code...
				eval_instruction=new EvalInstruction(
					eval_operator_to_byte_code(split_node->operator_type)
				);
			}

			// push to the end
			eval_instructions->push_back(
					(zs_int)eval_instruction
			);
		}


		// OPTIMIZATION PART
		//------------------------------------------------------------------------------------

		if(eval_instruction!=NULL){
			eval_instruction->instruction_source_info= InstructionSourceInfo(
					eval_data->current_parsing_file
					,split_node->line
					,NULL
			);
		}
	}

	void eval_deallocate_zs_ei_assign_loader_instructions_post_expression(zs_vector & zs_ei_assign_loader_instructions_post_expression){
		for(int i=0; i<zs_ei_assign_loader_instructions_post_expression.count; i++ ){
			delete (zs_vector *)zs_ei_assign_loader_instructions_post_expression.items[i];
		}
		zs_ei_assign_loader_instructions_post_expression.clear();
	}

	char * eval_expression_to_byte_code(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, zs_vector *dst_instructions
			, zs_vector * token_nodes
			, uint16_t properties
			, zs_vector *unique_call_instructions
		){

		char *aux_p=(char *)s;
		zs_vector assing_token_informations;
		int idx_start=0;
		int idx_end=(int)(token_nodes->count-1);
		zs_vector zs_ei_assign_loader_instructions_post_expression; // zs_vector<zs_vector<EvalInstruction *>>
		zs_vector ei_assign_store_instruction_post_expression;
		int idx_start_instructions=0;
		zs_vector logical_and_jnt,logical_or_jt;
		bool create_jmp_false=false;

		// search for assign
		for(int i=idx_end; i >= 0; i--){
			Operator token_operator = ((TokenNode *)token_nodes->items[i])->operator_type;

			if(IS_OPERATOR_TYPE_ASSIGN(token_operator)){ // ... save all assignables from operator split
					idx_start=i+1;
					break;
			}
		}

		//--------------------------------------------------------------
		// operator assign = found --> assign operators, load identifiers first
		for(int i=0; i < idx_start; i+=2){ // starting from assign operator if idx_start > 0 += 2 is because there's a symbol followed by its operator
			EvalInstruction *eval_instruction=NULL;
			TokenNode * token_node_symbol = (TokenNode *)token_nodes->items[i];
			TokenNode * token_node_operator = (TokenNode *)token_nodes->items[i+1];
			Operator operator_type=token_node_operator->operator_type;
			zs_vector *ei_assign_loader_instructions_post_expression=NULL;

			// Check for operators found. Each operator found it has to be (=,+=,-=,... etc)
			if(!(
					(token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR)
				&&	(IS_OPERATOR_TYPE_ASSIGN(operator_type))

				)){ // ... save all assignables from operator split
				EVAL_ERROR_FILE_LINE_GOTO(eval_data->current_parsing_file,token_node_operator->line,eval_error_byte_code,"Operation '%s' in assignment is not allowed",eval_data_operators[operator_type].str);
			}

			// should be identifier...
			if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
				EVAL_ERROR_FILE_LINE_GOTO(eval_data->current_parsing_file,token_node_symbol->line,eval_error_byte_code,"Assign a literal '%s' is not allowed",token_node_symbol->value.c_str());
			}

			EvalInstruction *ei_load_assign_instruction=((EvalInstruction *)token_node_symbol->eval_instructions.items[token_node_symbol->eval_instructions.count-1]);

			if(IS_BYTE_CODE_CALL(
					ei_load_assign_instruction->vm_instruction.byte_code
			)){
				EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,ei_load_assign_instruction->instruction_source_info.line
						,eval_error_byte_code
						,"Calling a function in left assignment is not allowed");
			}

			zs_ei_assign_loader_instructions_post_expression.push_back((zs_int)(ei_assign_loader_instructions_post_expression=new zs_vector()));

			// assign operators: add instructions related about its accessors...
			for(int j=0;j<token_node_symbol->eval_instructions.count;j++){
				ei_load_assign_instruction=(EvalInstruction *)token_node_symbol->eval_instructions.items[j];
				ei_assign_loader_instructions_post_expression->push_back((zs_int)(token_node_symbol->eval_instructions.items[j]));
			}

			// get last instruction...
			EvalInstruction *ei_last_load_instruction=(EvalInstruction *)ei_assign_loader_instructions_post_expression->items[ei_assign_loader_instructions_post_expression->count-1];
			Instruction *last_load_instruction=&ei_last_load_instruction->vm_instruction;

			if(byte_code_is_load_var_type(last_load_instruction->byte_code)){

				if(last_load_instruction->byte_code==BYTE_CODE_LOAD_THIS){
					EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,ei_last_load_instruction->instruction_source_info.line
						,eval_error_byte_code
						,"'this' is not assignable");
				}

				last_load_instruction->byte_code=byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
			}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
				last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}

			// ... add assign operations to the list to add later after eval all instructions
			ei_assign_store_instruction_post_expression.push_back((zs_int)(eval_instruction=new EvalInstruction(
					eval_operator_to_byte_code(operator_type),1
			)));

			eval_instruction->instruction_source_info= InstructionSourceInfo(
					eval_data->current_parsing_file
					,token_node_operator->line
					,NULL
			);
		}
		//--------------------------------------------------------------

		// eval tokens

		idx_start_instructions=dst_instructions->count;

		try{
			eval_expression_tokens_to_byte_code(
				 eval_data
				, scope_info
				, token_nodes
				, dst_instructions
				, &logical_and_jnt
				, &logical_or_jt
				, dst_instructions->count
				, idx_start
				, idx_end
				, properties
			);
		}catch(std::exception & error){
			EVAL_ERROR_BYTE_CODEF(error.what());
		}

		if(logical_or_jt.count > 0 || logical_and_jnt.count > 0){
			dst_instructions->push_back(
				(zs_int)(new EvalInstruction(
					BYTE_CODE_JMP
					,ZS_IDX_UNDEFINED
					,2
				))
			);
		}


		// check logical and/or jnt/jt from start instructions
		/*for(int i=idx_start_instructions; i < dst_instructions->count; i++){
			EvalInstruction *eval_instruction=(EvalInstruction *)dst_instructions->items[i];
			if((eval_instruction->vm_instruction.byte_code == BYTE_CODE_JNT) && (eval_instruction->vm_instruction.value_op1== ZS_IDX_INSTRUCTION_JNT_LOGIC_NEXT_OR)){
				//logical_and_jnt.push_back((intptr_t)eval_instruction);
				// go to next logic or
				int idx_next_or_found=-1;
				for(int j=i; j < dst_instructions->count; j++){
					EvalInstruction *eval_instruction_post_instruction=(EvalInstruction *)dst_instructions->items[j];
					if((eval_instruction_post_instruction->vm_instruction.byte_code == BYTE_CODE_JT) && (eval_instruction_post_instruction->vm_instruction.value_op1== ZS_IDX_INSTRUCTION_JT_LOGIC_OK)){
						idx_next_or_found=j;
						break;
					}
				}

				if(idx_next_or_found != -1){ // next or found
					eval_instruction->vm_instruction.value_op2=idx_next_or_found-i+1;
				}else{ // go to end with false
					eval_instruction->vm_instruction.value_op2=dst_instructions->count-i+1;
					create_jmp_false=true;
				}
			}
			else
			if((eval_instruction->vm_instruction.byte_code == BYTE_CODE_JT) && (eval_instruction->vm_instruction.value_op1== ZS_IDX_INSTRUCTION_JT_LOGIC_OK)){
				eval_instruction->vm_instruction.value_op2=dst_instructions->count-i;
				logical_or_jt.push_back((intptr_t)eval_instruction);
			}
		}*/



		if(logical_or_jt.count > 0){

			dst_instructions->push_back(
				(zs_int)(new EvalInstruction(
					BYTE_CODE_LOAD_BOOL
					,ZS_IDX_UNDEFINED
					,1
				))
			);

			dst_instructions->push_back(
				(zs_int)(new EvalInstruction(
					BYTE_CODE_JMP
					,ZS_IDX_UNDEFINED
					,2
				))
			);
		}

		if(logical_and_jnt.count>0){

			if(logical_or_jt.count > 0){ // modify jmp instruction
				((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction.value_op2+=2;
			}

			dst_instructions->push_back(
				(zs_int)(new EvalInstruction(
					BYTE_CODE_LOAD_BOOL
					,ZS_IDX_UNDEFINED
					,0
				))
			);

		}
			/*eval_instructions->push_back(
				(zs_int)(new EvalInstruction(
					BYTE_CODE_LOAD_BOOL
					,ZS_IDX_UNDEFINED
					,1
				))
			);

			for(int i=0; i < logical_and_jnt.count;i++){
			}
		}

		if(logical_and_jnt.count > 0){
			// insert JT/acording type of JNT
			eval_instructions->push_back(
				(zs_int)(new EvalInstruction(
					BYTE_CODE_LOAD_BOOL
					,ZS_IDX_UNDEFINED
					,0
				))
			);

			for(int i=0; i < logical_and_jnt.count;i++){

			}
		}*/



		// if ends with ternary then continues performing expressions
		if(*aux_p == '?'){ // ternary
			EvalInstruction *ei_ternary_if_jnt=NULL;
			EvalInstruction *ei_ternary_else_jmp=NULL;
			Instruction *last_instruction=NULL;

			// insert JNT
			int jnt_instructions_start=dst_instructions->count;
			dst_instructions->push_back((zs_int)(ei_ternary_if_jnt=new EvalInstruction(BYTE_CODE_JNT)));
			ei_ternary_if_jnt->instruction_source_info.file=eval_data->current_parsing_file;
			ei_ternary_if_jnt->instruction_source_info.line=line;
			int jmp_instructions_start=0;
			int body_size_if=0;
			int body_size_else=0;

			// eval ? part
			if((aux_p=eval_sub_expression(
				eval_data
				,aux_p+1
				, line
				, scope_info
				, dst_instructions
				, NULL
				, properties
				, unique_call_instructions
				//, n_return_values
			))==NULL){
				goto eval_error_byte_code;
			}

			if(unique_call_instructions != NULL){
				last_instruction=&(((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction);
				if(IS_BYTE_CODE_CALL(last_instruction->byte_code) && token_nodes->count==1){
					//INSTRUCTION_SET_RETURN_COUNT(last_instruction,n_return_values);
					unique_call_instructions->push_back((zs_int)last_instruction);
				}
			}

			// TODO: JEB Check whether expression is constant true/false
			if(*aux_p != ':'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					eval_data->current_parsing_file
					,line
					,eval_error_byte_code
					,"Expected ':' on ternary expression"
				);
			}


			jmp_instructions_start=dst_instructions->count;
			dst_instructions->push_back((zs_int)(ei_ternary_else_jmp=new EvalInstruction(BYTE_CODE_JMP)));
			body_size_if=dst_instructions->count-jnt_instructions_start; // size body "if" takes jmp as part of it



			// eval : part
			if((aux_p=eval_sub_expression(
				eval_data
				, aux_p+1
				, line
				, scope_info
				, dst_instructions
				, NULL
				, properties
				,unique_call_instructions
				//,n_return_values
			))==NULL){
				goto eval_error_byte_code;
			}

			body_size_else=dst_instructions->count-jmp_instructions_start;

			if(unique_call_instructions != NULL){
				last_instruction=&((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction;
				if(IS_BYTE_CODE_CALL(last_instruction->byte_code) && token_nodes->count==1){
					unique_call_instructions->push_back((zs_int)last_instruction);
				}
			}


			ei_ternary_if_jnt->vm_instruction.value_op2=body_size_if; // +1 because
			ei_ternary_else_jmp->vm_instruction.value_op2=body_size_else;

		}else{
			if(unique_call_instructions != NULL){
				Instruction *last_instruction=&((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction;
				if(IS_BYTE_CODE_CALL(last_instruction->byte_code) && token_nodes->count==1){
					unique_call_instructions->push_back((zs_int)last_instruction);
				}
			}
		}
		//--------------------------------------------------------------

		// ... finally save store operators
		for(int i=(int)(zs_ei_assign_loader_instructions_post_expression.count-1); i >=0 ;i--){
			//loaders
			dst_instructions->concat((zs_vector *)zs_ei_assign_loader_instructions_post_expression.items[i]);

			// push back assign operator
			dst_instructions->push_back(
				ei_assign_store_instruction_post_expression.items[i]
			);
		}

		eval_deallocate_zs_ei_assign_loader_instructions_post_expression(zs_ei_assign_loader_instructions_post_expression);

		return aux_p;

eval_error_byte_code:

		// only delete the new ones
		for(int i=0; i < ei_assign_store_instruction_post_expression.count; i++){
			delete (EvalInstruction *)ei_assign_store_instruction_post_expression.items[i];
		}

		eval_deallocate_zs_ei_assign_loader_instructions_post_expression(zs_ei_assign_loader_instructions_post_expression);

		return NULL;

	}

}

