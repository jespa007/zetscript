#define IS_TOKEN_SIMPLIFIABLE(token) \
(\
	(token->instructions.size()==1)\
	&& (\
		token->instructions[0]->vm_instruction.isConstant()\
	||  IS_BYTE_CODE_LOAD_LOCAL_GLOBAL_OR_FIND(token->instructions[0]->vm_instruction.byte_code)\
	)\
)

namespace zetscript{
	namespace eval{

		typedef struct{
			EvalInstruction *	instruction_symbol; // affected symbol
			Operator			assign_operator; // operator
			TokenNode			*token_node;
		}AssignTokenInformation;

		char * eval_expression_main(
				EvalData *eval_data
				,const char *s
				, int & line
				, Scope *scope_info
				, std::vector<EvalInstruction *> 	* instructions
				, std::vector<char> expected_ending_char={}
				, uint16_t properties=0 // uint16_t properties
				, std::vector<EvalInstruction *> *only_call_instructions=NULL
		);

		ByteCode convert_operator_to_byte_code(Operator op){
			switch(op){
			default:
				break;
			// assign and arithmetic with assign
			case Operator::OPERATOR_ASSIGN:
				return ByteCode::BYTE_CODE_STORE;
			case Operator::OPERATOR_ASSIGN_ADD:
				return ByteCode::BYTE_CODE_STORE_ADD;
			case Operator::OPERATOR_ASSIGN_SUB:
				return ByteCode::BYTE_CODE_STORE_SUB;
			case Operator::OPERATOR_ASSIGN_MUL:
				return ByteCode::BYTE_CODE_STORE_MUL;
			case Operator::OPERATOR_ASSIGN_DIV:
				return ByteCode::BYTE_CODE_STORE_DIV;
			case Operator::OPERATOR_ASSIGN_MOD:
				return ByteCode::BYTE_CODE_STORE_MOD;
			case Operator::OPERATOR_ASSIGN_XOR:
				return ByteCode::BYTE_CODE_STORE_XOR;
			case Operator::OPERATOR_ASSIGN_AND:
				return ByteCode::BYTE_CODE_STORE_AND;
			case Operator::OPERATOR_ASSIGN_OR:
				return ByteCode::BYTE_CODE_STORE_OR;
			case Operator::OPERATOR_ASSIGN_SHIFT_LEFT:
				return ByteCode::BYTE_CODE_STORE_SHL;
			case Operator::OPERATOR_ASSIGN_SHIFT_RIGHT:
				return ByteCode::BYTE_CODE_STORE_SHR;


			// arithmetic ops
			case Operator::OPERATOR_ADD:
				return ByteCode::BYTE_CODE_ADD;
			case Operator::OPERATOR_SUB:
				return ByteCode::BYTE_CODE_SUB;
			case Operator::OPERATOR_MUL:
				return ByteCode::BYTE_CODE_MUL;
			case Operator::OPERATOR_DIV:
				return ByteCode::BYTE_CODE_DIV;
			case Operator::OPERATOR_MOD:
				return ByteCode::BYTE_CODE_MOD;
			case Operator::OPERATOR_XOR:
				return ByteCode::BYTE_CODE_XOR;
			case Operator::OPERATOR_AND:
				return ByteCode::BYTE_CODE_AND;
			case Operator::OPERATOR_OR:
				return ByteCode::BYTE_CODE_OR;
			case Operator::OPERATOR_SHIFT_LEFT:
				return ByteCode::BYTE_CODE_SHL;
			case Operator::OPERATOR_SHIFT_RIGHT:
				return ByteCode::BYTE_CODE_SHR;

			// logic
			case Operator::OPERATOR_LOGIC_AND:
				return ByteCode::BYTE_CODE_LOGIC_AND;
			case Operator::OPERATOR_LOGIC_OR:
				return ByteCode::BYTE_CODE_LOGIC_OR;
			case Operator::OPERATOR_LOGIC_EQUAL:
				return ByteCode::BYTE_CODE_EQU;
			case Operator::OPERATOR_LOGIC_NOT_EQUAL:
				return ByteCode::BYTE_CODE_NOT_EQU;
			case Operator::OPERATOR_LOGIC_GT:
				return ByteCode::BYTE_CODE_GT;
			case Operator::OPERATOR_LOGIC_LT:
				return ByteCode::BYTE_CODE_LT;
			case Operator::OPERATOR_LOGIC_GTE:
				return ByteCode::BYTE_CODE_GTE;
			case Operator::OPERATOR_LOGIC_LTE:
				return ByteCode::BYTE_CODE_LTE;
			case Operator::OPERATOR_INSTANCEOF:
				return ByteCode::BYTE_CODE_INSTANCEOF;
			}

			THROW_RUNTIME_ERROR("Convert %i to byte code not implemented",op);
			return ByteCode::BYTE_CODE_END_FUNCTION;
		}

