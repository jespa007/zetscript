/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{

	#pragma pack(push, 1)

	struct Instruction {

		ByteCode byte_code_op;
		unsigned char op1_value;
		intptr_t op2_value;
		unsigned short properties;

		Instruction(ByteCode _op_code
		 ,unsigned char _index_op1=ZS_UNDEFINED_IDX
		 ,intptr_t _index_op2=ZS_UNDEFINED_IDX
		 ,unsigned short _properties=0
		 );

	};

	#pragma pack(pop)

	typedef Instruction *PtrInstruction;

}
