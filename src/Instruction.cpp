/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
		Instruction::Instruction(){
			byte_code=ByteCode::ByteCodeId::BYTE_CODE_ID_INVALID;
			value_op1= ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED;
			value_op2=ZS_UNDEFINED_IDX;
			properties=0;
		}

		Instruction::Instruction(ByteCode::ByteCodeId _byte_code
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

			if((this->byte_code == ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING)
					|| (this->properties & ZS_INSTRUCTION_PROPERTY_STRING)
					|| (byte_code == ByteCode::ByteCodeId::BYTE_CODE_ID_INSTANCEOF)
			){

				stk=(StackElement *)this->value_op2;
				obj = (ScriptObject *)stk->value;

			}else{
				ZS_THROW_EXCEPTION("instruction is not constant string");
			}

			return obj->toString();
		}

		zs_float Instruction::getConstantFloat(){

			if(((this->byte_code == ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FLOAT) || (this->properties & ZS_INSTRUCTION_PROPERTY_FLOAT))==false){
				ZS_THROW_EXCEPTION("instruction is not constant " ZS_STR(zs_float));
			}

			return ZS_READ_INTPTR_FLOAT(this->value_op2);

		}

		zs_int Instruction::getConstantInt(){

			if(((this->byte_code == ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT) || (this->properties & ZS_INSTRUCTION_PROPERTY_INT))==false){
				ZS_THROW_EXCEPTION("instruction is not constant " ZS_STR(zs_int));
			}

			return (zs_int)this->value_op2;

		}

		bool Instruction::isConstant(){
			return ZS_IS_BYTE_CODE_ID_LOAD_CONSTANT(byte_code);
		}

		zs_string Instruction::getConstantValueOp2ToString( bool _str_with_quotes){
			zs_string value_op2_string="unknow-value";

			if(byte_code ==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_BOOL || (this->properties & ZS_INSTRUCTION_PROPERTY_BOOL) ){
				value_op2_string=this->value_op2 == 0 ? "false":"true";
			}else if(byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT || (this->properties & ZS_INSTRUCTION_PROPERTY_INT)){
				value_op2_string=zs_strutils::zs_int_to_str(this->value_op2);
			}else if(byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FLOAT|| (this->properties & ZS_INSTRUCTION_PROPERTY_FLOAT)){
				value_op2_string=zs_strutils::zs_float_to_str(this->getConstantFloat());
			}else if(byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING || (this->properties & ZS_INSTRUCTION_PROPERTY_STRING)){
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

			if(properties & ZS_INSTRUCTION_PROPERTY_ILOAD){

				int num_stack=1;
				switch(properties & ZS_INSTRUCTION_PROPERTY_ILOAD){
				default:
				case ZS_INSTRUCTION_PROPERTY_ILOAD_R:
				case ZS_INSTRUCTION_PROPERTY_ILOAD_KR:
				case ZS_INSTRUCTION_PROPERTY_ILOAD_RK:
					break;
				case ZS_INSTRUCTION_PROPERTY_ILOAD_RR:
					if((properties & ZS_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR)
							&&
						(_instruction->value_op2 & ZS_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR))
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
				case ByteCode::ByteCodeId::BYTE_CODE_ID_STORE_CONST:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_OBJECT_ITEM:
					return -2;
				// pop -2 and push stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_EQU:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NOT_EQU:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LTE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_GT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_GTE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_ADD:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SUB:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_AND:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_OR:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_DIV:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_MUL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_MOD:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SHL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SHR:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_ARRAY_ITEM:
				// byte_code_XXX_store, it pops -2 and push +1 = -1
				case ByteCode::ByteCodeId::BYTE_CODE_ID_ADD_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SUB_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_MUL_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_DIV_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_MOD_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SHL_ASSIGN:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SHR_ASSIGN:
					return -1;
				// pop -1 and push stk +0 = -1
				case ByteCode::ByteCodeId::BYTE_CODE_ID_JNT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_JT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_JE_CASE:
					return -1;
				// pop -1 and push stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NOT:			// pop -1 and push stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_RET:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_JMP:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_JMP_CASE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_DELETE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_POP_SCOPE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_SCOPE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_IT_INIT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_INC:			// pop -1 and stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_DEC:			// pop -1 and stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_POST_INC:		// pop -1 and stk +1 = 0
				//case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG_POST_INC: 	// pop -1 and stk +1 = 0
				//case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC_POST_INC: 	// pop -1 and stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_POST_DEC:		// pop -1 and stk +1 = 0
				//case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG_POST_DEC:	// pop -1 and stk +1 = 0
				//case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC_POST_DEC:	// pop -1 and stk +1 = 0
				case ByteCode::ByteCodeId::BYTE_CODE_ID_RESET_STACK:		// pop -1 and stk +1 = 0
				//case ByteCode::ByteCodeId::BYTE_CODE_ID_CLEAR_ZERO_POINTERS:
					return 0;
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_OBJECT_ITEM:
					// If load item is for a calling member it does pop-1 and push+2 due it needs to push function + object
					if((_instruction->properties & ZS_INSTRUCTION_PROPERTY_CALLING_FUNCTION)!=0){
						return 1;
					}
					// else it will create object function member
					return 0;
				case ByteCode::ByteCodeId::BYTE_CODE_ID_STORE:
					return -_instruction->value_op1;
				case ByteCode::ByteCodeId::BYTE_CODE_ID_FIND_VARIABLE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_INSTANCEOF:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_GLOBAL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_GLOBAL_IRGO:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_LOCAL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_ARRAY_ITEM:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_OBJECT_ITEM:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS_VARIABLE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_GLOBAL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_LOCAL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_REF:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_CONSTRUCTOR_FUNCT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_VARIABLE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_ARRAY_ITEM:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FUNCTION:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_UNDEFINED:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_NULL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STACK_ELEMENT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FLOAT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_BOOL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_TYPE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_ARRAY:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT_BY_TYPE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT_BY_VALUE:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_STRING:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_TYPEOF:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_IN:
					return 1;
				case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_FUNCTION:
					return 2;
				case ByteCode::ByteCodeId::BYTE_CODE_ID_STACK_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_INDIRECT_LOCAL_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_INDIRECT_GLOBAL_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_THIS_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_SUPER_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_INDIRECT_THIS_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_CALL:
				case ByteCode::ByteCodeId::BYTE_CODE_ID_UNRESOLVED_CALL:
					return ZS_INSTRUCTION_GET_RETURN_COUNT(_instruction)-(ZS_INSTRUCTION_GET_PARAMETER_COUNT(_instruction)+0);
				 case  ByteCode::ByteCodeId::BYTE_CODE_ID_CONSTRUCTOR_CALL:
				 case  ByteCode::ByteCodeId::BYTE_CODE_ID_MEMBER_CALL:
					 // +1 to pop MemberFunctionScriptObject or MemberFunction for CONSTRUCTOR
					return ZS_INSTRUCTION_GET_RETURN_COUNT(_instruction)-(ZS_INSTRUCTION_GET_PARAMETER_COUNT(_instruction)+1);
				default:
					
					break;
				}
			}

			return ZS_NUM_REQUIRED_BYTE_CODE_ID_NOT_MANAGED;
		}



}
