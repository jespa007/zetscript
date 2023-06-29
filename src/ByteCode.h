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
||((byte_code)==ByteCode::BYTE_CODE_PUSH_STK_ARRAY_ITEM)\
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

#define IS_BYTE_BOOLEAN_OP(byte_code) \
(\
	  (BYTE_CODE_BEGIN_BOOLEAN_OPS <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=BYTE_CODE_END_BOOLEAN_OPS) \
)



#define IS_BYTE_CODE_PRE_POST(byte_code) \
	(byte_code==BYTE_CODE_PRE_INC)\
	|| (byte_code==	BYTE_CODE_PRE_DEC)\
	|| (byte_code==BYTE_CODE_POST_INC)\
	|| (byte_code==BYTE_CODE_NEG_POST_INC)\
	|| (byte_code==BYTE_CODE_BWC_POST_INC)\
	|| (byte_code==BYTE_CODE_POST_DEC)\
	|| (byte_code==BYTE_CODE_NEG_POST_DEC)\
	|| (byte_code==BYTE_CODE_BWC_POST_DEC)

#define IS_BYTE_CODE_LOAD(byte_code) \
(\
	(byte_code==BYTE_CODE_FIND_VARIABLE)\
		||\
	(\
		  (BYTE_CODE_LOAD_INI_LOAD <= (byte_code)) \
					  && \
		 ((byte_code)<=BYTE_CODE_LOAD_STACK_ELEMENT) \
	)\
)

#define IS_BYTE_CODE_OPERATOR_METAMETHOD_WITH_TWO_ARGS()

namespace zetscript{

	typedef enum:uint8_t{
		BYTE_CODE_INVALID=255,

		// ARITMETHIC OPERATORS.
		BYTE_CODE_END_FUNCTION = 0,

		BYTE_CODE_FIND_VARIABLE,
		//----- BEGIN CALL
		BYTE_CODE_CALL,
		BYTE_CODE_BEGIN_CALL=BYTE_CODE_CALL,		// direct
		BYTE_CODE_INDIRECT_GLOBAL_CALL,	// through symbol
		BYTE_CODE_INDIRECT_LOCAL_CALL,	// through symbol
		BYTE_CODE_STACK_CALL,		// stack call
		BYTE_CODE_THIS_CALL,	// this direct
		BYTE_CODE_SUPER_CALL,	// super direct
		BYTE_CODE_MEMBER_CALL, // through function and member loaded
		BYTE_CODE_INDIRECT_THIS_CALL, // call throught function
		BYTE_CODE_CONSTRUCTOR_CALL, // constructor...
		BYTE_CODE_END_CALL=BYTE_CODE_CONSTRUCTOR_CALL,
		//----- END CALL

		BYTE_CODE_UNRESOLVED_CALL,
		BYTE_CODE_PUSH_STK_GLOBAL_IRGO,

		//------ LOAD INI-----------------------
		// load ptr var
		BYTE_CODE_LOAD_INI_LOAD,
		BYTE_CODE_PUSH_STK_GLOBAL=BYTE_CODE_LOAD_INI_LOAD,
		BYTE_CODE_PUSH_STK_LOCAL,
		//BYTE_CODE_PUSH_STK_REF,
		BYTE_CODE_PUSH_STK_THIS,
		BYTE_CODE_PUSH_STK_ARRAY_ITEM,
		BYTE_CODE_PUSH_STK_THIS_VARIABLE,
		BYTE_CODE_PUSH_STK_THIS_FUNCTION,
		BYTE_CODE_PUSH_STK_OBJECT_ITEM,

		// load var content
		BYTE_CODE_LOAD_TYPE,
		BYTE_CODE_LOAD_GLOBAL,
		BYTE_CODE_LOAD_LOCAL,
		BYTE_CODE_LOAD_REF,
		BYTE_CODE_LOAD_THIS,
		BYTE_CODE_LOAD_ARRAY_ITEM,
		BYTE_CODE_LOAD_THIS_VARIABLE,
		BYTE_CODE_LOAD_THIS_FUNCTION,
		BYTE_CODE_LOAD_OBJECT_ITEM,

		// special load
		BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT,

		// load constants
		BYTE_CODE_LOAD_FUNCTION,
		BYTE_CODE_LOAD_UNDEFINED,
		BYTE_CODE_LOAD_NULL,
		BYTE_CODE_LOAD_STRING,
		BYTE_CODE_LOAD_ZS_FLOAT,
		BYTE_CODE_LOAD_BOOL,
		BYTE_CODE_LOAD_ZS_INT,

		BYTE_CODE_LOAD_STACK_ELEMENT,
		BYTE_CODE_LOAD_END=BYTE_CODE_LOAD_STACK_ELEMENT,
		//------ LOAD END -----------------------

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

		BYTE_CODE_PUSH_ARRAY_ITEM, // Value push for vector
		BYTE_CODE_PUSH_OBJECT_ITEM,
		//-------------------------------
		//
		// BOOLEAN OPS
		//
		BYTE_CODE_BEGIN_BOOLEAN_OPS,
		BYTE_CODE_EQU=BYTE_CODE_BEGIN_BOOLEAN_OPS,  // ==
		BYTE_CODE_NOT_EQU,  // !=
		BYTE_CODE_LT,  // <
		BYTE_CODE_LTE,  // <=
		BYTE_CODE_GT,  // >
		BYTE_CODE_GTE, // >=
		BYTE_CODE_NOT, // !
		BYTE_CODE_END_BOOLEAN_OPS=BYTE_CODE_NOT,
		//
		// BOOLEAN OPS
		//
		//-------------------------------

		BYTE_CODE_LOGIC_AND, // &&
		BYTE_CODE_LOGIC_OR,  // ||
		BYTE_CODE_NEG, // -a
		BYTE_CODE_BWC, // ~a
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
		BYTE_CODE_JMP_CASE,
		BYTE_CODE_JT, // goto if true ... goes end to conditional.
		BYTE_CODE_NEW_OBJECT_BY_TYPE, // new operator...
		BYTE_CODE_NEW_OBJECT_BY_VALUE, // new operator...
		BYTE_CODE_DELETE,
		BYTE_CODE_NEW_ARRAY, // Array object
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
		BYTE_CODE_BWC_POST_INC,
		BYTE_CODE_POST_DEC,
		BYTE_CODE_NEG_POST_DEC,
		BYTE_CODE_BWC_POST_DEC,
		BYTE_CODE_RESET_STACK, // just in case there's no assignment on left or after last assignment
		//BYTE_CODE_CLEAR_ZERO_POINTERS,
		BYTE_CODE_TYPEOF,
		BYTE_CODE_IN,
		MAX_BYTE_CODES
	}ByteCode;

