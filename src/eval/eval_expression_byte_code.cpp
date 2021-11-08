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
			, const char *expected_ending_char=NULL
			, uint16_t properties=0 // uint16_t properties
			, int n_recursive_level=0
			, int n_return_values=1
	);



	// eval operator expression only evaluates expression with normal operators (+,-,>>,<<,etc) respecting always its preference. Assign operators (=,+=,-=,etc) should be extracted
	void eval_expression_tokens_to_byte_code(
			  EvalData *eval_data
			, Scope *scope
			, zs_vector 	* 	token_nodes
			, zs_vector 	*	eval_instructions
			, int idx_start
			, int idx_end
			, uint16_t properties
	){
		EvalInstruction 	*eval_instruction=NULL;
		int 				idx_split=-1;
		TokenNode      		*split_node = NULL;
		Operator 			op_split=Operator::OPERATOR_MAX;
		EvalInstruction 	*left_eval_constant=NULL;
		EvalInstruction 	*right_eval_constant=NULL;
		unsigned char 		idx_group_split=OPERATOR_GROUP_MAX;
		bool is_left_branch_end=false, is_right_branch_end=false;
		bool is_left_token_simplifiable=false,is_right_token_simplifiable=false;
		//TokenNode		*left_token,*right_token;
		bool simplified_ok=false;
		//ByteCode byte_code;

		// trivial case (symbol node)
		if(idx_start>=idx_end){
			// concatenate instructions ...
			TokenNode *end_node=(TokenNode *)token_nodes->items[idx_start];
			end_node->are_instructions_moved=true; // mark as processed

			eval_instructions->concat(&end_node->eval_instructions);
			return;
		}

		// get the most preference operator by group(see eval_data::Operator)...
		for(int i=idx_end; i >= idx_start; i--){

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
			THROW_RUNTIME_ERROR("Cannot fint operator");
		}

		// split left/right by operator precedence...
		split_node=(TokenNode *)token_nodes->items[idx_split];

		// perform left side op (can have operators)...
		eval_expression_tokens_to_byte_code(
			eval_data
			,scope
			,token_nodes
			,eval_instructions
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
			,idx_split+1
			,idx_end
			,properties
		);

		//------------------------------------------------------------------------------------
		// OPTIMIZATION PART: Try to simplify 2 ops into one op
		//byte_code=convert_operator_to_byte_code(split_node->operator_type);
		if((eval_instruction=eval_expression_optimize(eval_data,scope,split_node, eval_instructions))==NULL){
		 	// cannot be simplified...
			// push operator byte code...
			eval_instruction=new EvalInstruction(
				eval_operator_to_byte_code(split_node->operator_type)
			);
		}

		eval_instructions->push_back(
				(zs_int)eval_instruction
		);
		// OPTIMIZATION PART
		//------------------------------------------------------------------------------------


		eval_instruction->instruction_source_info= InstructionSourceInfo(
				eval_data->current_parsing_file
				,split_node->line
				,NULL
		);
	}

	void eval_deallocate_zs_ei_assign_loader_instructions_post_expression(zs_vector & zs_ei_assign_loader_instructions_post_expression){
		for(unsigned i=0; i<zs_ei_assign_loader_instructions_post_expression.count; i++ ){
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
			, int n_recursion_level
			, uint8_t n_return_values
		){

		char *aux_p=(char *)s;
		zs_vector assing_token_informations;
		int idx_start=0;
		int idx_end=(int)(token_nodes->count-1);
		zs_vector zs_ei_assign_loader_instructions_post_expression; // zs_vector<zs_vector<EvalInstruction *>>
		zs_vector ei_assign_store_instruction_post_expression;

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
			int idx_post_operation = i >> 1;
			TokenNode * token_node_symbol = (TokenNode *)token_nodes->items[i];
			TokenNode * token_node_operator = (TokenNode *)token_nodes->items[i+1];
			Operator operator_type=token_node_operator->operator_type;
			zs_vector *ei_assign_loader_instructions_post_expression=NULL;
			int end_idx=(int)(token_nodes->count-1);

			// Check for operators found. Each operator found it has to be (=,+=,-=,... etc)
			if(!(
					(token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR)
				&&	(IS_OPERATOR_TYPE_ASSIGN(operator_type))

				)){ // ... save all assignables from operator split
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_error_byte_code,eval_data->current_parsing_file,token_node_operator->line,"Operation \"%s\" in assignment is not allowed",eval_data_operators[operator_type].str);
			}

			// should be identifier...
			if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_error_byte_code,eval_data->current_parsing_file,token_node_symbol->line,"Assign a literal \"%s\" is not allowed",token_node_symbol->value.c_str());
			}

			zs_ei_assign_loader_instructions_post_expression.push_back((zs_int)(ei_assign_loader_instructions_post_expression=new zs_vector()));


			// add instructions related about its accessors...
			for(unsigned j=0;j<token_node_symbol->eval_instructions.count;j++){
				EvalInstruction *ei_load_assign_instruction=(EvalInstruction *)token_node_symbol->eval_instructions.items[j];
				if(INSTRUCTION_IS_BYTE_CODE_CALL(&ei_load_assign_instruction->vm_instruction)){
					EVAL_ERROR_FILE_LINE_AND_GOTO(
							eval_error_byte_code
							,eval_data->current_parsing_file
							,ei_load_assign_instruction->instruction_source_info.line
							,"Calling a function in left assignment is not allowed");
				}
				ei_assign_loader_instructions_post_expression->push_back((zs_int)(token_node_symbol->eval_instructions.items[j]));
			}

			// get last instruction...
			EvalInstruction *ei_last_load_instruction=(EvalInstruction *)ei_assign_loader_instructions_post_expression->items[ei_assign_loader_instructions_post_expression->count-1];
			Instruction *last_load_instruction=&ei_last_load_instruction->vm_instruction;

			if(byte_code_is_load_var_type(last_load_instruction->byte_code)){

				if(last_load_instruction->byte_code==BYTE_CODE_LOAD_THIS){
					EVAL_ERROR_FILE_LINE_AND_GOTO(
						eval_error_byte_code
						,eval_data->current_parsing_file
						,ei_last_load_instruction->instruction_source_info.line
						,"'this' is not assignable");
				}

				last_load_instruction->byte_code=byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
			}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
				last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}

			// ... add arithmetic operator byte code
			ei_assign_store_instruction_post_expression.push_back((zs_int)(eval_instruction=new EvalInstruction(
					eval_operator_to_byte_code(operator_type)
			)));

			eval_instruction->instruction_source_info= InstructionSourceInfo(
					eval_data->current_parsing_file
					,token_node_operator->line
					,NULL
			);

		}
		//--------------------------------------------------------------

		// eval right expression
		try{
			eval_expression_tokens_to_byte_code(
				 eval_data
				, scope_info
				, token_nodes
				, dst_instructions
				, idx_start
				, idx_end
				, properties
			);
		}catch(std::exception & error){
			EVAL_ERROR_BYTE_CODE(error.what());
		}

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
				, n_recursion_level+1
				, n_return_values
			))==NULL){
				goto eval_error_byte_code;
			}

			last_instruction=&(((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction);
			if(INSTRUCTION_IS_BYTE_CODE_CALL(last_instruction)){//(n_recursion_level == 0) && INSTRUCTION_IS_BYTE_CODE_CALL(last_instruction) && (properties & EVAL_EXPRESSION_ON_MAIN_BLOCK)){ // --> allow all stack return
				INSTRUCTION_SET_RETURN_COUNT(last_instruction,n_return_values);
			}

			// TODO: JEB Check whether expression is constant true/false
			if(*aux_p != ':'){
				EVAL_ERROR_FILE_LINE_AND_GOTO(eval_error_byte_code,eval_data->current_parsing_file,line ,"Expected ':' on ternary expression");
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
				, n_recursion_level+1
				,n_return_values
			))==NULL){
				goto eval_error_byte_code;
			}

			body_size_else=dst_instructions->count-jmp_instructions_start;

			last_instruction=&((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction;
			if(INSTRUCTION_IS_BYTE_CODE_CALL(last_instruction)){//(n_recursion_level == 0) && INSTRUCTION_IS_BYTE_CODE_CALL(last_instruction) && (properties & EVAL_EXPRESSION_ON_MAIN_BLOCK)){ // --> allow all stack return
				INSTRUCTION_SET_RETURN_COUNT(last_instruction,n_return_values);
			}


			ei_ternary_if_jnt->vm_instruction.value_op2=body_size_if; // +1 because
			ei_ternary_else_jmp->vm_instruction.value_op2=body_size_else;

		}else{
			Instruction *last_instruction=&((EvalInstruction *)dst_instructions->items[dst_instructions->count-1])->vm_instruction;
			if(INSTRUCTION_IS_BYTE_CODE_CALL(last_instruction)){//(n_recursion_level == 0) && INSTRUCTION_IS_BYTE_CODE_CALL(last_instruction) && (properties & EVAL_EXPRESSION_ON_MAIN_BLOCK)){ // --> allow all stack return
				INSTRUCTION_SET_RETURN_COUNT(last_instruction,n_return_values);
			}
		}
		//--------------------------------------------------------------

		// ... finally save store operators
		for(int i=(int)(zs_ei_assign_loader_instructions_post_expression.count-1); i >=0 ;i--){
			//loaders
			zs_vector *ei_assign_loader_instructions_post_expression=(zs_vector *)zs_ei_assign_loader_instructions_post_expression.items[i];

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
		for(unsigned i=0; i < ei_assign_store_instruction_post_expression.count; i++){
			delete (EvalInstruction *)ei_assign_store_instruction_post_expression.items[i];
		}

		eval_deallocate_zs_ei_assign_loader_instructions_post_expression(zs_ei_assign_loader_instructions_post_expression);

		return NULL;

	}

}

