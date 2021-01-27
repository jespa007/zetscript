
#pragma once

namespace zetscript{
	namespace eval{

		struct EvalData;

		char * 	eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_function(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool check_anonymous_function=false, std::string * function_name_result=NULL);
		char * 	eval_keyword_var(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
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
}
