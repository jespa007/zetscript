/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define ZS_MAX_REGISTER_LENGTH	128

#define ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(_file,_line,_message,...) \
	ZS_THROW_EXCEPTION(zs_strutils::format_file_line(_file,_line,_message,__VA_ARGS__).c_str())


#define ZS_EVAL_CONSTANT_ARITHMETIC_OPERATION(ARITHMETIC_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_INT){\
		result_op_int=(i1->value_op2) ARITHMETIC_OP (i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_INT;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){\
		result_op_float=(i1->value_op2) ARITHMETIC_OP (ZS_READ_INTPTR_FLOAT(i2->value_op2));\
		ZS_FLOAT_COPY(&result_op_int,&result_op_float);\
		result_bc=BYTE_CODE_LOAD_FLOAT;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_INT){\
		result_op_float=(ZS_READ_INTPTR_FLOAT(i1->value_op2)) ARITHMETIC_OP (i2->value_op2);\
		ZS_FLOAT_COPY(&result_op_int,&result_op_float);\
		result_bc=BYTE_CODE_LOAD_FLOAT;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){\
		result_op_float=(ZS_READ_INTPTR_FLOAT(i1->value_op2)) ARITHMETIC_OP (ZS_READ_INTPTR_FLOAT(i2->value_op2));\
		ZS_FLOAT_COPY(&result_op_int,&result_op_float);\
		result_bc=BYTE_CODE_LOAD_FLOAT;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			eval_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant arithmetic operation %s %s %s"\
			,i1->getConstantValueOp2ToString().c_str()\
			,ZS_STR(ARITHMETIC_OP)\
			,i2->getConstantValueOp2ToString().c_str());\
	}


#define ZS_EVAL_CONSTANT_COMPARE_OPERATION(__COMPARE_OP__) \
	if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_INT){\
		result_op_bool=(i1->value_op2)__COMPARE_OP__(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_BOOL && i2->byte_code == BYTE_CODE_LOAD_BOOL){\
		result_op_bool=(i1->value_op2)__COMPARE_OP__(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){\
		result_op_float=(i1->value_op2)__COMPARE_OP__ ZS_READ_INTPTR_FLOAT(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_INT){\
		result_op_bool=ZS_READ_INTPTR_FLOAT(i1->value_op2)__COMPARE_OP__(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){\
		result_op_bool=ZS_READ_INTPTR_FLOAT(i1->value_op2)__COMPARE_OP__ ZS_READ_INTPTR_FLOAT(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if( i1->byte_code == BYTE_CODE_LOAD_STRING && i2->byte_code==BYTE_CODE_LOAD_STRING){\
		result_op_bool=ZS_STRCMP(i1->getConstantValueOp2ToString().c_str(), __COMPARE_OP__ ,i2->getConstantValueOp2ToString().c_str());\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			eval_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant compare operation %s %s %s"\
			,i1->getConstantValueOp2ToString().c_str()\
			,ZS_STR(__COMPARE_OP__)\
			,i2->getConstantValueOp2ToString().c_str()\
		);\
	}

#define ZS_EVAL_CONSTANT_BINARY_OPERATION(BINARY_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_INT){\
		result_op_int=(i1->value_op2)BINARY_OP(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_INT;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			 eval_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant bitwise operation %s %s %s"\
			,i1->getConstantValueOp2ToString().c_str()\
			,ZS_STR(BINARY_OP)\
			,i2->getConstantValueOp2ToString().c_str()\
		);\
	}

#define ZS_EVAL_CONSTANT_LOGIC_OPERATION(LOGIC_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_BOOL && i2->byte_code == BYTE_CODE_LOAD_BOOL){\
		result_op_bool=(i1->value_op2)LOGIC_OP(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			eval_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant boolean operation %s %s %s"\
			,i1->getConstantValueOp2ToString().c_str()\
			,ZS_STR(LOGIC_OP)\
			,i2->getConstantValueOp2ToString().c_str()\
		);\
	}


namespace zetscript{



	unsigned short eval_expression_load_const_byte_code_to_instruction_property(ByteCode byte_code){

		switch(byte_code){
		default:
			break;
		case BYTE_CODE_LOAD_INT:
			return INSTRUCTION_PROPERTY_INT;
			break;
		case BYTE_CODE_LOAD_BOOL:
			return INSTRUCTION_PROPERTY_BOOL;
			break;
		case BYTE_CODE_LOAD_FLOAT:
			return INSTRUCTION_PROPERTY_FLOAT;
			break;
		case BYTE_CODE_LOAD_STRING:
			return INSTRUCTION_PROPERTY_STRING;
			break;
		}

		return 0;
	}

	bool eval_expression_get_register(EvalData *eval_data, Scope *scope,EvalInstruction *i1, ByteCode & load_byte_code, zs_int & load_value_op2){
		Symbol *symbol_found=NULL;

		 if(!BYTE_CODE_IS_LOAD_VARIABLE_IMMEDIATE(i1->vm_instruction.byte_code)){
			 return false;
		 }

		 // is a type, not register. Note may be we could optimize but generally we are doing ops between registers
		 if(eval_data->script_type_factory->getScriptType(i1->symbol_name)!=NULL){
			return false;
		}

		 load_value_op2=i1->vm_instruction.value_op2;
		 load_byte_code=i1->vm_instruction.byte_code;

		if(i1->vm_instruction.byte_code == BYTE_CODE_FIND_VARIABLE){

			if((symbol_found = eval_find_local_symbol(eval_data,scope,i1->symbol_name)) != NULL){
				load_byte_code=BYTE_CODE_LOAD_LOCAL;
				load_value_op2=symbol_found->idx_position;
				if((symbol_found->properties & (ZS_SYMBOL_PROPERTY_ARG_BY_REF)) == ZS_SYMBOL_PROPERTY_ARG_BY_REF){
					load_byte_code=BYTE_CODE_LOAD_REF;
				}
			}else { // if global or not found cannot be optimized
				return false;
			}
		}else if(i1->vm_instruction.byte_code == BYTE_CODE_LOAD_THIS_VARIABLE){
			if(load_value_op2 == ZS_UNDEFINED_IDX){
				ScriptType *sc=NULL;
				if(
					   ( eval_data->current_function->script_function->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)!= 0
					&& (	scope->script_type_owner->id != ScriptTypeId::SCRIPT_TYPE_ID_CLASS_MAIN)
					   // is function member
					){ // type members are defined as functions
					sc=scope->script_type_owner;
				}

				if(sc != NULL){
					symbol_found = sc->getSymbolVariableMember(i1->symbol_name.c_str());
				}

				if(symbol_found == NULL){
					return false;
				}

				load_value_op2=symbol_found->idx_position;
			}

			// should assign the idx
		}

		if(load_value_op2 >= ZS_MAX_REGISTER_LENGTH){ // if number of vars is > ZS_MAX_REGISTER_LENGTH cannot be registered either
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
		zs_float  result_op_float=0;
		zs_int result_op_int=0;
		bool	result_op_bool=false;
		zs_string result_op_str="";
		zs_string str_constant_key="";
		EvalInstruction *result_instruction=NULL;
		ByteCode result_bc=BYTE_CODE_INVALID;
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
				result_op_str=zs_strutils::format("%s%s"
						,i1->getConstantString().c_str()
						,i2->byte_code==BYTE_CODE_LOAD_STRING?i2->getConstantString().c_str():i2->getConstantValueOp2ToString().c_str());
				result_bc=BYTE_CODE_LOAD_STRING;
			}
			else if(i1->isConstant() && i2->byte_code == BYTE_CODE_LOAD_STRING){
				result_op_str=zs_strutils::format("%s%s"
						,i1->byte_code==BYTE_CODE_LOAD_STRING?i1->getConstantString().c_str():i1->getConstantValueOp2ToString().c_str()
						,i2->getConstantString().c_str());
				result_bc=BYTE_CODE_LOAD_STRING;
			}else{
				ZS_EVAL_CONSTANT_ARITHMETIC_OPERATION(+);
			}

			break;
		case BYTE_CODE_SUB:
			ZS_EVAL_CONSTANT_ARITHMETIC_OPERATION(-);
			break;
		case BYTE_CODE_MUL:
			ZS_EVAL_CONSTANT_ARITHMETIC_OPERATION(*);
			break;
		case BYTE_CODE_DIV:
			if(i2->value_op2==0){
				ZS_THROW_EXCEPTION_FILE_LINE(
						 eval_data->current_parsing_file
						,token_operator->line
						,"constant divide operation by 0"
				);
			}
			if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_INT){
				result_op_float=((zs_float)(i1->value_op2))/((zs_float)(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_INT){
				result_op_float=ZS_READ_INTPTR_FLOAT(i1->value_op2)/(i2->value_op2);
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
				result_op_float=ZS_READ_INTPTR_FLOAT(i1->value_op2)/(i2->value_op2);
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
				result_op_float=ZS_READ_INTPTR_FLOAT(i1->value_op2)/ ZS_READ_INTPTR_FLOAT(i2->value_op2);
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else{
				ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(
					eval_data->current_parsing_file
					,token_operator->line
					,"Invalid constant arithmetic operation %s / %s"
					,i1->getConstantValueOp2ToString().c_str()
					,i2->getConstantValueOp2ToString().c_str()
				);
			}
			break;
		case BYTE_CODE_MOD:
			if(i2->value_op2==0){
				ZS_THROW_EXCEPTION_FILE_LINE(
					eval_data->current_parsing_file
					,token_operator->line
					,"constant module operation by 0"
				);
			}
			if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_INT){
				result_op_float=fmod((i1->value_op2),(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_INT){
				result_op_float=fmod(ZS_READ_INTPTR_FLOAT(i1->value_op2),(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
				result_op_float=fmod(ZS_READ_INTPTR_FLOAT(i1->value_op2),(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_FLOAT && i2->byte_code == BYTE_CODE_LOAD_FLOAT){
				result_op_float=fmod(ZS_READ_INTPTR_FLOAT(i1->value_op2), ZS_READ_INTPTR_FLOAT(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_FLOAT;
			}else{
				ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(
					eval_data->current_parsing_file
					,token_operator->line
					,"Invalid constant arithmetic operation %s % %s"
					,i1->getConstantValueOp2ToString().c_str()
					,i2->getConstantValueOp2ToString().c_str()
				);
			}
			break;
		// bitwise ops
		case BYTE_CODE_BITWISE_AND:
			ZS_EVAL_CONSTANT_BINARY_OPERATION(&);
			break;
		case BYTE_CODE_BITWISE_OR:
			ZS_EVAL_CONSTANT_BINARY_OPERATION(|);
			break;
		case BYTE_CODE_BITWISE_XOR:
			ZS_EVAL_CONSTANT_BINARY_OPERATION(^);
			break;
		case BYTE_CODE_SHL:
			ZS_EVAL_CONSTANT_BINARY_OPERATION(<<);
			break;
		case BYTE_CODE_SHR:
			ZS_EVAL_CONSTANT_BINARY_OPERATION(>>);
			break;

		// logic ops
		case BYTE_CODE_LOGIC_AND:
			ZS_EVAL_CONSTANT_LOGIC_OPERATION(&&);
			break;
		case BYTE_CODE_LOGIC_OR:
			ZS_EVAL_CONSTANT_LOGIC_OPERATION(||);
			break;
		// compare op
		case BYTE_CODE_LT:
			ZS_EVAL_CONSTANT_COMPARE_OPERATION(<);
			break;
		case BYTE_CODE_GT:
			ZS_EVAL_CONSTANT_COMPARE_OPERATION(>);
			break;
		case BYTE_CODE_GTE:
			ZS_EVAL_CONSTANT_COMPARE_OPERATION(>=);
			break;
		case BYTE_CODE_LTE:
			ZS_EVAL_CONSTANT_COMPARE_OPERATION(<=);
			break;
		case BYTE_CODE_EQU:
			ZS_EVAL_CONSTANT_COMPARE_OPERATION(==);
			break;
		case BYTE_CODE_NOT_EQU:
			ZS_EVAL_CONSTANT_COMPARE_OPERATION(!=);
			break;
		case BYTE_CODE_IN:
			if(INSTRUCTION_BYTE_CODE_IS_LOAD_INT(i1) && INSTRUCTION_BYTE_CODE_IS_LOAD_STRING(i2)){
				result_op_bool=strchr(i2->getConstantString().c_str(),i1->getConstantInt()) != NULL;
				result_bc=BYTE_CODE_LOAD_BOOL;
			}else if(INSTRUCTION_BYTE_CODE_IS_LOAD_STRING(i1) && INSTRUCTION_BYTE_CODE_IS_LOAD_STRING(i2)){
				result_op_bool=zs_strutils::contains(i2->getConstantString().c_str(),i1->getConstantString().c_str());
				result_bc=BYTE_CODE_LOAD_BOOL;
			}
			else{
				ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(
					eval_data->current_parsing_file
					,token_operator->line
					,"Invalid constant operation %s in %s"
					,i1->getConstantValueOp2ToString().c_str()
					,i2->getConstantValueOp2ToString().c_str()
				);
			}
			break;
		default:
			ZS_THROW_EXCEPTION_FILE_LINE(
				eval_data->current_parsing_file
				,token_operator->line
				,"Invalid constant operation %s %s %s"
				,i1->getConstantValueOp2ToString().c_str()
				,ByteCodeHelper::getNameOperator(byte_code)
				,i2->getConstantValueOp2ToString().c_str()
			);
			break;
		}

		switch(result_bc){
		default:
			break;
		case BYTE_CODE_LOAD_INT:
			result_instruction=new EvalInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,result_op_int
			);
			break;
		case BYTE_CODE_LOAD_FLOAT:
			result_instruction=new EvalInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,result_op_int
			);
			break;
		case BYTE_CODE_LOAD_STRING:
			str_constant_key=zs_string("\"")+result_op_str+"\"";
			result_instruction=new EvalInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,(zs_int)eval_data->zs->registerStkConstantStringObject(str_constant_key,result_op_str)
			);
			break;
		case BYTE_CODE_LOAD_BOOL:
			result_instruction=new EvalInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,result_op_bool
			);
			break;
		}

		return result_instruction;
	}

	EvalInstruction *eval_expression_optimize(
			EvalData *eval_data
			,Scope *scope_info
			,TokenNode   *token_operation
			, zs_vector<EvalInstruction *> *eval_instructions
	){
		int size_instructions=eval_instructions->size();
		EvalInstruction *instruction=NULL;
		bool is_i1_K=false;
		bool is_i2_K=false;
		bool is_i1_R=false;
		bool is_i2_R=false;
		int n_eval_ops=0;

		if(size_instructions < 2){
			return NULL;
		}

		EvalInstruction *i1=(EvalInstruction *)eval_instructions->get(size_instructions-2);
		EvalInstruction *i2=(EvalInstruction *)eval_instructions->get(size_instructions-1);


		is_i1_K=i1->vm_instruction.isConstant();
		is_i2_K=i2->vm_instruction.isConstant();

		// can be reduce the number of bytes codes down to 2, yeah!
		if(token_operation->operator_type == ZS_OPERATOR_INSTANCEOF){
			instruction=new EvalInstruction(
					BYTE_CODE_INSTANCEOF
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,i2->vm_instruction.value_op2
			);

			instruction->symbol_name=token_operation->value;
			instruction->instruction_source_info = eval_instruction_source_info(
				eval_data
				,eval_data->current_parsing_file
				,token_operation->line
				,token_operation->value
			);
			i1=i2; // swap instruction to erase...
			n_eval_ops=1;
		}else if(is_i1_K && is_i2_K){
			instruction=eval_expression_perform_KK_operation(eval_data,token_operation,i1,i2);
			n_eval_ops=2;
		}else{ // try KR/RK/RR/k or R
			ByteCode load_byte_code_1= i1->vm_instruction.byte_code;
			zs_int	 load_value_op2_1= i1->vm_instruction.value_op2;
			ByteCode load_byte_code_2= i2->vm_instruction.byte_code;
			zs_int	 load_value_op2_2= i2->vm_instruction.value_op2;

			ByteCode byte_code=eval_operator_to_byte_code(token_operation->operator_type);
			unsigned short k_properties=0;
			n_eval_ops=2;

			is_i1_R=eval_expression_get_register(eval_data, scope_info, i1, load_byte_code_1,load_value_op2_1);
			is_i2_R=eval_expression_get_register(eval_data, scope_info, i2, load_byte_code_2,load_value_op2_2);

			if(is_i1_R && is_i2_R){ // RR
				instruction=new EvalInstruction(
						byte_code
						,load_value_op2_1
						,((load_value_op2_2 & 0xff) << 16) // pack value + properties
						  | (
								load_byte_code_2 == BYTE_CODE_LOAD_THIS_VARIABLE ? INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR
								:0
							)
						,INSTRUCTION_PROPERTY_ILOAD_RR
						| (
								load_byte_code_1 == BYTE_CODE_LOAD_THIS_VARIABLE ? INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR
								:0
						)
				);

			}else if(is_i1_R && is_i2_K){ // RK
				if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_2))==0){
					return NULL;
				}

				instruction= new EvalInstruction(
						byte_code
						,load_value_op2_1
						,load_value_op2_2
						,INSTRUCTION_PROPERTY_ILOAD_RK
						| (

								load_byte_code_1 == BYTE_CODE_LOAD_THIS_VARIABLE ? INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR
								:0
						)
						| k_properties
				);
			}else if(is_i1_K && is_i2_R){ // KR
				if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
					return NULL;
				}

				instruction=new EvalInstruction(
						byte_code
						,load_value_op2_2
						,load_value_op2_1
						,INSTRUCTION_PROPERTY_ILOAD_KR
						| (
								load_byte_code_2 == BYTE_CODE_LOAD_THIS_VARIABLE ? INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR
								:0
						)
						| k_properties
				);
			}else{ // is K or R
				i1=i2;
				n_eval_ops=1;
				if(is_i2_K){ // is K
					if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
						return NULL;
					}

					instruction=new EvalInstruction(
							byte_code
							, INSTRUCTION_VALUE_OP1_NOT_DEFINED
							, load_value_op2_2
							, INSTRUCTION_PROPERTY_ILOAD_K |k_properties
					);

				}else if(is_i2_R){ // is R

					instruction=new EvalInstruction(
							byte_code
							,load_value_op2_2
							,ZS_UNDEFINED_IDX
							,INSTRUCTION_PROPERTY_ILOAD_R | (
									load_byte_code_2 == BYTE_CODE_LOAD_THIS_VARIABLE ? INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR
									:0
							)
					);
				}else{ // no optimization...
					n_eval_ops=0;
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
		eval_instructions->resize(eval_instructions->size()-n_eval_ops);

		// and push the new one
		return instruction;

	}
}

