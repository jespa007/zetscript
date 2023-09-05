/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once




#define ZS_IS_BYTE_CODE_PUSH_STK_VARIABLE_TYPE(byte_code) \
(\
  ((byte_code)==ZS_BYTE_CODE_FIND_VARIABLE)\
||((byte_code)==ZS_BYTE_CODE_PUSH_STK_GLOBAL)\
||((byte_code)==ZS_BYTE_CODE_PUSH_STK_LOCAL)\
||((byte_code)==ZS_BYTE_CODE_PUSH_STK_THIS)\
||((byte_code)==ZS_BYTE_CODE_PUSH_STK_ARRAY_ITEM)\
||((byte_code)==ZS_BYTE_CODE_PUSH_STK_THIS_VARIABLE)\
||((byte_code)==ZS_BYTE_CODE_PUSH_STK_OBJECT_ITEM)\
)

#define ZS_IS_BYTE_CODE_LOAD_CONSTANT(byte_code) \
(\
   ((byte_code)== ZS_BYTE_CODE_LOAD_BOOL) \
|| ((byte_code)== ZS_BYTE_CODE_LOAD_INT) \
|| ((byte_code)== ZS_BYTE_CODE_LOAD_FLOAT) \
|| ((byte_code)== ZS_BYTE_CODE_LOAD_STRING) \
)

#define ZS_IS_BYTE_CODE_LOAD_VARIABLE_IMMEDIATE(byte_code) \
(\
   ((byte_code)== ZS_BYTE_CODE_LOAD_LOCAL) \
|| ((byte_code)== ZS_BYTE_CODE_FIND_VARIABLE) \
|| ((byte_code)== ZS_BYTE_CODE_LOAD_THIS_VARIABLE)\
)

#define ZS_IS_BYTE_CODE_OPERATION(byte_code) 		(ZS_BYTE_CODE_EQU <=(byte_code) && (byte_code)<= ZS_BYTE_CODE_SHR)

#define ZS_IS_BYTE_CODE_STORE_WITH_OPERATION(b) 	(ZS_BYTE_CODE_ADD_STORE<=(b) && (b) <=ZS_BYTE_CODE_SHR_STORE)
//#define ZS_IS_BYTE_CODE_STORE(b) 					(ZS_IS_BYTE_CODE_STORE_WITH_OPERATION(b) || ((b)==ZS_BYTE_CODE_STORE))

#define ZS_IS_BYTE_CODE_CALL(byte_code) \
(\
	  (ZS_BYTE_CODE_BEGIN_CALL <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=ZS_BYTE_CODE_END_CALL) \
)

#define ZS_IS_BYTE_CODE_STORE(byte_code) \
(\
	  (ZS_BYTE_CODE_BEGIN_STORE <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=ZS_BYTE_CODE_END_STORE) \
)

#define IS_BYTE_BOOLEAN_OP(byte_code) \
(\
	  (ZS_BYTE_CODE_BEGIN_BOOLEAN_OPS <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=ZS_BYTE_CODE_END_BOOLEAN_OPS) \
)



#define ZS_IS_BYTE_CODE_PRE_POST(byte_code) \
	(byte_code==ZS_BYTE_CODE_PRE_INC)\
	|| (byte_code==	ZS_BYTE_CODE_PRE_DEC)\
	|| (byte_code==ZS_BYTE_CODE_POST_INC)\
	/*|| (byte_code==ZS_BYTE_CODE_NEG_POST_INC)\
	|| (byte_code==ZS_BYTE_CODE_BWC_POST_INC)*/\
	|| (byte_code==ZS_BYTE_CODE_POST_DEC)\
	/*|| (byte_code==ZS_BYTE_CODE_NEG_POST_DEC)\
	|| (byte_code==ZS_BYTE_CODE_BWC_POST_DEC)*/

#define ZS_IS_BYTE_CODE_LOAD(byte_code) \
(\
	(byte_code==ZS_BYTE_CODE_FIND_VARIABLE)\
		||\
	(\
		  (ZS_BYTE_CODE_LOAD_INI_LOAD <= (byte_code)) \
					  && \
		 ((byte_code)<=ZS_BYTE_CODE_LOAD_STACK_ELEMENT) \
	)\
)

namespace zetscript{

