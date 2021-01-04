/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once


//#define ZS_IDX_INSTRUCTION_OP2_UNDEFINED				-1
//#define ZS_IDX_INSTRUCTION_OP2_SOLVE_AT_RUNTIME		-2
#define ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR				-3
#define ZS_IDX_INSTRUCTION_OP2_THIS						-4
//#define ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR_NOT_FOUND	-5
#define ZS_IDX_INSTRUCTION_JMP_BREAK					-6
#define ZS_IDX_INSTRUCTION_JMP_CONTINUE					-7


// properties shared by compiler + instruction ..
typedef enum{
	BIT_INSTRUCTION_PROPERTY_PRE_NEG,														// 0x0001
	BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL,												// 0x0002
	BIT_INSTRUCTION_PROPERTY_PACK_MEMBER_INFO,												// 0x0004
	BIT_INSTRUCTION_PROPERTY_ILOAD_K,														// 0x0008
	BIT_INSTRUCTION_PROPERTY_ILOAD_R,														// 0x0010
	BIT_INSTRUCTION_PROPERTY_ILOAD_KR,														// 0x0020
	BIT_INSTRUCTION_PROPERTY_ILOAD_RK,														// 0x0040
	BIT_INSTRUCTION_PROPERTY_ILOAD_RR,														// 0x0080
	BIT_INSTRUCTION_PROPERTY_NEXT_FIELD_LOAD,												// 0x0100 // will do instant field load instead of perform instruction
	BIT_INSTRUCTION_PROPERTY_MAX
}BitInstructionProperty;




// PRE/POST OP
/*#define MSK_INSTRUCTION_PROPERTY_PRE_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_INC) // ++
#define MSK_INSTRUCTION_PROPERTY_POST_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_INC) // ++
#define MSK_INSTRUCTION_PROPERTY_PRE_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_DEC) // --
#define MSK_INSTRUCTION_PROPERTY_POST_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_DEC) // --
#define MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT				(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT)  // ! or neg
#define MSK_INSTRUCTION_PROPERTY_PRE_POST_OP				(((0x1<<(BIT_INSTRUCTION_PROPERTY_PRE_POST_OP-BIT_INSTRUCTION_PROPERTY_PRE_INC))-1)<<(BIT_INSTRUCTION_PROPERTY_PRE_INC))
#define GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_PRE_POST_OP)
#define HAS_PRE_POST_INC_DEC_OP(prop)						(((MSK_INSTRUCTION_PROPERTY_PRE_INC|MSK_INSTRUCTION_PROPERTY_POST_INC|MSK_INSTRUCTION_PROPERTY_PRE_DEC|MSK_INSTRUCTION_PROPERTY_POST_DEC)&prop)!=0)*/

//#define MSK_INSTRUCTION_PROPERTY_PRE						(MSK_INSTRUCTION_PROPERTY_PRE_INC|MSK_INSTRUCTION_PROPERTY_PRE_DEC|MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT)
//#define MSK_INSTRUCTION_PROPERTY_POST						(MSK_INSTRUCTION_PROPERTY_POST_INC|MSK_INSTRUCTION_PROPERTY_POST_DEC)

#define	MSK_INSTRUCTION_PROPERTY_PRE_NEG 					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_NEG)

#define	MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL			(0x1<<BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL) //GLOBAL_SCOPE,   // by default
//#define MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS			(0x1<<BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)
//#define MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD			(0x1<<BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD)
//#define MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR			(0x1<<BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR)
//#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE					(((0x1<<(BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_LAST-BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIRST))-1)<<(BIT_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIRST))

//#define GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)

// BYTE_CODE_CALL TYPE
#define MSK_INSTRUCTION_PROPERTY_PACK_MEMBER_INFO			(0x1 << BIT_INSTRUCTION_PROPERTY_PACK_MEMBER_INFO)

#define MSK_INSTRUCTION_PROPERTY_ILOAD_K					(0x1 << BIT_INSTRUCTION_PROPERTY_ILOAD_K)
#define MSK_INSTRUCTION_PROPERTY_ILOAD_R					(0x1 << BIT_INSTRUCTION_PROPERTY_ILOAD_R)
#define MSK_INSTRUCTION_PROPERTY_ILOAD_KR					(0x1 << BIT_INSTRUCTION_PROPERTY_ILOAD_KR)
#define MSK_INSTRUCTION_PROPERTY_ILOAD_RK					(0x1 << BIT_INSTRUCTION_PROPERTY_ILOAD_RK)
#define MSK_INSTRUCTION_PROPERTY_ILOAD_RR					(0x1 << BIT_INSTRUCTION_PROPERTY_ILOAD_RR)

#define MSK_INSTRUCTION_ILOAD								(MSK_INSTRUCTION_PROPERTY_ILOAD_K|MSK_INSTRUCTION_PROPERTY_ILOAD_R|MSK_INSTRUCTION_PROPERTY_ILOAD_KR|MSK_INSTRUCTION_PROPERTY_ILOAD_RK|MSK_INSTRUCTION_PROPERTY_ILOAD_RR)

#define MSK_INSTRUCTION_PROPERTY_NEXT_FIELD_LOAD			(0x1 << BIT_INSTRUCTION_PROPERTY_NEXT_FIELD_LOAD)


//#define	MSK_INSTRUCTION_PROPERTY_POP_ONE					(0x1<<BIT_INSTRUCTION_PROPERTY_POP_ONE)
//#define MSK_INSTRUCTION_PROPERTY_START_EXPRESSION			(0x1<<BIT_INSTRUCTION_PROPERTY_START_EXPRESSION)

namespace zetscript{

	#pragma pack(push, 1)


	struct Instruction {

		ByteCode byte_code;
		unsigned char value_op1;
		zs_int value_op2;
		unsigned short properties;

		Instruction();

		Instruction(ByteCode _byte_code
		 ,unsigned char _value_op1=ZS_IDX_UNDEFINED
		 ,zs_int _value_op2=ZS_IDX_UNDEFINED
		 ,unsigned short _properties=0
		 );

		std::string getConstantString();
		float getConstantFloat();
		bool isConstant();

		std::string getConstantValueOp2ToString();

	};

	#pragma pack(pop)

	struct InstructionSourceInfo {

		const char * file;
		short line;
		std::string * ptr_str_symbol_name; // should be deallocated

		InstructionSourceInfo(){
			ptr_str_symbol_name=NULL;
			file="unknow_file";
			line=-1;
		}

		InstructionSourceInfo(const char * _file, short _line,std::string *_ptr_str_symbol_name){
			file=_file;
			line=_line;
			ptr_str_symbol_name=_ptr_str_symbol_name;
		}
	};


	typedef Instruction *PtrInstruction;

}
