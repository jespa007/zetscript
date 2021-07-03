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
			, std::vector<EvalInstruction *> 	* instructions
			, std::vector<char> expected_ending_char={}
			, uint16_t properties=0 // uint16_t properties
			, int n_recursive_level=0
			//, std::vector<EvalInstruction *> *only_call_instructions=NULL
	);



	// eval operator expression only evaluates expression with normal operators (+,-,>>,<<,etc) respecting always its preference. Assign operators (=,+=,-=,etc) should be extracted
	void eval_expression_tokens_to_byte_code(
			  EvalData *eval_data
			, Scope *scope
			, std::vector<TokenNode> * expression_tokens
			, std::vector<EvalInstruction *> *instructions
			, int idx_start
			, int idx_end
			, uint16_t properties
	){
		EvalInstruction *instruction=NULL;
		int 			idx_split=-1;
		TokenNode      *split_node = NULL;
		Operator 	op_split=Operator::OPERATOR_MAX;
		EvalInstruction *left_eval_constant=NULL;
		EvalInstruction *right_eval_constant=NULL;
		unsigned char 	idx_group_split=OPERATOR_GROUP_MAX;
		bool is_left_branch_end=false, is_right_branch_end=false;
		bool is_left_token_simplifiable=false,is_right_token_simplifiable=false;
		//TokenNode		*left_token,*right_token;
		bool simplified_ok=false;
		//ByteCode byte_code;

		// trivial case (symbol node)
		if(idx_start>=idx_end){
			// concatenate instructions ...
			TokenNode *end_node=&expression_tokens->at(idx_start);
			end_node->are_instructions_moved=true; // mark as processed

			instructions->insert(
					instructions->end()
				,	end_node->instructions.begin()
				, 	end_node->instructions.end()
			);
			return;
		}

		// get the most preference operator by group(see eval_data::Operator)...
		for(int i=idx_end; i >= idx_start; i--){

			// get split preference
			if(expression_tokens->at(i).token_type == TokenType::TOKEN_TYPE_OPERATOR){
				unsigned char idx_group=get_operator_type_group(expression_tokens->at(i).operator_type);

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
		split_node=&expression_tokens->at(idx_split);

		// perform left side op (can have operators)...
		eval_expression_tokens_to_byte_code(
			eval_data
			,scope
			,expression_tokens
			,instructions
			,idx_start
			,idx_split-1
			,properties
		);

		// perform right side op...
		eval_expression_tokens_to_byte_code(
			eval_data
			,scope
			,expression_tokens
			,instructions
			,idx_split+1
			,idx_end
			,properties
		);

		//------------------------------------------------------------------------------------
		// OPTIMIZATION PART: Try to simplify 2 ops into one op
		//byte_code=convert_operator_to_byte_code(split_node->operator_type);
		if((instruction=eval_expression_optimize(eval_data,scope,split_node, instructions))==NULL){
		 	// cannot be simplified...
			// push operator byte code...
			instruction=new EvalInstruction(
				eval_operator_to_byte_code(split_node->operator_type)
			);

		}


		instructions->push_back(
				instruction
		);
		// OPTIMIZATION PART
		//------------------------------------------------------------------------------------


		instruction->instruction_source_info= InstructionSourceInfo(
				eval_data->current_parsing_file
				,split_node->line
				,NULL
		);
	}

	char * eval_expression_to_byte_code(
			EvalData *eval_data
			,const char *s
			, int & line
			, Scope *scope_info
			, std::vector<EvalInstruction *> *dst_instructions
			, std::vector<TokenNode> * expression_tokens
			, uint16_t properties
			, int n_recursion_level

			//, std::vector<EvalInstruction *> *only_call_instructions
		){

		char *aux_p=(char *)s;
		std::vector<AssignTokenInformation> assing_tokens;
		int idx_start=0;
		int idx_end=(int)(expression_tokens->size()-1);
		std::vector<std::vector<EvalInstruction *>> assign_loader_instructions_post_expression;
		std::vector<EvalInstruction *> assign_store_instruction_post_expression;

		// search for assign
		for(int i=idx_end; i >= 0; i--){
			Operator token_operator = expression_tokens->at(i).operator_type;

			if(IS_OPERATOR_TYPE_ASSIGN(token_operator)){ // ... save all assignables from operator split
					idx_start=i+1;
					break;
			}
		}

		//--------------------------------------------------------------
		// operator assign = found --> assign operators, load identifiers first
		for(int i=0; i < idx_start; i+=2){ // starting from assign operator if idx_start > 0 += 2 is because there's a symbol followed by its operator
			EvalInstruction *instruction=NULL;
			int idx_post_operation = i >> 1;
			TokenNode * token_node_symbol = &expression_tokens->at(i);
			TokenNode * token_node_operator = &expression_tokens->at(i+1);
			Operator operator_type=token_node_operator->operator_type;
			int end_idx=(int)(expression_tokens->size()-1);

			// Check for operators found. Each operator found it has to be (=,+=,-=,... etc)
			if(!(
					(token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR)
				&&	(IS_OPERATOR_TYPE_ASSIGN(operator_type))

				)){ // ... save all assignables from operator split
				EVAL_ERROR_BYTE_CODE_FILE_LINE(eval_data->current_parsing_file,token_node_operator->line,"Operation \"%s\" in assignment is not allowed",eval_data_operators[operator_type].str);
			}

			// should be identifier...
			if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
				EVAL_ERROR_BYTE_CODE_FILE_LINE(eval_data->current_parsing_file,token_node_symbol->line,"Assign a literal \"%s\" is not allowed",token_node_symbol->value.c_str());
			}

			assign_loader_instructions_post_expression.push_back({});


			// add instructions related about its accessors...
			for(unsigned j=0;j<token_node_symbol->instructions.size();j++){
				EvalInstruction *ei_load_assign_instruction=token_node_symbol->instructions[j];
				if(ei_load_assign_instruction->vm_instruction.byte_code ==  BYTE_CODE_CALL){
					EVAL_ERROR_BYTE_CODE_FILE_LINE(
							eval_data->current_parsing_file
							,ei_load_assign_instruction->instruction_source_info.line
							,"Calling a function in left assignment is not allowed");
				}
				assign_loader_instructions_post_expression[idx_post_operation].push_back(token_node_symbol->instructions[j]);
			}

			// get last instruction...
			Instruction *last_load_instruction=&assign_loader_instructions_post_expression[idx_post_operation][assign_loader_instructions_post_expression[idx_post_operation].size()-1]->vm_instruction;



			if(byte_code_is_load_type(last_load_instruction->byte_code)){
				last_load_instruction->byte_code=byte_code_load_to_push_stk(last_load_instruction->byte_code);
			}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
				last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}

			// ... add arithmetic operator byte code
			assign_store_instruction_post_expression.push_back(instruction=new EvalInstruction(
					eval_operator_to_byte_code(operator_type)
			));

			instruction->instruction_source_info= InstructionSourceInfo(
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
				, expression_tokens
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
			int jnt_instructions_start=dst_instructions->size();
			dst_instructions->push_back(ei_ternary_if_jnt=new EvalInstruction(BYTE_CODE_JNT));
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
				, std::vector<char>{}
				, properties
				, n_recursion_level+1
			))==NULL){
				goto eval_error_byte_code;
			}

			last_instruction=&dst_instructions->at(dst_instructions->size()-1)->vm_instruction;
			if((n_recursion_level == 0) && (last_instruction->byte_code == BYTE_CODE_CALL) && (properties & EVAL_EXPRESSION_ON_MAIN_BLOCK)){ // --> allow all stack return
				last_instruction->value_op2=ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK;
			}

			// TODO: JEB Check whether expression is constant true/false
			if(*aux_p != ':'){
				EVAL_ERROR_BYTE_CODE_FILE_LINE(eval_data->current_parsing_file,line ,"Expected ':' on ternary expression");
			}


			jmp_instructions_start=dst_instructions->size();
			dst_instructions->push_back(ei_ternary_else_jmp=new EvalInstruction(BYTE_CODE_JMP));
			body_size_if=dst_instructions->size()-jnt_instructions_start; // size body "if" takes jmp as part of it



			// eval : part
			if((aux_p=eval_sub_expression(
				eval_data
				, aux_p+1
				, line
				, scope_info
				, dst_instructions
				, std::vector<char>{}
				, properties
				, n_recursion_level+1
			))==NULL){
				goto eval_error_byte_code;
			}

			body_size_else=dst_instructions->size()-jmp_instructions_start;

			last_instruction=&dst_instructions->at(dst_instructions->size()-1)->vm_instruction;
			if((n_recursion_level == 0) && (last_instruction->byte_code == BYTE_CODE_CALL) && (properties & EVAL_EXPRESSION_ON_MAIN_BLOCK)){ // --> allow all stack return
				last_instruction->value_op2=ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK;
			}


			ei_ternary_if_jnt->vm_instruction.value_op2=body_size_if; // +1 because
			ei_ternary_else_jmp->vm_instruction.value_op2=body_size_else;

		}else{
			Instruction *last_instruction=&dst_instructions->at(dst_instructions->size()-1)->vm_instruction;
			if((n_recursion_level == 0) && (last_instruction->byte_code == BYTE_CODE_CALL) && (properties & EVAL_EXPRESSION_ON_MAIN_BLOCK)){ // --> allow all stack return
				last_instruction->value_op2=ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK;
			}
		}
		//--------------------------------------------------------------

		// ... finally save store operators
		for(int i=(int)(assign_loader_instructions_post_expression.size()-1); i >=0 ;i--){
			//loaders
			dst_instructions->insert(
				dst_instructions->end()
				,assign_loader_instructions_post_expression[i].begin()
				,assign_loader_instructions_post_expression[i].end()
			);

			// push back assign operator
			dst_instructions->push_back(
				assign_store_instruction_post_expression[i]
			);
		}

		return aux_p;

eval_error_byte_code:

		// only delete the new ones
		for(unsigned i=0; i < assign_store_instruction_post_expression.size(); i++){
			delete assign_store_instruction_post_expression[i];
		}

		return NULL;

	}

}

