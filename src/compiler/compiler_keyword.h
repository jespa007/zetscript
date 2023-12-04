/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	struct CompilerData;


	// function
	typedef enum :uint16_t{
		EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS 		=0x0001
		,EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY 	=0x0002
	}EvalKeywordFunctionProperty;

	// variable
	typedef enum:uint16_t{
		EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR	=0x0001
	}EvalKeywordVariableProperty;


	char * 	compiler_keyword_delete(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char * 	compiler_keyword_function(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info, uint16_t properties=0,Symbol ** result_symbol_function=NULL, const String  & custom_symbol_name="");
	char * 	compiler_keyword_var(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info, uint16_t properties = 0);
	char * 	compiler_keyword_if_else(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char * 	compiler_keyword_while(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char * 	compiler_keyword_do_while(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char * 	compiler_keyword_return(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char * 	compiler_keyword_switch(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char * 	compiler_keyword_for(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
	char *	compiler_keyword_break(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info);
	char *	compiler_keyword_continue(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info);
	char * 	compiler_keyword_static(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info);
}

