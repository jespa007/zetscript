
#define MAX_REGISTER_LENGTH	128


#define PERFORM_ARITHMETIC_OPERATION(ARITHMETIC_OP) \
if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
	result_op_zs_int=(i1->value_op2)ARITHMETIC_OP(i2->value_op2);\
	result_bc=BYTE_CODE_LOAD_ZS_INT;\
}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
	result_op_float=*((float *)&i1->value_op2)ARITHMETIC_OP(i2->value_op2);\
	result_bc=BYTE_CODE_LOAD_FLOAT;\
}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){\
	result_op_float=*((float *)&i1->value_op2)ARITHMETIC_OP(i2->value_op2);\
	result_bc=BYTE_CODE_LOAD_FLOAT;\
}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){\
	result_op_float=*((float *)&i1->value_op2)ARITHMETIC_OP *((float *)&i2->value_op2);\
	result_bc=BYTE_CODE_LOAD_FLOAT;\
}else{\
	THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform arithmetic operation %s '%s' %s"\
			,eval_data->current_parsing_file\
			,token_operator->line\
			,i1->getConstantValueOp2ToString().c_str()\
			,STR(ARITHMETIC_OP)\
			,i2->getConstantValueOp2ToString().c_str()));\
}

#define PERFORM_BINARY_OPERATION(BINARY_OP) \
if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
	result_op_zs_int=(i1->value_op2)BINARY_OP(i2->value_op2);\
	result_bc=BYTE_CODE_LOAD_ZS_INT;\
}else{\
	THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform binary operation %s '%s' %s"\
			,eval_data->current_parsing_file\
			,token_operator->line\
			,i1->getConstantValueOp2ToString().c_str()\
			,STR(BINARY_OP)\
			,i2->getConstantValueOp2ToString().c_str()));\
}

#define PERFORM_LOGIC_OPERATION(LOGIC_OP) \
if(i1->byte_code == BYTE_CODE_LOAD_BOOL && i2->byte_code == BYTE_CODE_LOAD_BOOL){\
	result_op_bool=(i1->value_op2)LOGIC_OP(i2->value_op2);\
	result_bc=BYTE_CODE_LOAD_BOOL;\
}else{\
	THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform logic operation %s '%s' %s"\
			,eval_data->current_parsing_file\
			,token_operator->line\
			,i1->getConstantValueOp2ToString().c_str()\
			,STR(LOGIC_OP)\
			,i2->getConstantValueOp2ToString().c_str()));\
}


namespace zetscript{

	namespace eval{



		unsigned short eval_expression_load_const_byte_code_to_instruction_property(ByteCode byte_code){

			switch(byte_code){
			case BYTE_CODE_LOAD_ZS_INT:
				return MSK_INSTRUCTION_PROPERTY_ZS_INT;
				break;
			case BYTE_CODE_LOAD_BOOL:
				return MSK_INSTRUCTION_PROPERTY_BOOL;
				break;
			case BYTE_CODE_LOAD_FLOAT:
				return MSK_INSTRUCTION_PROPERTY_FLOAT;
				break;
			case BYTE_CODE_LOAD_STRING:
				return MSK_INSTRUCTION_PROPERTY_STRING;
				break;
			}

			return 0;
		}

		bool eval_expression_get_register(EvalData *eval_data, Scope *scope,EvalInstruction *i1, ByteCode & load_byte_code, zs_int & load_value_op2){
			Symbol *symbol_found=NULL;

			 if(!IS_BYTE_CODE_LOAD_LOCAL_GLOBAL_OR_FIND(i1->vm_instruction.byte_code)){
				 return false;
			 }

			 load_value_op2=i1->vm_instruction.value_op2;
			 load_byte_code=i1->vm_instruction.byte_code;

			if(i1->vm_instruction.byte_code == BYTE_CODE_FIND_VARIABLE){

				if((symbol_found = eval_find_local_symbol(eval_data,scope,i1->symbol.name)) != NULL){
					load_byte_code=BYTE_CODE_LOAD_LOCAL;
					load_value_op2=symbol_found->idx_position;
				}else if ((symbol_found = eval_find_global_symbol(eval_data,i1->symbol.name)) != NULL){
					load_byte_code=BYTE_CODE_LOAD_GLOBAL;
					load_value_op2=symbol_found->idx_position;
				}else{
					return false;
				}
			}

			if(load_value_op2 >= MAX_REGISTER_LENGTH){
				return false;
			}

			return true;
		}

		//-------------------------------------------------------------------------------------------------------------------------------
		// IMMEDIATE OPERATION 2 OPS
		EvalInstruction * eval_expression_perform_KK_operation(
				EvalData *eval_data
				,TokenNode *token_operator
				, EvalInstruction *ei1
				, EvalInstruction *ei2
		){
			float  result_op_float=0;
			zs_int result_op_zs_int=0;
			bool	result_op_bool=false;
			std::string result_op_str="";
			EvalInstruction *result_instruction=NULL;
			ByteCode result_bc=ByteCode::BYTE_CODE_INVALID;
			Instruction *i1=&ei1->vm_instruction;
			Instruction *i2=&ei2->vm_instruction;
			ByteCode byte_code=eval_operator_to_byte_code(token_operator->operator_type);

			// check last two instructions stk op1 and stk op2 are bool/int/float or string
			if(!(i1->isConstant() && i2->isConstant())){
				return NULL;
			}

			// which operation ?
			switch(byte_code){
			case BYTE_CODE_ADD: // int & int/int & float/float&float

				if(i1->byte_code == BYTE_CODE_LOAD_STRING && i2->isConstant()){
					result_op_str=zs_strutils::format("%s%s",i1->getConstantString().c_str(),i2->getConstantValueOp2ToString().c_str());
					result_bc=BYTE_CODE_LOAD_STRING;
				}
				else if(i1->isConstant() && i2->byte_code == BYTE_CODE_LOAD_STRING){
					result_op_str=zs_strutils::format("%s%s",i1->getConstantValueOp2ToString().c_str(),i2->getConstantString().c_str());
					result_bc=BYTE_CODE_LOAD_STRING;
				}else{
					PERFORM_ARITHMETIC_OPERATION(+);
				}

				break;
			case BYTE_CODE_SUB:
				PERFORM_ARITHMETIC_OPERATION(-);
				break;
			case BYTE_CODE_MUL:
				PERFORM_ARITHMETIC_OPERATION(*);
				break;
			case BYTE_CODE_DIV:
				if(i2->value_op2==0){
					THROW_EXCEPTION(zs_strutils::format("[%s:%i] divide by 0"
							,eval_data->current_parsing_file
							,token_operator->line
					));
				}
				PERFORM_ARITHMETIC_OPERATION(/);
				break;
			case BYTE_CODE_MOD:
				if(i2->value_op2==0){
					THROW_EXCEPTION(zs_strutils::format("[%s:%i] divide by 0"
							,eval_data->current_parsing_file
							,token_operator->line
					));
				}
				if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_zs_int=(i1->value_op2)%(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_ZS_INT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_float=fmod(*((float *)&i1->value_op2),(i2->value_op2));
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=fmod(*((float *)&i1->value_op2),(i2->value_op2));
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=fmod(*((float *)&i1->value_op2), *((float *)&i2->value_op2));
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else{
					THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform constant operation %s '/' %s"
							,eval_data->current_parsing_file
							,token_operator->line
							,i1->getConstantValueOp2ToString().c_str()
							,i2->getConstantValueOp2ToString().c_str()));
				}
				break;
			// binary ops
			case BYTE_CODE_AND:
				PERFORM_BINARY_OPERATION(&);
				break;
			case BYTE_CODE_OR:
				PERFORM_BINARY_OPERATION(|);
				break;
			case BYTE_CODE_XOR:
				PERFORM_BINARY_OPERATION(^);
				break;
			// logic ops
			case BYTE_CODE_LOGIC_AND:
				PERFORM_LOGIC_OPERATION(&&);
				break;
			case BYTE_CODE_LOGIC_OR:
				PERFORM_LOGIC_OPERATION(||);
				break;
			case BYTE_CODE_LT:
				PERFORM_LOGIC_OPERATION(<);
				break;
			case BYTE_CODE_GT:
				PERFORM_LOGIC_OPERATION(>);
				break;
			case BYTE_CODE_GTE:
				PERFORM_LOGIC_OPERATION(>=);
				break;
			case BYTE_CODE_LTE:
				PERFORM_LOGIC_OPERATION(<=);
				break;
			case BYTE_CODE_EQU:
				PERFORM_LOGIC_OPERATION(==);
				break;
			case BYTE_CODE_NOT_EQU:
				PERFORM_LOGIC_OPERATION(==);
				break;

			default:
				THROW_EXCEPTION(zs_strutils::format("const operation KK '%s' not implemented",eval_operator_to_str(token_operator->operator_type)));
				break;
			}


			switch(result_bc){
			case BYTE_CODE_LOAD_ZS_INT:
				result_instruction=new EvalInstruction(
						result_bc
						,ZS_IDX_UNDEFINED
						,result_op_zs_int
				);
				break;
			case BYTE_CODE_LOAD_FLOAT:
				result_instruction=new EvalInstruction(
						result_bc
						,ZS_IDX_UNDEFINED
						,result_op_float
				);
				break;
			case BYTE_CODE_LOAD_STRING:
				result_instruction=new EvalInstruction(
						result_bc
						,ZS_IDX_UNDEFINED
						,(zs_int)eval_data->zs->registerConstantScriptObjectString(result_op_str)
				);
				break;

			}

			return result_instruction;
		}

		EvalInstruction *eval_expression_optimize(EvalData *eval_data,Scope *scope_info,TokenNode   *token_operation, std::vector<EvalInstruction *> *instructions){
			size_t size_instructions=instructions->size();
			EvalInstruction *instruction=NULL;
			bool is_i1_K=false;
			bool is_i2_K=false;
			bool is_i1_R=false;
			bool is_i2_R=false;
			int n_eval_ops=0;

			if(size_instructions < 2){
				return NULL;
			}

			EvalInstruction *i1=instructions->at(size_instructions-2);
			EvalInstruction *i2=instructions->at(size_instructions-1);

			is_i1_K=i1->vm_instruction.isConstant();
			is_i2_K=i2->vm_instruction.isConstant();

			// can be computed, yeah!
			if(is_i1_K && is_i2_K){
				instruction=eval_expression_perform_KK_operation(eval_data,token_operation,i1,i2);
				n_eval_ops=2;
			}else{ // try KR/RK/RR/k or R
				ByteCode load_byte_code_1= i1->vm_instruction.byte_code;
				zs_int	 load_value_op2_1= i1->vm_instruction.value_op2;
				ByteCode load_byte_code_2= i2->vm_instruction.byte_code;
				zs_int	 load_value_op2_2= i2->vm_instruction.value_op2;
				ByteCode byte_code=eval_operator_to_byte_code(token_operation->operator_type);
				unsigned short k_properties=0;


				is_i1_R=eval_expression_get_register(eval_data, scope_info, i1, load_byte_code_1,load_value_op2_1);
				is_i2_R=eval_expression_get_register(eval_data, scope_info, i2, load_byte_code_2,load_value_op2_2);
				if(is_i1_R && is_i2_R){ // RR
					instruction=new EvalInstruction(
							byte_code
							,load_value_op2_1
							,(load_byte_code_2 & 0xff) | (load_byte_code_2 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0) << (8)
							,MSK_INSTRUCTION_PROPERTY_ILOAD_RR | (load_byte_code_1 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
					);

				}else if(is_i1_R && is_i2_K){ // RK
					n_eval_ops=2;

					if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_2))==0){
						return NULL;
					}


					instruction= new EvalInstruction(
							byte_code
							,load_value_op2_1
							,load_value_op2_2
							,MSK_INSTRUCTION_PROPERTY_ILOAD_RK
							| (load_byte_code_1 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
							| k_properties
					);
				}else if(is_i1_K && is_i2_R){ // KR
					n_eval_ops=2;

					if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
						return NULL;
					}


					instruction=new EvalInstruction(
							byte_code
							,load_value_op2_2
							,load_value_op2_1
							,MSK_INSTRUCTION_PROPERTY_ILOAD_KR
							| (load_byte_code_2 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
							| k_properties
					);
				}else{ // check last instructions
					i1=i2;
					if(is_i2_K){
						n_eval_ops=1;

						if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
							return NULL;
						}

						instruction=new EvalInstruction(
								byte_code
								, ZS_IDX_UNDEFINED
								, load_value_op2_2
								, MSK_INSTRUCTION_PROPERTY_ILOAD_K |k_properties
						);

					}else if(is_i2_R){
						n_eval_ops=1;

						instruction=new EvalInstruction(
								byte_code
								,load_value_op2_2
								,ZS_IDX_UNDEFINED
								,MSK_INSTRUCTION_PROPERTY_ILOAD_R | (load_byte_code_2 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
						);
					}
				}
			}

			if(n_eval_ops == 0){
				return NULL;
			}


			// remove last two instructions from vector
			delete i1;
			if(n_eval_ops == 2){
				delete i2;
			}

			// erase last two instructions
			instructions->resize(instructions->size()-n_eval_ops);

			// and push the new one

			return instruction;

		}
	}
}
