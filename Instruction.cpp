#include "ZetScript.h"

namespace zetscript{

		Instruction::Instruction(ByteCode _op_code
					 ,unsigned char _index_op1
					 ,intptr_t _index_op2
					 ,unsigned short _properties
					 ){
			byte_code_op=_op_code;
			op1_value=_index_op1;
			op2_value=_index_op2;
			properties=_properties;

		}

}
