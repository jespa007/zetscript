/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once




#define ZS_IS_BYTE_CODE_ID_PUSH_STK_VARIABLE_TYPE(byte_code) \
(\
  ((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_FIND_VARIABLE)\
||((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_GLOBAL)\
||((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_LOCAL)\
||((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS)\
||((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_ARRAY_ITEM)\
||((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS_VARIABLE)\
||((byte_code)==ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_OBJECT_ITEM)\
)

#define ZS_IS_BYTE_CODE_ID_LOAD_CONSTANT(byte_code) \
(\
   ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_BOOL) \
|| ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT) \
|| ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FLOAT) \
|| ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING) \
)

#define ZS_IS_BYTE_CODE_ID_LOAD_VARIABLE_IMMEDIATE(byte_code) \
(\
   ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_LOCAL) \
|| ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_FIND_VARIABLE) \
|| ((byte_code)== ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_VARIABLE)\
)

#define ZS_IS_BYTE_CODE_ID_OPERATION(byte_code) 		(ByteCode::ByteCodeId::BYTE_CODE_ID_EQU <=(byte_code) && (byte_code)<= ByteCode::ByteCodeId::BYTE_CODE_ID_SHR)

#define ZS_IS_BYTE_CODE_ID_STORE_WITH_OPERATION(b) 	(ByteCode::ByteCodeId::BYTE_CODE_ID_ADD_ASSIGN<=(b) && (b) <=ByteCode::ByteCodeId::BYTE_CODE_ID_SHR_ASSIGN)
//#define ZS_IS_BYTE_CODE_ID_STORE(b) 					(ZS_IS_BYTE_CODE_ID_STORE_WITH_OPERATION(b) || ((b)==BYTE_CODE_ID_STORE))

#define ZS_IS_BYTE_CODE_ID_CALL(byte_code) \
(\
	  (ByteCode::ByteCodeId::BYTE_CODE_ID_BEGIN_CALL <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=ByteCode::ByteCodeId::BYTE_CODE_ID_END_CALL) \
)

#define ZS_IS_BYTE_CODE_ID_STORE(byte_code) \
(\
	  (ByteCode::ByteCodeId::BYTE_CODE_ID_BEGIN_STORE <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=ByteCode::ByteCodeId::BYTE_CODE_ID_END_STORE) \
)

#define IS_BYTE_BOOLEAN_OP(byte_code) \
(\
	  (ByteCode::ByteCodeId::BYTE_CODE_ID_BEGIN_BOOLEAN_OPS <= (byte_code)) \
	  	  	  	  && \
	 ((byte_code)<=ByteCode::ByteCodeId::BYTE_CODE_ID_END_BOOLEAN_OPS) \
)



#define ZS_IS_BYTE_CODE_ID_PRE_POST(byte_code) \
	(byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_INC)\
	|| (byte_code==	ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_DEC)\
	|| (byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_POST_INC)\
	/*|| (byte_code==BYTE_CODE_ID_NEG_POST_INC)\
	|| (byte_code==BYTE_CODE_ID_BWC_POST_INC)*/\
	|| (byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_POST_DEC)\
	/*|| (byte_code==BYTE_CODE_ID_NEG_POST_DEC)\
	|| (byte_code==BYTE_CODE_ID_BWC_POST_DEC)*/

#define ZS_IS_BYTE_CODE_ID_LOAD(byte_code) \
(\
	(byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_FIND_VARIABLE)\
		||\
	(\
		  (ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INI_LOAD <= (byte_code)) \
					  && \
		 ((byte_code)<=ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STACK_ELEMENT) \
	)\
)

namespace zetscript{

