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

	int eval_link_unresolved_symbols(EvalData *eval_data);

	void eval_parse_and_compile(ZetScript *zs
			,const char * str_code
			, const char *  _filename
			, int _line
			, ScriptFunction *_sf
			, std::vector<ScriptFunctionParam> * function_args


		){
		EvalData *eval_data=new EvalData(zs);
		char *aux_p=NULL;
		int line =_line;
		bool error;
		std::string error_str;
		std::string error_file="";
		int error_line=-1;
		Scope *scope_info=MAIN_SCOPE(eval_data);
		eval_data->current_parsing_file=_filename;
		ScriptFunction *sf = _sf == NULL?MAIN_FUNCTION(eval_data):_sf;


		if(sf != MAIN_FUNCTION(eval_data)){
			scope_info = zs->getScopeFactory()->newScope(sf->idx_script_function,MAIN_SCOPE(eval_data));
			MAIN_SCOPE(eval_data)->scopes->push_back((zs_int)scope_info);

			if(function_args != NULL){

				// register args as part of stack...
				for(unsigned i=0; i < function_args->size(); i++){

					sf->registerLocalArgument(
						scope_info
						,""
						,-1
						,function_args->at(i).param_name
						,0
					);

				}
			}
		}

		eval_push_function(eval_data,sf);

		aux_p=eval_parse_and_compile_recursive(eval_data,str_code,line,scope_info);
		if(aux_p!=NULL){
			if(*aux_p=='}'){
				eval_data->error=true;
				eval_data->error_str=ZS_LOG_FILE_LINE_STR(_filename,line)+zetscript::zs_strutils::format("unexpected ending block ('}')");
			}
		}

		if(sf != MAIN_FUNCTION(eval_data)){ // is anonyomuse function
			if(scope_info->symbol_variables->count > 0){ // if there's local symbols insert push/pop scope for there symbols

					/*eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()
							,new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(zs_int)scope_info)
					);*/

					// and finally insert pop scope
					//eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_POP_SCOPE,0));

			}
			else{ // remove scope
				scope_info->markAsUnusued();
			}
		}

		eval_pop_and_compile_function(eval_data);

		if(eval_data->error == false){
			eval_link_unresolved_symbols(eval_data);
		}

		// link unresolved functions...
		error=eval_data->error;
		error_str=eval_data->error_str;
		error_file=eval_data->error_file;
		error_line=eval_data->error_line;

		delete eval_data;

		if(error){
			THROW_SCRIPT_ERROR_FILE_LINE(error_file,error_line,error_str.c_str());
		}
	}

	int eval_link_unresolved_symbols(EvalData *eval_data){
		ScriptClass *sc_found=NULL;
		if(eval_data->unresolved_symbols.size() > 0){
			UnresolvedInstructionInfo *unresolved_instruction_it=eval_data->unresolved_symbols.data();
			const char *str_aux=NULL;


			for(unsigned i=0;i < eval_data->unresolved_symbols.size();i++,unresolved_instruction_it++){
				const char *ptr_str_symbol_to_find=SFI_GET_SYMBOL_NAME(unresolved_instruction_it->calling_function,unresolved_instruction_it->instruction);
				const char *instruction_file=SFI_GET_FILE(unresolved_instruction_it->calling_function,unresolved_instruction_it->instruction);
				int instruction_line=SFI_GET_LINE(unresolved_instruction_it->calling_function,unresolved_instruction_it->instruction);
				Symbol *symbol_found=NULL;
				 if((sc_found= eval_data->script_class_factory->getScriptClass(ptr_str_symbol_to_find))!= NULL){ // check if class
					 unresolved_instruction_it->instruction->byte_code=BYTE_CODE_LOAD_CLASS;
					 unresolved_instruction_it->instruction->value_op2=(zs_int)sc_found;
				 }else if((str_aux=strstr(ptr_str_symbol_to_find,"::")) != NULL){ // static
					 std::string static_error;
					char copy_aux[512]={0};

					// get class
					strncpy(copy_aux,ptr_str_symbol_to_find,str_aux-ptr_str_symbol_to_find);

					sc_found=eval_data->zs->getScriptClassFactory()->getScriptClass(copy_aux);

					if(sc_found==NULL){
						EVAL_ERROR_FILE_LINE(
							instruction_file
							,instruction_line
							,"Cannot link static access '%s' class '%s' not exist"
							,ptr_str_symbol_to_find
							,copy_aux
						);
					}

					// advance ::
					str_aux+=2;

					//get member
					strcpy(copy_aux,str_aux);

					symbol_found=sc_found->getSymbol(copy_aux); // ... and member as well we can define the instruction here

					if(symbol_found == NULL){
						EVAL_ERROR_FILE_LINE(
							instruction_file
							,instruction_line
							,"Cannot link static access '%s' member '%s' not exist"
							,ptr_str_symbol_to_find
							,copy_aux
						);
					}

				 }else 	if((symbol_found = eval_find_global_symbol(eval_data,ptr_str_symbol_to_find))==NULL){ // give the error properly

					char *str_start_class=(char *)ptr_str_symbol_to_find;
					char *str_end_class=NULL;

					if((str_end_class=strstr(str_start_class,"::"))!=NULL){ // static access
						char class_name[512]={0};

						strncpy(class_name,str_start_class,str_end_class-str_start_class);


						if(eval_data->script_class_factory->getScriptClass(class_name) == NULL){
							EVAL_ERROR_FILE_LINE(
									instruction_file
									,instruction_line
									,"class '%s' not exist"
									,class_name
							);
						}

						EVAL_ERROR_FILE_LINE(
								instruction_file
								,instruction_line
								,"static symbol '%s' not exist in '%s'"
								,str_end_class+2
								,class_name
						);
					}else{
						EVAL_ERROR_FILE_LINE(
								instruction_file
								,instruction_line
								,"Symbol '%s' not defined"
								,ptr_str_symbol_to_find
						);
					}

				}

				if(symbol_found->properties & SYMBOL_PROPERTY_FUNCTION){
					if(unresolved_instruction_it->instruction->byte_code==BYTE_CODE_FIND_VARIABLE){
						unresolved_instruction_it->instruction->byte_code=BYTE_CODE_LOAD_FUNCTION;
					}else{
						unresolved_instruction_it->instruction->byte_code=BYTE_CODE_IMMEDIATE_CALL;
					}
					unresolved_instruction_it->instruction->value_op2=(zs_int)(ScriptFunction *)symbol_found->ref_ptr; // store script function
				}
				else{ // global variable

					if(unresolved_instruction_it->instruction->properties & INSTRUCTION_PROPERTY_USE_PUSH_STK){
						unresolved_instruction_it->instruction->byte_code=BYTE_CODE_PUSH_STK_GLOBAL;
					}else{
						unresolved_instruction_it->instruction->byte_code=BYTE_CODE_LOAD_GLOBAL;
					}

					unresolved_instruction_it->instruction->value_op2=symbol_found->idx_position;
				}

			}
		}

		return 1;
	}

	Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_scope_function){
		Scope *new_scope = NEW_SCOPE(eval_data,eval_data->current_function->script_function->idx_script_function,scope_parent);
		scope_parent->scopes->push_back((zs_int)new_scope);
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
		if(scope->symbol_variables->count > 0){ // if there's local symbols insert push/pop scope for there symbols
			if(scope->tmp_idx_instruction_push_scope!=ZS_IDX_UNDEFINED){
				eval_data->current_function->instructions.insert(
						eval_data->current_function->instructions.begin()+scope->tmp_idx_instruction_push_scope
						,new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(zs_int)scope)
				);

				// and finally insert pop scope
				eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_POP_SCOPE,0));
			}
		}
		else{ // remove scope
			scope->markAsUnusued();
		}
	}

	char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, ScriptFunction *sf,std::vector<ScriptFunctionParam> * args){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;

		Scope *new_scope_info=  NULL;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		if(*aux_p == '{'){
			bool is_function = sf!=NULL && args != NULL;
			aux_p++;

			if(scope_info->numInnerScopes() >= MAX_INNER_SCOPES_FUNCTION){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Reacged max scopes (Max: %i)",MAX_INNER_SCOPES_FUNCTION);
			}

			new_scope_info = eval_new_scope(eval_data,scope_info,is_function); // special case... ast is created later ...

			if(is_function){
				// register args as part of stack...
				for(unsigned i=0; i < args->size(); i++){
					try{
						sf->registerLocalArgument(
							new_scope_info
							,eval_data->current_parsing_file
							,args->at(i).line
							,args->at(i).param_name
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

				if(is_function == false){
					eval_check_scope(eval_data,new_scope_info);
				}
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
					case DIRECTIVE_IMPORT:
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

						ZS_LOG_DEBUG("import file: %s",str_symbol.c_str());

						// save current file info...
						if(!zs_file::exists(str_symbol)){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Error import: file '%s' not exist",str_symbol.c_str(),str_symbol.c_str());
						}


						try{
							// compile but not execute, it will execute the last eval
							eval_data->zs->evalFile(str_symbol,EvalOption::EVAL_OPTION_NO_EXECUTE);
						}catch(zs_exception & ex){
							EVAL_ERROR_FILE_LINE(str_symbol,ex.getErrorLine(),ex.getErrorDescription().c_str());
						}catch(std::exception & ex){
							EVAL_ERROR("%s : %s",str_symbol.c_str(),ex.what());
						}

						aux++;// advance ..
						break;
					default:
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"directive \"%s\" not supported",eval_data_directives[directive].str);
						break;
					}

					end_expr=aux;
				}
			}else if(*aux == '{' && (eval_object_test(eval_data,aux) == false)) { // can be a block or object...

				if(eval_data->error){
					return NULL;
				}

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
		Symbol * sc_var = scope->getSymbol(symbol_to_find, NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);

		if(sc_var != NULL){ // local symbol found

			if(sc_var->n_params==NO_PARAMS_SYMBOL_ONLY){ // symbol is variable...
				return sc_var->scope->getSymbol(symbol_to_find, NO_PARAMS_SYMBOL_ONLY, REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
			}
			else{ // symbol is function...
				return sc_var->scope->getSymbol(symbol_to_find,NO_PARAMS_SYMBOL_ONLY, REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
			}
		}

		// try find global variable...

		return NULL;
	}

	Symbol *eval_find_global_symbol(EvalData *eval_data, const std::string & symbol_to_find){

		// try find global variable...
		return MAIN_SCOPE(eval_data)->getSymbol(symbol_to_find,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);

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

	void eval_create_lookup_linear_local_var_recursive(Scope  *current_scope, short *lookup_table, int & n_variable){
		for(int i=0; i < current_scope->symbol_variables->count; i++){
			Symbol *s=current_scope->symbol_variables->items[i];
			lookup_linear_stk[s->idx_position]=n_variable++;
		}

		for(int i=0; i < current_scope->scopes->count; i++){
			eval_create_looktup_linear_local_var_recursive(current_scope->scopes->items[i],lookup_table,n_variable);
		}
	}

	short *eval_create_lookup_linear_local_var(ScriptFunction *sf){
		int n_var_fun=eval_data->current_function->script_function->local_variables->count();
		int n_var_scope=eval_data->current_function->script_function->symbol.scope->countVariables(true);

		if(n_var_fun != n_var_scope){
			EVAL_ERROR(
				 "internal: n_var_fun != n_var_scope (%i!=%i)"
				,n_var_fun
				,n_var_scope
			);
		}

		short *lookup_linear_stk=(short *)malloc(sizeof(short)*n_var_fun);
		Scope *current_scope=eval_data->current_function->script_function->symbol.scope;
		int n_variable=0;

		eval_create_stk_local_variable_lookup_recursive(current_scope,lookup_linear_stk,n_variable)


		free(lookup_linear_stk);
	}


	int eval_pop_and_compile_function(EvalData *eval_data){

		std::string static_error;
		ScriptFunction *sf = eval_data->current_function->script_function;
		ScriptClass *sc_sf = GET_SCRIPT_CLASS(eval_data,sf->idx_class);
		ScriptClass *sc_found=NULL;
		int sum_stk_load_stk=0;

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
		int ok=FALSE;
		const char *str_aux=NULL;

		short *lookup_table=eval_create_lookup_linear_local_var(sf);


		for(unsigned i=0; i < eval_data->current_function->instructions.size(); i++){

			Symbol *vis=NULL;
			EvalInstruction *instruction = eval_data->current_function->instructions[i];
			std::string *ptr_str_symbol_to_find=NULL;
			ScriptClass *sc_aux=NULL;
			//bool is_local=false;
			ptr_str_symbol_to_find=&instruction->symbol.name;

			//is_static&=((instruction->vm_instruction.properties & INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)==0);
			switch(instruction->vm_instruction.byte_code){

			// LOAD
			case BYTE_CODE_LOAD_GLOBAL:
			case BYTE_CODE_LOAD_REF:
			case BYTE_CODE_LOAD_THIS:
			case BYTE_CODE_LOAD_MEMBER_VAR:
			case BYTE_CODE_LOAD_ELEMENT_VECTOR:
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:
			// PUSH_STK
			case BYTE_CODE_PUSH_STK_GLOBAL:
			case BYTE_CODE_PUSH_STK_LOCAL:
			case BYTE_CODE_PUSH_STK_THIS:
			case BYTE_CODE_PUSH_STK_MEMBER_VAR:
			case BYTE_CODE_PUSH_STK_ELEMENT_VECTOR:
			case BYTE_CODE_PUSH_STK_ELEMENT_THIS:
			case BYTE_CODE_PUSH_STK_ELEMENT_OBJECT:
				// CONSTRUCTOR
			case BYTE_CODE_LOAD_CONSTRUCTOR:

				// load constants
			case BYTE_CODE_LOAD_FUNCTION:
			case BYTE_CODE_LOAD_NULL:
			case BYTE_CODE_LOAD_STRING:
			case BYTE_CODE_LOAD_ZS_FLOAT:
			case BYTE_CODE_LOAD_BOOL:
			case BYTE_CODE_LOAD_ZS_INT:
			case BYTE_CODE_LOAD_STACK_ELEMENT:
				sum_stk_load_stk++;
				break;
			case BYTE_CODE_RESET_STACK: // <-- reset stack
			case BYTE_CODE_CALL: // <-- reset stack
				if(sf->min_stack_needed<sum_stk_load_stk){
					sf->min_stack_needed=sum_stk_load_stk;
				}
				sum_stk_load_stk=0; // and reset stack
				break;
			case BYTE_CODE_LOAD_ELEMENT_THIS:

					sum_stk_load_stk++;
				// try to solve symbol...
					if(*ptr_str_symbol_to_find == SYMBOL_VALUE_SUPER){
						// get current function name and find first ancestor in heritance
						Symbol *symbol_sf_foundf=NULL;
						std::string target_name;

						bool is_constructor = sf->symbol.name == sc_sf->class_name;

						for(int i = sf->symbol.idx_position-1; i >=0 && symbol_sf_foundf==NULL; i--){
							Symbol *symbol_member = (Symbol *)sc_sf->class_scope->symbol_functions->items[i];
							bool match_names=false;
							if(is_constructor==true){
								if(symbol_member->scope == NULL){ // is constant...
									continue;
								}
								match_names=symbol_member->scope->script_class->class_name==symbol_member->name;
							}else{
								match_names=symbol_member->name==sf->symbol.name;
							}

							ScriptFunction *sf_member=(ScriptFunction *)symbol_member->ref_ptr;
							bool match_params=(symbol_member->properties & SYMBOL_PROPERTY_C_OBJECT_REF?match_names:true);
							if(
									(match_names)
								&& (match_params)
								){
								symbol_sf_foundf = symbol_member;
							}

						}

						// ok get the super function...
						if(symbol_sf_foundf == NULL){
							EVAL_ERROR_FILE_LINE_AND_GOTO(
								lbl_exit_pop_function
								,eval_data->current_parsing_file
								,instruction->instruction_source_info.line
								,"Cannot find parent function %s::%s"
								,sc_sf->class_name.c_str()
								,sf->symbol.name.c_str()
							);
						}
						instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_MEMBER_VAR;
						instruction->vm_instruction.value_op2=symbol_sf_foundf->idx_position;
						instruction->instruction_source_info.ptr_str_symbol_name =get_mapped_name(eval_data,std::string(symbol_sf_foundf->scope->script_class->class_name)+"::"+symbol_sf_foundf->name);

					}else{ // is "this" symbol, check whether symbol is member
						// TODO: review load function member !!
						if(instruction->vm_instruction.value_op2 == ZS_IDX_UNDEFINED){
							// is automatically created on vm...
							Symbol *symbol_function=sc_sf->getSymbol(*ptr_str_symbol_to_find,NO_PARAMS_SYMBOL_ONLY);
							if(symbol_function!=NULL){
								// functions always loads dynamically because we can have an override function
								// so we don't load as member in a fix position else is a member variable ...
								if((symbol_function->properties & SYMBOL_PROPERTY_FUNCTION) == 0){ // if not function set as load immediate
									instruction->vm_instruction.value_op2=symbol_function->idx_position;
									instruction->vm_instruction.byte_code=ByteCode::BYTE_CODE_LOAD_MEMBER_VAR; // immediate load
								}
							}
						}
						// TODO: review load function member !!
					}
					break;
			case BYTE_CODE_FIND_VARIABLE:
			case BYTE_CODE_FIND_IMMEDIATE_CALL:
				eval_data->unresolved_symbols.push_back(UnresolvedInstructionInfo(&sf->instructions[i],sf));
				if(instruction->vm_instruction.byte_code == BYTE_CODE_FIND_VARIABLE){
					sum_stk_load_stk++;
				}else{
					sum_stk_load_stk=0; // call function reset stk
				}
				break;
			}


			// add any instruction that references global instruction
			ByteCode byte_code=instruction->vm_instruction.byte_code;
			uint16_t properties_1=instruction->vm_instruction.properties;
			uint16_t properties_2=instruction->vm_instruction.value_op2 & 0xffff;
			if(
					byte_code == BYTE_CODE_LOAD_GLOBAL
					|| byte_code == BYTE_CODE_PUSH_STK_GLOBAL
					|| ((properties_1 & INSTRUCTION_PROPERTY_ILOAD_R) && ILOAD_ACCESS_IS_GLOBAL(properties_1))
					|| ((properties_1 & INSTRUCTION_PROPERTY_ILOAD_KR) && ILOAD_ACCESS_IS_GLOBAL(properties_1))
					|| ((properties_1 & INSTRUCTION_PROPERTY_ILOAD_RK) && ILOAD_ACCESS_IS_GLOBAL(properties_1))
					|| ((properties_1 & INSTRUCTION_PROPERTY_ILOAD_RR) && ILOAD_ACCESS_IS_GLOBAL(properties_1))
					|| ((properties_1 & INSTRUCTION_PROPERTY_ILOAD_RR) && ILOAD_ACCESS_IS_GLOBAL(properties_2))

			){
				eval_data->global_ref_instructions.push_back(instruction);
			}else{ // local ?
				switch(byte_code){ // reallocate instructions
				case BYTE_CODE_LOAD_GLOBAL:
				case BYTE_CODE_LOAD_REF:
				case BYTE_CODE_PUSH_STK_LOCAL:
					instruction->vm_instruction.value_op2=lookup_table[instruction->vm_instruction.value_op2];
					break;
				default:
					// check if local access
					if( (properties_1 & (INSTRUCTION_PROPERTY_ILOAD_R | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL))
							==
						(INSTRUCTION_PROPERTY_ILOAD_R | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL)){

						instruction->vm_instruction.value_op2=lookup_table[instruction->vm_instruction.value_op2];


					}else if((properties_1 & (INSTRUCTION_PROPERTY_ILOAD_KR| INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL))
							==
						(INSTRUCTION_PROPERTY_ILOAD_KR| INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL)
					){

						instruction->vm_instruction.value_op1=lookup_table[instruction->vm_instruction.value_op1];

					}else if((properties_1 & (INSTRUCTION_PROPERTY_ILOAD_RK| INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL))
								==
						(INSTRUCTION_PROPERTY_ILOAD_RK| INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL)
					){
						instruction->vm_instruction.value_op1=lookup_table[instruction->vm_instruction.value_op1];

					}else{
						if ((properties_1 & (INSTRUCTION_PROPERTY_ILOAD_RR | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL))
											==
							(INSTRUCTION_PROPERTY_ILOAD_RR | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL)
						){
							instruction->vm_instruction.value_op1=lookup_table[instruction->vm_instruction.value_op1];
						}

						if ((properties_2 & (INSTRUCTION_PROPERTY_ILOAD_RR | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL))
											==
							(INSTRUCTION_PROPERTY_ILOAD_RR | INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL)
						){
							instruction->vm_instruction.value_op2=(lookup_table[instruction->vm_instruction.value_op2>>16]<<16) | properties_2;
						}
					}
				}

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

		// update variables symbol...
		for(int i=0; i < sf->local_variables->count; i++){
			// swap ??
			sf->local_variables->items[i]=lookup_table[sf->local_variables->items[i]];
		}


		free(lookup_table);

		ok=TRUE;

lbl_exit_pop_function:

		eval_pop_current_function(eval_data);

		// update min stk needed
		if(sf->min_stack_needed<sum_stk_load_stk){
			sf->min_stack_needed=sum_stk_load_stk;
		}



		return ok;
	}
}
