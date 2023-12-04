/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"

namespace zetscript{

	typedef struct{
		CompilerInstruction *	instruction_symbol; // affected symbol
		Operator			assign_operator; // operator
		TokenNode			*token_node;
	}AssignTokenInformation;

	ByteCode compiler_byte_code_load_var_type_to_push_stk(ByteCode _byte_code){
		// load ptr var
		switch(_byte_code){
			// load var content
		default:
			break;
			case BYTE_CODE_LOAD_GLOBAL:return BYTE_CODE_PUSH_STK_GLOBAL;
			case BYTE_CODE_LOAD_LOCAL:return BYTE_CODE_PUSH_STK_LOCAL;
			case BYTE_CODE_LOAD_REF:return BYTE_CODE_LOAD_REF; // PUSH STK NOT EXIST, IS A REF ITSELF
			case BYTE_CODE_LOAD_THIS:return BYTE_CODE_PUSH_STK_THIS;
			case BYTE_CODE_LOAD_ARRAY_ITEM:return BYTE_CODE_PUSH_STK_ARRAY_ITEM;
			case BYTE_CODE_LOAD_THIS_VARIABLE:return BYTE_CODE_PUSH_STK_THIS_VARIABLE;
			case BYTE_CODE_LOAD_OBJECT_ITEM:return BYTE_CODE_PUSH_STK_OBJECT_ITEM;
		}
		return BYTE_CODE_INVALID;
	}

	bool	compiler_is_byte_code_load_var_type(ByteCode _byte_code){
		return _byte_code==BYTE_CODE_LOAD_GLOBAL
				|| _byte_code==BYTE_CODE_LOAD_LOCAL
				|| _byte_code==BYTE_CODE_LOAD_REF
				|| _byte_code==BYTE_CODE_LOAD_THIS
				|| _byte_code==BYTE_CODE_LOAD_ARRAY_ITEM
				|| _byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
				|| _byte_code==BYTE_CODE_LOAD_OBJECT_ITEM;
	}


	char * compiler_sub_expression(
			CompilerData *compiler_data
			,const char *s
			, int & line
			, ScriptScope *scope_info
			, Vector<CompilerInstruction *> 	* compiler_instructions
			, const char *expected_ending_char
			, uint16_t properties // uint16_t properties
			, Vector<Instruction *> *unique_call_instruction
	);


	bool compiler_two_last_instruction_are_constants(Vector<CompilerInstruction *> *_compiler_instructions){

		int size_instructions=_compiler_instructions->length();

		if(size_instructions >= 2){

			CompilerInstruction *i1=(CompilerInstruction *)_compiler_instructions->get(size_instructions-2);
			CompilerInstruction *i2=(CompilerInstruction *)_compiler_instructions->get(size_instructions-1);

			return i1->vm_instruction.isConstant() && i2->vm_instruction.isConstant();
		}

		return false;
	}

