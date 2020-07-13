/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{

	typedef enum:char{

		// ARITMETHIC OPERATORS.

		BYTE_CODE_INVALID=-1,
		BYTE_CODE_END_FUNCTION = 0,
		BYTE_CODE_LOAD, // primitive value like number/std::string or boolean...
		BYTE_CODE_VGET, // std::vector access after each index is processed...
		BYTE_CODE_STORE, // mov expression to var
		BYTE_CODE_VPUSH, // Value push for std::vector
		BYTE_CODE_PUSH_ATTR,
		BYTE_CODE_EQU,  // ==
		BYTE_CODE_NOT_EQU,  // !=
		BYTE_CODE_LT,  // <
		BYTE_CODE_LTE,  // <=
		BYTE_CODE_GT,  // >
		BYTE_CODE_GTE, // >=
		BYTE_CODE_LOGIC_AND, // &&
		BYTE_CODE_LOGIC_OR,  // ||
		BYTE_CODE_NOT, // !
		BYTE_CODE_NEG, // -a
		BYTE_CODE_ADD, // +
		BYTE_CODE_MUL, // *
		BYTE_CODE_DIV, // /
		BYTE_CODE_MOD,  // %
		BYTE_CODE_AND, // bitwise logic and
		BYTE_CODE_OR, // bitwise logic or
		BYTE_CODE_XOR, // logic xor
		BYTE_CODE_SHL, // shift left
		BYTE_CODE_SHR, // shift right
		BYTE_CODE_JMP,
		BYTE_CODE_INSTANCEOF,
		BYTE_CODE_JNT, // goto if not true ... goes end to conditional.
		BYTE_CODE_JT, // goto if true ... goes end to conditional.
		BYTE_CODE_CALL, // call function...
		BYTE_CODE_NEW, // new operator...
		BYTE_CODE_DELETE,
		BYTE_CODE_DECL_VEC, // Vector object
		BYTE_CODE_DECL_DICTIONARY,
		BYTE_CODE_RET, // ret instruction ..
		BYTE_CODE_PUSH_SCOPE,
		BYTE_CODE_POP_SCOPE,
		BYTE_CODE_IT_INI,
		BYTE_CODE_SET_AND_NEXT,
		BYTE_CODE_IT_CHK_END,
		BYTE_CODE_MAX

	}ByteCode;


	typedef enum:char {
			BYTE_CODE_METAMETHOD_EQU=0,  // ==
			BYTE_CODE_METAMETHOD_NOT_EQU,  // !=
			BYTE_CODE_METAMETHOD_LT,  // <
			BYTE_CODE_METAMETHOD_LTE,  // <=
			BYTE_CODE_METAMETHOD_NOT, // !
			BYTE_CODE_METAMETHOD_GT,  // >
			BYTE_CODE_METAMETHOD_GTE, // >=
			BYTE_CODE_METAMETHOD_NEG, // -a
			BYTE_CODE_METAMETHOD_ADD, // +
			BYTE_CODE_METAMETHOD_DIV, // /
			BYTE_CODE_METAMETHOD_MUL, // *
			BYTE_CODE_METAMETHOD_MOD,  // %
			BYTE_CODE_METAMETHOD_AND, // bitwise logic and
			BYTE_CODE_METAMETHOD_OR, // bitwise logic or
			BYTE_CODE_METAMETHOD_XOR, // logic xor
			BYTE_CODE_METAMETHOD_SHL, // shift left
			BYTE_CODE_METAMETHOD_SHR, // shift right
			BYTE_CODE_METAMETHOD_SET, // store '='

			BYTE_CODE_METAMETHOD_MAX
	}ByteCodeMetamethod;

	const char * ByteCodeToStr(ByteCode  op);
	const char * ByteCodeMetamethodToStr(ByteCodeMetamethod op);

}