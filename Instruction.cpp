#include "zetscript.h"

namespace zetscript{
		Instruction::Instruction(){
			byte_code=ByteCode::BYTE_CODE_INVALID;
			value_op1=ZS_IDX_UNDEFINED;
			value_op2=ZS_IDX_UNDEFINED;
			properties=0;
		}

		Instruction::Instruction(ByteCode _byte_code
		 ,unsigned char _value_op1=ZS_IDX_UNDEFINED
		 ,zs_int _value_op2=ZS_IDX_UNDEFINED
		 ,unsigned short _properties=0
		 ){
			byte_code=_byte_code;
			value_op1=_value_op1;
			value_op2=_value_op2;
			properties=_properties;
		}


		std::string Instruction::getConstantString(){
			ScriptObjectString *obj=NULL;
			StackElement *stk=NULL;

			if(this->byte_code != BYTE_CODE_LOAD_STRING){
				THROW_EXCEPTION("instruction is not constant string");
			}

			stk=(StackElement *)this->value_op2;
			obj = (ScriptObject *)stk->stk_value;

			return obj->toString();

		}

		float Instruction::getConstantFloat(){

			if(this->byte_code != BYTE_CODE_LOAD_FLOAT){
				THROW_EXCEPTION("instruction is not constant float");
			}
			return *((float *)&this->value_op2);
		}

		bool Instruction::isConstant(){
			return IS_BYTE_CODE_LOAD_CONSTANT(byte_code);
		}

		std::string Instruction::getConstantValueOp2ToString(){
			std::string value_op2_string="unknow value";


			switch(byte_code){
			case BYTE_CODE_LOAD_BOOL:
				value_op2_string=this->value_op2 == 0 ? "false":"true";
				break;
			case BYTE_CODE_LOAD_ZS_INT:
				value_op2_string=zs_strutils::zs_int_to_str(this->value_op2);
				break;
			case BYTE_CODE_LOAD_FLOAT:
				value_op2_string=zs_strutils::float_to_str(this->getConstantFloat());
				break;
			case BYTE_CODE_LOAD_STRING:
				value_op2_string=this->getConstantString();
				break;

			}

			return value_op2_string;
		}

}