	// compiler operator expression only compileruates expression with normal operators (+,-,>>,<<,etc) respecting always its preference. Assign operators (=,+=,-=,etc) should be extracted
	void compiler_expression_tokens_to_byte_code(
			  CompilerData *compiler_data
			, ScriptScope *scope
			, Vector<TokenNode *> 			* 	token_nodes
			, Vector<CompilerInstruction *> 		*	compiler_instructions
			, Vector<CompilerInstruction *>     	*   logical_and_jnt
			, Vector<CompilerInstruction *>     	*  	logical_or_jt
			, int idx_start_compiler_instructions
			, int idx_start
			, int idx_end
			, uint16_t properties

	){
		CompilerInstruction 	*compiler_instruction=NULL;
		int 				idx_split=-1;
		TokenNode      		*split_node = NULL;
		uint8_t 		idx_split_group_preference=ZS_PREFERENCE_OPERATOR_GROUP_MAX;
		bool custom_insert_instruction=false;


		// trivial case (symbol node)
		if(idx_start>=idx_end){
			// concatenate instructions ...
			TokenNode *end_node=(TokenNode *)token_nodes->get(idx_start);
			end_node->are_instructions_moved=true; // mark as processed

			compiler_instructions->concat(end_node->compiler_instructions);
			return;
		}

		// get the most preference operator by group(see compiler_data::Operator)...
		for(int i=idx_start; i<= idx_end; i++){

			// get split preference
			if(((TokenNode *)(token_nodes->get(i)))->token_type == TokenType::TOKEN_TYPE_OPERATOR){
				uint8_t idx_group_preference=get_preference_operator_group(((TokenNode *)(token_nodes->get(i)))->operator_type);

				if(idx_group_preference <= idx_split_group_preference){
					idx_split_group_preference=idx_group_preference;
					idx_split=i;
				}
			}
		}

		if(idx_split == -1){
			ZS_THROW_RUNTIME_ERRORF("Cannot fint operator");
		}

		// split left/right by operator precedence...
		split_node=(TokenNode *)token_nodes->get(idx_split);

		// perform left side op (can have operators)...
		compiler_expression_tokens_to_byte_code(
			compiler_data
			,scope
			,token_nodes
			,compiler_instructions
			, logical_and_jnt
			,logical_or_jt
			, idx_start_compiler_instructions
			,idx_start
			,idx_split-1
			,properties
		);

		//------------------------------------------------------------------------------------
		// OPTIMIZATION PART: Try to simplify 2 ops into one op


		// TODO: IF BOOLEAN EXPRESSION AND OP IS LOGICAL_AND OR LOGICAL_OR, INSERT JT/JMP
		bool two_last_instructions_are_constants=compiler_two_last_instruction_are_constants(compiler_instructions);
		custom_insert_instruction=false;
		if((split_node->operator_type == ZS_OPERATOR_LOGIC_AND) && (two_last_instructions_are_constants==false)){
			custom_insert_instruction=true;
			// insert JT/acording type of JNT, the jump for next or
						compiler_instructions->push(
							compiler_instruction=new CompilerInstruction(
								BYTE_CODE_JNT
								,INSTRUCTION_VALUE_OP1_JNT_LOGIC_NEXT_OR
							)
						);
						logical_and_jnt->push(compiler_instruction);
		}else if((split_node->operator_type == ZS_OPERATOR_LOGIC_OR) && (two_last_instructions_are_constants==false)){
			custom_insert_instruction=true;
			compiler_instructions->push(
				compiler_instruction=new CompilerInstruction(
						BYTE_CODE_JT
						,INSTRUCTION_VALUE_OP1_JT_LOGIC_OK
				)
			);
			logical_or_jt->push(compiler_instruction);
		}

		// perform right side op...
		compiler_expression_tokens_to_byte_code(
			compiler_data
			,scope
			,token_nodes
			,compiler_instructions
			,logical_and_jnt
			,logical_or_jt
			,idx_start_compiler_instructions
			,idx_split+1
			,idx_end
			,properties
		);

		if(custom_insert_instruction ==false){
			if((compiler_instruction=compiler_expression_optimize(compiler_data,scope,split_node, compiler_instructions))==NULL){
				// cannot be simplified...
				// push operator byte code...
				compiler_instruction=new CompilerInstruction(
					compiler_operator_to_byte_code(split_node->operator_type)
				);
			}

			// push to the end
			compiler_instructions->push(
					compiler_instruction
			);
		}


		// OPTIMIZATION PART
		//------------------------------------------------------------------------------------

		if(compiler_instruction!=NULL){
			compiler_instruction->instruction_source_info= compiler_instruction_source_info(
				compiler_data
				,compiler_data->current_parsing_file
				,split_node->line
				,compiler_instruction->symbol_name
			);
		}
	}

	void compiler_deallocate_ei_assign_loader_instructions_post_expression(Vector<Vector<CompilerInstruction *> *> & zs_ei_assign_loader_instructions_post_expression){
		for(int i=0; i<zs_ei_assign_loader_instructions_post_expression.length(); i++ ){
			delete zs_ei_assign_loader_instructions_post_expression.get(i);
		}
		zs_ei_assign_loader_instructions_post_expression.clear();


	}

