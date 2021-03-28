/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once



namespace zetscript{

	typedef enum:unsigned char{

		// ARITMETHIC OPERATORS.
		BYTE_CODE_END_FUNCTION = 0,
		BYTE_CODE_FIND_VARIABLE,

		// load ptr var
		BYTE_CODE_PUSH_STK_GLOBAL,
		BYTE_CODE_PUSH_STK_LOCAL,
		BYTE_CODE_PUSH_STK_REF,
		BYTE_CODE_PUSH_STK_THIS,
		BYTE_CODE_PUSH_STK_MEMBER_VAR,
		BYTE_CODE_PUSH_STK_ELEMENT_VECTOR,
		BYTE_CODE_PUSH_STK_ELEMENT_THIS,
		BYTE_CODE_PUSH_STK_ELEMENT_OBJECT,


		// load var content
		BYTE_CODE_LOAD_GLOBAL,
		BYTE_CODE_LOAD_LOCAL,
		BYTE_CODE_LOAD_REF,
		BYTE_CODE_LOAD_THIS,
		BYTE_CODE_LOAD_MEMBER_VAR,
		BYTE_CODE_LOAD_ELEMENT_VECTOR,
		BYTE_CODE_LOAD_ELEMENT_THIS,
		BYTE_CODE_LOAD_ELEMENT_OBJECT,

		// special load
		BYTE_CODE_LOAD_CONSTRUCTOR,

		// load constants
		BYTE_CODE_LOAD_FUNCTION,
		BYTE_CODE_LOAD_NULL,
		BYTE_CODE_LOAD_STRING,
		BYTE_CODE_LOAD_FLOAT,
		BYTE_CODE_LOAD_BOOL,
		BYTE_CODE_LOAD_ZS_INT,
		BYTE_CODE_LOAD_CLASS,
		BYTE_CODE_LOAD_STACK_ELEMENT,


		BYTE_CODE_STORE_CONST, // it stores and set stack only-read
		BYTE_CODE_STORE, 	 // mov expression to var ( > 1 multiple)
		BYTE_CODE_STORE_ADD, //
		BYTE_CODE_STORE_SUB, //
		BYTE_CODE_STORE_MUL, //
		BYTE_CODE_STORE_DIV, //
		BYTE_CODE_STORE_MOD, //
		BYTE_CODE_STORE_BITWISE_AND, //
		BYTE_CODE_STORE_BITWISE_OR,  //
		BYTE_CODE_STORE_BITWISE_XOR, //
		BYTE_CODE_STORE_SHL, //
		BYTE_CODE_STORE_SHR, //
		BYTE_CODE_PUSH_VECTOR_ELEMENT, // Value push for std::vector
		BYTE_CODE_PUSH_OBJECT_ELEMENT,
		//-------------------------------
		// OPERATIONS
		//
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
		BYTE_CODE_SUB, // -
		BYTE_CODE_MUL, // *
		BYTE_CODE_DIV, // /
		BYTE_CODE_MOD,  // %
		BYTE_CODE_BITWISE_AND, // binary and
		BYTE_CODE_BITWISE_OR, // binary or
		BYTE_CODE_BITWISE_XOR, // binary xor
		BYTE_CODE_SHL, // binary shift left
		BYTE_CODE_SHR, // binary shift right
		//
		// OPERATIONS
		//-------------------------------------
		BYTE_CODE_INSTANCEOF,
		BYTE_CODE_JMP,
		BYTE_CODE_JNT, // goto if not true ... goes end to conditional.
		BYTE_CODE_JE, //
		BYTE_CODE_JT, // goto if true ... goes end to conditional.
		BYTE_CODE_CALL, // call function...
		BYTE_CODE_CALL_CONSTRUCTOR, // call function...
		BYTE_CODE_NEW_CLASS, // new operator...
		BYTE_CODE_DELETE,
		BYTE_CODE_NEW_VECTOR, // Vector object
		BYTE_CODE_NEW_OBJECT,
		BYTE_CODE_NEW_STRING,
		BYTE_CODE_RET, // ret instruction ..
		BYTE_CODE_PUSH_SCOPE,
		BYTE_CODE_POP_SCOPE,
		BYTE_CODE_IT_NEXT,
		BYTE_CODE_IT_END,
		BYTE_CODE_PRE_INC,
		BYTE_CODE_PRE_DEC,
		BYTE_CODE_POST_INC,
		BYTE_CODE_NEG_POST_INC,
		BYTE_CODE_POST_DEC,
		BYTE_CODE_NEG_POST_DEC,
		BYTE_CODE_RESET_STACK, // just in case there's no assignment on left or after last assignment
		//---------------------------
		BYTE_CODE_INVALID=255,

	}ByteCode;

