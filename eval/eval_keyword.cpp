#include "eval.h"

namespace zetscript{

	char * eval_keyword_static(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
		char *aux_p = (char *)s;
		Keyword key_w;
		ScriptClass *sc=NULL;

		// check class scope...
		if(eval_is_keyword(aux_p)==Keyword::KEYWORD_STATIC){

			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[Keyword::KEYWORD_STATIC].str),line);

			key_w=eval_is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_FUNCTION){
				return eval_keyword_function(eval_data,s,line,scope_info);
			}

			else if(key_w == Keyword::KEYWORD_VAR){
				return eval_keyword_var(eval_data,s,line,scope_info);
			}else{ // not supported
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"expected \"var\" or \"function\" after \"static\"");
			}
		}

		return NULL;
	}


	char *eval_keyword(EvalData *eval_data,const char *s, int & line, Scope *scope_info, Keyword keyw){
		char *aux_p= (char *)s;

		// check if condition...
		if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...

			switch(keyw){
			case KEYWORD_STATIC:
			case KEYWORD_FUNCTION:
				return  eval_keyword_function(eval_data,s,line,scope_info);
				break;
			case KEYWORD_CLASS:
				return  eval_keyword_class(eval_data,s,line,scope_info);
				break;
			case KEYWORD_CONST:
			case KEYWORD_VAR:
				return  eval_keyword_var(eval_data,s,line,scope_info);
				break;
			default:
				if(eval_data_keywords[keyw].eval_fun != NULL){
					return (*eval_data_keywords[keyw].eval_fun)(eval_data,s,line,scope_info);
				}else{
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected '%s' keyword",eval_data_keywords[keyw].str);
				}
				break;
			}
		}
		return NULL; // is not keyword
	}
}