	char * compiler_expression_to_byte_code(
			CompilerData *compiler_data
			,const char *s
			, int & line
			, ScriptScope *scope_info
			, Vector<CompilerInstruction *> *dst_instructions
			, Vector<TokenNode *> * token_nodes
			, uint16_t properties
			, Vector<Instruction *> *unique_call_instructions
		){

		char *aux_p=(char *)s;
		Vector<TokenNode *> assing_token_informations;
		int idx_start=0;
		int idx_end=(int)(token_nodes->length()-1);
		Vector<Vector<CompilerInstruction *> *> zs_ei_assign_loader_instructions_post_expression; // Vector<Vector<CompilerInstruction *>>
		Vector<CompilerInstruction *> ei_assign_store_instruction_post_expression;
		int idx_start_instructions=0;
		Vector<CompilerInstruction *> logical_and_jnt,logical_or_jt;
		//bool create_jmp_false=false;

		// search for assign
		for(int i=idx_end; i >= 0; i--){
			Operator token_operator = ((TokenNode *)token_nodes->get(i))->operator_type;

			if(IS_OPERATOR_TYPE_ASSIGN(token_operator)){ // ... save all assignables from operator split
					idx_start=i+1;
					break;
			}
		}

		//--------------------------------------------------------------
		// operator assign = found --> assign operators, load identifiers first
		for(int i=idx_start-2; i >= 0; i-=2){ // starting from assign operator if idx_start > 0 += 2 is because there's a symbol followed by its operator
			CompilerInstruction *compiler_instruction=NULL;
			TokenNode * token_node_symbol = (TokenNode *)token_nodes->get(i);
			TokenNode * token_node_operator = (TokenNode *)token_nodes->get(i+1);
			Operator operator_type=token_node_operator->operator_type;
			Vector<CompilerInstruction *> *ei_assign_loader_instructions_post_expression=NULL;

			// Check for operators found. Each operator found it has to be (=,+=,-=,... etc)
			if(!(
					(token_node_operator->token_type == TokenType::TOKEN_TYPE_OPERATOR)
				&&	(IS_OPERATOR_TYPE_ASSIGN(operator_type))

				)){ // ... save all assignables from operator split
				EVAL_ERROR_FILE_LINE_GOTO(
					compiler_data->current_parsing_file
					,token_node_operator->line
					,compiler_error_byte_code
					,"Unexpected operator '%s' with assignment"
					,compiler_data_operators[operator_type].str
				);
			}

			// should be identifier...
			if(token_node_symbol->token_type != TokenType::TOKEN_TYPE_IDENTIFIER){
				EVAL_ERROR_FILE_LINE_GOTO(
					compiler_data->current_parsing_file
					,token_node_symbol->line
					,compiler_error_byte_code
					,"Assign a literal '%s' is not allowed"
					,token_node_symbol->value.toConstChar()
				);
			}

			CompilerInstruction *ei_load_assign_instruction=((CompilerInstruction *)token_node_symbol->compiler_instructions.get(token_node_symbol->compiler_instructions.length()-1));

			if(BYTE_CODE_IS_CALL(
					ei_load_assign_instruction->vm_instruction.byte_code
			)){
				EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,ei_load_assign_instruction->instruction_source_info.line
						,compiler_error_byte_code
						,"Calling a function in left assignment is not allowed");
			}

			zs_ei_assign_loader_instructions_post_expression.push(ei_assign_loader_instructions_post_expression=new Vector<CompilerInstruction *>());//ei_assign_loader_instructions_post_expression=new Vector<CompilerInstruction *>());

			// assign operators: add instructions related about its accessors...
			for(int j=0;j<token_node_symbol->compiler_instructions.length();j++){
				ei_load_assign_instruction=(CompilerInstruction *)token_node_symbol->compiler_instructions.get(j);
				ei_assign_loader_instructions_post_expression->push(token_node_symbol->compiler_instructions.get(j));
			}

			// get last instruction...
			CompilerInstruction *ei_last_load_instruction=(CompilerInstruction *)ei_assign_loader_instructions_post_expression->get(ei_assign_loader_instructions_post_expression->length()-1);
			Instruction *last_load_instruction=&ei_last_load_instruction->vm_instruction;

			if(compiler_is_byte_code_load_var_type(last_load_instruction->byte_code)){

				if(last_load_instruction->byte_code==BYTE_CODE_LOAD_THIS){
					EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,ei_last_load_instruction->instruction_source_info.line
						,compiler_error_byte_code
						,"'this' is not assignable");
				}

				last_load_instruction->byte_code=compiler_byte_code_load_var_type_to_push_stk(last_load_instruction->byte_code);
			}else if(last_load_instruction->byte_code == BYTE_CODE_FIND_VARIABLE){
				last_load_instruction->properties=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}