		// eval operator expression only evaluates expression with normal operators (+,-,>>,<<,etc) respecting always its preference. Assign operators (=,+=,-=,etc) should be extracted
		void eval_expression_tokens_to_byte_code(
				  EvalData *eval_data
				, Scope *scope
				, std::vector<TokenNode> * expression_tokens
				, std::vector<EvalInstruction *> *instructions
				, int idx_start
				, int idx_end
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
			TokenNode		*left_token,*right_token;
			bool simplified_ok=false;
			ByteCode byte_code;

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

					if(idx_group < idx_group_split){
						idx_group_split=idx_group;
						idx_split=i;
					}
				}
			}

			if(idx_split == -1){
				THROW_RUNTIME_ERROR("Unexpected error");
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
			);

			// perform right side op...
			eval_expression_tokens_to_byte_code(
				eval_data
				,scope
				,expression_tokens
				,instructions
				,idx_split+1
				,idx_end
			);

			//------------------------------------------------------------------------------------
			// OPTIMIZATION PART: Try to simplify 2 ops into one op
			byte_code=convert_operator_to_byte_code(split_node->operator_type);

			if((instruction=eval_expression_optimize(eval_data,scope,byte_code, instructions))==NULL){ // cannot be simplified...
			// push operator byte code...
				instruction=new EvalInstruction(
					convert_operator_to_byte_code(split_node->operator_type)
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
				, std::vector<EvalInstruction *> *only_call_instructions
			){

			char *aux_p=(char *)s;
			std::vector<AssignTokenInformation> assing_tokens;
			int idx_start=0;
			int idx_end=(int)(expression_tokens->size()-1);
			std::vector<std::vector<EvalInstruction *>> assign_instructions_post_expression;

			// search for assign
			for(int i=idx_end; i >= 0; i--){
				Operator token_operator = expression_tokens->at(i).operator_type;

				if(IS_OPERATOR_TYPE_ASSIGN(token_operator)){ // ... save all assignables from operator split
						idx_start=i+1;
						break;
				}
			}

			//--------------------------------------------------------------
			// operator = found --> assign operators, load identifiers first
			for(int i=0; i < idx_start; i+=2){ // starting from assign operator if idx_start > 0 += 2 is because there's a symbol followed by its operator
				EvalInstruction *instruction=NULL;
				TokenNode * token_node_symbol = &expression_tokens->at(i);
				TokenNode * token_node_operator = &expression_tokens->at(i+1);
				Operator operator_type=token_node_operator->operator_type;
				int end_idx=(int)(expression_tokens->size()-1);

				// Check for operators found. Each operator found it has to be (=,+=,-=,... etc)
				if(!(
						(token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR)
					&&	(IS_OPERATOR_TYPE_ASSIGN(operator_type))

					)){ // ... save all assignables from operator split
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,token_node_operator->line,"Operation \"%s\" in assignment is not allowed",eval_data_operators[operator_type].str);
				}

				// should be identifier...
				if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,token_node_symbol->line,"Assign a literal \"%s\" is not allowed",token_node_symbol->value.c_str());
				}

				assign_instructions_post_expression.push_back({});


