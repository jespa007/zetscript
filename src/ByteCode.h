/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once


#define IS_BYTE_CODE_PUSH_STK_VARIABLE_TYPE(byte_code) \
(\
  ((byte_code)==ByteCode::BYTE_CODE_FIND_VARIABLE)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_GLOBAL)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_LOCAL)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_THIS)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_MEMBER_VAR)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_VECTOR_ITEM)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_THIS_VARIABLE)\
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_OBJECT_ITEM)\
)

#define IS_BYTE_CODE_LOAD_CONSTANT(byte_code) \
(\
   ((byte_code)== BYTE_CODE_LOAD_BOOL) \
|| ((byte_code)== BYTE_CODE_LOAD_ZS_INT) \
|| ((byte_code)== BYTE_CODE_LOAD_ZS_FLOAT) \
|| ((byte_code)== BYTE_CODE_LOAD_STRING) \
)

#define IS_BYTE_CODE_LOAD_VARIABLE_IMMEDIATE(byte_code) \
(\
   ((byte_code)== BYTE_CODE_LOAD_LOCAL) \
|| ((byte_code)== BYTE_CODE_FIND_VARIABLE) \
|| ((byte_code)== BYTE_CODE_LOAD_THIS_VARIABLE)\
)

#define IS_BYTE_CODE_OPERATION(byte_code) 		(BYTE_CODE_EQU <=(byte_code) && (byte_code)<= BYTE_CODE_SHR)

#define IS_BYTE_CODE_STORE_WITH_OPERATION(b) 	(ByteCode::BYTE_CODE_ADD_STORE<=(b) && (b) <=ByteCode::BYTE_CODE_SHR_STORE)
//#define IS_BYTE_CODE_STORE(b) 					(IS_BYTE_CODE_STORE_WITH_OPERATION(b) || ((b)==ByteCode::BYTE_CODE_STORE))

#define IS_BYTE_CODE_CALL(byte_code) \
(\
	  (BYTE_CODE_BEGIN_CALL <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=BYTE_CODE_END_CALL) \
)

#define IS_BYTE_CODE_STORE(byte_code) \
(\
	  (BYTE_CODE_BEGIN_STORE <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=BYTE_CODE_END_STORE) \
)

#define IS_BYTE_CODE_OPERATOR_METAMETHOD_WITH_TWO_ARGS()

namespace zetscript{

	typedef enum:unsigned char{

		// ARITMETHIC OPERATORS.
		BYTE_CODE_END_FUNCTION = 0,

		BYTE_CODE_FIND_VARIABLE,
		//----- BEGIN CALL
		BYTE_CODE_CALL,
		BYTE_CODE_BEGIN_CALL=BYTE_CODE_CALL,		// direct
		BYTE_CODE_INDIRECT_GLOBAL_CALL,	// through symbol
		BYTE_CODE_INDIRECT_LOCAL_CALL,	// through symbol
		BYTE_CODE_THIS_CALL,	// this direct
		BYTE_CODE_THIS_MEMBER_CALL,
		BYTE_CODE_MEMBER_CALL, // through function and member loaded
		BYTE_CODE_CONSTRUCTOR_CALL, // constructor...
		BYTE_CODE_END_CALL=BYTE_CODE_CONSTRUCTOR_CALL,
		//----- END CALL

		BYTE_CODE_UNRESOLVED_CALL,
		BYTE_CODE_UNRESOLVED_THIS_CALL,

		// load ptr var
		BYTE_CODE_PUSH_STK_GLOBAL,
		BYTE_CODE_PUSH_STK_LOCAL,
		//BYTE_CODE_PUSH_STK_REF,
		BYTE_CODE_PUSH_STK_THIS,
		BYTE_CODE_PUSH_STK_MEMBER_VAR,
		BYTE_CODE_PUSH_STK_VECTOR_ITEM,
		BYTE_CODE_PUSH_STK_THIS_VARIABLE,
		BYTE_CODE_PUSH_STK_THIS_FUNCTION,
		BYTE_CODE_PUSH_STK_OBJECT_ITEM,


		// load var content
		BYTE_CODE_LOAD_TYPE,
		BYTE_CODE_LOAD_GLOBAL,
		BYTE_CODE_LOAD_LOCAL,
		BYTE_CODE_LOAD_REF,
		BYTE_CODE_LOAD_THIS,
		BYTE_CODE_LOAD_VECTOR_ITEM,
		BYTE_CODE_LOAD_THIS_VARIABLE,
		BYTE_CODE_LOAD_THIS_FUNCTION,
		BYTE_CODE_LOAD_OBJECT_ITEM,

		// special load
		BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT,

		// load constants
		BYTE_CODE_LOAD_FUNCTION,
		BYTE_CODE_LOAD_NULL,
		BYTE_CODE_LOAD_STRING,
		BYTE_CODE_LOAD_ZS_FLOAT,
		BYTE_CODE_LOAD_BOOL,
		BYTE_CODE_LOAD_ZS_INT,

		BYTE_CODE_LOAD_STACK_ELEMENT,

		//----- BEGIN STORE
		BYTE_CODE_BEGIN_STORE,
		BYTE_CODE_STORE_CONST=BYTE_CODE_BEGIN_STORE, // it stores and set stack only-read
		BYTE_CODE_STORE, 	 // mov expression to var ( > 1 multiple)
		BYTE_CODE_ADD_STORE, //
		BYTE_CODE_SUB_STORE, //
		BYTE_CODE_MUL_STORE, //
		BYTE_CODE_DIV_STORE, //
		BYTE_CODE_MOD_STORE, //
		BYTE_CODE_BITWISE_AND_STORE, //
		BYTE_CODE_BITWISE_OR_STORE,  //
		BYTE_CODE_BITWISE_XOR_STORE, //
		BYTE_CODE_SHL_STORE, //
		BYTE_CODE_SHR_STORE, //
		BYTE_CODE_END_STORE=BYTE_CODE_SHR_STORE,
		//----- END STORE

		BYTE_CODE_PUSH_VECTOR_ITEM, // Value push for vector
		BYTE_CODE_PUSH_OBJECT_ITEM,
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
		BYTE_CODE_JE_CASE, //
		BYTE_CODE_JT, // goto if true ... goes end to conditional.
		BYTE_CODE_NEW_OBJECT_BY_KNOWN_TYPE, // new operator...
		BYTE_CODE_NEW_OBJECT_BY_VALUE, // new operator...
		BYTE_CODE_DELETE,
		BYTE_CODE_NEW_VECTOR, // Vector object
		BYTE_CODE_NEW_OBJECT,
		BYTE_CODE_NEW_STRING,
		BYTE_CODE_RET, // ret instruction ..
		BYTE_CODE_PUSH_SCOPE,
		BYTE_CODE_POP_SCOPE,
		BYTE_CODE_IT_INIT,
		BYTE_CODE_PRE_INC,
		BYTE_CODE_PRE_DEC,
		BYTE_CODE_POST_INC,
		BYTE_CODE_NEG_POST_INC,
		BYTE_CODE_POST_DEC,
		BYTE_CODE_NEG_POST_DEC,
		BYTE_CODE_RESET_STACK, // just in case there's no assignment on left or after last assignment
		BYTE_CODE_TYPEOF,
		BYTE_CODE_IN,
		//---------------------------
		BYTE_CODE_INVALID=255,

	}ByteCode;

