/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_IDX_INSTRUCTION_OP2_THIS							-4
#define ZS_IDX_INSTRUCTION_JMP_BREAK						(uint8_t)(-6)
#define ZS_IDX_INSTRUCTION_JMP_CONTINUE						(uint8_t)(-7)

#define ZS_IDX_INSTRUCTION_JNT_LOGIC_NEXT_OR				(uint8_t)(-8)
#define ZS_IDX_INSTRUCTION_JT_LOGIC_OK						(uint8_t)(-9)

#define ZS_IDX_INSTRUCTION_PUSH_STK_GLOBAL_IRGO				(uint8_t)(-10)

#define	ZS_NUM_REQUIRED_BYTE_CODE_ID_NOT_MANAGED				1024

#define ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED					(uint8_t)(ZS_UNDEFINED_IDX)


#define ZS_INSTRUCTION_IS_STRING(ins)							((ins)->byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING || ((ins)->properties & ZS_INSTRUCTION_PROPERTY_STRING))
#define ZS_INSTRUCTION_IS_INT(ins)							((ins)->byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT || ((ins)->properties & ZS_INSTRUCTION_PROPERTY_INT))


// properties shared by compiler + instruction ..
typedef enum:unsigned short {

	ZS_INSTRUCTION_PROPERTY_USE_PUSH_STK				=0x0001,	// for create and store member
	ZS_INSTRUCTION_PROPERTY_ILOAD_K					=0x0002,
	ZS_INSTRUCTION_PROPERTY_ILOAD_R					=0x0004,
	//ZS_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL		=0x0008,   // for register access type
	ZS_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR	=0x0010,   // for register access type
	ZS_INSTRUCTION_PROPERTY_ILOAD_KR					=0x0020,
	ZS_INSTRUCTION_PROPERTY_ILOAD_RK					=0x0040,
	ZS_INSTRUCTION_PROPERTY_ILOAD_RR					=0x0080,
	ZS_INSTRUCTION_PROPERTY_INT						=0x0100,
	ZS_INSTRUCTION_PROPERTY_FLOAT					=0x0200,
	ZS_INSTRUCTION_PROPERTY_BOOL						=0x0400,
	ZS_INSTRUCTION_PROPERTY_STRING						=0x0800,
	ZS_INSTRUCTION_PROPERTY_RESET_STACK				=0x2000,
	ZS_INSTRUCTION_PROPERTY_CALLING_FUNCTION			=0x4000,
	ZS_INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT	=0x8000,

}InstructionProperty;

#define ZS_INSTRUCTION_PROPERTY_FOR_ASSIGN								(ZS_INSTRUCTION_PROPERTY_USE_PUSH_STK | ZS_INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT)
#define ZS_INSTRUCTION_PROPERTY_ILOAD									(ZS_INSTRUCTION_PROPERTY_ILOAD_K|ZS_INSTRUCTION_PROPERTY_ILOAD_R|ZS_INSTRUCTION_PROPERTY_ILOAD_KR|ZS_INSTRUCTION_PROPERTY_ILOAD_RK|ZS_INSTRUCTION_PROPERTY_ILOAD_RR)
//#define ZS_INSTRUCTION_PROPERTY_ILOAD_ACCESS_IS_GLOBAL(properties) 	((properties) & (ZS_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL | ZS_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) == 0)


#define ZS_INSTRUCTION_CONST_TO_STK_CONST_VALUE(val,properties)\
	((properties) & (ZS_INSTRUCTION_PROPERTY_INT | ZS_INSTRUCTION_PROPERTY_BOOL | ZS_INSTRUCTION_PROPERTY_FLOAT) ) ? val \
	:((properties) & ZS_INSTRUCTION_PROPERTY_STRING) ? (((StackElement *)(val))->value) \
	:0

