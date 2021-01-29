#pragma once

#include "eval_operator.h"
#include "eval_keyword.h"

namespace zetscript{

	namespace eval{

		struct EvalInstruction;

		char * 	eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
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