	typedef enum: uint8_t {
		// static
		METAMETHOD_BYTE_CODE_GET=254,  // ==
		METAMETHOD_BYTE_CODE_INVALID=255,  // ==
		METAMETHOD_BYTE_CODE_EQU=0,  // ==
		METAMETHOD_BYTE_CODE_NOT_EQU,  // !=
		METAMETHOD_BYTE_CODE_LT,  // <
		METAMETHOD_BYTE_CODE_LTE,  // <=
		METAMETHOD_BYTE_CODE_GT,  // >
		METAMETHOD_BYTE_CODE_GTE, // >=
		METAMETHOD_BYTE_CODE_ADD, // +
		METAMETHOD_BYTE_CODE_SUB, // -
		METAMETHOD_BYTE_CODE_DIV, // /
		METAMETHOD_BYTE_CODE_MUL, // *
		METAMETHOD_BYTE_CODE_MOD,  // %
		METAMETHOD_BYTE_CODE_AND, // & bitwise logic and
		METAMETHOD_BYTE_CODE_OR, // | bitwise logic or
		METAMETHOD_BYTE_CODE_XOR, // ^ logic xor
		METAMETHOD_BYTE_CODE_SHL, // << shift left
		METAMETHOD_BYTE_CODE_SHR, // >> shift right
		METAMETHOD_BYTE_CODE_ADD_SET, // set '+='
		METAMETHOD_BYTE_CODE_SUB_SET, // set '-='
		METAMETHOD_BYTE_CODE_MUL_SET, // set '*='
		METAMETHOD_BYTE_CODE_DIV_SET, // set '/='
		METAMETHOD_BYTE_CODE_MOD_SET, // set '%='
		METAMETHOD_BYTE_CODE_AND_SET, // set '&='
		METAMETHOD_BYTE_CODE_OR_SET,  // set '|='
		METAMETHOD_BYTE_CODE_XOR_SET, // set '^='
		METAMETHOD_BYTE_CODE_SHL_SET, // set '<<'
		METAMETHOD_BYTE_CODE_SHR_SET, // set '>>'
		METAMETHOD_BYTE_CODE_BTW_SET, // set '~'

		//METAMETHOD_BYTE_CODE_GET, // get '='
		METAMETHOD_BYTE_CODE_NOT, // !
		METAMETHOD_BYTE_CODE_NEG, // -a
		METAMETHOD_BYTE_CODE_BWC, // ~a
		METAMETHOD_BYTE_CODE_SET, // set '='
		METAMETHOD_BYTE_CODE_TO_STRING, // toString
		METAMETHOD_BYTE_CODE_POST_INC, // i++
		METAMETHOD_BYTE_CODE_POST_DEC, // i--
		METAMETHOD_BYTE_CODE_PRE_INC, // ++i
		METAMETHOD_BYTE_CODE_PRE_DEC, // --i
		METAMETHOD_BYTE_CODE_IN, // special metamethod in
		MAX_METAMETHOD_BYTE_CODES,
	}MetamethodByteCode;


	const char * byte_code_to_str(ByteCode  op);
	const char * byte_code_to_operator_str(ByteCode op);
	const char * metamethod_byte_code_to_operator_str(MetamethodByteCode op);
	const char * metamethod_byte_code_to_symbol_str(MetamethodByteCode op);
	int			 metamethod_byte_code_get_num_arguments(MetamethodByteCode op);
	bool		 metamethod_byte_code_should_be_static(MetamethodByteCode op);
	//int			 get_num_arguments_non_metamethod(MetamethodByteCode op);
	ByteCode	 byte_code_load_var_type_to_push_stk(ByteCode op);
	bool		 byte_code_is_load_var_type(ByteCode op);






}
