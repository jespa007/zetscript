/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "eval_operator.h"
#include "eval_keyword.h"

namespace zetscript{

	struct EvalInstruction{
		Instruction 					vm_instruction;
		Symbol							symbol;
		InstructionSourceInfo 			instruction_source_info;

		EvalInstruction(){
		}

		EvalInstruction(
			ByteCode _byte_code
			,uint8_t _index_op1=ZS_IDX_UNDEFINED
			,zs_int _index_op2=ZS_IDX_UNDEFINED
			,unsigned short _properties=0
		 ){
			vm_instruction=Instruction(_byte_code,_index_op1,_index_op2,_properties);
		}

		EvalInstruction(const EvalInstruction & _eval_instruction){
			vm_instruction=_eval_instruction.vm_instruction;
			symbol = _eval_instruction.symbol;
			instruction_source_info=_eval_instruction.instruction_source_info;
		}
	};

	void	eval_init();

	char * 	eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,ScriptFunction *sf=NULL,ScriptFunctionParam *params=NULL, size_t params_len=0,bool _force_push_pop=false);
	char * 	eval_parse_and_compile_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool return_on_break_or_case=false);
	Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_function=false);
	void 	eval_check_scope(EvalData *eval_data, Scope *scope, bool _force_push_pop=false);
	void 	eval_push_function(EvalData *eval_data,ScriptFunction *script_function);
	int 	eval_pop_and_compile_function(EvalData *_eval_data);
	void 	eval_pop_current_function(EvalData *eval_data);
	void    eval_generate_byte_code_field_initializer(EvalData *data, ScriptFunction *sf, zs_vector *instructions, Symbol *symbol_member_var);

	bool 	eval_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,zs_string & static_error);
	void 	eval_parse_and_compile(
			ZetScript *zs
			,const char * str_code
			,EvalData *_eval_data_from=NULL
			, const char *  _filename=""
			, int _line=1
			,ScriptFunction *sf=NULL
			);

	void	eval_deinit();

}
