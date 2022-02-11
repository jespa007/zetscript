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
			,const char * str_code
			, const char *  _filename
			, int _line
			, ScriptFunction *_sf
			, ScriptFunctionParam * function_params
			, size_t function_params_len
	){
		EvalData *eval_data=new EvalData(zs);
		char *aux_p=NULL;
		int line =_line;
		bool error;
		zs_string error_str;
		zs_string error_file="";
		int error_line=-1;
		Scope *scope_info=MAIN_SCOPE(eval_data);
		eval_data->current_parsing_file=_filename;
		ScriptFunction *sf = _sf == NULL?MAIN_FUNCTION(eval_data):_sf;


		if(sf != MAIN_FUNCTION(eval_data)){
			scope_info = NEW_SCOPE(eval_data,sf->idx_script_function,MAIN_SCOPE(eval_data),SCOPE_PROPERTY_IS_SCOPE_FUNCTION);
			MAIN_SCOPE(eval_data)->scopes->push_back((zs_int)scope_info);

			if(function_params != NULL){

				// register args as part of stack...
				for(unsigned i=0; i < function_params_len; i++){

					sf->registerLocalArgument(
						scope_info
						,""
						,-1
						,function_params[i].name
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

		CLEAR_UNUSUED_SCOPES(eval_data);

		eval_pop_and_compile_function(eval_data);

		// link unresolved functions...
		error=eval_data->error;
		error_str=eval_data->error_str;
		error_file=eval_data->error_file;
		error_line=eval_data->error_line;

		delete eval_data;

		if(error){
			THROW_SCRIPT_ERROR_FILE_LINEF(error_file,error_line,error_str.c_str());
		}
	}


	Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_scope_function){
		Scope *new_scope = NEW_SCOPE(
				eval_data
				,eval_data->current_function->script_function->idx_script_function
				,scope_parent
				,is_scope_function?SCOPE_PROPERTY_IS_SCOPE_FUNCTION:0
		);

		scope_parent->scopes->push_back((zs_int)new_scope);
		if(is_scope_function){
			new_scope->tmp_idx_instruction_push_scope=0;
		}
		else{
			new_scope->tmp_idx_instruction_push_scope=(int)eval_data->current_function->eval_instructions.count;
		}

		return new_scope;
	}

	void eval_check_scope(EvalData *eval_data, Scope *scope){
		if(scope->symbol_variables->count > 0){ // if there's local symbols insert push/pop scope for there symbols
			if(scope->tmp_idx_instruction_push_scope!=ZS_IDX_UNDEFINED){
				eval_data->current_function->eval_instructions.insert(
						scope->tmp_idx_instruction_push_scope
						,(zs_int)(
								new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(zs_int)scope)
						)
				);

				// and finally insert pop scope
				eval_data->current_function->eval_instructions.push_back((zs_int)(
						new EvalInstruction(BYTE_CODE_POP_SCOPE,0)
				));
			}
		}
		else{ // remove scope
			scope->markAsUnusued();
		}
	}

	char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, ScriptFunction *sf,ScriptFunctionParam *params, size_t params_len){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;

		Scope *new_scope_info=  NULL;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		if(*aux_p == '{'){
			bool is_function = sf!=NULL && params != NULL;
			aux_p++;

			if(scope_info->numInnerScopes() >= MAX_INNER_SCOPES_FUNCTION){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Reached max scopes (Max: %i)",MAX_INNER_SCOPES_FUNCTION);
			}

			new_scope_info = eval_new_scope(eval_data,scope_info,is_function);

			if(is_function){

				// set scope to the function
				sf->function_scope=new_scope_info;

				// register args as part of stack...
				for(unsigned i=0; i < params_len; i++){
					try{
						sf->registerLocalArgument(
							new_scope_info
							,eval_data->current_parsing_file
							,params[i].line
							,params[i].name
							,params[i].properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF?SYMBOL_PROPERTY_ARG_BY_REF:0
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
					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Expected '}' ");
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
				zs_string str_symbol;
				if(directive != Directive::DIRECTIVE_UNKNOWN){
					switch(directive){
					case DIRECTIVE_IMPORT:
						aux += strlen(eval_data_directives[directive].str);
						IGNORE_BLANKS(aux,eval_data,aux,line);
						if(*aux != '\"'){
							EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"expected starting \" directive");
						}
						aux++;
						start_var=aux;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"expected end \" directive");
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
							EVAL_ERROR_FILE_LINEF(str_symbol,ex.getErrorLine(),ex.getErrorDescription().c_str());
						}catch(std::exception & ex){
							EVAL_ERROR("%s : %s",str_symbol.c_str(),ex.what());
						}

						aux++;// advance ..
						break;
					default:
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"directive '%s' not supported",eval_data_directives[directive].str);
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
					,&eval_data->current_function->eval_instructions
					,NULL
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_ALLOW_SEQUENCE_ASSIGNMENT | EVAL_EXPRESSION_RESET_STACK_LAST_LOAD
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
		eval_data->eval_functions.push_back((zs_int)(
				eval_data->current_function=new EvalFunction(script_function)
		));
	}

	Symbol *eval_find_local_symbol(EvalData *eval_data,Scope *scope, const zs_string & symbol_to_find){
		ZS_UNUSUED_PARAM(eval_data);
		return scope->getSymbol(symbol_to_find, NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
	}

	Symbol *eval_find_global_symbol(EvalData *eval_data, const zs_string & symbol_to_find){
		// try find global variable...
		return MAIN_SCOPE(eval_data)->getSymbol(symbol_to_find,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL);
	}

	void eval_pop_current_function(EvalData *eval_data){
		// delete and popback function information...
		delete eval_data->current_function;
		eval_data->eval_functions.pop_back();

		eval_data->current_function = NULL;
		if(eval_data->eval_functions.count > 0){
			eval_data->current_function = (EvalFunction *)eval_data->eval_functions.items[eval_data->eval_functions.count-1];
		}

	}

	bool eval_all_local_variables_in_scopes_already_sorted(Scope  *current_scope,  int & idx_local_variable){
		for(int i=0; i < current_scope->symbol_variables->count; i++){
			Symbol *s=(Symbol *)current_scope->symbol_variables->items[i];
			if(s->idx_position!=idx_local_variable++){
				return false;
			}
		}

		for(int i=0; i < current_scope->scopes->count; i++){
			Scope *scope=(Scope *)current_scope->scopes->items[i];
			if((scope->properties & (SCOPE_PROPERTY_IS_SCOPE_FUNCTION | SCOPE_PROPERTY_IS_SCOPE_CLASS)) == 0){ // ignore local functions/classes
				bool ok=eval_all_local_variables_in_scopes_already_sorted(scope,idx_local_variable);

				if(ok == false){
					return false;
				}
			}
		}

		return true;
	}

	void eval_fill_lookup_local_variable(Scope  *current_scope, short *lookup_table, int & n_variable,zs_vector *order_local_vars){
		for(int i=0; i < current_scope->symbol_variables->count; i++){
			Symbol *s=(Symbol *)current_scope->symbol_variables->items[i];
			lookup_table[s->idx_position]=n_variable++;
			order_local_vars->push_back(s->idx_position);
		}

		for(int i=0; i < current_scope->scopes->count; i++){
			Scope *scope=(Scope *)current_scope->scopes->items[i];
			if((scope->properties & (SCOPE_PROPERTY_IS_SCOPE_FUNCTION | SCOPE_PROPERTY_IS_SCOPE_CLASS)) == 0){ // ignore local functions/classes
				eval_fill_lookup_local_variable(scope,lookup_table,n_variable,order_local_vars);
			}
		}
	}

	short *eval_create_lookup_sorted_table_local_variables(EvalData *eval_data,zs_vector *order_local_vars){

		ScriptFunction *sf = eval_data->current_function->script_function;

		int n_local_variable=0;
		if(eval_all_local_variables_in_scopes_already_sorted(sf->function_scope,n_local_variable) == true){
			return NULL;
		}

		int n_var_fun=sf->local_variables->count;
		int n_var_scope=sf->function_scope->countVariables(true);

		if(n_var_fun != n_var_scope){

			EVAL_ERROR(
				 "internal: n_var_fun != n_var_scope (%i!=%i)"
				,n_var_fun
				,n_var_scope
			);
		}

		short *lookup_linear_stk=(short *)ZS_MALLOC(sizeof(short)*n_var_fun);
		Scope *current_scope=sf->function_scope;
		n_local_variable=0;

		eval_fill_lookup_local_variable(current_scope,lookup_linear_stk,n_local_variable,order_local_vars);

		return lookup_linear_stk;
	}


	int eval_pop_and_compile_function(EvalData *eval_data){

		zs_string static_error;
		ScriptFunction *sf = eval_data->current_function->script_function;
		ScriptType *sc_sf = GET_SCRIPT_CLASS(eval_data,sf->idx_type);
		int sum_stk_load_stk=0;
		int max_acc_stk_load=0;

		if(sf->instructions != NULL){
			free(sf->instructions);
			sf->instructions=NULL;
		}

		// get total size op + 1 ends with 0 (INVALID BYTE_CODE)
		size_t count =eval_data->current_function->eval_instructions.count;
		size_t len=count + 1; // +1 for end instruction
		size_t total_size_bytes = (len) * sizeof(Instruction);
		sf->instructions_len=len;
		sf->instructions = (PtrInstruction)ZS_MALLOC(total_size_bytes);
		zs_vector order_local_vars;


		int ok=FALSE;
		short *lookup_sorted_table_local_variables=eval_create_lookup_sorted_table_local_variables(eval_data,&order_local_vars);

		for(unsigned i=0; i < count; i++){

			EvalInstruction *eval_instruction = (EvalInstruction *)eval_data->current_function->eval_instructions.items[i];
			zs_string *ptr_str_symbol_to_find=NULL;
			ptr_str_symbol_to_find=&eval_instruction->symbol.name;

			sum_stk_load_stk+=instruction_num_required_stack(&eval_instruction->vm_instruction);
			max_acc_stk_load=MAX(max_acc_stk_load,sum_stk_load_stk);

			switch(eval_instruction->vm_instruction.byte_code){

			case BYTE_CODE_THIS_CALL:

				// try to solve symbol...
				if(*ptr_str_symbol_to_find == SYMBOL_VALUE_SUPER){
					// get current function name and find first ancestor in heritance
					Symbol *symbol_sf_foundf=NULL;
					zs_string target_name;

					// find constructor symbol through other members...
					for(int j = sf->idx_position-1; j >=0 && symbol_sf_foundf==NULL; j--){
						Symbol *symbol_member = (Symbol *)sc_sf->class_scope->symbol_functions->items[j];
						ScriptFunction *sf_member=(ScriptFunction *)symbol_member->ref_ptr;
						bool match_names=false;
						if((sf->properties &  FUNCTION_PROPERTY_CONSTRUCTOR) != 0){
							match_names=( sf_member->properties & FUNCTION_PROPERTY_CONSTRUCTOR) != 0;//symbol_member->scope->script_type->type_name==symbol_member->name;
						}else{
							match_names=symbol_member->name==sf->function_name;
						}

						bool match_params=(sf_member->properties & SYMBOL_PROPERTY_C_OBJECT_REF?match_names:true);
						if(
								(match_names)
							&& (match_params)
							){
							symbol_sf_foundf = symbol_member;
						}

					}

					// ok get the super function...
					if(symbol_sf_foundf == NULL){
						if((sf->properties &  FUNCTION_PROPERTY_CONSTRUCTOR) != 0){
							EVAL_ERROR_FILE_LINE_GOTO_NO_AUX(
								eval_data->current_parsing_file
								,eval_instruction->instruction_source_info.line
								,lbl_exit_pop_function
								,"Cannot find parent constructor of '%s'"
								,sc_sf->type_name.c_str()
							);
						}else{
							EVAL_ERROR_FILE_LINE_GOTO_NO_AUX(
								eval_data->current_parsing_file
								,eval_instruction->instruction_source_info.line
								,lbl_exit_pop_function
								,"Cannot find parent function '%s::%s'"
								,sc_sf->type_name.c_str()
								,sf->function_name.c_str()
							);
						}
					}
					eval_instruction->vm_instruction.value_op2=(zs_int)symbol_sf_foundf;//->idx_position;
					eval_instruction->instruction_source_info.ptr_str_symbol_name =get_mapped_name(eval_data,zs_string(symbol_sf_foundf->scope->script_type->type_name)+"::"+symbol_sf_foundf->name);
				}

				if(eval_instruction->vm_instruction.value_op2==ZS_IDX_UNDEFINED){
					eval_instruction->vm_instruction.byte_code=BYTE_CODE_UNRESOLVED_THIS_CALL;
					eval_data->zs->addUnresolvedSymbol(sf,i);
				}
				break;
			case BYTE_CODE_CALL:
				if(eval_instruction->vm_instruction.value_op2==ZS_IDX_UNDEFINED){
					eval_instruction->vm_instruction.byte_code=BYTE_CODE_UNRESOLVED_CALL;
					eval_data->zs->addUnresolvedSymbol(sf,i);
				}
				break;
			case BYTE_CODE_FIND_VARIABLE:
				// add instruction reference to solve later
				eval_data->zs->addUnresolvedSymbol(sf,i);
				break;
			}

			if(eval_instruction->vm_instruction.byte_code == BYTE_CODE_RESET_STACK
				|| eval_instruction->vm_instruction.byte_code == BYTE_CODE_RET
				|| (eval_instruction->vm_instruction.properties & INSTRUCTION_PROPERTY_RESET_STACK)
			){
				// <-- reset stack
				sum_stk_load_stk=0; // and reset stack
			}

			//------------------------ SET NEW SORTED POSITION
			if(lookup_sorted_table_local_variables != NULL){
				// add any instruction that references global instruction
				ByteCode byte_code=eval_instruction->vm_instruction.byte_code;
				uint16_t properties_1=eval_instruction->vm_instruction.properties;
				uint16_t properties_2=(uint16_t)(eval_instruction->vm_instruction.value_op2 & 0xffff);

				switch(byte_code){ // reallocate instructions
				case BYTE_CODE_INDIRECT_LOCAL_CALL:
				case BYTE_CODE_LOAD_LOCAL:
				case BYTE_CODE_LOAD_REF:
				case BYTE_CODE_PUSH_STK_LOCAL:
					eval_instruction->vm_instruction.value_op2=lookup_sorted_table_local_variables[eval_instruction->vm_instruction.value_op2];
					break;
				default:
					// check if local access
					if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_R) {// R
						eval_instruction->vm_instruction.value_op2=lookup_sorted_table_local_variables[eval_instruction->vm_instruction.value_op2];
					}else if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_KR) {// KR
						eval_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[eval_instruction->vm_instruction.value_op1];
					}else if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_RK){ // RK
						eval_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[eval_instruction->vm_instruction.value_op1];
					}else if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_RR){ // is RR local access
						eval_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[eval_instruction->vm_instruction.value_op1];
						eval_instruction->vm_instruction.value_op2=(lookup_sorted_table_local_variables[eval_instruction->vm_instruction.value_op2>>16]<<16) | properties_2;
					}
				}

			}

			// set min stk required
			sf->min_stack_needed=max_acc_stk_load;

			//------------------------ SORT ALL LOCAL VARIABLES
			// save instruction ...
			sf->instructions[i]=eval_instruction->vm_instruction;

			//------------------------------------
			// symbol value to save at runtime ...
			InstructionSourceInfo instruction_info=eval_instruction->instruction_source_info;

			// Save str_symbol that was created on eval process, and is destroyed when eval finish.
			instruction_info.ptr_str_symbol_name=eval_instruction->instruction_source_info.ptr_str_symbol_name;

			sf->instruction_source_info.push_back((zs_int)(new InstructionSourceInfo(instruction_info)));
		}

		if(lookup_sorted_table_local_variables != NULL){
			// update variables symbol...
			zs_vector *local_vars_dst=new zs_vector();

			for(int i=0; i < sf->local_variables->count; i++){
				int idx_var=order_local_vars.items[i];
				Symbol *s=(Symbol *)sf->local_variables->items[idx_var];
				s->idx_position=lookup_sorted_table_local_variables[idx_var];
				local_vars_dst->push_back((zs_int)s);
			}

			delete sf->local_variables;

			// set vector with all local variables sorted
			sf->local_variables=local_vars_dst;


			free(lookup_sorted_table_local_variables);
		}

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