	typedef enum:uint8_t{
		ZS_BYTE_CODE_INVALID=255,

		// ARITMETHIC OPERATORS.
		ZS_BYTE_CODE_END_FUNCTION = 0,

		ZS_BYTE_CODE_FIND_VARIABLE,
		//----- BEGIN CALL
		ZS_BYTE_CODE_CALL,
		ZS_BYTE_CODE_BEGIN_CALL=ZS_BYTE_CODE_CALL,		// direct
		ZS_BYTE_CODE_INDIRECT_GLOBAL_CALL,	// through symbol
		ZS_BYTE_CODE_INDIRECT_LOCAL_CALL,	// through symbol
		ZS_BYTE_CODE_STACK_CALL,		// stack call
		ZS_BYTE_CODE_THIS_CALL,	// this direct
		ZS_BYTE_CODE_SUPER_CALL,	// super direct
		ZS_BYTE_CODE_MEMBER_CALL, // through function and member loaded
		ZS_BYTE_CODE_INDIRECT_THIS_CALL, // call throught function
		ZS_BYTE_CODE_CONSTRUCTOR_CALL, // constructor...
		ZS_BYTE_CODE_END_CALL=ZS_BYTE_CODE_CONSTRUCTOR_CALL,
		//----- END CALL

		ZS_BYTE_CODE_UNRESOLVED_CALL,
		ZS_BYTE_CODE_PUSH_STK_GLOBAL_IRGO,

		//------ LOAD INI-----------------------
		// load ptr var
		ZS_BYTE_CODE_LOAD_INI_LOAD,
		ZS_BYTE_CODE_PUSH_STK_GLOBAL=ZS_BYTE_CODE_LOAD_INI_LOAD,
		ZS_BYTE_CODE_PUSH_STK_LOCAL,
		//ZS_BYTE_CODE_PUSH_STK_REF,
		ZS_BYTE_CODE_PUSH_STK_THIS,
		ZS_BYTE_CODE_PUSH_STK_ARRAY_ITEM,
		ZS_BYTE_CODE_PUSH_STK_THIS_VARIABLE,
		ZS_BYTE_CODE_PUSH_STK_THIS_FUNCTION,
		ZS_BYTE_CODE_PUSH_STK_OBJECT_ITEM,

		// load var content
		ZS_BYTE_CODE_LOAD_TYPE,
		ZS_BYTE_CODE_LOAD_GLOBAL,
		ZS_BYTE_CODE_LOAD_LOCAL,
		ZS_BYTE_CODE_LOAD_REF,
		ZS_BYTE_CODE_LOAD_THIS,
		ZS_BYTE_CODE_LOAD_ARRAY_ITEM,
		ZS_BYTE_CODE_LOAD_THIS_VARIABLE,
		ZS_BYTE_CODE_LOAD_THIS_FUNCTION,
		ZS_BYTE_CODE_LOAD_OBJECT_ITEM,

		// special load
		ZS_BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT,

		// load constants
		ZS_BYTE_CODE_LOAD_FUNCTION,
		ZS_BYTE_CODE_LOAD_UNDEFINED,
		ZS_BYTE_CODE_LOAD_NULL,
		ZS_BYTE_CODE_LOAD_STRING,
		ZS_BYTE_CODE_LOAD_FLOAT,
		ZS_BYTE_CODE_LOAD_BOOL,
		ZS_BYTE_CODE_LOAD_INT,

		ZS_BYTE_CODE_LOAD_STACK_ELEMENT,
		ZS_BYTE_CODE_LOAD_END=ZS_BYTE_CODE_LOAD_STACK_ELEMENT,
		//------ LOAD END -----------------------

		//----- BEGIN STORE
		ZS_BYTE_CODE_BEGIN_STORE,
		ZS_BYTE_CODE_STORE_CONST=ZS_BYTE_CODE_BEGIN_STORE, // it stores and set stack only-read
		ZS_BYTE_CODE_STORE, 	 // mov expression to var ( > 1 multiple)
		ZS_BYTE_CODE_ADD_STORE, //
		ZS_BYTE_CODE_SUB_STORE, //
		ZS_BYTE_CODE_MUL_STORE, //
		ZS_BYTE_CODE_DIV_STORE, //
		ZS_BYTE_CODE_MOD_STORE, //
		ZS_BYTE_CODE_BITWISE_AND_STORE, //
		ZS_BYTE_CODE_BITWISE_OR_STORE,  //
		ZS_BYTE_CODE_BITWISE_XOR_STORE, //
		ZS_BYTE_CODE_SHL_STORE, //
		ZS_BYTE_CODE_SHR_STORE, //
		ZS_BYTE_CODE_END_STORE=ZS_BYTE_CODE_SHR_STORE,
		//----- END STORE

		ZS_BYTE_CODE_PUSH_ARRAY_ITEM, // Value push for vector
		ZS_BYTE_CODE_PUSH_OBJECT_ITEM,
		//-------------------------------
		//
		// BOOLEAN OPS
		//
		ZS_BYTE_CODE_BEGIN_BOOLEAN_OPS,
		ZS_BYTE_CODE_EQU=ZS_BYTE_CODE_BEGIN_BOOLEAN_OPS,  // ==
		ZS_BYTE_CODE_NOT_EQU,  // !=
		ZS_BYTE_CODE_LT,  // <
		ZS_BYTE_CODE_LTE,  // <=
		ZS_BYTE_CODE_GT,  // >
		ZS_BYTE_CODE_GTE, // >=
		ZS_BYTE_CODE_NOT, // !
		ZS_BYTE_CODE_END_BOOLEAN_OPS=ZS_BYTE_CODE_NOT,
		//
		// BOOLEAN OPS
		//
		//-------------------------------

		ZS_BYTE_CODE_LOGIC_AND, // &&
		ZS_BYTE_CODE_LOGIC_OR,  // ||
		ZS_BYTE_CODE_NEG, // -a
		ZS_BYTE_CODE_BWC, // ~a
		ZS_BYTE_CODE_ADD, // +
		ZS_BYTE_CODE_SUB, // -
		ZS_BYTE_CODE_MUL, // *
		ZS_BYTE_CODE_DIV, // /
		ZS_BYTE_CODE_MOD,  // %
		ZS_BYTE_CODE_BITWISE_AND, // binary and
		ZS_BYTE_CODE_BITWISE_OR, // binary or
		ZS_BYTE_CODE_BITWISE_XOR, // binary xor
		ZS_BYTE_CODE_SHL, // binary shift left
		ZS_BYTE_CODE_SHR, // binary shift right
		//
		// OPERATIONS
		//-------------------------------------
		ZS_BYTE_CODE_INSTANCEOF,
		ZS_BYTE_CODE_JMP,
		ZS_BYTE_CODE_JNT, // goto if not true ... goes end to conditional.
		ZS_BYTE_CODE_JE_CASE, //
		ZS_BYTE_CODE_JMP_CASE,
		ZS_BYTE_CODE_JT, // goto if true ... goes end to conditional.
		ZS_BYTE_CODE_NEW_OBJECT_BY_TYPE, // new operator...
		ZS_BYTE_CODE_NEW_OBJECT_BY_VALUE, // new operator...
		ZS_BYTE_CODE_DELETE,
		ZS_BYTE_CODE_NEW_ARRAY, // Array object
		ZS_BYTE_CODE_NEW_OBJECT,
		ZS_BYTE_CODE_NEW_STRING,
		ZS_BYTE_CODE_RET, // ret instruction ..
		ZS_BYTE_CODE_PUSH_SCOPE,
		ZS_BYTE_CODE_POP_SCOPE,
		ZS_BYTE_CODE_IT_INIT,
		ZS_BYTE_CODE_PRE_INC,
		ZS_BYTE_CODE_PRE_DEC,
		ZS_BYTE_CODE_POST_INC,
		//ZS_BYTE_CODE_NEG_POST_INC,
		//ZS_BYTE_CODE_BWC_POST_INC,
		ZS_BYTE_CODE_POST_DEC,
		//ZS_BYTE_CODE_NEG_POST_DEC,
		//ZS_BYTE_CODE_BWC_POST_DEC,
		ZS_BYTE_CODE_RESET_STACK, // just in case there's no assignment on left or after last assignment
		//ZS_BYTE_CODE_CLEAR_ZERO_POINTERS,
		ZS_BYTE_CODE_TYPEOF,
		ZS_BYTE_CODE_IN,
		ZS_MAX_BYTE_CODES
	}ZS_ByteCode;


	const char * byte_code_to_str(ByteCode  op);
	const char * byte_code_to_operator_str(ByteCode op);
	ByteCode	 byte_code_load_var_type_to_push_stk(ByteCode op);
	bool		 byte_code_is_load_var_type(ByteCode op);






}
