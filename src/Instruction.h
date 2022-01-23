/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_IDX_INSTRUCTION_OP2_THIS							-4
#define ZS_IDX_INSTRUCTION_JMP_BREAK						(uint8_t)(-6)
#define ZS_IDX_INSTRUCTION_JMP_CONTINUE						(uint8_t)(-7)


#define INSTRUCTION_IS_STRING(ins)							((ins)->byte_code==BYTE_CODE_LOAD_STRING || ((ins)->properties & INSTRUCTION_PROPERTY_STRING))
#define INSTRUCTION_IS_ZS_INT(ins)							((ins)->byte_code==BYTE_CODE_LOAD_ZS_INT || ((ins)->properties & INSTRUCTION_PROPERTY_ZS_INT))


// properties shared by compiler + instruction ..
typedef enum:unsigned short {

	INSTRUCTION_PROPERTY_USE_PUSH_STK				=0x0001,	// for create and store member
	INSTRUCTION_PROPERTY_ILOAD_K					=0x0002,
	INSTRUCTION_PROPERTY_ILOAD_R					=0x0004,
	//INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL		=0x0008,   // for register access type
	INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR	=0x0010,   // for register access type
	INSTRUCTION_PROPERTY_ILOAD_KR					=0x0020,
	INSTRUCTION_PROPERTY_ILOAD_RK					=0x0040,
	INSTRUCTION_PROPERTY_ILOAD_RR					=0x0080,
	INSTRUCTION_PROPERTY_ZS_INT						=0x0100,
	INSTRUCTION_PROPERTY_ZS_FLOAT					=0x0200,
	INSTRUCTION_PROPERTY_BOOL						=0x0400,
	INSTRUCTION_PROPERTY_STRING						=0x0800,
	INSTRUCTION_PROPERTY_RESET_STACK				=0x2000

}InstructionProperty;

#define INSTRUCTION_PROPERTY_ILOAD									(INSTRUCTION_PROPERTY_ILOAD_K|INSTRUCTION_PROPERTY_ILOAD_R|INSTRUCTION_PROPERTY_ILOAD_KR|INSTRUCTION_PROPERTY_ILOAD_RK|INSTRUCTION_PROPERTY_ILOAD_RR)
//#define INSTRUCTION_PROPERTY_ILOAD_ACCESS_IS_GLOBAL(properties) 	((properties) & (INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) == 0)

#define INSTRUCTION_IS_BYTE_CODE_CALL(instruction) (\
 ((instruction)->byte_code == BYTE_CODE_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_INDIRECT_GLOBAL_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_INDIRECT_LOCAL_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_THIS_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_THIS_MEMBER_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_MEMBER_CALL) \
 || ((instruction)->byte_code == 		BYTE_CODE_UNRESOLVED_CALL) \
 ||	((instruction)->byte_code == 		BYTE_CODE_UNRESOLVED_THIS_CALL) \
)

#define INSTRUCTION_CONST_TO_STK_CONST_VALUE(val,properties)\
	((properties) & (INSTRUCTION_PROPERTY_ZS_INT | INSTRUCTION_PROPERTY_BOOL | INSTRUCTION_PROPERTY_ZS_FLOAT) ) ? val \
	:((properties) & INSTRUCTION_PROPERTY_STRING) ? (((StackElement *)(val))->value) \
	:0

#define INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(properties)\
	 (properties) & INSTRUCTION_PROPERTY_ZS_INT ? STK_PROPERTY_ZS_INT \
	:(properties) & INSTRUCTION_PROPERTY_BOOL ? STK_PROPERTY_BOOL \
	:(properties) & INSTRUCTION_PROPERTY_ZS_FLOAT ? STK_PROPERTY_ZS_FLOAT \
	:(properties) & INSTRUCTION_PROPERTY_STRING ? (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY) \
	:0

#define INSTRUCTION_GET_PARAMETER_COUNT(instruction) 	((instruction)->value_op1 & 0x0f)
#define INSTRUCTION_GET_RETURN_COUNT(instruction) 		(((instruction)->value_op1 & 0xf0)>>4)

#define INSTRUCTION_VALUE_OP1_RETURN_COUNT(return_count) \
		(((return_count)&0xf)<<4)

#define INSTRUCTION_VALUE_OP1_PARAMETER_COUNT(parameter_count) \
		((parameter_count)&0xf)

#define INSTRUCTION_SET_VALUE_OP1_RETURN_PARAMETER_COUNT(return_count,parameter_count) \
		(((return_count & 0x0f)<<0x4) | ((parameter_count)&0x0f))

#define INSTRUCTION_SET_RETURN_COUNT(instruction, return_count) \
		(instruction)->value_op1&=(0x0f); \
		(instruction)->value_op1|=INSTRUCTION_VALUE_OP1_RETURN_COUNT(return_count);

#define INSTRUCTION_SET_PARAMETER_COUNT(instruction, parameter_count) \
		(instruction)->value_op1&=(0xf0); \
		(instruction)->value_op1|=INSTRUCTION_VALUE_OP1_PARAMETER_COUNT(parameter_count);


namespace zetscript{

	#pragma pack(push, 1)


	struct Instruction {

		ByteCode byte_code;
		uint8_t  value_op1;
		zs_int 	value_op2;
		uint16_t properties;

		Instruction();

		Instruction(ByteCode _byte_code
		 ,uint8_t _value_op1=IDX_ZS_UNDEFINED
		 ,zs_int _value_op2=IDX_ZS_UNDEFINED
		 ,uint16_t _properties=0
		 );

		zs_string getConstantString();
		zs_float getConstantFloat();
		zs_int getConstantInt();
		bool isConstant();

		zs_string getConstantValueOp2ToString();


	};

	#pragma pack(pop)

	class InstructionSourceInfo {
	public:

		const char *file;
		short line;
		zs_string * ptr_str_symbol_name; // should be deallocated

		InstructionSourceInfo(){
			ptr_str_symbol_name=NULL;
			file="";
			line=-1;
		}

		InstructionSourceInfo(const char * _file,short _line,zs_string *_ptr_str_symbol_name=NULL){
			file=_file;
			line=_line;
			ptr_str_symbol_name=_ptr_str_symbol_name;
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
