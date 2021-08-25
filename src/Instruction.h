/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_IDX_INSTRUCTION_OP2_THIS							-4
#define ZS_IDX_INSTRUCTION_JMP_BREAK						(uint8_t)(-6)
#define ZS_IDX_INSTRUCTION_JMP_CONTINUE						(uint8_t)(-7)
#define ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK				-10

#define INSTRUCTION_IS_STRING(ins)							((ins)->byte_code==BYTE_CODE_LOAD_STRING || ((ins)->properties & INSTRUCTION_PROPERTY_STRING))
#define INSTRUCTION_IS_ZS_INT(ins)							((ins)->byte_code==BYTE_CODE_LOAD_ZS_INT || ((ins)->properties & INSTRUCTION_PROPERTY_ZS_INT))


// properties shared by compiler + instruction ..
typedef enum:unsigned short {

	INSTRUCTION_PROPERTY_USE_PUSH_STK				=0x0001,	// for create and store member
	INSTRUCTION_PROPERTY_ILOAD_K					=0x0002,
	INSTRUCTION_PROPERTY_ILOAD_R					=0x0004,
	INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL		=0x0008,   // for register access type
	INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER	=0x0010,   // for register access type
	INSTRUCTION_PROPERTY_ILOAD_KR					=0x0020,
	INSTRUCTION_PROPERTY_ILOAD_RK					=0x0040,
	INSTRUCTION_PROPERTY_ILOAD_RR					=0x0080,
	INSTRUCTION_PROPERTY_ZS_INT						=0x0100,
	INSTRUCTION_PROPERTY_ZS_FLOAT					=0x0200,
	INSTRUCTION_PROPERTY_BOOL						=0x0400,
	INSTRUCTION_PROPERTY_STRING						=0x0800,
	INSTRUCTION_PROPERTY_MEMBER_FUNCTION_CALLER		=0x1000

}InstructionProperty;

#define INSTRUCTION_PROPERTY_ILOAD						(INSTRUCTION_PROPERTY_ILOAD_K|INSTRUCTION_PROPERTY_ILOAD_R|INSTRUCTION_PROPERTY_ILOAD_KR|INSTRUCTION_PROPERTY_ILOAD_RK|INSTRUCTION_PROPERTY_ILOAD_RR)

#define INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(properties)\
	 (properties) & INSTRUCTION_PROPERTY_ZS_INT ? STK_PROPERTY_ZS_INT \
	:(properties) & INSTRUCTION_PROPERTY_BOOL ? STK_PROPERTY_BOOL \
	:(properties) & INSTRUCTION_PROPERTY_ZS_FLOAT ? STK_PROPERTY_ZS_FLOAT \
	:(properties) & INSTRUCTION_PROPERTY_STRING ? STK_PROPERTY_PTR_STK \
	:0


namespace zetscript{

	#pragma pack(push, 1)


	struct Instruction {

		ByteCode byte_code;
		uint8_t  value_op1;
		zs_int 	value_op2;
		uint16_t properties;

		Instruction();

		Instruction(ByteCode _byte_code
		 ,uint8_t _value_op1=ZS_IDX_UNDEFINED
		 ,zs_int _value_op2=ZS_IDX_UNDEFINED
		 ,uint16_t _properties=0
		 );

		std::string getConstantString();
		zs_float getConstantFloat();
		zs_int getConstantInt();
		bool isConstant();

		std::string getConstantValueOp2ToString();


	};

	#pragma pack(pop)

	struct InstructionSourceInfo {

		const char *file;
		short line;
		std::string * ptr_str_symbol_name; // should be deallocated

		InstructionSourceInfo(){
			ptr_str_symbol_name=NULL;
			file="";
			line=-1;
		}

		InstructionSourceInfo(const char * _file,short _line,std::string *_ptr_str_symbol_name=NULL){
			file=_file;
			line=_line;
			ptr_str_symbol_name=_ptr_str_symbol_name;
		}
	};

	typedef Instruction *PtrInstruction;

}