	typedef enum: char {
		// static
		BYTE_CODE_METAMETHOD_INVALID=-1,  // ==
		BYTE_CODE_METAMETHOD_EQU=0,  // ==
		BYTE_CODE_METAMETHOD_NOT_EQU,  // !=
		BYTE_CODE_METAMETHOD_LT,  // <
		BYTE_CODE_METAMETHOD_LTE,  // <=
		BYTE_CODE_METAMETHOD_GT,  // >
		BYTE_CODE_METAMETHOD_GTE, // >=
		BYTE_CODE_METAMETHOD_ADD, // +
		BYTE_CODE_METAMETHOD_SUB, // -
		BYTE_CODE_METAMETHOD_DIV, // /
		BYTE_CODE_METAMETHOD_MUL, // *
		BYTE_CODE_METAMETHOD_MOD,  // %
		BYTE_CODE_METAMETHOD_AND, // & bitwise logic and
		BYTE_CODE_METAMETHOD_OR, // | bitwise logic or
		BYTE_CODE_METAMETHOD_XOR, // ^ logic xor
		BYTE_CODE_METAMETHOD_SHL, // << shift left
		BYTE_CODE_METAMETHOD_SHR, // >> shift right
		BYTE_CODE_METAMETHOD_ADD_SET, // set '+='
		BYTE_CODE_METAMETHOD_SUB_SET, // set '-='
		BYTE_CODE_METAMETHOD_MUL_SET, // set '*='
		BYTE_CODE_METAMETHOD_DIV_SET, // set '/='
		BYTE_CODE_METAMETHOD_MOD_SET, // set '%='
		BYTE_CODE_METAMETHOD_AND_SET, // set '&='
		BYTE_CODE_METAMETHOD_OR_SET,  // set '|='
		BYTE_CODE_METAMETHOD_XOR_SET, // set '^='
		BYTE_CODE_METAMETHOD_SHL_SET, // set '<<'
		BYTE_CODE_METAMETHOD_SHR_SET, // set '>>'

		//BYTE_CODE_METAMETHOD_GET, // get '='
		BYTE_CODE_METAMETHOD_NOT, // !
		BYTE_CODE_METAMETHOD_NEG, // -a
		BYTE_CODE_METAMETHOD_SET, // set '='
		BYTE_CODE_METAMETHOD_TO_STRING, // toString
		BYTE_CODE_METAMETHOD_POST_INC, // i++
		BYTE_CODE_METAMETHOD_POST_DEC, // i--
		BYTE_CODE_METAMETHOD_PRE_INC, // ++i
		BYTE_CODE_METAMETHOD_PRE_DEC, // --i
		BYTE_CODE_METAMETHOD_IN, // special metamethod in
		BYTE_CODE_METAMETHOD_MAX
	}ByteCodeMetamethod;



	const char * byte_code_to_str(ByteCode  op);
	const char * byte_code_to_operator_str(ByteCode op);
	const char * byte_code_metamethod_to_operator_str(ByteCodeMetamethod op);
	const char * byte_code_metamethod_to_symbol_str(ByteCodeMetamethod op);
	int			 byte_code_metamethod_get_num_arguments(ByteCodeMetamethod op);
	bool		 byte_code_metamethod_should_be_static(ByteCodeMetamethod op);
	//int			 get_num_arguments_non_metamethod(ByteCodeMetamethod op);
	ByteCode	 byte_code_load_var_type_to_push_stk(ByteCode op);
	bool		 byte_code_is_load_var_type(ByteCode op);
	ByteCodeMetamethod byte_code_symbol_to_setter_metamethod(const char *_symbol_name);



}