	typedef enum:unsigned char {
		BYTE_CODE_METAMETHOD_EQU=0,  // ==
		BYTE_CODE_METAMETHOD_NOT_EQU,  // !=
		BYTE_CODE_METAMETHOD_LT,  // <
		BYTE_CODE_METAMETHOD_LTE,  // <=
		BYTE_CODE_METAMETHOD_NOT, // !
		BYTE_CODE_METAMETHOD_GT,  // >
		BYTE_CODE_METAMETHOD_GTE, // >=
		BYTE_CODE_METAMETHOD_NEG, // -a
		BYTE_CODE_METAMETHOD_ADD, // +
		BYTE_CODE_METAMETHOD_SUB, // -
		BYTE_CODE_METAMETHOD_DIV, // /
		BYTE_CODE_METAMETHOD_MUL, // *
		BYTE_CODE_METAMETHOD_MOD,  // %
		BYTE_CODE_METAMETHOD_AND, // bitwise logic and
		BYTE_CODE_METAMETHOD_OR, // bitwise logic or
		BYTE_CODE_METAMETHOD_XOR, // logic xor
		BYTE_CODE_METAMETHOD_SHL, // shift left
		BYTE_CODE_METAMETHOD_SHR, // shift right
		BYTE_CODE_METAMETHOD_SET, // store '='
		BYTE_CODE_METAMETHOD_TO_STRING, // toString
		BYTE_CODE_METAMETHOD_ITER, // _iter
		BYTE_CODE_METAMETHOD_NEXT, // _next
		BYTE_CODE_METAMETHOD_END, // _end
		BYTE_CODE_METAMETHOD_GET, // _getter
		BYTE_CODE_METAMETHOD_EXIST, // _exist
		BYTE_CODE_METAMETHOD_MAX
	}ByteCodeMetamethod;

#define IS_BYTE_CODE_LOAD_VARIABLE_TYPE(byte_code) \
(\
  ((byte_code)==ByteCode::BYTE_CODE_FIND_VARIABLE)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_GLOBAL)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_LOCAL)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_THIS)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_MEMBER_VAR)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_ELEMENT_VECTOR)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_ELEMENT_THIS)\
||((byte_code)==ByteCode::BYTE_CODE_LOAD_ELEMENT_OBJECT)\
)

#define IS_BYTE_CODE_LOAD_CONSTANT(byte_code) \
(\
   ((byte_code)== BYTE_CODE_LOAD_BOOL) \
|| ((byte_code)== BYTE_CODE_LOAD_ZS_INT) \
|| ((byte_code)== BYTE_CODE_LOAD_FLOAT) \
|| ((byte_code)== BYTE_CODE_LOAD_STRING) \
)

#define IS_BYTE_CODE_LOAD_VARIABLE_IMMEDIATE(byte_code) \
(\
   ((byte_code)== BYTE_CODE_LOAD_LOCAL) \
|| ((byte_code)== BYTE_CODE_FIND_VARIABLE) \
|| ((byte_code)== BYTE_CODE_LOAD_GLOBAL)\
|| ((byte_code)== BYTE_CODE_LOAD_MEMBER_VAR)\
)

#define IS_BYTE_CODE_OPERATION(byte_code) 		(BYTE_CODE_EQU <=(byte_code) && (byte_code)<= BYTE_CODE_SHR)

#define IS_BYTE_CODE_STORE_WITH_OPERATION(b) 	(ByteCode::BYTE_CODE_STORE_ADD<=(b) && (b) <=ByteCode::BYTE_CODE_STORE_SHR)
//#define IS_BYTE_CODE_STORE(b) 					(IS_BYTE_CODE_STORE_WITH_OPERATION(b) || ((b)==ByteCode::BYTE_CODE_STORE))



	const char * byte_code_to_str(ByteCode  op);
	const char * byte_code_to_operator_str(ByteCode op);
	const char * byte_code_metamethod_to_operator_str(ByteCodeMetamethod op);
	const char * byte_code_metamethod_to_symbol_str(ByteCodeMetamethod op);
	int			 byte_code_get_num_arguments_static_metamethod(ByteCodeMetamethod op);
	int			 get_num_arguments_non_static_metamethod(ByteCodeMetamethod op);
	ByteCode	 byte_code_load_to_push_stk(ByteCode op);
	bool		 byte_code_is_load_type(ByteCode op);


}
