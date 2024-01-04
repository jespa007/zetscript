/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "compiler_operator.h"
#include "compiler_keyword.h"

namespace zetscript{

	struct CompilerInstruction{
		Instruction 					vm_instruction;
		String						symbol_name;
		ScriptScope							*symbol_scope;
		InstructionSourceInfo 			instruction_source_info;

		CompilerInstruction(){
			symbol_scope=NULL;
		}

		CompilerInstruction(
			ByteCode _byte_code
			,uint8_t _index_op1=ZS_UNDEFINED_IDX
			,zs_int _index_op2=ZS_UNDEFINED_IDX
			,uint16_t _properties=0
		 ){
			vm_instruction=Instruction(_byte_code,_index_op1,_index_op2,_properties);
			symbol_scope=NULL;
		}

		CompilerInstruction(const CompilerInstruction & _compiler_instruction){
			vm_instruction=_compiler_instruction.vm_instruction;
			symbol_name = _compiler_instruction.symbol_name;
			symbol_scope= _compiler_instruction.symbol_scope;
			instruction_source_info=_compiler_instruction.instruction_source_info;
		}
	};

	void	compiler_init();

	char * compiler_block_function(

			CompilerData *compiler_data
			,const char *s
			,int & line
			,  ScriptScope *scope_info
			, ScriptFunction *sf
			,FunctionParam *params
			, int params_len
	);

	char * compiler_block_body(

			CompilerData *compiler_data
			,const char *s
			,int & line
			,  ScriptScope *scope_info
			, ScriptScope *_new_scope=NULL
			, bool _is_block_body_loop=false
	);

	String  compiler_copy_const_char_diff(const char *p1, const char *p2);
	char * 	compiler_compile_recursive(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info, bool return_on_break_or_case=false);
	ScriptScope * compiler_new_scope_function(CompilerData *compiler_data, ScriptScope *scope_parent);
	ScriptScope * compiler_new_scope_block(CompilerData *compiler_data, ScriptScope *scope_parent);
	void 	compiler_check_scope(CompilerData *compiler_data, ScriptScope *scope, bool _force_push_pop=false);
	void 	compiler_push_function(CompilerData *compiler_data,ScriptFunction *script_function);
	int 	compiler_pop_and_compile_function(CompilerData *_compiler_data);
	void 	compiler_pop_current_function(CompilerData *compiler_data);
	void    compiler_generate_byte_code_field_initializer(
			CompilerData *_compiler_data
			, ScriptFunction *_sf
			, int _line
			,const String & _symbol_name
			, Vector<CompilerInstruction *> *_compiler_instructions
	);

	bool 	compiler_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,String & static_error);
	void 	compiler_compile_and_link(
			zetscript::ScriptEngine *_script_engine
			,const char * _str_code
			,CompilerData *_compiler_data_from=NULL
			, const char *  _filename=""
			, int _line=1
			,ScriptFunction *sf=NULL
			);

	void	compiler_deinit();

}
