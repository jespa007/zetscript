/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define ZS_MAX_REGISTER_LENGTH	128

#define ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(_file,_line,_message,...) \
	ZS_THROW_EXCEPTION(String::formatFileLine(_file,_line,_message,__VA_ARGS__).toConstChar())


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
			compiler_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant arithmetic operation %s %s %s"\
			,i1->getConstantValueOp2ToString().toConstChar()\
			,ZS_STR(ARITHMETIC_OP)\
			,i2->getConstantValueOp2ToString().toConstChar());\
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
		result_op_bool=ZS_STRCMP(i1->getConstantValueOp2ToString().toConstChar(), __COMPARE_OP__ ,i2->getConstantValueOp2ToString().toConstChar());\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			compiler_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant compare operation %s %s %s"\
			,i1->getConstantValueOp2ToString().toConstChar()\
			,ZS_STR(__COMPARE_OP__)\
			,i2->getConstantValueOp2ToString().toConstChar()\
		);\
	}

#define ZS_EVAL_CONSTANT_BINARY_OPERATION(BINARY_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_INT && i2->byte_code == BYTE_CODE_LOAD_INT){\
		result_op_int=(i1->value_op2)BINARY_OP(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_INT;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			 compiler_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant bitwise operation %s %s %s"\
			,i1->getConstantValueOp2ToString().toConstChar()\
			,ZS_STR(BINARY_OP)\
			,i2->getConstantValueOp2ToString().toConstChar()\
		);\
	}

#define ZS_EVAL_CONSTANT_LOGIC_OPERATION(LOGIC_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_BOOL && i2->byte_code == BYTE_CODE_LOAD_BOOL){\
		result_op_bool=(i1->value_op2)LOGIC_OP(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else{\
		ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(\
			compiler_data->current_parsing_file\
			,token_operator->line\
			,"Invalid constant boolean operation %s %s %s"\
			,i1->getConstantValueOp2ToString().toConstChar()\
			,ZS_STR(LOGIC_OP)\
			,i2->getConstantValueOp2ToString().toConstChar()\
		);\
	}


namespace zetscript{



	unsigned short compiler_expression_load_const_byte_code_to_instruction_property(ByteCode byte_code){

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

	bool compiler_expression_get_register(CompilerData *compiler_data, ScriptScope *scope,CompilerInstruction *i1, ByteCode & load_byte_code, zs_int & load_value_op2){
		Symbol *symbol_found=NULL;

		 if(!BYTE_CODE_IS_LOAD_VARIABLE_IMMEDIATE(i1->vm_instruction.byte_code)){
			 return false;
		 }

		 // is a type, not register. Note may be we could optimize but generally we are doing ops between registers
		 if(compiler_data->script_types_factory->getScriptType(i1->symbol_name)!=NULL){
			return false;
		}

		 load_value_op2=i1->vm_instruction.value_op2;
		 load_byte_code=i1->vm_instruction.byte_code;

		if(i1->vm_instruction.byte_code == BYTE_CODE_FIND_VARIABLE){

			if((symbol_found = compiler_find_local_symbol(compiler_data,scope,i1->symbol_name)) != NULL){
				load_byte_code=BYTE_CODE_LOAD_LOCAL;
				load_value_op2=symbol_found->idx_position;
				if((symbol_found->properties & (SYMBOL_PROPERTY_ARG_BY_REF)) == SYMBOL_PROPERTY_ARG_BY_REF){
					load_byte_code=BYTE_CODE_LOAD_REF;
				}
			}else { // if global or not found cannot be optimized
				return false;
			}
		}else if(i1->vm_instruction.byte_code == BYTE_CODE_LOAD_THIS_VARIABLE){
			if(load_value_op2 == ZS_UNDEFINED_IDX){
				ScriptType *sc=NULL;
				if(
					   ( compiler_data->current_function->script_function->properties & SCRIPT_FUNCTION_PROPERTY_MEMBER_FUNCTION)!= 0
					&& (	scope->owner_type->id != SCRIPT_TYPE_ID_CLASS_MAIN)
					   // is function member
					){ // type members are defined as functions
					sc=scope->owner_type;
				}

				if(sc != NULL){
					symbol_found = sc->getSymbolVariableMember(i1->symbol_name.toConstChar());
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
	CompilerInstruction * compiler_expression_perform_KK_operation(
			CompilerData *compiler_data
			,TokenNode *token_operator
			, CompilerInstruction *ei1
			, CompilerInstruction *ei2
	){
		zs_float  result_op_float=0;
		zs_int result_op_int=0;
		bool	result_op_bool=false;
		String result_op_str="";
		String str_constant_key="";
		CompilerInstruction *result_instruction=NULL;
		ByteCode result_bc=BYTE_CODE_INVALID;
		Instruction *i1=&ei1->vm_instruction;
		Instruction *i2=&ei2->vm_instruction;
		ByteCode byte_code=compiler_operator_to_byte_code(token_operator->operator_type);

		// check last two instructions stk op1 and stk op2 are bool/int/float or string
		if(!(i1->isConstant() && i2->isConstant())){
			return NULL;
		}

		// which operation ?
		switch(byte_code){
		case BYTE_CODE_ADD: // int & int/int & float/float&float

			if(i1->byte_code == BYTE_CODE_LOAD_STRING && i2->isConstant()){
				result_op_str=String::format("%s%s"
						,i1->getConstantString().toConstChar()
						,i2->byte_code==BYTE_CODE_LOAD_STRING?i2->getConstantString().toConstChar():i2->getConstantValueOp2ToString().toConstChar());
				result_bc=BYTE_CODE_LOAD_STRING;
			}
			else if(i1->isConstant() && i2->byte_code == BYTE_CODE_LOAD_STRING){
				result_op_str=String::format("%s%s"
						,i1->byte_code==BYTE_CODE_LOAD_STRING?i1->getConstantString().toConstChar():i1->getConstantValueOp2ToString().toConstChar()
						,i2->getConstantString().toConstChar());
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
						 compiler_data->current_parsing_file
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
					compiler_data->current_parsing_file
					,token_operator->line
					,"Invalid constant arithmetic operation %s / %s"
					,i1->getConstantValueOp2ToString().toConstChar()
					,i2->getConstantValueOp2ToString().toConstChar()
				);
			}
			break;
		case BYTE_CODE_MOD:
			if(i2->value_op2==0){
				ZS_THROW_EXCEPTION_FILE_LINE(
					compiler_data->current_parsing_file
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
					compiler_data->current_parsing_file
					,token_operator->line
					,"Invalid constant arithmetic operation %s % %s"
					,i1->getConstantValueOp2ToString().toConstChar()
					,i2->getConstantValueOp2ToString().toConstChar()
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
				result_op_bool=strchr(i2->getConstantString().toConstChar(),i1->getConstantInt()) != NULL;
				result_bc=BYTE_CODE_LOAD_BOOL;
			}else if(INSTRUCTION_BYTE_CODE_IS_LOAD_STRING(i1) && INSTRUCTION_BYTE_CODE_IS_LOAD_STRING(i2)){
				result_op_bool=i2->getConstantString().contains(i1->getConstantString());
				result_bc=BYTE_CODE_LOAD_BOOL;
			}
			else{
				ZS_THROW_EXCEPTION_CONSTANT_EVAL_OPERATION(
					compiler_data->current_parsing_file
					,token_operator->line
					,"Invalid constant operation %s in %s"
					,i1->getConstantValueOp2ToString().toConstChar()
					,i2->getConstantValueOp2ToString().toConstChar()
				);
			}
			break;
		default:
			ZS_THROW_EXCEPTION_FILE_LINE(
				compiler_data->current_parsing_file
				,token_operator->line
				,"Invalid constant operation %s %s %s"
				,i1->getConstantValueOp2ToString().toConstChar()
				,ByteCodeHelper::getByteCodeOperatorName(byte_code)
				,i2->getConstantValueOp2ToString().toConstChar()
			);
			break;
		}

		switch(result_bc){
		default:
			break;
		case BYTE_CODE_LOAD_INT:
			result_instruction=new CompilerInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,result_op_int
			);
			break;
		case BYTE_CODE_LOAD_FLOAT:
			result_instruction=new CompilerInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,result_op_int
			);
			break;
		case BYTE_CODE_LOAD_STRING:
			str_constant_key=String("\"")+result_op_str+"\"";
			result_instruction=new CompilerInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,(zs_int)compiler_data->script_engine->registerStkConstantStringScriptObject(str_constant_key,result_op_str)
			);
			break;
		case BYTE_CODE_LOAD_BOOL:
			result_instruction=new CompilerInstruction(
					result_bc
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,result_op_bool
			);
			break;
		}

		return result_instruction;
	}

	CompilerInstruction *compiler_expression_optimize(
			CompilerData *compiler_data
			,ScriptScope *scope_info
			,TokenNode   *token_operation
			, Vector<CompilerInstruction *> *compiler_instructions
	){
		int size_instructions=compiler_instructions->length();
		CompilerInstruction *instruction=NULL;
		bool is_i1_K=false;
		bool is_i2_K=false;
		bool is_i1_R=false;
		bool is_i2_R=false;
		int n_compiler_ops=0;

		if(size_instructions < 2){
			return NULL;
		}

		CompilerInstruction *i1=(CompilerInstruction *)compiler_instructions->get(size_instructions-2);
		CompilerInstruction *i2=(CompilerInstruction *)compiler_instructions->get(size_instructions-1);


		is_i1_K=i1->vm_instruction.isConstant();
		is_i2_K=i2->vm_instruction.isConstant();

		// can be reduce the number of bytes codes down to 2, yeah!
		if(token_operation->operator_type == ZS_OPERATOR_INSTANCEOF){
			instruction=new CompilerInstruction(
					BYTE_CODE_INSTANCEOF
					, INSTRUCTION_VALUE_OP1_NOT_DEFINED
					,i2->vm_instruction.value_op2
			);

			instruction->symbol_name=token_operation->value;
			instruction->instruction_source_info = compiler_instruction_source_info(
				compiler_data
				,compiler_data->current_parsing_file
				,token_operation->line
				,token_operation->value
			);
			i1=i2; // swap instruction to erase...
			n_compiler_ops=1;
		}else if(is_i1_K && is_i2_K){
			instruction=compiler_expression_perform_KK_operation(compiler_data,token_operation,i1,i2);
			n_compiler_ops=2;
		}else{ // try KR/RK/RR/k or R
			ByteCode load_byte_code_1= i1->vm_instruction.byte_code;
			zs_int	 load_value_op2_1= i1->vm_instruction.value_op2;
			ByteCode load_byte_code_2= i2->vm_instruction.byte_code;
			zs_int	 load_value_op2_2= i2->vm_instruction.value_op2;

			ByteCode byte_code=compiler_operator_to_byte_code(token_operation->operator_type);
			unsigned short k_properties=0;
			n_compiler_ops=2;

			is_i1_R=compiler_expression_get_register(compiler_data, scope_info, i1, load_byte_code_1,load_value_op2_1);
			is_i2_R=compiler_expression_get_register(compiler_data, scope_info, i2, load_byte_code_2,load_value_op2_2);

			if(is_i1_R && is_i2_R){ // RR
				instruction=new CompilerInstruction(
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
				if((k_properties=compiler_expression_load_const_byte_code_to_instruction_property(load_byte_code_2))==0){
					return NULL;
				}

				instruction= new CompilerInstruction(
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
				if((k_properties=compiler_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
					return NULL;
				}

				instruction=new CompilerInstruction(
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
				n_compiler_ops=1;
				if(is_i2_K){ // is K
					if((k_properties=compiler_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
						return NULL;
					}

					instruction=new CompilerInstruction(
							byte_code
							, INSTRUCTION_VALUE_OP1_NOT_DEFINED
							, load_value_op2_2
							, INSTRUCTION_PROPERTY_ILOAD_K |k_properties
					);

				}else if(is_i2_R){ // is R

					instruction=new CompilerInstruction(
							byte_code
							,load_value_op2_2
							,ZS_UNDEFINED_IDX
							,INSTRUCTION_PROPERTY_ILOAD_R | (
									load_byte_code_2 == BYTE_CODE_LOAD_THIS_VARIABLE ? INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR
									:0
							)
					);
				}else{ // no optimization...
					n_compiler_ops=0;
				}
			}
		}

		if(n_compiler_ops == 0){
			return NULL;
		}

		// remove last two instructions from vector
		delete i1;
		if(n_compiler_ops == 2){
			delete i2;
		}

		// erase last two instructions
		compiler_instructions->resize(compiler_instructions->length()-n_compiler_ops);

		// and push the new one
		return instruction;

	}
}

