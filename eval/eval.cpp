/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr ZS_LOG_DEBUG
#else
#define print_eval_cr(s,...)
#endif

#include "eval_operator.cpp"
#include "eval_data.cpp"
#include "eval_object.cpp"
#include "eval_expression_optimize.cpp"
#include "eval_expression_byte_code.cpp"
#include "eval_symbol.cpp"
#include "eval_expression_token_symbol.cpp"
#include "eval_keyword_class.cpp"
#include "eval_keyword_conditional.cpp"
#include "eval_keyword_loop.cpp"
#include "eval_keyword_var_function.cpp"
#include "eval_expression.cpp"
#include "eval_keyword.cpp"

namespace zetscript{

	void eval_parse_and_compile(ZetScript *zs
			, ScriptFunction *sf
			,const char * str
			, const char *  _filename
			, int _line
		){
		EvalData *eval_data=new EvalData(zs);
		char *aux_p=NULL;
		int line =_line;
		bool error;
		std::string error_str;
		eval_data->current_parsing_file=_filename;
		eval_push_function(eval_data,sf);
		aux_p=eval_parse_and_compile_recursive(eval_data,str,line,MAIN_SCOPE(eval_data));
		if(aux_p!=NULL){
			if(*aux_p=='}'){
				eval_data->error=true;
				eval_data->error_str=ZS_LOG_FILE_LINE_STR(_filename,line)+zetscript::zs_strutils::format("unexpected ending block ('}')");
			}
		}

		eval_pop_and_compile_function(eval_data);

		error=eval_data->error;
		error_str=eval_data->error_str;

		delete eval_data;

		if(error){
			THROW_EXCEPTION(error_str);
		}
	}

/*	void eval_parse_and_compile_anonymous(ZetScript *zs,const char * str, const char *  _filename, int _line){
		ScriptFunction *sf_main=zs->getScriptFunctionFactory()->getScriptFunction(IDX_SCRIPT_FUNCTION_MAIN);
		Scope *sc_main=zs->getScopeFactory()->getMainScope();
		// 1. create a function from main
		eval_parse_and_compile(zs,str,_filename,_line,NULL,sc_main);
	}

	void eval_parse_and_compile_main(ZetScript *zs,const char * str, const char *  _filename, int _line){
		ScriptFunction *sf_main=zs->getScriptFunctionFactory()->getScriptFunction(IDX_SCRIPT_FUNCTION_MAIN);
		Scope *sc_main=zs->getScopeFactory()->getMainScope();
		eval_parse_and_compile(zs,str,_filename,_line,sf_main,sc_main);
	}*/

	Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_scope_function){
		Scope *new_scope = NEW_SCOPE(eval_data,eval_data->current_function->script_function->idx_script_function,scope_parent);
		scope_parent->registered_scopes->push_back((zs_int)new_scope);
		new_scope->is_scope_function=is_scope_function;
		if(is_scope_function){
			new_scope->tmp_idx_instruction_push_scope=0;
		}
		else{
			new_scope->tmp_idx_instruction_push_scope=(int)eval_data->current_function->instructions.size();
		}

		return new_scope;
	}

	void eval_check_scope(EvalData *eval_data, Scope *scope){
		if(scope->n_registered_symbols_as_variables > 0){ // if there's local symbols insert push/pop scope for there symbols
			if(scope->tmp_idx_instruction_push_scope!=ZS_IDX_UNDEFINED){
				eval_data->current_function->instructions.insert(
						eval_data->current_function->instructions.begin()+scope->tmp_idx_instruction_push_scope
						,new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(zs_int)scope)
				);

				//inc_jmp_codes(eval_data,idx_instruction_start,(int)(eval_data->current_function->instructions.size()),1);

				// and finally insert pop scope
				eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_POP_SCOPE,0));
			}
		}
		else{ // remove scope
			scope->markAsUnusued();
		}
	}

	char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, ScriptFunction *sf,std::vector<ScriptFunctionArg> * args){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;

		Scope *new_scope_info=  NULL;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		if(*aux_p == '{'){
			bool is_function = sf!=NULL && args != NULL;
			//int idx_instruction_start_block=(int)(eval_data->current_function->instructions.size());

			aux_p++;

			new_scope_info = eval_new_scope(eval_data,scope_info,is_function); // special case... ast is created later ...

			if(is_function){
				// register args as part of stack...
				for(unsigned i=0; i < args->size(); i++){
					try{
						sf->registerLocalArgument(
							new_scope_info
							,eval_data->current_parsing_file
							,args->at(i).line
							,args->at(i).arg_name
							,args->at(i).properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF?SYMBOL_PROPERTY_ARG_BY_REF:0
						);


					}catch(std::exception & ex){
						eval_data->error=true;
						eval_data->error_str=ex.what();
						return NULL;
					}
				}
			}

			if((aux_p = eval_parse_and_compile_recursive(
					eval_data
					,aux_p
					, line
					, new_scope_info
			)) != NULL){

				if(*aux_p != '}'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '}' ");
				}

				eval_check_scope(eval_data,new_scope_info);
				return aux_p+1;
			}
		}
		return NULL;
	}

	char * eval_parse_and_compile_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool return_on_break_or_case){
		// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)
		bool custom_quit = false;
		char *aux = (char *)s;
		char *end_expr=0;

		Keyword keyw;
		Directive directive;

		IGNORE_BLANKS(aux,eval_data,aux, line);

		while(*aux != 0 && !custom_quit){

			// ignore all ;
			while(*aux==';' && *aux != 0){
				IGNORE_BLANKS(aux,eval_data,aux+1, line);
			}

			if(*aux==0){ // custom case exit..
				return aux;
			}

			keyw=eval_is_keyword(aux);

			if(*aux == '}'){ // ending block trivial cases...
				return aux;
			}else if(keyw != Keyword::KEYWORD_UNKNOWN && keyw < KEYWORDS_WITHIN_EXPRESSIONS && (is_anonymous_function(eval_data,aux,line)==false)){ // it cuts current expression to link breaks...

				if(((keyw == Keyword::KEYWORD_BREAK) || (keyw == Keyword::KEYWORD_CASE)|| (keyw == Keyword::KEYWORD_DEFAULT)) && return_on_break_or_case){
					return aux;
				}

				end_expr = eval_keyword(
					eval_data
					,aux
					, line
					, scope_info
					, keyw
				);

			}else if((directive = is_directive(aux)) != Directive::DIRECTIVE_UNKNOWN){ // eval directive
				// try directive ...
				char *start_var,* end_var;
				std::string str_symbol;
				if(directive != Directive::DIRECTIVE_UNKNOWN){
					switch(directive){
					case DIRECTIVE_INCLUDE:
						aux += strlen(eval_data_directives[directive].str);
						IGNORE_BLANKS(aux,eval_data,aux,line);
						if(*aux != '\"'){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"expected starting \" directive");
						}
						aux++;
						start_var=aux;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"expected end \" directive");
						}

						end_var=aux;

						zs_strutils::copy_from_ptr_diff(str_symbol,start_var,end_var);

						ZS_LOG_DEBUG("include file: %s",str_symbol);

						// save current file info...
						eval_data->zs->evalFile(str_symbol);

						aux++;// advance ..
						break;
					default:
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"directive \"%s\" not supported",eval_data_directives[directive].str);
						break;
					}

					end_expr=aux;
				}
			}else if(*aux == '{') { // eval block

				// 2nd. check whether eval a block
				end_expr = eval_block(
					eval_data
					,aux
					,line
					, scope_info
				);

			}else{ // eval expression

				end_expr = eval_expression(
					eval_data
					,aux
					,line
					, scope_info
					,&eval_data->current_function->instructions
					,{}
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_ALLOW_SEQUENCE_ASSIGNMENT | EVAL_EXPRESSION_ON_MAIN_BLOCK
				);
			}

			if(end_expr == NULL){
				return NULL;
			}

			aux=end_expr;
			IGNORE_BLANKS(aux,eval_data,aux, line);
		}
		return aux;
	}

	void eval_push_function(EvalData *eval_data,ScriptFunction *script_function){
		eval_data->functions.push_back(eval_data->current_function=new EvalFunction(script_function));
	}

	Symbol *eval_find_local_symbol(EvalData *eval_data,Scope *scope, const std::string & symbol_to_find){

		EvalFunction *sf=eval_data->current_function;
		Symbol * sc_var = scope->getSymbol(symbol_to_find, NO_PARAMS_SYMBOL_ONLY,ScopeDirection::SCOPE_DIRECTION_DOWN);

		if(sc_var != NULL){ // local symbol found

			if(sc_var->n_params==NO_PARAMS_SYMBOL_ONLY){ // symbol is variable...
				return sf->script_function->getSymbol(sc_var->scope,symbol_to_find);
			}
			else{ // symbol is function...
				return sf->script_function->getSymbol(sc_var->scope,symbol_to_find,NO_PARAMS_SYMBOL_ONLY);
			}
		}

		// try find global variable...

		return NULL;
	}

	Symbol *eval_find_global_symbol(EvalData *eval_data, const std::string & symbol_to_find){

		// try find global variable...
		return eval_data->zs->getScriptClassFactory()->getMainFunction()->getSymbol(MAIN_SCOPE(eval_data),symbol_to_find);

	}

	void eval_pop_current_function(EvalData *eval_data){
		// delete and popback function information...
		delete eval_data->current_function;
		eval_data->functions.pop_back();

		eval_data->current_function = NULL;
		if(eval_data->functions.size() > 0){
			eval_data->current_function = eval_data->functions.at(eval_data->functions.size()-1);
		}

	}

	int eval_pop_and_compile_function(EvalData *eval_data){

		std::string static_error;
		ScriptFunction *sf = eval_data->current_function->script_function;
		ScriptClass *sf_class = GET_SCRIPT_CLASS(eval_data,sf->idx_class);

		if(sf->instructions != NULL){
			free(sf->instructions);
			sf->instructions=NULL;
		}

		// get total size op + 1 ends with 0 (INVALID BYTE_CODE)
		//eval_data->current_function->instructions.size() + 1
		size_t len=eval_data->current_function->instructions.size() + 1; // +1 for end instruction
		size_t total_size_bytes = (len) * sizeof(Instruction);
		sf->instructions_len=len;
		sf->instructions = (PtrInstruction)malloc(total_size_bytes);
		memset(sf->instructions, 0, total_size_bytes);
		//bool is_static = eval_data->current_function->script_function->symbol.properties & SYMBOL_PROPERTY_STATIC;
		int ok=TRUE;
		//int idx_instruction=0;


		for(unsigned i=0; i < eval_data->current_function->instructions.size(); i++){

			Symbol *vis=NULL;
			EvalInstruction *instruction = eval_data->current_function->instructions[i];
			std::string *ptr_str_symbol_to_find=NULL;
			ScriptClass *sc_aux=NULL;
			//bool is_local=false;
			ptr_str_symbol_to_find=&instruction->symbol.name;

			//is_static&=((instruction->vm_instruction.properties & INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)==0);
			switch(instruction->vm_instruction.byte_code){
			case BYTE_CODE_LOAD_ELEMENT_THIS:
				// try to solve symbol...
					if(*ptr_str_symbol_to_find == SYMBOL_VALUE_SUPER){
						// get current function name and find first ancestor in heritance
						Symbol *symbol_sf_foundf=NULL;
						std::string target_name;

						bool is_constructor = sf->symbol.name == sf_class->symbol_class.name;

						for(int i = sf->symbol.idx_position-1; i >=0 && symbol_sf_foundf==NULL; i--){
							Symbol *symbol_member = (Symbol *)sf_class->symbol_members->items[i];
							bool match_names=false;
							if(is_constructor==true){
								if(symbol_member->scope == NULL){ // is constant...
									continue;
								}
								match_names=symbol_member->scope->script_class->symbol_class.name==symbol_member->name;
							}else{
								match_names=symbol_member->name==sf->symbol.name;
							}

							if(symbol_member->properties & SYMBOL_PROPERTY_FUNCTION){
								ScriptFunction *sf_member=(ScriptFunction *)symbol_member->ref_ptr;
								bool match_params=(symbol_member->properties & SYMBOL_PROPERTY_C_OBJECT_REF?match_names:true);
								if(
										(match_names)
									&& (match_params)
									){
									symbol_sf_foundf = symbol_member;
								}
							}
						}

						// ok get the super function...
						if(symbol_sf_foundf == NULL){
							EVAL_ERROR_POP_FUNCTION(
									instruction->instruction_source_info.file
									,instruction->instruction_source_info.line
									,"Cannot find parent function %s::%s"
									,sf_class->symbol_class.name.c_str()
									,sf->symbol.name.c_str()
							);
						}
						instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_MEMBER_VAR;
						instruction->vm_instruction.value_op2=symbol_sf_foundf->idx_position;
						instruction->instruction_source_info.ptr_str_symbol_name =get_mapped_name(eval_data,std::string(symbol_sf_foundf->scope->script_class->symbol_class.name)+"::"+symbol_sf_foundf->name);

					}else{ // is "this" symbol, check whether symbol is member

						if(instruction->vm_instruction.value_op2 == ZS_IDX_UNDEFINED){
							// is automatically created on vm...
							Symbol *symbol_function=sf_class->getSymbol(*ptr_str_symbol_to_find,ANY_PARAMS_SYMBOL_ONLY);
							if(symbol_function!=NULL){
								// functions always loads dynamically because we can have an override function
								// so we don't load as member in a fix position else is a member variable ...
								if((symbol_function->properties & SYMBOL_PROPERTY_FUNCTION) == 0){ // if not function set as load immediate
									instruction->vm_instruction.value_op2=symbol_function->idx_position;
									instruction->vm_instruction.byte_code=ByteCode::BYTE_CODE_LOAD_MEMBER_VAR; // immediate load
								}
							}
						}
					}
				//}
					break;
			case BYTE_CODE_FIND_VARIABLE:
				if(instruction->symbol.scope != MAIN_SCOPE(eval_data)){ // find global symbol if not global
					vis = eval_find_global_symbol(eval_data,*ptr_str_symbol_to_find);
				}else{ // give the error properly

					char *str_start_class=(char *)ptr_str_symbol_to_find->c_str();
					char *str_end_class=NULL;

					if((str_end_class=strstr(str_start_class,"::"))!=NULL){
						char class_name[512]={0};
						strncpy(class_name,str_start_class,str_end_class-str_start_class);

						EVAL_ERROR_POP_FUNCTION(
								instruction->instruction_source_info.file
								,instruction->instruction_source_info.line
								,"static symbol '%s' not exist in '%s'"
								//,sf_class->symbol_class.name.c_str()
								,str_end_class+2
								,class_name
						);
					}
					else{
						EVAL_ERROR_POP_FUNCTION(
								instruction->instruction_source_info.file
								,instruction->instruction_source_info.line
								,"symbol '%s' not defined"
								//,sf_class->symbol_class.name.c_str()
								,ptr_str_symbol_to_find->c_str()
						);
					}

				}
				//}

				if(vis != NULL){
					if(vis->properties & SYMBOL_PROPERTY_FUNCTION){
						instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_FUNCTION;
						instruction->vm_instruction.value_op2=(zs_int)(ScriptFunction *)vis->ref_ptr; // store script function
					}
					else{ // global variable

						if(instruction->vm_instruction.properties & INSTRUCTION_PROPERTY_USE_PUSH_STK){
							instruction->vm_instruction.byte_code=BYTE_CODE_PUSH_STK_GLOBAL;
						}else{
							instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_GLOBAL;
						}

						instruction->vm_instruction.value_op2=vis->idx_position;
					}
				}
				break;
			}

			// save instruction ...
			sf->instructions[i]=instruction->vm_instruction;

			//------------------------------------
			// symbol value to save at runtime ...
			InstructionSourceInfo instruction_info=instruction->instruction_source_info;

			// Save str_symbol that was created on eval process, and is destroyed when eval finish.
			instruction_info.ptr_str_symbol_name=instruction->instruction_source_info.ptr_str_symbol_name;


			sf->instruction_source_info[i]=instruction_info;
		}

lbl_exit_pop_function:

		eval_pop_current_function(eval_data);

		return ok;
	}
}
