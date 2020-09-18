/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once


#define ZS_IDX_INSTRUCTION_OP2_UNDEFINED			-1
#define ZS_IDX_INSTRUCTION_OP2_SOLVE_AT_RUNTIME		-2
#define ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR			-3


// properties shared by compiler + instruction ..
typedef enum{
	//-- PRE/POST OPERATORS (Byy default there's no operators involved)
	BIT_INSTRUCTION_PROPERTY_PRE_INC = 0,												// 0x0001
	BIT_INSTRUCTION_PROPERTY_POST_INC,													// 0x0002
	BIT_INSTRUCTION_PROPERTY_PRE_DEC,													// 0x0004
	BIT_INSTRUCTION_PROPERTY_POST_DEC,													// 0x0008
	BIT_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT,											// 0x0010
	BIT_INSTRUCTION_PROPERTY_PRE_POST_OP,												// 0x0020
	BIT_INSTRUCTION_PROPERTY_PRE_POST_MAX,

	//-- SCOPE TYPE (By default is global scope)
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL = BIT_INSTRUCTION_PROPERTY_PRE_POST_MAX, 	// 0x0040
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS,											// 0x0080
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER,											// 0x0100
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS,											// 0x0200
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX,
	//-- BYTE_CODE_CALL TYPE
	BIT_INSTRUCTION_PROPERTY_CONSTRUCT_CALL= BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX,	// 0x0400
	//BIT_INSTRUCTION_PROPERTY_IGNORE_FUNCTION_CALL,									// 0x0800
	//BIT_INSTRUCTION_PROPERTY_FUNCTION_CALL,											// 0x1000
	BIT_INSTRUCTION_PROPERTY_POP_ONE,													// 0x0800
	BIT_INSTRUCTION_PROPERTY_START_EXPRESSION,											// 0x1000
	BIT_INSTRUCTION_PROPERTY_MAX
}BitInstructionProperty;




// PRE/POST OP
#define MSK_INSTRUCTION_PROPERTY_PRE_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_INC) // ++
#define MSK_INSTRUCTION_PROPERTY_POST_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_INC) // ++
#define MSK_INSTRUCTION_PROPERTY_PRE_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_DEC) // --
#define MSK_INSTRUCTION_PROPERTY_POST_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_DEC) // --
#define MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT				(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT)  // ! or neg
#define MSK_INSTRUCTION_PROPERTY_PRE_POST_OP				(((0x1<<(BIT_INSTRUCTION_PROPERTY_PRE_POST_OP-BIT_INSTRUCTION_PROPERTY_PRE_INC))-1)<<(BIT_INSTRUCTION_PROPERTY_PRE_INC))
#define GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_PRE_POST_OP)
#define HAS_PRE_POST_INC_DEC_OP(prop)						(((MSK_INSTRUCTION_PROPERTY_PRE_INC|MSK_INSTRUCTION_PROPERTY_POST_INC|MSK_INSTRUCTION_PROPERTY_PRE_DEC|MSK_INSTRUCTION_PROPERTY_POST_DEC)&prop)!=0)

//GLOBAL_SCOPE,   // by default
#define	MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE					(((0x1<<(BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX-BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS))-1)<<(BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS))

#define GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)
//#define REMOVE_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPES(prop)	((prop)&=~MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)

// BYTE_CODE_CALL TYPE
//#define	MSK_INSTRUCTION_PROPERTY_FUNCTION_CALL				(0x1<<BIT_INSTRUCTION_PROPERTY_FUNCTION_CALL)
#define	MSK_INSTRUCTION_PROPERTY_POP_ONE					(0x1<<BIT_INSTRUCTION_PROPERTY_POP_ONE)
//#define	MSK_INSTRUCTION_PROPERTY_VECTOR_ACCESS				(0x1<<BIT_INSTRUCTION_PROPERTY_VECTOR_ACCESS)
#define MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL				(0x1<<BIT_INSTRUCTION_PROPERTY_CONSTRUCT_CALL)
//#define MSK_INSTRUCTION_PROPERTY_IGNORE_FUNCTION_CALL		(0x1<<BIT_INSTRUCTION_PROPERTY_IGNORE_FUNCTION_CALL)
#define MSK_INSTRUCTION_PROPERTY_START_EXPRESSION			(0x1<<BIT_INSTRUCTION_PROPERTY_START_EXPRESSION)

//#define INSTRUCTION_NO_VALUE_OP1 ZS_IDX_UNDEFINED
//#define INSTRUCTION_NO_VALUE_OP2 ZS_IDX_UNDEFINED

namespace zetscript{

	#pragma pack(push, 1)

	struct Instruction {

		ByteCode byte_code;
		unsigned char value_op1;
		intptr_t value_op2;
		unsigned short properties;

		Instruction(){
			byte_code=ByteCode::BYTE_CODE_INVALID;
			value_op1=ZS_IDX_UNDEFINED;
			value_op2=ZS_IDX_UNDEFINED;
			properties=0;
		}

		Instruction(ByteCode _byte_code
		 ,unsigned char _value_op1=ZS_IDX_UNDEFINED
		 ,intptr_t _value_op2=ZS_IDX_UNDEFINED
		 ,unsigned short _properties=0
		 ){
			byte_code=_byte_code;
			value_op1=_value_op1;
			value_op2=_value_op2;
			properties=_properties;
		}

	};

	#pragma pack(pop)

	struct InstructionSourceInfo {

		const char * file;
		short line;
		std::string * str_symbol; // should be deallocated

		InstructionSourceInfo(){
			str_symbol=NULL;
			file="unknow_file";
			line=-1;
		}

		InstructionSourceInfo(const char * _file, short _line,std::string *_str_symbol){

			file=_file;
			line=_line;
			str_symbol=_str_symbol;
		}
	};


	typedef Instruction *PtrInstruction;

}