			// ... add assign operations to the list to add later after compiler all instructions
			ei_assign_store_instruction_post_expression.push(compiler_instruction=new CompilerInstruction(
					compiler_operator_to_byte_code(operator_type),1
			));

			compiler_instruction->instruction_source_info= InstructionSourceInfo(
				compiler_data->current_parsing_file
				,token_node_operator->line
			);
		}
		//--------------------------------------------------------------

		// compiler tokens

		idx_start_instructions=dst_instructions->length();

		try{
			compiler_expression_tokens_to_byte_code(
				 compiler_data
				, scope_info
				, token_nodes
				, dst_instructions
				, &logical_and_jnt
				, &logical_or_jt
				, dst_instructions->length()
				, idx_start
				, idx_end
				, properties
			);
		}catch(std::exception & error){
			EVAL_ERROR_BYTE_CODEF(error.what());
		}


		// there's a logical and or logical or that will place a true/false for its final value
		if(logical_or_jt.length() > 0 || logical_and_jnt.length() > 0){

			int idx_jmp_load_true_value=dst_instructions->length();
			int idx_jmp_load_false_value=dst_instructions->length();
			int idx_jmp_end=dst_instructions->length();

			dst_instructions->push(

				new CompilerInstruction(
					BYTE_CODE_JMP
					,(uint8_t)ZS_UNDEFINED_IDX
					,2 // +2 jmp instructions by default
				)
			);


			if(logical_or_jt.length() > 0){

				idx_jmp_load_true_value=dst_instructions->length();
				dst_instructions->push(
					new CompilerInstruction(
						BYTE_CODE_LOAD_BOOL
						, (uint8_t)ZS_UNDEFINED_IDX
						,1
					)
				);

			}

			if(logical_and_jnt.length()>0){

				if(logical_or_jt.length() > 0){ // modify jmp instruction
					((CompilerInstruction *)dst_instructions->get(idx_jmp_end))->vm_instruction.value_op2=4;

					// to jmp true value space
					dst_instructions->push(
						new CompilerInstruction(
							BYTE_CODE_JMP
							, (uint8_t)ZS_UNDEFINED_IDX
							,2
						)
					);
				}

				idx_jmp_load_false_value=dst_instructions->length();

				dst_instructions->push(
					new CompilerInstruction(
						BYTE_CODE_LOAD_BOOL
						, (uint8_t)ZS_UNDEFINED_IDX
						,0
					)
				);
			}

			for(int i=idx_start_instructions; i < dst_instructions->length(); i++){
				CompilerInstruction *compiler_instruction=(CompilerInstruction *)dst_instructions->get(i);
				if((compiler_instruction->vm_instruction.byte_code == BYTE_CODE_JNT) && (compiler_instruction->vm_instruction.value_op1== INSTRUCTION_VALUE_OP1_JNT_LOGIC_NEXT_OR)){
					//logical_and_jnt.append((intptr_t)compiler_instruction);
					// go to next logic or
					int idx_next_or_found=-1;
					for(int j=i; j < dst_instructions->length(); j++){
						CompilerInstruction *compiler_instruction_post_instruction=(CompilerInstruction *)dst_instructions->get(j);
						if((compiler_instruction_post_instruction->vm_instruction.byte_code == BYTE_CODE_JT) && (compiler_instruction_post_instruction->vm_instruction.value_op1== INSTRUCTION_VALUE_OP1_JT_LOGIC_OK)){
							idx_next_or_found=j;
							break;
						}
					}

					if(idx_next_or_found != -1){ // next or found
						compiler_instruction->vm_instruction.value_op2=idx_next_or_found-i+1;
					}else{ // go to end with false
						compiler_instruction->vm_instruction.value_op2=idx_jmp_load_false_value-i;
					}

					compiler_instruction->vm_instruction.value_op1= (uint8_t)ZS_UNDEFINED_IDX; // mark as undefined due the jmp offset it was assigned
				}
				else if((compiler_instruction->vm_instruction.byte_code == BYTE_CODE_JT) && (compiler_instruction->vm_instruction.value_op1== INSTRUCTION_VALUE_OP1_JT_LOGIC_OK)){

					compiler_instruction->vm_instruction.value_op2=idx_jmp_load_true_value-i;

					compiler_instruction->vm_instruction.value_op1= (uint8_t)ZS_UNDEFINED_IDX;  // mark as undefined due the jmp offset it was assigned
				}
			}

		}


		// if ends with ternary then continues performing expressions
		if(*aux_p == '?'){ // ternary
			CompilerInstruction *ei_ternary_if_jnt=NULL;
			CompilerInstruction *ei_ternary_else_jmp=NULL;
			Instruction *last_instruction=NULL;

			// insert JNT
			int jnt_instructions_start=dst_instructions->length();
			dst_instructions->push(ei_ternary_if_jnt=new CompilerInstruction(BYTE_CODE_JNT));
			ei_ternary_if_jnt->instruction_source_info.file=compiler_data->current_parsing_file;
			ei_ternary_if_jnt->instruction_source_info.line=line;
			int jmp_instructions_start=0;
			int body_size_if=0;
			int body_size_else=0;

			// compiler ? part
			if((aux_p=compiler_sub_expression(
				compiler_data
				,aux_p+1
				, line
				, scope_info
				, dst_instructions
				, NULL
				, properties
				, unique_call_instructions
				//, n_return_values
			))==NULL){
				goto compiler_error_byte_code;
			}

			if(unique_call_instructions != NULL){
				last_instruction=&(((CompilerInstruction *)dst_instructions->get(dst_instructions->length()-1))->vm_instruction);
				if(BYTE_CODE_IS_CALL(last_instruction->byte_code) && token_nodes->length()==1){
					unique_call_instructions->push(last_instruction);
				}
			}

			// TODO: JEB Check whether expression is constant true/false
			if(*aux_p != ':'){
				EVAL_ERROR_FILE_LINE_GOTOF(
					compiler_data->current_parsing_file
					,line
					,compiler_error_byte_code
					,"Expected ':' on ternary expression"
				);
			}


			jmp_instructions_start=dst_instructions->length();
			dst_instructions->push(ei_ternary_else_jmp=new CompilerInstruction(BYTE_CODE_JMP));
			body_size_if=dst_instructions->length()-jnt_instructions_start; // size body "if" takes jmp as part of it



			// compiler : part
			if((aux_p=compiler_sub_expression(
				compiler_data
				, aux_p+1
				, line
				, scope_info
				, dst_instructions
				, NULL
				, properties
				,unique_call_instructions
				//,n_return_values
			))==NULL){
				goto compiler_error_byte_code;
			}

			body_size_else=dst_instructions->length()-jmp_instructions_start;

			if(unique_call_instructions != NULL){
				last_instruction=&((CompilerInstruction *)dst_instructions->get(dst_instructions->length()-1))->vm_instruction;
				if(BYTE_CODE_IS_CALL(last_instruction->byte_code) && token_nodes->length()==1){
					unique_call_instructions->push(last_instruction);
				}
			}


			ei_ternary_if_jnt->vm_instruction.value_op2=body_size_if; // +1 because
			ei_ternary_else_jmp->vm_instruction.value_op2=body_size_else;

		}else{
			if(unique_call_instructions != NULL){
				Instruction *last_instruction=&((CompilerInstruction *)dst_instructions->get(dst_instructions->length()-1))->vm_instruction;
				if(BYTE_CODE_IS_CALL(last_instruction->byte_code) && token_nodes->length()==1){
					unique_call_instructions->push(last_instruction);
				}
			}
		}
		//--------------------------------------------------------------

		// ... finally save store operators
		for(int i=(int)(zs_ei_assign_loader_instructions_post_expression.length()-1); i >=0 ;i--){
			//loaders
			dst_instructions->concat(*zs_ei_assign_loader_instructions_post_expression.get(i));

			// push back assign operator
			dst_instructions->push(
				ei_assign_store_instruction_post_expression.get(i)
			);
		}

		compiler_deallocate_ei_assign_loader_instructions_post_expression(zs_ei_assign_loader_instructions_post_expression);

		return aux_p;

compiler_error_byte_code:

		// only delete the new ones
		for(int i=0; i < ei_assign_store_instruction_post_expression.length(); i++){
			delete (CompilerInstruction *)ei_assign_store_instruction_post_expression.get(i);
		}

		compiler_deallocate_ei_assign_loader_instructions_post_expression(zs_ei_assign_loader_instructions_post_expression);

		return NULL;

	}

}

