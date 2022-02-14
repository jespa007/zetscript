/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define MAX_REGISTER_LENGTH	128


#define EVAL_OPERATION_ARITHMETIC(ARITHMETIC_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
		result_op_zs_int=(i1->value_op2) ARITHMETIC_OP (i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_ZS_INT;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_FLOAT){\
		result_op_float=(i1->value_op2) ARITHMETIC_OP (*((zs_float *)&i2->value_op2));\
		ZS_FLOAT_COPY(&result_op_zs_int,&result_op_float);\
		result_bc=BYTE_CODE_LOAD_ZS_FLOAT;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
		result_op_float=(*((zs_float *)&i1->value_op2)) ARITHMETIC_OP (i2->value_op2);\
		ZS_FLOAT_COPY(&result_op_zs_int,&result_op_float);\
		result_bc=BYTE_CODE_LOAD_ZS_FLOAT;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_FLOAT){\
		result_op_float=(*((zs_float *)&i1->value_op2)) ARITHMETIC_OP (*((zs_float *)&i2->value_op2));\
		ZS_FLOAT_COPY(&result_op_zs_int,&result_op_float);\
		result_bc=BYTE_CODE_LOAD_ZS_FLOAT;\
	}else{\
		THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform arithmetic operation '%s %s %s'"\
				,eval_data->current_parsing_file\
				,token_operator->line\
				,i1->getConstantValueOp2ToString().c_str()\
				,ZS_STR(ARITHMETIC_OP)\
				,i2->getConstantValueOp2ToString().c_str()));\
	}


#define EVAL_OPERATION_COMPARE(__COMPARE_OP__) \
	if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
		result_op_bool=(i1->value_op2)__COMPARE_OP__(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_BOOL && i2->byte_code == BYTE_CODE_LOAD_BOOL){\
		result_op_bool=(i1->value_op2)__COMPARE_OP__(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_FLOAT){\
		result_op_float=(i1->value_op2)__COMPARE_OP__*((zs_float *)&i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
		result_op_bool=*((zs_float *)&i1->value_op2)__COMPARE_OP__(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_FLOAT){\
		result_op_bool=*((zs_float *)&i1->value_op2)__COMPARE_OP__ *((zs_float *)&i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else if( i1->byte_code == BYTE_CODE_LOAD_STRING && i2->byte_code==BYTE_CODE_LOAD_STRING){\
		result_op_bool=ZS_STRCMP(i1->getConstantValueOp2ToString().c_str(), __COMPARE_OP__ ,i2->getConstantValueOp2ToString().c_str());\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else{\
		THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform compare operation '%s %s %s'"\
			,eval_data->current_parsing_file\
			,token_operator->line\
			,i1->getConstantValueOp2ToString().c_str()\
			,ZS_STR(__COMPARE_OP__)\
			,i2->getConstantValueOp2ToString().c_str()));\
	}

#define EVAL_OPERATION_BINARY(BINARY_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){\
		result_op_zs_int=(i1->value_op2)BINARY_OP(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_ZS_INT;\
	}else{\
		THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform binary operation '%s %s %s'"\
				,eval_data->current_parsing_file\
				,token_operator->line\
				,i1->getConstantValueOp2ToString().c_str()\
				,ZS_STR(BINARY_OP)\
				,i2->getConstantValueOp2ToString().c_str()));\
	}

#define EVAL_OPERATION_LOGIC(LOGIC_OP) \
	if(i1->byte_code == BYTE_CODE_LOAD_BOOL && i2->byte_code == BYTE_CODE_LOAD_BOOL){\
		result_op_bool=(i1->value_op2)LOGIC_OP(i2->value_op2);\
		result_bc=BYTE_CODE_LOAD_BOOL;\
	}else{\
		THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform boolean operation '%s %s %s'"\
				,eval_data->current_parsing_file\
				,token_operator->line\
				,i1->getConstantValueOp2ToString().c_str()\
				,ZS_STR(LOGIC_OP)\
				,i2->getConstantValueOp2ToString().c_str()));\
	}


namespace zetscript{



	unsigned short eval_expression_load_const_byte_code_to_instruction_property(ByteCode byte_code){

		switch(byte_code){
		case BYTE_CODE_LOAD_ZS_INT:
			return INSTRUCTION_PROPERTY_ZS_INT;
			break;
		case BYTE_CODE_LOAD_BOOL:
			return INSTRUCTION_PROPERTY_BOOL;
			break;
		case BYTE_CODE_LOAD_ZS_FLOAT:
			return INSTRUCTION_PROPERTY_ZS_FLOAT;
			break;
		case BYTE_CODE_LOAD_STRING:
			return INSTRUCTION_PROPERTY_STRING;
			break;
		}

		return 0;
	}

	bool eval_expression_get_register(EvalData *eval_data, Scope *scope,EvalInstruction *i1, ByteCode & load_byte_code, zs_int & load_value_op2){
		Symbol *symbol_found=NULL;

		 if(!IS_BYTE_CODE_LOAD_VARIABLE_IMMEDIATE(i1->vm_instruction.byte_code)){
			 return false;
		 }

		 // is a type, not register. Note may be we could optimize but generally we are doing ops between registers
		 if(eval_data->script_type_factory->getScriptType(i1->symbol.name)!=NULL){
			return false;
		}

		 load_value_op2=i1->vm_instruction.value_op2;
		 load_byte_code=i1->vm_instruction.byte_code;

		if(i1->vm_instruction.byte_code == BYTE_CODE_FIND_VARIABLE){

			if((symbol_found = eval_find_local_symbol(eval_data,scope,i1->symbol.name)) != NULL){
				load_byte_code=BYTE_CODE_LOAD_LOCAL;
				load_value_op2=symbol_found->idx_position;
				if((symbol_found->properties & (SYMBOL_PROPERTY_ARG_BY_REF)) == SYMBOL_PROPERTY_ARG_BY_REF){
					load_byte_code=BYTE_CODE_LOAD_REF;
				}
			}else { // if global or not found cannot be optimized
				return false;
			}
		}else if(i1->vm_instruction.byte_code == BYTE_CODE_LOAD_THIS_VARIABLE){
			if(load_value_op2 == ZS_IDX_UNDEFINED){
				ScriptType *sc=NULL;
				if(
					   ( eval_data->current_function->script_function->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)!= 0
					&& (	scope->script_type->idx_type != IDX_TYPE_CLASS_MAIN)
					   // is function member
					){ // class members are defined as functions
					sc=scope->script_type;
				}

				if(sc != NULL){
					symbol_found = sc->getSymbolVariableMember(i1->symbol.name);
				}

				if(symbol_found == NULL){
					return false;
				}

				load_value_op2=symbol_found->idx_position;
			}

			// should assign the idx
		}

		if(load_value_op2 >= MAX_REGISTER_LENGTH){ // if number of vars is > MAX_REGISTER_LENGTH cannot be registered either
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
		zs_int result_op_zs_int=0;
		bool	result_op_bool=false;
		zs_string result_op_str="";
		zs_string str_constant_key="";
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
				EVAL_OPERATION_ARITHMETIC(+);
			}

			break;
		case BYTE_CODE_SUB:
			EVAL_OPERATION_ARITHMETIC(-);
			break;
		case BYTE_CODE_MUL:
			EVAL_OPERATION_ARITHMETIC(*);
			break;
		case BYTE_CODE_DIV:
			if(i2->value_op2==0){
				THROW_EXCEPTION(zs_strutils::format("[%s:%i] divide by 0"
						,eval_data->current_parsing_file
						,token_operator->line
				));
			}
			EVAL_OPERATION_ARITHMETIC(/);
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
			}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_INT){
				result_op_float=fmod(*((zs_float *)&i1->value_op2),(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_zs_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_ZS_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_INT && i2->byte_code == BYTE_CODE_LOAD_ZS_FLOAT){
				result_op_float=fmod(*((zs_float *)&i1->value_op2),(i2->value_op2));
				ZS_FLOAT_COPY(&result_op_zs_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_ZS_FLOAT;
			}else if(i1->byte_code == BYTE_CODE_LOAD_ZS_FLOAT && i2->byte_code == BYTE_CODE_LOAD_ZS_FLOAT){
				result_op_float=fmod(*((zs_float *)&i1->value_op2), *((zs_float *)&i2->value_op2));
				ZS_FLOAT_COPY(&result_op_zs_int,&result_op_float);
				result_bc=BYTE_CODE_LOAD_ZS_FLOAT;
			}else{
				THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform constant operation '%s % %s'"
						,eval_data->current_parsing_file
						,token_operator->line
						,i1->getConstantValueOp2ToString().c_str()
						,i2->getConstantValueOp2ToString().c_str()));
			}
			break;
		// binary ops
		case BYTE_CODE_BITWISE_AND:
			EVAL_OPERATION_BINARY(&);
			break;
		case BYTE_CODE_BITWISE_OR:
			EVAL_OPERATION_BINARY(|);
			break;
		case BYTE_CODE_BITWISE_XOR:
			EVAL_OPERATION_BINARY(^);
			break;
		case BYTE_CODE_SHL:
			EVAL_OPERATION_BINARY(<<);
			break;
		case BYTE_CODE_SHR:
			EVAL_OPERATION_BINARY(>>);
			break;

		// logic ops
		case BYTE_CODE_LOGIC_AND:
			EVAL_OPERATION_LOGIC(&&);
			break;
		case BYTE_CODE_LOGIC_OR:
			EVAL_OPERATION_LOGIC(||);
			break;
		// compare op
		case BYTE_CODE_LT:
			EVAL_OPERATION_COMPARE(<);
			break;
		case BYTE_CODE_GT:
			EVAL_OPERATION_COMPARE(>);
			break;
		case BYTE_CODE_GTE:
			EVAL_OPERATION_COMPARE(>=);
			break;
		case BYTE_CODE_LTE:
			EVAL_OPERATION_COMPARE(<=);
			break;
		case BYTE_CODE_EQU:
			EVAL_OPERATION_COMPARE(==);
			break;
		case BYTE_CODE_NOT_EQU:
			EVAL_OPERATION_COMPARE(!=);
			break;
		case BYTE_CODE_IN:
			if(INSTRUCTION_IS_ZS_INT(i1) && INSTRUCTION_IS_STRING(i2)){
				result_op_bool=strchr(i2->getConstantString().c_str(),i1->getConstantInt()) != NULL;
				result_bc=BYTE_CODE_LOAD_BOOL;
			}else if(INSTRUCTION_IS_STRING(i1) && INSTRUCTION_IS_STRING(i2)){
				result_op_bool=zs_strutils::contains(i2->getConstantString(),i1->getConstantString());
				result_bc=BYTE_CODE_LOAD_BOOL;
			}
			else{
				THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform constant operation '%s in %s'"
					,eval_data->current_parsing_file
					,token_operator->line
					,i1->getConstantValueOp2ToString().c_str()
					,i2->getConstantValueOp2ToString().c_str())
				);
			}
			break;
		default:
			//THROW_EXCEPTION(zs_strutils::format("cannot perform K '%s' not implemented",eval_operator_to_str(token_operator->operator_type)));
			THROW_EXCEPTION(zs_strutils::format("[%s:%i] I don't know how to perform constant operation '%s %s %s'"
				,eval_data->current_parsing_file
				,token_operator->line
				,i1->getConstantValueOp2ToString().c_str()
				,byte_code_to_operator_str(byte_code)
				,i2->getConstantValueOp2ToString().c_str())
			);
			break;
/*			break;
		default:
			THROW_EXCEPTION(zs_strutils::format("const operation KK '%s' not implemented",eval_operator_to_str(token_operator->operator_type)));
			break;*/
		}


		switch(result_bc){
		case BYTE_CODE_LOAD_ZS_INT:
			result_instruction=new EvalInstruction(
					result_bc
					, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
					,result_op_zs_int
			);
			break;
		case BYTE_CODE_LOAD_ZS_FLOAT:
			result_instruction=new EvalInstruction(
					result_bc
					, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
					,result_op_zs_int
			);
			break;
		case BYTE_CODE_LOAD_STRING:
			str_constant_key=zs_string("\"")+result_op_str+"\"";
			result_instruction=new EvalInstruction(
					result_bc
					, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
					,(zs_int)eval_data->zs->registerStkStringObject(str_constant_key,result_op_str)
			);
			break;
		case BYTE_CODE_LOAD_BOOL:
			result_instruction=new EvalInstruction(
					result_bc
					, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
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
			, zs_vector *eval_instructions
	){
		size_t size_instructions=eval_instructions->count;
		EvalInstruction *instruction=NULL;
		bool is_i1_K=false;
		bool is_i2_K=false;
		bool is_i1_R=false;
		bool is_i2_R=false;
		int n_eval_ops=0;

		if(size_instructions < 2){
			return NULL;
		}

		EvalInstruction *i1=(EvalInstruction *)eval_instructions->items[size_instructions-2];
		EvalInstruction *i2=(EvalInstruction *)eval_instructions->items[size_instructions-1];

		is_i1_K=i1->vm_instruction.isConstant();
		is_i2_K=i2->vm_instruction.isConstant();

		// can be reduce the number of bytes codes down to 2, yeah!
		if(token_operation->operator_type == OPERATOR_INSTANCEOF){
			instruction=new EvalInstruction(
					BYTE_CODE_INSTANCEOF
					, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
					,i2->vm_instruction.value_op2
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
			}else{ // check last instructions
				i1=i2;
				n_eval_ops=1;
				if(is_i2_K){
					if((k_properties=eval_expression_load_const_byte_code_to_instruction_property(load_byte_code_1))==0){
						return NULL;
					}

					instruction=new EvalInstruction(
							byte_code
							, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
							, load_value_op2_2
							, INSTRUCTION_PROPERTY_ILOAD_K |k_properties
					);

				}else if(is_i2_R){

					instruction=new EvalInstruction(
							byte_code
							,load_value_op2_2
							,ZS_IDX_UNDEFINED
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
		eval_instructions->resize(eval_instructions->count-n_eval_ops);

		// and push the new one
		return instruction;

	}
}