	class ByteCode{
	public:
		typedef enum:uint8_t{
			BYTE_CODE_ID_INVALID=255,

			// ARITMETHIC OPERATORS.
			BYTE_CODE_ID_END_FUNCTION = 0,

			BYTE_CODE_ID_FIND_VARIABLE,
			//----- BEGIN CALL
			BYTE_CODE_ID_CALL,
			BYTE_CODE_ID_BEGIN_CALL=BYTE_CODE_ID_CALL,		// direct
			BYTE_CODE_ID_INDIRECT_GLOBAL_CALL,	// through symbol
			BYTE_CODE_ID_INDIRECT_LOCAL_CALL,	// through symbol
			BYTE_CODE_ID_STACK_CALL,		// stack call
			BYTE_CODE_ID_THIS_CALL,	// this direct
			BYTE_CODE_ID_SUPER_CALL,	// super direct
			BYTE_CODE_ID_MEMBER_CALL, // through function and member loaded
			BYTE_CODE_ID_INDIRECT_THIS_CALL, // call throught function
			BYTE_CODE_ID_CONSTRUCTOR_CALL, // constructor...
			BYTE_CODE_ID_END_CALL=BYTE_CODE_ID_CONSTRUCTOR_CALL,
			//----- END CALL

			BYTE_CODE_ID_UNRESOLVED_CALL,
			BYTE_CODE_ID_PUSH_STK_GLOBAL_IRGO,

			//------ LOAD INI-----------------------
			// load ptr var
			BYTE_CODE_ID_LOAD_INI_LOAD,
			BYTE_CODE_ID_PUSH_STK_GLOBAL=BYTE_CODE_ID_LOAD_INI_LOAD,
			BYTE_CODE_ID_PUSH_STK_LOCAL,
			//BYTE_CODE_ID_PUSH_STK_REF,
			BYTE_CODE_ID_PUSH_STK_THIS,
			BYTE_CODE_ID_PUSH_STK_ARRAY_ITEM,
			BYTE_CODE_ID_PUSH_STK_THIS_VARIABLE,
			BYTE_CODE_ID_PUSH_STK_THIS_FUNCTION,
			BYTE_CODE_ID_PUSH_STK_OBJECT_ITEM,

			// load var content
			BYTE_CODE_ID_LOAD_TYPE,
			BYTE_CODE_ID_LOAD_GLOBAL,
			BYTE_CODE_ID_LOAD_LOCAL,
			BYTE_CODE_ID_LOAD_REF,
			BYTE_CODE_ID_LOAD_THIS,
			BYTE_CODE_ID_LOAD_ARRAY_ITEM,
			BYTE_CODE_ID_LOAD_THIS_VARIABLE,
			BYTE_CODE_ID_LOAD_THIS_FUNCTION,
			BYTE_CODE_ID_LOAD_OBJECT_ITEM,

			// special load
			BYTE_CODE_ID_LOAD_CONSTRUCTOR_FUNCT,

			// load constants
			BYTE_CODE_ID_LOAD_FUNCTION,
			BYTE_CODE_ID_LOAD_UNDEFINED,
			BYTE_CODE_ID_LOAD_NULL,
			BYTE_CODE_ID_LOAD_STRING,
			BYTE_CODE_ID_LOAD_FLOAT,
			BYTE_CODE_ID_LOAD_BOOL,
			BYTE_CODE_ID_LOAD_INT,

			BYTE_CODE_ID_LOAD_STACK_ELEMENT,
			BYTE_CODE_ID_LOAD_END=BYTE_CODE_ID_LOAD_STACK_ELEMENT,
			//------ LOAD END -----------------------

			//----- BEGIN STORE
			BYTE_CODE_ID_BEGIN_STORE,
			BYTE_CODE_ID_STORE_CONST=BYTE_CODE_ID_BEGIN_STORE, // it stores and set stack only-read
			BYTE_CODE_ID_STORE, 	 // mov expression to var ( > 1 multiple)
			BYTE_CODE_ID_ADD_ASSIGN, //
			BYTE_CODE_ID_SUB_ASSIGN, //
			BYTE_CODE_ID_MUL_ASSIGN, //
			BYTE_CODE_ID_DIV_ASSIGN, //
			BYTE_CODE_ID_MOD_ASSIGN, //
			BYTE_CODE_ID_BITWISE_AND_ASSIGN, //
			BYTE_CODE_ID_BITWISE_OR_ASSIGN,  //
			BYTE_CODE_ID_BITWISE_XOR_ASSIGN, //
			BYTE_CODE_ID_SHL_ASSIGN, //
			BYTE_CODE_ID_SHR_ASSIGN, //
			BYTE_CODE_ID_END_STORE=BYTE_CODE_ID_SHR_ASSIGN,
			//----- END STORE

			BYTE_CODE_ID_PUSH_ARRAY_ITEM, // Value push for vector
			BYTE_CODE_ID_PUSH_OBJECT_ITEM,
			//-------------------------------
			//
			// BOOLEAN OPS
			//
			BYTE_CODE_ID_BEGIN_BOOLEAN_OPS,
			BYTE_CODE_ID_EQU=BYTE_CODE_ID_BEGIN_BOOLEAN_OPS,  // ==
			BYTE_CODE_ID_NOT_EQU,  // !=
			BYTE_CODE_ID_LT,  // <
			BYTE_CODE_ID_LTE,  // <=
			BYTE_CODE_ID_GT,  // >
			BYTE_CODE_ID_GTE, // >=
			BYTE_CODE_ID_NOT, // !
			BYTE_CODE_ID_END_BOOLEAN_OPS=BYTE_CODE_ID_NOT,
			//
			// BOOLEAN OPS
			//
			//-------------------------------

			BYTE_CODE_ID_LOGIC_AND, // &&
			BYTE_CODE_ID_LOGIC_OR,  // ||
			BYTE_CODE_ID_NEG, // -a
			BYTE_CODE_ID_BWC, // ~a
			BYTE_CODE_ID_ADD, // +
			BYTE_CODE_ID_SUB, // -
			BYTE_CODE_ID_MUL, // *
			BYTE_CODE_ID_DIV, // /
			BYTE_CODE_ID_MOD,  // %
			BYTE_CODE_ID_BITWISE_AND, // binary and
			BYTE_CODE_ID_BITWISE_OR, // binary or
			BYTE_CODE_ID_BITWISE_XOR, // binary xor
			BYTE_CODE_ID_SHL, // binary shift left
			BYTE_CODE_ID_SHR, // binary shift right
			//
			// OPERATIONS
			//-------------------------------------
			BYTE_CODE_ID_INSTANCEOF,
			BYTE_CODE_ID_JMP,
			BYTE_CODE_ID_JNT, // goto if not true ... goes end to conditional.
			BYTE_CODE_ID_JE_CASE, //
			BYTE_CODE_ID_JMP_CASE,
			BYTE_CODE_ID_JT, // goto if true ... goes end to conditional.
			BYTE_CODE_ID_NEW_OBJECT_BY_TYPE, // new operator...
			BYTE_CODE_ID_NEW_OBJECT_BY_VALUE, // new operator...
			BYTE_CODE_ID_DELETE,
			BYTE_CODE_ID_NEW_ARRAY, // Array object
			BYTE_CODE_ID_NEW_OBJECT,
			BYTE_CODE_ID_NEW_STRING,
			BYTE_CODE_ID_RET, // ret instruction ..
			BYTE_CODE_ID_PUSH_SCOPE,
			BYTE_CODE_ID_POP_SCOPE,
			BYTE_CODE_ID_IT_INIT,
			BYTE_CODE_ID_PRE_INC,
			BYTE_CODE_ID_PRE_DEC,
			BYTE_CODE_ID_POST_INC,
			//BYTE_CODE_ID_NEG_POST_INC,
			//BYTE_CODE_ID_BWC_POST_INC,
			BYTE_CODE_ID_POST_DEC,
			//BYTE_CODE_ID_NEG_POST_DEC,
			//BYTE_CODE_ID_BWC_POST_DEC,
			BYTE_CODE_ID_RESET_STACK, // just in case there's no assignment on left or after last assignment
			//BYTE_CODE_ID_CLEAR_ZERO_POINTERS,
			BYTE_CODE_ID_TYPEOF,
			BYTE_CODE_ID_IN,
			MAX_BYTE_CODES
		}ByteCodeId;

		static const char * toString(ByteCodeId  op);
		static const char * toOperatorString(ByteCodeId op);
		static ByteCode::ByteCodeId	 	loadVarTypeToPushStk(ByteCodeId op);
		static bool		 	isLoadVarType(ByteCodeId op);
	};


}
