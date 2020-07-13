/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

// properties shared by compiler + instruction ..
typedef enum{
	//-- PRE/POST OPERATORS (Byy default there's no operators involved)
	BIT_INSTRUCTION_PROPERTY_PRE_INC = 0,	// 0x1
	BIT_INSTRUCTION_PROPERTY_POST_INC,		// 0x2
	BIT_INSTRUCTION_PROPERTY_PRE_DEC,		// 0x4
	BIT_INSTRUCTION_PROPERTY_POST_DEC,		// 0x8
	BIT_INSTRUCTION_PROPERTY_PRE_NOT,		// 0x10
	BIT_INSTRUCTION_PROPERTY_PRE_POST_OP,

	//-- SCOPE TYPE (By default is global scope)
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL = BIT_INSTRUCTION_PROPERTY_PRE_POST_OP, 	// 0x20
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS,							// 0x40
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER,						// 0x80
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS,						// 0x100
	BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX,

	//-- BYTE_CODE_CALL TYPE
	BIT_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION = BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX,// 0x200
	BIT_INSTRUCTION_PROPERTY_DIRECT_CALL_RETURN,					// 0x400
	BIT_INSTRUCTION_PROPERTY_DEDUCE_C_CALL,						// 0x800
	BIT_INSTRUCTION_PROPERTY_CONSTRUCT_CALL,						// 0x1000
	BIT_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL,					// 0x2000
	BIT_INSTRUCTION_PROPERTY_MAX,
}BitInstructionProperty;

// PRE/POST OP
#define MSK_INSTRUCTION_PROPERTY_PRE_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_INC) // ++
#define MSK_INSTRUCTION_PROPERTY_POST_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_INC) // ++
#define MSK_INSTRUCTION_PROPERTY_PRE_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_DEC) // --
#define MSK_INSTRUCTION_PROPERTY_POST_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_DEC) // --
#define MSK_INSTRUCTION_PROPERTY_PRE_NOT					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_NOT)  // !
#define MSK_INSTRUCTION_PROPERTY_PRE_POST_OP				(((0x1<<(BIT_INSTRUCTION_PROPERTY_PRE_POST_OP-BIT_INSTRUCTION_PROPERTY_PRE_INC))-1)<<(BIT_INSTRUCTION_PROPERTY_PRE_INC))
#define GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_PRE_POST_OP)

//GLOBAL_SCOPE,   // by default
#define	MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_LOCAL)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS)
#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE					(((0x1<<(BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX-BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS))-1)<<(BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS))

#define GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)
//#define REMOVE_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPES(prop)	((prop)&=~MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)

// BYTE_CODE_CALL TYPE
#define	MSK_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION			(0x1<<BIT_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION)
#define MSK_INSTRUCTION_PROPERTY_DIRECT_CALL_RETURN		(0x1<<BIT_INSTRUCTION_PROPERTY_DIRECT_CALL_RETURN)
#define MSK_INSTRUCTION_PROPERTY_DEDUCE_C_CALL				(0x1<<BIT_INSTRUCTION_PROPERTY_DEDUCE_C_CALL)
#define MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL				(0x1<<BIT_INSTRUCTION_PROPERTY_CONSTRUCT_CALL)
#define MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL			(0x1<<BIT_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL)

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
			//Symbol * _symbol;
			std::string * str_symbol;

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