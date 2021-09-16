
#pragma once

namespace zetscript{

	struct EvalData;

	#define EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS 		0x0001
	#define EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_ATTRIB 	0x0002

	#define EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR			0x0001


	char * 	eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char * 	eval_keyword_function(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, uint16_t properties=0,Symbol ** result_symbol_function=NULL, const zs_string  & custom_symbol_name="");
	char * 	eval_keyword_var(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, uint16_t properties = 0);
	char * 	eval_keyword_if_else(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char * 	eval_keyword_while(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char * 	eval_keyword_do_while(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char * 	eval_keyword_return(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char * 	eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char * 	eval_keyword_for(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
	char *	eval_keyword_break(EvalData *eval_data,const char *s, int & line, Scope *scope_info);
	char *	eval_keyword_continue(EvalData *eval_data,const char *s, int & line, Scope *scope_info);
	char * 	eval_keyword_static(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
}