#define ZS_INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(properties)\
	 (properties) & ZS_INSTRUCTION_PROPERTY_INT ? ZS_STK_PROPERTY_INT \
	:(properties) & ZS_INSTRUCTION_PROPERTY_BOOL ? ZS_STK_PROPERTY_BOOL \
	:(properties) & ZS_INSTRUCTION_PROPERTY_FLOAT ? ZS_STK_PROPERTY_FLOAT \
	:(properties) & ZS_INSTRUCTION_PROPERTY_STRING ? (ZS_STK_PROPERTY_SCRIPT_OBJECT | ZS_STK_PROPERTY_READ_ONLY) \
	:0

#define ZS_INSTRUCTION_GET_PARAMETER_COUNT(instruction) 	((instruction)->value_op1 & 0x0f)
#define ZS_INSTRUCTION_GET_RETURN_COUNT(instruction) 		(((instruction)->value_op1 & 0xf0)>>4)

#define ZS_INSTRUCTION_VALUE_OP1_RETURN_COUNT(return_count) \
		(((return_count)&0xf)<<4)

#define ZS_INSTRUCTION_VALUE_OP1_PARAMETER_COUNT(parameter_count) \
		((parameter_count)&0xf)

#define ZS_INSTRUCTION_SET_VALUE_OP1_RETURN_AND_PARAMETER_COUNT(return_count,parameter_count) \
		(((return_count & 0x0f)<<0x4) | ((parameter_count)&0x0f))

#define ZS_INSTRUCTION_SET_RETURN_COUNT(instruction, return_count) \
		(instruction)->value_op1&=(0x0f); \
		(instruction)->value_op1|=ZS_INSTRUCTION_VALUE_OP1_RETURN_COUNT(return_count);

#define ZS_INSTRUCTION_SET_PARAMETER_COUNT(instruction, parameter_count) \
		(instruction)->value_op1&=(0xf0); \
		(instruction)->value_op1|=ZS_INSTRUCTION_VALUE_OP1_PARAMETER_COUNT(parameter_count);


namespace zetscript{

#ifndef __arm__
// Instruction structure is unaligned structure due pragma(1) and in ARM environment it derives a
// seg fault bus error when converts from float from intptr_t so in ARM platform is disabled. Another
// option could be align the structure putting value op2 at the beginning of Instruction, to ensure the
// conversion from float from intptr_t without error.
#pragma pack(push, 1)
#endif

	struct Instruction {

		ByteCode::ByteCodeId byte_code;
		uint8_t  value_op1;
		zs_int 	value_op2;
		uint16_t properties;

		Instruction();

		Instruction(ByteCode::ByteCodeId _byte_code
		 ,uint8_t _value_op1=ZS_UNDEFINED_IDX
		 ,zs_int _value_op2=ZS_UNDEFINED_IDX
		 ,uint16_t _properties=0
		 );

		zs_string getConstantString();
		zs_float getConstantFloat();
		zs_int getConstantInt();
		bool isConstant();

		zs_string getConstantValueOp2ToString( bool _str_with_quotes=true);


	};
#ifndef __arm__
#pragma pack(pop)
#endif

	class InstructionSourceInfo {
	public:

		const char *file;
		short line;
		const char * ptr_str_symbol_name; // should be deallocated

		InstructionSourceInfo(){
			ptr_str_symbol_name=NULL;
			file="";
			line=-1;
		}

		InstructionSourceInfo(const char * _file,short _line){
			file=_file;
			line=_line;
			ptr_str_symbol_name=NULL;
		}

		InstructionSourceInfo(const InstructionSourceInfo & _instruction_source_info){
			copy(_instruction_source_info);
		}

		InstructionSourceInfo & operator=(const InstructionSourceInfo & _instruction_source_info){
			copy(_instruction_source_info);
			return *this;
		}

	private:
		void copy(const InstructionSourceInfo & _instruction_source_info){
			ptr_str_symbol_name=_instruction_source_info.ptr_str_symbol_name;
			file=_instruction_source_info.file;
			line=_instruction_source_info.line;
		}
	};

	typedef Instruction *PtrInstruction;

	int			 instruction_num_required_stack(Instruction *_instruction);

}
