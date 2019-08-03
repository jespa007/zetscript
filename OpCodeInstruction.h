#pragma once

namespace zetscript{

	typedef enum:char{

		// ARITMETHIC OPERATORS.

		INVALID_OP=-1,
		END_FUNCTION = 0,
		LOAD, // primitive value like number/std::string or boolean...
		VGET, // std::vector access after each index is processed...
		STORE, // mov expression to var
		VPUSH, // Value push for std::vector
		PUSH_ATTR,
		EQU,  // ==
		NOT_EQU,  // !=
		LT,  // <
		LTE,  // <=
		GT,  // >
		GTE, // >=
		LOGIC_AND, // &&
		LOGIC_OR,  // ||
		NOT, // !
		NEG, // -a
		ADD, // +
		MUL, // *
		DIV, // /
		MOD,  // %
		AND, // bitwise logic and
		OR, // bitwise logic or
		XOR, // logic xor
		SHL, // shift left
		SHR, // shift right
		JMP,
		INSTANCEOF,
		JNT, // goto if not true ... goes end to conditional.
		JT, // goto if true ... goes end to conditional.
		CALL, // call function...
		NEW, // new operator...
		DELETE_OP,
		DECL_VEC, // Vector object
		DECL_STRUCT,
		RET, // ret instruction ..
		PUSH_SCOPE,
		POP_SCOPE,
		IT_INI,
		IT_SET_AND_NEXT,
		IT_CHK_END,
		MAX_OP_CODES

	}OP_CODE;


	typedef enum:char {
			EQU_METAMETHOD=0,  // ==
			NOT_EQU_METAMETHOD,  // !=
			LT_METAMETHOD,  // <
			LTE_METAMETHOD,  // <=
			NOT_METAMETHOD, // !
			GT_METAMETHOD,  // >
			GTE_METAMETHOD, // >=
			NEG_METAMETHOD, // -a
			ADD_METAMETHOD, // +
			DIV_METAMETHOD, // /
			MUL_METAMETHOD, // *
			MOD_METAMETHOD,  // %
			AND_METAMETHOD, // bitwise logic and
			OR_METAMETHOD, // bitwise logic or
			XOR_METAMETHOD, // logic xor
			SHL_METAMETHOD, // shift left
			SHR_METAMETHOD, // shift right
			SET_METAMETHOD, // store '='

			MAX_METAMETHOD_OPERATORS
	}METAMETHOD_OPERATOR;

	struct OpCodeInstruction {
		OP_CODE op_code;
		unsigned char index_op1;	// left and right respectively
		intptr_t index_op2;
		unsigned short properties;
		//short idxAstNode; // define ast node for give some information at run time
		OpCodeInstruction(OP_CODE _op_code
		 ,unsigned char _index_op1=ZS_UNDEFINED_IDX
		 ,intptr_t _index_op2=ZS_UNDEFINED_IDX
		 ,unsigned short _properties=0
		 );
	};

	const char * op_2_str(OP_CODE  op);
	const char * metamethod_op_2_str(METAMETHOD_OPERATOR op);



	typedef OpCodeInstruction *PtrInstruction;
}
