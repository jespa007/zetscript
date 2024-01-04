/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"

namespace zetscript{

	char * compiler_keyword_static(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info){
		char *aux_p = (char *)s;
		Keyword key_w;

		// check type scope...
		if(compiler_is_keyword(aux_p)==Keyword::KEYWORD_STATIC){

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[Keyword::KEYWORD_STATIC].str),line);

			key_w=compiler_is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_FUNCTION){
				return compiler_keyword_function(compiler_data,s,line,scope_info);
			}

			else if(key_w == Keyword::KEYWORD_VAR){
				return compiler_keyword_var(compiler_data,s,line,scope_info);
			}else{ // not supported
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"expected 'var' or 'function' after 'static'");
			}
		}

		return NULL;
	}


	char *compiler_keyword(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info, Keyword keyw){
		// check if condition...
		if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...

			switch(keyw){
			case KEYWORD_STATIC:
			case KEYWORD_FUNCTION:
				return  compiler_keyword_function(compiler_data,s,line,scope_info);
				break;
			case KEYWORD_CLASS:
				return  compiler_keyword_class(compiler_data,s,line,scope_info);
				break;
			case KEYWORD_CONST:
			case KEYWORD_VAR:
				return  compiler_keyword_var(compiler_data,s,line,scope_info);
				break;
			default:
				if(compiler_data_keywords[keyw].compiler_fun != NULL){
					return (*compiler_data_keywords[keyw].compiler_fun)(compiler_data,s,line,scope_info);
				}else{
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Unexpected '%s' keyword",compiler_data_keywords[keyw].str);
				}
				break;
			}
		}
		return NULL; // is not keyword
	}
}

