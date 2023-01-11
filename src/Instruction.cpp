/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
		Instruction::Instruction(){
			byte_code=ByteCode::BYTE_CODE_INVALID;
			value_op1= ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED;
			value_op2=ZS_IDX_UNDEFINED;
			properties=0;
		}

		Instruction::Instruction(ByteCode _byte_code
		 ,uint8_t _value_op1
		 ,zs_int _value_op2
		 ,unsigned short _properties
		 ){
			byte_code=_byte_code;
			value_op1=_value_op1;
			value_op2=_value_op2;
			properties=_properties;
		}


		zs_string Instruction::getConstantString(){
			ScriptObject *obj=NULL;
			StackElement *stk=NULL;

			if((this->byte_code == BYTE_CODE_LOAD_STRING)
					|| (this->properties & INSTRUCTION_PROPERTY_STRING)
					|| (byte_code == BYTE_CODE_INSTANCEOF)
			){

				stk=(StackElement *)this->value_op2;
				obj = (ScriptObject *)stk->value;

			}else{
				ZS_THROW_EXCEPTION("instruction is not constant string");
			}

			return obj->toString();
		}

		zs_float Instruction::getConstantFloat(){

			if(((this->byte_code == BYTE_CODE_LOAD_ZS_FLOAT) || (this->properties & INSTRUCTION_PROPERTY_ZS_FLOAT))==false){
				ZS_THROW_EXCEPTION("instruction is not constant " ZS_STR(zs_float));
			}

			return ZS_READ_INTPTR_FLOAT(this->value_op2);

		}

		zs_int Instruction::getConstantInt(){

			if(((this->byte_code == BYTE_CODE_LOAD_ZS_INT) || (this->properties & INSTRUCTION_PROPERTY_ZS_INT))==false){
				ZS_THROW_EXCEPTION("instruction is not constant " ZS_STR(zs_int));
			}

			return (zs_int)this->value_op2;

		}

		bool Instruction::isConstant(){
			return IS_BYTE_CODE_LOAD_CONSTANT(byte_code);
		}

		zs_string Instruction::getConstantValueOp2ToString( bool _str_with_quotes){
			zs_string value_op2_string="unknow-value";

			if(byte_code ==BYTE_CODE_LOAD_BOOL || (this->properties & INSTRUCTION_PROPERTY_BOOL) ){
				value_op2_string=this->value_op2 == 0 ? "false":"true";
			}else if(byte_code==BYTE_CODE_LOAD_ZS_INT || (this->properties & INSTRUCTION_PROPERTY_ZS_INT)){
				value_op2_string=zs_strutils::zs_int_to_str(this->value_op2);
			}else if(byte_code==BYTE_CODE_LOAD_ZS_FLOAT|| (this->properties & INSTRUCTION_PROPERTY_ZS_FLOAT)){
				value_op2_string=zs_strutils::zs_float_to_str(this->getConstantFloat());
			}else if(byte_code==BYTE_CODE_LOAD_STRING || (this->properties & INSTRUCTION_PROPERTY_STRING)){
				if(_str_with_quotes){
					value_op2_string="\""+this->getConstantString()+"\"";
				}else{
					value_op2_string=this->getConstantString();
				}
			}

			return value_op2_string;
		}

		int instruction_num_required_stack(Instruction *_instruction){
			uint16_t properties = _instruction->properties;

			if(properties & INSTRUCTION_PROPERTY_ILOAD){

				int num_stack=1;
				switch(properties & INSTRUCTION_PROPERTY_ILOAD){
				default:
				case INSTRUCTION_PROPERTY_ILOAD_R:
				case INSTRUCTION_PROPERTY_ILOAD_KR:
				case INSTRUCTION_PROPERTY_ILOAD_RK:
					break;
				case INSTRUCTION_PROPERTY_ILOAD_RR:
					if((properties & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR)
							&&
						(_instruction->value_op2 & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR))
					{
						num_stack=2;
					}
					break;
				}

				return num_stack;
			}
			else{
				switch(_instruction->byte_code){
				// 2 ops
				case BYTE_CODE_STORE_CONST:
				case BYTE_CODE_PUSH_OBJECT_ITEM:
					return -2;
				// pop -2 and push stk +1 = 0
				case BYTE_CODE_EQU:
				case BYTE_CODE_NOT_EQU:
				case BYTE_CODE_LT:
				case BYTE_CODE_LTE:
				case BYTE_CODE_GT:
				case BYTE_CODE_GTE:
				case BYTE_CODE_ADD:
				case BYTE_CODE_SUB:
				case BYTE_CODE_LOGIC_AND:
				case BYTE_CODE_LOGIC_OR:
				case BYTE_CODE_DIV:
				case BYTE_CODE_MUL:
				case BYTE_CODE_MOD:
				case BYTE_CODE_BITWISE_AND:
				case BYTE_CODE_BITWISE_OR:
				case BYTE_CODE_BITWISE_XOR:
				case BYTE_CODE_SHL:
				case BYTE_CODE_SHR:
				case BYTE_CODE_PUSH_VECTOR_ITEM:
				// byte_code_XXX_store, it pops -2 and push +1 = -1
				case BYTE_CODE_ADD_STORE:
				case BYTE_CODE_SUB_STORE:
				case BYTE_CODE_MUL_STORE:
				case BYTE_CODE_DIV_STORE:
				case BYTE_CODE_MOD_STORE:
				case BYTE_CODE_BITWISE_AND_STORE:
				case BYTE_CODE_BITWISE_OR_STORE:
				case BYTE_CODE_BITWISE_XOR_STORE:
				case BYTE_CODE_SHL_STORE:
				case BYTE_CODE_SHR_STORE:
					return -1;
				// pop -1 and push stk +0 = -1
				case BYTE_CODE_JNT:
				case BYTE_CODE_JT:
				case BYTE_CODE_JE_CASE:
					return -1;
				// pop -1 and push stk +1 = 0
				case BYTE_CODE_NEG:
				case BYTE_CODE_BWC:
				case BYTE_CODE_NOT:			// pop -1 and push stk +1 = 0
				case BYTE_CODE_RET:
				case BYTE_CODE_JMP:
				case BYTE_CODE_JMP_CASE:
				case BYTE_CODE_DELETE:
				case BYTE_CODE_POP_SCOPE:
				case BYTE_CODE_PUSH_SCOPE:
				case BYTE_CODE_IT_INIT:
				case BYTE_CODE_PRE_INC:			// pop -1 and stk +1 = 0
				case BYTE_CODE_PRE_DEC:			// pop -1 and stk +1 = 0
				case BYTE_CODE_POST_INC:		// pop -1 and stk +1 = 0
				case BYTE_CODE_NEG_POST_INC: 	// pop -1 and stk +1 = 0
				case BYTE_CODE_BWC_POST_INC: 	// pop -1 and stk +1 = 0
				case BYTE_CODE_POST_DEC:		// pop -1 and stk +1 = 0
				case BYTE_CODE_NEG_POST_DEC:	// pop -1 and stk +1 = 0
				case BYTE_CODE_BWC_POST_DEC:	// pop -1 and stk +1 = 0
				case BYTE_CODE_RESET_STACK:		// pop -1 and stk +1 = 0
				//case BYTE_CODE_CLEAR_ZERO_POINTERS:
				case BYTE_CODE_LOAD_OBJECT_ITEM:
					return 0;
				case BYTE_CODE_STORE:
					return -_instruction->value_op1;
				case BYTE_CODE_FIND_VARIABLE:
				case BYTE_CODE_INSTANCEOF:
				case BYTE_CODE_PUSH_STK_GLOBAL:
				case BYTE_CODE_PUSH_STK_GLOBAL_IRGO:
				case BYTE_CODE_PUSH_STK_LOCAL:
				case BYTE_CODE_PUSH_STK_THIS:
				case BYTE_CODE_PUSH_STK_VECTOR_ITEM:
				case BYTE_CODE_PUSH_STK_OBJECT_ITEM:
				case BYTE_CODE_PUSH_STK_THIS_VARIABLE:
				case BYTE_CODE_LOAD_GLOBAL:
				case BYTE_CODE_LOAD_LOCAL:
				case BYTE_CODE_LOAD_REF:
				case BYTE_CODE_LOAD_THIS:
				case BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:
				case BYTE_CODE_LOAD_THIS_VARIABLE:
				case BYTE_CODE_LOAD_VECTOR_ITEM:
				case BYTE_CODE_LOAD_FUNCTION:
				case BYTE_CODE_LOAD_UNDEFINED:
				case BYTE_CODE_LOAD_NULL:
				case BYTE_CODE_LOAD_STACK_ELEMENT:
				case BYTE_CODE_LOAD_STRING:
				case BYTE_CODE_LOAD_ZS_FLOAT:
				case BYTE_CODE_LOAD_BOOL:
				case BYTE_CODE_LOAD_ZS_INT:
				case BYTE_CODE_LOAD_TYPE:
				case BYTE_CODE_NEW_VECTOR:
				case BYTE_CODE_NEW_OBJECT_BY_TYPE:
				case BYTE_CODE_NEW_OBJECT_BY_VALUE:
				case BYTE_CODE_NEW_OBJECT:
				case BYTE_CODE_NEW_STRING:
				case BYTE_CODE_TYPEOF:
				case BYTE_CODE_IN:
					return 1;
				case BYTE_CODE_LOAD_THIS_FUNCTION:
					return 2;
				case BYTE_CODE_STACK_CALL:
				case BYTE_CODE_INDIRECT_LOCAL_CALL:
				case BYTE_CODE_INDIRECT_GLOBAL_CALL:
				case BYTE_CODE_THIS_CALL:
				case BYTE_CODE_SUPER_CALL:
				case BYTE_CODE_INDIRECT_THIS_CALL:
				case BYTE_CODE_CALL:
				case BYTE_CODE_UNRESOLVED_CALL:
					return INSTRUCTION_GET_RETURN_COUNT(_instruction)-(INSTRUCTION_GET_PARAMETER_COUNT(_instruction)+0);
				 case  BYTE_CODE_CONSTRUCTOR_CALL:
				 case  BYTE_CODE_MEMBER_CALL: // calling function after all of args are processed...
					 // +1 to pop MemberFunctionScriptObject or MemberFunction for CONSTRUCTOR
					return INSTRUCTION_GET_RETURN_COUNT(_instruction)-(INSTRUCTION_GET_PARAMETER_COUNT(_instruction)+1);
				default:
					
					break;
				}
			}

			return ZS_NUM_REQUIRED_BYTE_CODE_NOT_MANAGED;
		}



}
