#pragma once

#include "eval_operator.h"
#include "eval_keyword.h"

namespace zetscript{

	namespace eval{

		struct EvalInstruction{
			Instruction 					vm_instruction;
			Symbol							symbol;
			InstructionSourceInfo 			instruction_source_info;

			EvalInstruction(
				ByteCode _byte_code
				,unsigned char _index_op1=ZS_IDX_UNDEFINED
				,zs_int _index_op2=ZS_IDX_UNDEFINED
				,unsigned short _properties=0
			 ){
				vm_instruction=Instruction(_byte_code,_index_op1,_index_op2,_properties);
			}
		};

		char * 	eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,ScriptFunction *sf=NULL,std::vector<FunctionArg> * args=NULL);
		char * 	eval_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool return_on_break_or_case=false);
		Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_function=false);
		void 	eval_check_scope(EvalData *eval_data, Scope *scope);
		void 	eval_push_function(EvalData *eval_data,ScriptFunction *script_function);
		int 	eval_pop_function(EvalData *eval_data);
		void    eval_generate_byte_code_field_initializer(EvalData *data, ScriptFunction *sf, std::vector<EvalInstruction *> *instructions, Symbol *symbol_member_var);

		bool 	eval_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,std::string & static_error);
		void 	eval(ZetScript *zs,const char * str, const char *  _filename=NULL, int _line=1);
	}
}
