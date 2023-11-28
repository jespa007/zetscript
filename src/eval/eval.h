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
		String						symbol_name;
		Scope							*symbol_scope;
		InstructionSourceInfo 			instruction_source_info;

		EvalInstruction(){
			symbol_scope=NULL;
		}

		EvalInstruction(
			ByteCode _byte_code
			,uint8_t _index_op1=ZS_UNDEFINED_IDX
			,zs_int _index_op2=ZS_UNDEFINED_IDX
			,uint16_t _properties=0
		 ){
			vm_instruction=Instruction(_byte_code,_index_op1,_index_op2,_properties);
			symbol_scope=NULL;
		}

		EvalInstruction(const EvalInstruction & _eval_instruction){
			vm_instruction=_eval_instruction.vm_instruction;
			symbol_name = _eval_instruction.symbol_name;
			symbol_scope= _eval_instruction.symbol_scope;
			instruction_source_info=_eval_instruction.instruction_source_info;
		}
	};

	void	eval_init();

	char * eval_block_function(

			EvalData *eval_data
			,const char *s
			,int & line
			,  Scope *scope_info
			, Function *sf
			,FunctionParam *params
			, int params_len
	);

	char * eval_block_body(

			EvalData *eval_data
			,const char *s
			,int & line
			,  Scope *scope_info
			, Scope *_new_scope=NULL
			, bool _is_block_body_loop=false
	);

	String  eval_copy_from_const_char_diff(const char *p1, const char *p2);
	char * 	eval_parse_and_compile_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool return_on_break_or_case=false);
	Scope * eval_new_scope_function(EvalData *eval_data, Scope *scope_parent);
	Scope * eval_new_scope_block(EvalData *eval_data, Scope *scope_parent);
	void 	eval_check_scope(EvalData *eval_data, Scope *scope, bool _force_push_pop=false);
	void 	eval_push_function(EvalData *eval_data,Function *script_function);
	int 	eval_pop_and_compile_function(EvalData *_eval_data);
	void 	eval_pop_current_function(EvalData *eval_data);
	void    eval_generate_byte_code_field_initializer(
			EvalData *_eval_data
			, Function *_sf
			, int _line
			,const String & _symbol_name
			, Vector<EvalInstruction *> *_eval_instructions
	);

	bool 	eval_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,String & static_error);
	void 	eval_parse_and_compile(
			ZetScript *zs
			,const char * str_code
			,EvalData *_eval_data_from=NULL
			, const char *  _filename=""
			, int _line=1
			,Function *sf=NULL
			);

	void	eval_deinit();

}