				// add instructions related about its accessors...
				for(unsigned i=0;i<token_node_symbol->instructions.size();i++){
					EvalInstruction *ei_load_assign_instruction=token_node_symbol->instructions[i];
					if(ei_load_assign_instruction->vm_instruction.byte_code ==  BYTE_CODE_CALL){
						EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,ei_load_assign_instruction->instruction_source_info.line
								,"Calling a function in left assignment is not allowed");
					}
					assign_instructions_post_expression[i>>1].push_back(token_node_symbol->instructions[i]);
				}

				// get last instruction...
				Instruction *last_load_instruction=&assign_instructions_post_expression[i>>1][assign_instructions_post_expression[i>>1].size()-1]->vm_instruction;

				// if is a access property ...
				if(last_load_instruction->byte_code == BYTE_CODE_LOAD_ELEMENT_THIS //properties & (MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS | MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD)){
				||last_load_instruction->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT){
					// .. add information last load that it will be stored
					last_load_instruction->properties |= MSK_INSTRUCTION_PROPERTY_PACK_MEMBER_INFO;
				}

				// ... add arithmetic operator byte code
				assign_instructions_post_expression[i>>1].push_back(instruction=new EvalInstruction(
						convert_operator_to_byte_code(operator_type)
				));

				instruction->instruction_source_info= InstructionSourceInfo(
						eval_data->current_parsing_file
						,token_node_operator->line
						,NULL
				);
				// set pops one to do the operation but keeps the variable to store on the top of stack
				//instruction->vm_instruction.properties |= MSK_INSTRUCTION_PROPERTY_POP_ONE;


				//--------------------------------------------------------------
				// assign operators, add store byte code
				/*instruction->instruction_source_info= InstructionSourceInfo(
						eval_data->current_parsing_file
						,token_node_operator->line
						,NULL
				);*/
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
				);
			}catch(std::exception & error){
				EVAL_ERROR(error.what());
			}

			// if ends with ternary then continues performing expressions
			if(*aux_p == '?'){ // ternary
				EvalInstruction *ei_ternary_if_jnt=NULL;
				EvalInstruction *ei_ternary_else_jmp=NULL;

				// insert JNT
				dst_instructions->push_back(ei_ternary_if_jnt=new EvalInstruction(BYTE_CODE_JNT));

				// eval ? part
				aux_p=eval_expression_main(
					eval_data
					,aux_p+1
					, line
					, scope_info
					, dst_instructions
					,std::vector<char>{}
				);

				// save instruction if only call in the end (for support multi assigments on the left)
				if(only_call_instructions != NULL && dst_instructions->at(dst_instructions->size()-1)->vm_instruction.byte_code == BYTE_CODE_CALL){
					only_call_instructions->push_back(dst_instructions->at(dst_instructions->size()-1));
				}

				// TODO: JEB Check whether expression is constant true/false
				if(*aux_p != ':'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Expected ':' on ternary expression");
				}

				dst_instructions->push_back(ei_ternary_else_jmp=new EvalInstruction(BYTE_CODE_JMP));

				ei_ternary_if_jnt->vm_instruction.value_op2=dst_instructions->size()+dst_instructions->size();

				// eval : part
				aux_p=eval_expression_main(
					eval_data
					,aux_p+1
					, line
					, scope_info
					, dst_instructions
					,std::vector<char>{}
				);

				// save instruction if only call in the end  (for support multi assigments on the left)
				if(only_call_instructions != NULL && dst_instructions->at(dst_instructions->size()-1)->vm_instruction.byte_code == BYTE_CODE_CALL){
					only_call_instructions->push_back(dst_instructions->at(dst_instructions->size()-1));
				}


				ei_ternary_else_jmp->vm_instruction.value_op2=dst_instructions->size()+dst_instructions->size();

			}else{
				// save instruction if only call in the end  (for support multi assigments on the left)
				if(only_call_instructions != NULL && dst_instructions->at(dst_instructions->size()-1)->vm_instruction.byte_code == BYTE_CODE_CALL){
					only_call_instructions->push_back(dst_instructions->at(dst_instructions->size()-1));
				}

			}
			//--------------------------------------------------------------

			// ... finally save store operators
			for(int i=(int)(assign_instructions_post_expression.size()-1); i >=0 ;i--){
				dst_instructions->insert(
					dst_instructions->end()
					,assign_instructions_post_expression[i].begin()
					,assign_instructions_post_expression[i].end()
				);
			}
			return aux_p;
		}

	}
}
