
#define MAX_REGISTER_LENGTH	256


namespace zetscript{

	namespace eval{

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


		unsigned eval_expression_load_const_byte_code_to_instruction_property(ByteCode byte_code){

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
		// IMMEDIATE OPERATION 1 OPS
		EvalInstruction * eval_expression_perform_K_operation(EvalData *eval_data,ByteCode byte_code, EvalInstruction *i1){
			EvalInstruction *result_instruction=NULL;
			unsigned k_properties=0;

			if(i1->vm_instruction.isConstant()==false){
				return NULL;
			}

			if(IS_BYTE_CODE_OPERATION(byte_code) == false){
				return NULL;
			}

			if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(i1->vm_instruction.byte_code))==0){
				return NULL;
			}

			return new EvalInstruction(
					byte_code
					, ZS_IDX_UNDEFINED
					, i1->vm_instruction.value_op2
					, MSK_INSTRUCTION_PROPERTY_ILOAD_K |k_properties
			);
		}

		EvalInstruction * eval_expression_perform_R_operation(EvalData *eval_data,Scope *scope,  ByteCode byte_code, EvalInstruction *i1){
			EvalInstruction *result_instruction=NULL;
			ByteCode load_byte_code= i1->vm_instruction.byte_code;
			zs_int	 load_value_op2= i1->vm_instruction.value_op2;

			if(IS_BYTE_CODE_OPERATION(byte_code) == false){
				return NULL;
			}

			if(IS_BYTE_CODE_LOAD_LOCAL_GLOBAL_OR_FIND(load_byte_code) == false){
				return NULL;
			}

			if(eval_expression_get_register(eval_data, scope, i1, load_byte_code,load_value_op2) == false){
				return NULL;
			}

			return new EvalInstruction(
					byte_code
					,load_value_op2
					,ZS_IDX_UNDEFINED
					,MSK_INSTRUCTION_PROPERTY_ILOAD_R | (load_byte_code == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
			);
		}
		//-------------------------------------------------------------------------------------------------------------------------------
		// IMMEDIATE OPERATION 2 OPS
		EvalInstruction * eval_expression_perform_KK_operation(EvalData *eval_data,ByteCode byte_code, EvalInstruction *ei1, EvalInstruction *ei2){
			float  result_op_float=0;
			zs_int result_op_zs_int=0;
			std::string result_op_str="";
			EvalInstruction *result_instruction=NULL;
			ByteCode result_bc=ByteCode::BYTE_CODE_INVALID;
			Instruction *i1=&ei1->vm_instruction;
			Instruction *i2=&ei2->vm_instruction;


			// check last two instructions stk op1 and stk op2 are bool/int/float or string
			if(!(i1->isConstant() && i2->isConstant())){
				return NULL;
			}


			// which operation ?
			switch(byte_code){
			case BYTE_CODE_ADD: // int & int/int & float/float&float
				if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_zs_int=(i1->value_op2)+(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_ZS_INT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_float=*((float *)&i1->value_op2)+(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=*((float *)&i1->value_op2)+(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=*((float *)&i1->value_op2)+*((float *)&i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_STRING && i2->isConstant()){
					result_op_str=zs_strutils::format("%s%s",i1->getConstantString().c_str(),i2->getConstantValueOp2ToString().c_str());
					result_bc=BYTE_CODE_LOAD_STRING;
				}
				else if(i1->isConstant() && i2->byte_code == BYTE_CODE_LOAD_STRING){
					result_op_str=zs_strutils::format("%s%s",i1->getConstantValueOp2ToString().c_str(),i2->getConstantString().c_str());
					result_bc=BYTE_CODE_LOAD_STRING;
				}else{
					THROW_EXCEPTION(zs_strutils::format("I don't know how to perform constant operation %s '+' %s",i1->getConstantValueOp2ToString().c_str(),i2->getConstantValueOp2ToString().c_str()));
				}

				break;
			case BYTE_CODE_MUL:
				if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_zs_int=(i1->value_op2)*(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_ZS_INT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_float=*((float *)&i1->value_op2)*(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=*((float *)&i1->value_op2)*(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=*((float *)&i1->value_op2)* *((float *)&i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else{
					THROW_EXCEPTION(zs_strutils::format("I don't know how to perform constant operation %s '*' %s",i1->getConstantValueOp2ToString().c_str(),i2->getConstantValueOp2ToString().c_str()));
				}

				break;
			case BYTE_CODE_DIV:
				if(i2->value_op2==0){
					THROW_EXCEPTION("divide by 0");
				}
				if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_zs_int=(i1->value_op2)/(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_ZS_INT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
					result_op_float=*((float *)&i1->value_op2)/(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=*((float *)&i1->value_op2)/(i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
					result_op_float=*((float *)&i1->value_op2)/ *((float *)&i2->value_op2);
					result_bc=BYTE_CODE_LOAD_FLOAT;
				}else{
					THROW_EXCEPTION(zs_strutils::format("I don't know how to perform constant operation %s '/' %s",i1->getConstantValueOp2ToString().c_str(),i2->getConstantValueOp2ToString().c_str()));
				}
				break;
			case BYTE_CODE_MOD:
				if(i2->value_op2==0){
					THROW_EXCEPTION("divide by 0");
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
					THROW_EXCEPTION(zs_strutils::format("I don't know how to perform constant operation %s '/' %s",i1->getConstantValueOp2ToString().c_str(),i2->getConstantValueOp2ToString().c_str()));
				}
				break;
			default:
				THROW_EXCEPTION(zs_strutils::format("const instruction %i not implemented",byte_code));
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

		EvalInstruction * eval_expression_perform_KR_operation(EvalData *eval_data,Scope *scope,ByteCode byte_code, EvalInstruction *ei1, EvalInstruction *ei2){
			// i1: K / i2: R
			Instruction *i1=&ei1->vm_instruction;
			Instruction *i2=&ei2->vm_instruction;
			ByteCode load_byte_code_2= i2->byte_code;
			zs_int	 load_value_op2_2= i2->value_op2;

			unsigned k_properties=0;


			if(IS_BYTE_CODE_OPERATION(byte_code) == false){
				return NULL;
			}

			if(i1->isConstant() == false){
				return NULL;
			}

			if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(i1->byte_code))==0){
				return NULL;
			}


			if(IS_BYTE_CODE_LOAD_LOCAL_GLOBAL_OR_FIND(load_byte_code_2) == false){
				return NULL;
			}

			if(eval_expression_get_register(eval_data, scope, ei2, load_byte_code_2,load_value_op2_2) == false){
				return NULL;
			}

			return new EvalInstruction(
					byte_code
					,i2->value_op2
					,i1->value_op2
					,MSK_INSTRUCTION_PROPERTY_ILOAD_KR
					| (load_byte_code_2 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
					| k_properties
			);

		}

		EvalInstruction * eval_expression_perform_RK_operation(EvalData *eval_data,Scope *scope,ByteCode byte_code, EvalInstruction *ei1, EvalInstruction *ei2){
			// i1: R / i2: K
			Instruction *i1=&ei1->vm_instruction;
			Instruction *i2=&ei2->vm_instruction;
			ByteCode load_byte_code_1= i1->byte_code;
			zs_int	 load_value_op2_1= i1->value_op2;

			unsigned k_properties=0;

			if(IS_BYTE_CODE_OPERATION(byte_code) == false){
				return NULL;
			}

			if(i2->isConstant() == false){
				return NULL;
			}


			if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(i2->byte_code))==0){
				return NULL;
			}


			if(IS_BYTE_CODE_LOAD_LOCAL_GLOBAL_OR_FIND(load_byte_code_1) == false){
				return NULL;
			}

			if(eval_expression_get_register(eval_data, scope, ei1, load_byte_code_1,load_value_op2_1) == false){
				return NULL;
			}

			return new EvalInstruction(
					byte_code
					,i1->value_op2
					,i2->value_op2
					,MSK_INSTRUCTION_PROPERTY_ILOAD_RK
					| (load_byte_code_1 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
					| k_properties
			);

		}

		EvalInstruction * eval_expression_perform_RR_operation(EvalData *eval_data,Scope *scope,ByteCode byte_code, EvalInstruction *ei1, EvalInstruction *ei2){
			EvalInstruction *result_instruction=NULL;
			Instruction *i1=&ei1->vm_instruction;
			Instruction *i2=&ei2->vm_instruction;
			ByteCode load_byte_code_1= i1->byte_code;
			zs_int	 load_value_op2_1= i1->value_op2;
			ByteCode load_byte_code_2= i2->byte_code;
			zs_int	 load_value_op2_2= i2->value_op2;

			if(IS_BYTE_CODE_OPERATION(byte_code) == false){
				return NULL;
			}


			if(eval_expression_get_register(eval_data, scope, ei1, load_byte_code_1,load_value_op2_1) == false){
				return NULL;
			}


			if(eval_expression_get_register(eval_data, scope, ei2, load_byte_code_2,load_value_op2_2) == false){
				return NULL;
			}

			return new EvalInstruction(
					byte_code
					,load_value_op2_1
					,(load_byte_code_2 & 0xff) | (load_byte_code_2 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0) << (8)
					,MSK_INSTRUCTION_PROPERTY_ILOAD_RR | (load_byte_code_1 == BYTE_CODE_LOAD_LOCAL ? MSK_INSTRUCTION_PROPERTY_ILOAD_R_LOCAL_ACCESS:0)
			);

		}

		EvalInstruction * eval_expression_optimize_2fn(EvalData *eval_data,Scope *scope,ByteCode byte_code, std::vector<EvalInstruction *> *instructions){
			// PRE: Last two instructions were generated by both final nodes
			size_t size_instructions=instructions->size();
			EvalInstruction *instruction;

			if(size_instructions < 2){
				return NULL;
			}

			EvalInstruction *i1=instructions->at(size_instructions-2);
			EvalInstruction *i2=instructions->at(size_instructions-1);


			if((instruction=eval_expression_perform_KK_operation(eval_data,byte_code,i1,i2))==NULL){
				if((instruction=eval_expression_perform_KR_operation(eval_data,scope,byte_code,i1,i2))!=NULL){
					if((instruction=eval_expression_perform_RK_operation(eval_data,scope,byte_code,i1,i2))==NULL){
						if((instruction=eval_expression_perform_RR_operation(eval_data,scope,byte_code,i1,i2))==NULL){
							return NULL; // cannot be simplified
						}
					}
				}
			}


			// remove last two instructions from vector
			delete i1;
			delete i2;

			// erase last two instructions
			instructions->resize(instructions->size()-2);

			// and push the new one
			instructions->push_back(instruction);

			return instruction;

		}

		EvalInstruction *  eval_expression_optimize_1fn(EvalData *eval_data,Scope *scope_info,ByteCode byte_code, std::vector<EvalInstruction *> *instructions){
			// PRE: Last instruction node was generated by a final node
			size_t size_instructions=instructions->size();
			EvalInstruction *instruction=NULL;

			if(size_instructions < 2){
				return NULL;
			}

			EvalInstruction *i1=instructions->at(size_instructions-2);
			EvalInstruction *i2=instructions->at(size_instructions-1);

			// can be computed, yeah!
			if(i1->vm_instruction.isConstant() && i2->vm_instruction.isConstant()){
				instruction=eval_expression_perform_KK_operation(eval_data,byte_code,i1,i2);

				// remove last two instructions from vector
				delete i1;
				delete i2;

				// erase last two instructions
				instructions->resize(instructions->size()-2);

				// and push the new one
				instructions->push_back(instruction);

				return instruction;
			}

			i1=i2;

			Instruction *vm_i1=&i1->vm_instruction;

			if((instruction=eval_expression_perform_K_operation(eval_data,byte_code,i1))==NULL){
				if((instruction=eval_expression_perform_R_operation(eval_data,scope_info,byte_code,i1))==NULL){
					return NULL;
				}
			}

			// erase last instruction
			delete i1;

			// erase last instruction
			instructions->resize(instructions->size()-1);

			// and push the new one
			instructions->push_back(instruction);

			return instruction;
		}
	}
}
