/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#ifdef  __ZS_VERBOSE_MESSAGE__

#define print_compiler_cr ZS_LOG_DEBUG
#else
#define print_compiler_cr(s,...)
#endif

#include "compiler_operator.cpp"
#include "compiler_data.cpp"
#include "compiler_object.cpp"
#include "compiler_expression_optimize.cpp"
#include "compiler_expression_byte_code.cpp"
#include "compiler_symbol.cpp"
#include "compiler_expression_token_symbol.cpp"
#include "compiler_keyword_class.cpp"
#include "compiler_keyword_conditional.cpp"
#include "compiler_keyword_loop.cpp"
#include "compiler_keyword_var.cpp"
#include "compiler_keyword_function.cpp"
#include "compiler_expression.cpp"
#include "compiler_keyword.cpp"

namespace zetscript{

	/**
	 * Given two pointers within that points within a String, this function copies String between its interval.
	 * @p1:start pointer
	 * @p2:end pointer
	 */

	String compiler_copy_const_char_diff(const char *p1, const char *p2){

		char aux_str_copy[ZS_MAX_STR_BUFFER] = {0};

		if(p1 == NULL || p2 == NULL){
			ZS_THROW_RUNTIME_ERROR("NULL entry (%p %p)",p1,p2);
		}

		int var_length=p2-p1;

		if(var_length < 0 || var_length >= (ZS_MAX_STR_BUFFER+1)){
			ZS_THROW_RUNTIME_ERROR("array out of bounds (Max:%i Min:%i Current:%i)",0,ZS_MAX_STR_BUFFER,var_length);
		}

		if(p1 == p2){
			ZS_THROW_RUNTIME_ERRORF("Nothing to copy");
		}

		strncpy(aux_str_copy,p1,var_length);

		return aux_str_copy;

	}


	void compiler_compile_and_link(ScriptEngine *_script_engine
			,const char * str_code
			,CompilerData *_compiler_data_from
			, const char *  _filename
			, int _line
			, ScriptFunction *_sf
	){
		CompilerData *compiler_data=_compiler_data_from;//new CompilerData(zs);
		if(_compiler_data_from==NULL){
			compiler_data=new CompilerData(_script_engine);
		}

		char *aux_p=NULL;
		int line =_line;
		bool error;
		String str_error="";
		String error_file="";
		int error_line=-1;
		ScriptScope *scope_info=ZS_MAIN_SCOPE(compiler_data);
		compiler_data->current_parsing_file=_filename;
		ScriptFunction *sf = _sf == NULL?ZS_MAIN_FUNCTION(compiler_data):_sf;
		sf->removeUnusuedScopes();
		sf->clearFunctionLastCalls();

		if(sf != ZS_MAIN_FUNCTION(compiler_data)){ // remove/reset old code
			scope_info =sf->scope;// ZS_NEW_SCOPE(compiler_data,sf->id,ZS_MAIN_SCOPE(compiler_data),SCOPE_PROPERTY_IS_SCOPE_FUNCTION);
		}

		if(_compiler_data_from == NULL){
			compiler_push_function(compiler_data,sf);
		}

		aux_p=compiler_compile_recursive(compiler_data,str_code,line,scope_info);
		if(aux_p!=NULL){
			if(*aux_p=='}'){
				compiler_data->error=true;
				compiler_data->error_file=_filename;
				compiler_data->error_line=line;
				compiler_data->str_error="unexpected ending block ('}')";
			}
		}

		// link unresolved functions...
		error=compiler_data->error;
		str_error=compiler_data->str_error;
		error_file=compiler_data->error_file;
		error_line=compiler_data->error_line;

		if(_compiler_data_from == NULL){

			if(sf != ZS_MAIN_FUNCTION(compiler_data) && sf->id != ZS_FUNCTION_EVAL_IDX){ // is anonyomuse function
				if(scope_info->symbol_variables->length() == 0){ // remove scope
					scope_info->markAsUnusued();
				}
			}


			ZS_CLEAR_UNUSUED_SCOPES(compiler_data);

			compiler_pop_and_compile_function(compiler_data);

			delete compiler_data;

		}


		if(error){
			ZS_THROW_EXCEPTION_FILE_LINE(error_file.toConstChar(),error_line,str_error.toConstChar());
		}

		// link function symbols
		_script_engine->link();
	}

	ScriptScope * compiler_new_scope_function(CompilerData *compiler_data, ScriptScope *scope_parent){
		ScriptScope *new_scope = ZS_NEW_SCOPE(
				compiler_data
				,compiler_data->current_function->script_function->id
				,scope_parent
				,SCOPE_PROPERTY_IS_SCOPE_FUNCTION
		);

		new_scope->offset_instruction_push_scope=0;

		return new_scope;
	}

	ScriptScope * compiler_new_scope_block(CompilerData *compiler_data, ScriptScope *scope_parent){
		ScriptScope *new_scope = ZS_NEW_SCOPE(
				compiler_data
				,compiler_data->current_function->script_function->id
				,scope_parent
				,SCOPE_PROPERTY_IS_SCOPE_BLOCK
		);

		new_scope->offset_instruction_push_scope=(int)compiler_data->current_function->compiler_instructions.length();
		return new_scope;
	}

	void compiler_check_scope(CompilerData *compiler_data, ScriptScope *scope, bool _is_block_body_loop){
		if(scope->symbol_variables->length() > 0 || _is_block_body_loop==true){
			// if there's local symbols insert push/pop scope
			if(scope->offset_instruction_push_scope!=ZS_UNDEFINED_IDX){
				compiler_data->current_function->compiler_instructions.insert(
						scope->offset_instruction_push_scope
						,
								new CompilerInstruction(BYTE_CODE_PUSH_SCOPE,0,(zs_int)scope)

				);

				// and finally insert pop scope
				compiler_data->current_function->compiler_instructions.push(
						new CompilerInstruction(BYTE_CODE_POP_SCOPE,0)
				);
			}
		}
		else{ // remove scope
			scope->markAsUnusued();
		}
	}

	char * compiler_block_internal(
			CompilerData *compiler_data
			,const char *s
			,int & line
			,  ScriptScope *scope_info
			, ScriptScope *_new_scope
	){
		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		// check for keyword ...
		if(*aux_p == '{'){
			aux_p++;

			if(scope_info->numInnerScopes() >= ZS_SCOPE_MAX_INNER_SCOPES_FUNCTION){
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Reached max scopes (Max: %i)",ZS_SCOPE_MAX_INNER_SCOPES_FUNCTION);
			}

			if((aux_p = compiler_compile_recursive(
					compiler_data
					,aux_p
					, line
					, _new_scope
			)) != NULL){

				if(*aux_p != '}'){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Expected '}' ");
				}

				return aux_p+1;
			}
		}
		return NULL;
	}

	char * compiler_block_body(

			CompilerData *compiler_data
			,const char *s
			,int & line
			,  ScriptScope *scope_info
			, ScriptScope *_new_scope
			, bool _is_block_body_loop
	){
		ScriptScope *new_scope=_new_scope;

		if(new_scope==NULL){
			new_scope = compiler_new_scope_block(compiler_data,scope_info);
		}

		char *result=compiler_block_internal(
						compiler_data
						,s
						, line
						, scope_info
						,new_scope
				);

		if(result==NULL){
			return NULL;
		}

		compiler_check_scope(compiler_data,new_scope,_is_block_body_loop);

		return result;

	}

	char * compiler_block_function(

			CompilerData *compiler_data
			,const char *s
			,int & line
			,  ScriptScope *scope_info
			, ScriptFunction *sf
			,FunctionParam *params
			, int params_len
	){


		ScriptScope *new_scope = compiler_new_scope_function(compiler_data,scope_info);


		// set scope to the function
		sf->scope=new_scope;

		// register args as part of stack...
		for(int i=0; i < params_len; i++){
			try{
				sf->registerLocalArgument(
						new_scope
					,compiler_data->current_parsing_file
					,params[i].line
					,params[i].name
					,params[i].properties & MSK_FUNCTION_ARG_PROPERTY_BY_REF?SYMBOL_PROPERTY_ARG_BY_REF:0
				);

			}catch(std::exception & ex){
				compiler_data->error=true;
				compiler_data->str_error=ex.what();
				return NULL;
			}
		}


		return compiler_block_internal(
				compiler_data
				,s
				, line
				,  scope_info
				,new_scope
		);
	}

	char * compiler_compile_recursive(CompilerData *compiler_data,const char *s, int & line, ScriptScope *scope_info, bool return_on_break_or_case){
		// PRE: *node_to_be_compileruated must be created (the pointer is only read mode)
		bool custom_quit = false;
		const char *current_parsing_file=NULL;
		char *aux = (char *)s;
		char *end_expr=0;

		Keyword keyw;
		Directive directive;

		IGNORE_BLANKS(aux,compiler_data,aux, line);

		while(*aux != 0 && !custom_quit){

			// ignore all ;
			while(*aux==';' && *aux != 0){
				IGNORE_BLANKS(aux,compiler_data,aux+1, line);
			}

			if(*aux==0){ // custom case exit..
				return aux;
			}

			keyw=compiler_is_keyword(aux);

			if(*aux == '}'){ // ending block trivial cases...
				return aux;
			}else if(keyw != Keyword::KEYWORD_UNKNOWN && keyw < KEYWORDS_WITHIN_EXPRESSIONS && (is_anonymous_function(compiler_data,aux,line)==false)){ // it cuts current expression to link breaks...

				if(((keyw == Keyword::KEYWORD_BREAK) || (keyw == Keyword::KEYWORD_CASE)|| (keyw == Keyword::KEYWORD_DEFAULT)) && return_on_break_or_case){
					return aux;
				}

				end_expr = compiler_keyword(
					compiler_data
					,aux
					, line
					, scope_info
					, keyw
				);

			}else if((directive = is_directive(aux)) != Directive::DIRECTIVE_UNKNOWN){ // compiler directive
				// try directive ...
				char *start_var,* end_var;
				String str_symbol;
				if(directive != Directive::DIRECTIVE_UNKNOWN){
					switch(directive){
					case DIRECTIVE_IMPORT:
						aux += strlen(compiler_data_directives[directive].str);
						IGNORE_BLANKS(aux,compiler_data,aux,line);
						if(*aux != '\"'){
							EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"expected starting \" directive");
						}
						aux++;
						start_var=aux;
						current_parsing_file=compiler_data->current_parsing_file;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							EVAL_ERROR_FILE_LINEF(
								compiler_data->current_parsing_file
								,line
								,"expected \" for finished directive declaration"
							);
						}

						end_var=aux;

						str_symbol=compiler_copy_const_char_diff(start_var,end_var);

						ZS_LOG_DEBUG("import file: %s",str_symbol.toConstChar());

						// save current file info...
						if(!File::exists(str_symbol)){
							EVAL_ERROR_FILE_LINE(
								compiler_data->current_parsing_file
								,line
								,"Error import: file '%s' not exist"
								,str_symbol.toConstChar()
							);
						}

						try{
							// compile but not execute, it will execute the last compiler
							compiler_data->script_engine->compileFile(str_symbol,compiler_data);
						}catch(Exception & ex){
							compiler_data->error=true;\
							compiler_data->error_file=ex.getFilename();
							compiler_data->error_line=ex.getLine();
							compiler_data->str_error=String::format(
									"%s \n"
									ZS_FORMAT_FILE_LINE" from import '%s'"
									,ex.what()
									,Path::getFilename(current_parsing_file).toConstChar(),line,str_symbol.toConstChar()
							);
							return 0;
						}

						compiler_data->current_parsing_file=current_parsing_file;

						aux++;// advance ..
						break;
					default:
						EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"directive '%i' not supported",directive);
						break;
					}

					end_expr=aux;
				}
			}else if(*aux == '{' && (compiler_object_test(compiler_data,aux) == false)) { // can be a block or object...

				if(compiler_data->error){
					return NULL;
				}

				// 2nd. check whether compiler a block
				end_expr = compiler_block_body(
					compiler_data
					,aux
					,line
					, scope_info
				);

			}else{ // compiler expression

				end_expr = compiler_expression(
					compiler_data
					,aux
					,line
					, scope_info
					,&compiler_data->current_function->compiler_instructions
					,NULL
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION | EVAL_EXPRESSION_ALLOW_SEQUENCE_ASSIGNMENT | EVAL_EXPRESSION_RESET_STACK_LAST_LOAD
				);
			}

			if(end_expr == NULL){
				return NULL;
			}

			aux=end_expr;
			IGNORE_BLANKS(aux,compiler_data,aux, line);
		}
		return aux;
	}

	void compiler_push_function(CompilerData *compiler_data,ScriptFunction *script_function){
		compiler_data->compiler_functions.push(
				compiler_data->current_function=new EvalFunction(script_function)
		);
	}

	Symbol *compiler_find_local_symbol(CompilerData *compiler_data,ScriptScope *scope, const String & symbol_to_find){
		ZS_UNUSUED_PARAM(compiler_data);
		return scope->getSymbol(symbol_to_find, ZS_NO_PARAMS_SYMBOL_ONLY,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_DOWN);
	}

	Symbol *compiler_find_global_symbol(CompilerData *compiler_data, const String & symbol_to_find){
		// try find global variable...
		return ZS_MAIN_SCOPE(compiler_data)->getSymbol(symbol_to_find,ZS_NO_PARAMS_SYMBOL_ONLY,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL);
	}

	void compiler_pop_current_function(CompilerData *compiler_data){
		// delete and popback function information...
		delete compiler_data->current_function;
		compiler_data->compiler_functions.pop();

		compiler_data->current_function = NULL;
		if(compiler_data->compiler_functions.length() > 0){
			compiler_data->current_function = (EvalFunction *)compiler_data->compiler_functions.get(compiler_data->compiler_functions.length()-1);
		}

	}

	bool compiler_all_local_variables_in_scopes_already_sorted(ScriptScope  *current_scope,  int & idx_local_variable){
		for(int i=0; i < current_scope->symbol_variables->length(); i++){
			Symbol *s=(Symbol *)current_scope->symbol_variables->get(i);
			if(s->idx_position!=idx_local_variable++){
				return false;
			}
		}

		auto scopes=current_scope->getScopes();
		for(int i=0; i < scopes->length(); i++){
			ScriptScope *scope=(ScriptScope *)scopes->get(i);
			if((scope->properties & (SCOPE_PROPERTY_IS_SCOPE_FUNCTION | SCOPE_PROPERTY_IS_SCOPE_CLASS)) == 0){ // ignore local functions/classes
				bool ok=compiler_all_local_variables_in_scopes_already_sorted(scope,idx_local_variable);

				if(ok == false){
					return false;
				}
			}
		}

		return true;
	}

	void compiler_fill_lookup_local_variable(ScriptScope  *current_scope, short *lookup_table, int & n_variable,Vector<zs_int> *order_local_vars){
		for(int i=0; i < current_scope->symbol_variables->length(); i++){
			Symbol *s=(Symbol *)current_scope->symbol_variables->get(i);
			lookup_table[s->idx_position]=n_variable++;
			order_local_vars->push(s->idx_position);
		}

		auto scopes=current_scope->getScopes();

		for(int i=0; i < scopes->length(); i++){
			ScriptScope *scope=(ScriptScope *)scopes->get(i);
			if((scope->properties & (SCOPE_PROPERTY_IS_SCOPE_FUNCTION | SCOPE_PROPERTY_IS_SCOPE_CLASS)) == 0){ // ignore local functions/classes
				compiler_fill_lookup_local_variable(scope,lookup_table,n_variable,order_local_vars);
			}
		}
	}

	short *compiler_create_lookup_sorted_table_local_variables(CompilerData *compiler_data,Vector<zs_int> *order_local_vars){

		ScriptFunction *sf = compiler_data->current_function->script_function;

		int n_local_variable=0;
		if(compiler_all_local_variables_in_scopes_already_sorted(sf->scope,n_local_variable) == true){
			return NULL;
		}

		int n_var_fun=sf->local_variables->length();
		int n_var_scope=sf->scope->countVariables(true);

		if(n_var_fun != n_var_scope){

			EVAL_ERROR(
				 "internal: n_var_fun != n_var_scope (%i!=%i)"
				,n_var_fun
				,n_var_scope
			);
		}

		short *lookup_linear_stk=(short *)ZS_MALLOC(sizeof(short)*n_var_fun);
		ScriptScope *current_scope=sf->scope;
		n_local_variable=0;

		compiler_fill_lookup_local_variable(current_scope,lookup_linear_stk,n_local_variable,order_local_vars);

		return lookup_linear_stk;
	}

	int compiler_pop_and_compile_function(CompilerData *compiler_data){

		String static_error;
		ScriptFunction *sf = compiler_data->current_function->script_function;
		ScriptType *sc_sf = ZS_GET_OBJECT_TYPE(compiler_data->script_types_factory,sf->owner_script_type_id);
		int sum_stk_load_stk=0;
		int max_acc_stk_load=0;

		// remove old instructions
		if(sf->instructions != NULL){
			free(sf->instructions);
			sf->instructions=NULL;
		}

		// remove old ref symbols
		for(int i=0; i < sf->instruction_source_infos.length(); i++){
			InstructionSourceInfo *isi=(InstructionSourceInfo *)sf->instruction_source_infos.get(i);
			delete isi;
		}

		sf->instruction_source_infos.clear();

		// get total size op + 1 ends with 0 (INVALID BYTE_CODE)
		int count =compiler_data->current_function->compiler_instructions.length();
		int len=count + 1; // +1 for end instruction
		int total_size_bytes = (len) * sizeof(Instruction);
		sf->instructions_len=len;
		sf->instructions = (PtrInstruction)ZS_MALLOC(total_size_bytes);
		Vector<zs_int> order_local_vars;
		Symbol *symbol_sf_foundf=NULL;
		String target_name="";

		int ok=FALSE;
		short *lookup_sorted_table_local_variables=compiler_create_lookup_sorted_table_local_variables(compiler_data,&order_local_vars);

		for(int i=0; i < count; i++){

			CompilerInstruction *compiler_instruction = (CompilerInstruction *)compiler_data->current_function->compiler_instructions.get(i);

			int req_stk=instruction_num_required_stack(&compiler_instruction->vm_instruction);
			if(req_stk==ZS_NUM_REQUIRED_BYTE_CODE_ID_NOT_MANAGED){
				ZS_THROW_RUNTIME_ERROR("byte_code_num_required_stack: byte_code '%i' not managed", compiler_instruction->vm_instruction.byte_code);
			}

			sum_stk_load_stk+=req_stk;
			max_acc_stk_load=ZS_MAX(max_acc_stk_load,sum_stk_load_stk);


			switch(compiler_instruction->vm_instruction.byte_code){
			default:
				break;
			case BYTE_CODE_SUPER_CALL:
				// get current function name and find first ancestor in heritance
				// find constructor symbol through other members...
				for(int j = sf->idx_position-1; j >=0 && symbol_sf_foundf==NULL; j--){
					Symbol *symbol_member = (Symbol *)sc_sf->scope->symbol_functions->get(j);
					ScriptFunction *sf_member=(ScriptFunction *)symbol_member->ref_ptr;
					bool match_names=sf_member->name == sf->name;
					bool match_params=(sf_member->properties & SYMBOL_PROPERTY_NATIVE_OBJECT_REF?match_names:true);
					if(
							(match_names)
						&& (match_params)
						){
						symbol_sf_foundf = symbol_member;
					}
				}

				// ok get the super function...
				if(symbol_sf_foundf == NULL){
					if(sf->name != sf->name){
						EVAL_ERROR_FILE_LINE_GOTO_NO_AUX(
							compiler_data->current_parsing_file
							,compiler_instruction->instruction_source_info.line
							,lbl_exit_pop_function
							,"Cannot find parent constructor of '%s'"
							,sc_sf->name.toConstChar()
						);
					}else{
						EVAL_ERROR_FILE_LINE_GOTO_NO_AUX(
							compiler_data->current_parsing_file
							,compiler_instruction->instruction_source_info.line
							,lbl_exit_pop_function
							,"Cannot find parent function of '%s::%s'"
							,sc_sf->name.toConstChar()
							,sf->name.toConstChar()
						);
					}
				}
				compiler_instruction->vm_instruction.value_op2=(zs_int)symbol_sf_foundf;//->idx_position;
				compiler_instruction->instruction_source_info.ptr_str_symbol_name =get_mapped_name(
						compiler_data
						,String(
								symbol_sf_foundf->scope->owner_type->name)+"::"+symbol_sf_foundf->name);

				break;
			case BYTE_CODE_CALL:
				if(compiler_instruction->vm_instruction.value_op2==ZS_UNDEFINED_IDX){
					compiler_instruction->vm_instruction.byte_code=BYTE_CODE_UNRESOLVED_CALL;
					compiler_data->script_engine->addUnresolvedSymbol(sf,i);
				}
				break;
			case BYTE_CODE_THIS_CALL:
				if(compiler_instruction->vm_instruction.value_op2==ZS_UNDEFINED_IDX){
					compiler_data->script_engine->addUnresolvedSymbol(sf,i);
				}
				break;
			case BYTE_CODE_INSTANCEOF:
				if(compiler_instruction->vm_instruction.value_op2==ZS_UNDEFINED_IDX){
					compiler_data->script_engine->addUnresolvedSymbol(sf,i);
				}
				break;
			case BYTE_CODE_FIND_VARIABLE:
				// add instruction reference to solve later
				compiler_data->script_engine->addUnresolvedSymbol(sf,i);
				break;
			}

			if(compiler_instruction->vm_instruction.byte_code == BYTE_CODE_RESET_STACK
				|| compiler_instruction->vm_instruction.byte_code == BYTE_CODE_RET
				|| (compiler_instruction->vm_instruction.properties & INSTRUCTION_PROPERTY_RESET_STACK)
			){
				// <-- reset stack
				sum_stk_load_stk=0; // and reset stack
			}

			//------------------------ SET NEW SORTED POSITION


			if(	(lookup_sorted_table_local_variables != NULL)
							&&
				// the instruction does not refers to this variable which is not in the scope of this sort
				((compiler_instruction->vm_instruction.properties & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) == 0)
			){
				// add any instruction that references global instruction
				ByteCode byte_code=compiler_instruction->vm_instruction.byte_code;
				uint16_t properties_1=compiler_instruction->vm_instruction.properties;
				uint16_t properties_2=(uint16_t)(compiler_instruction->vm_instruction.value_op2 & 0xffff);

				switch(byte_code){ // reallocate instructions
				case BYTE_CODE_INDIRECT_LOCAL_CALL:
				case BYTE_CODE_LOAD_LOCAL:
				case BYTE_CODE_LOAD_REF:
				case BYTE_CODE_PUSH_STK_LOCAL:
					compiler_instruction->vm_instruction.value_op2=lookup_sorted_table_local_variables[compiler_instruction->vm_instruction.value_op2];
					break;
				default:
					// check if local access
					if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_R) {// R
						compiler_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[compiler_instruction->vm_instruction.value_op1];
					}else if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_KR) {// KR
						compiler_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[compiler_instruction->vm_instruction.value_op1];
					}else if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_RK){ // RK
						compiler_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[compiler_instruction->vm_instruction.value_op1];
					}else if(properties_1 & INSTRUCTION_PROPERTY_ILOAD_RR){ // is RR local access
						compiler_instruction->vm_instruction.value_op1=lookup_sorted_table_local_variables[compiler_instruction->vm_instruction.value_op1];
						compiler_instruction->vm_instruction.value_op2=(lookup_sorted_table_local_variables[compiler_instruction->vm_instruction.value_op2>>16]<<16) | properties_2;
					}
				}

			}

			// set min stk required
			sf->min_code_stack_needed=max_acc_stk_load;

			//------------------------ SORT ALL LOCAL VARIABLES
			// save instruction ...
			sf->instructions[i]=compiler_instruction->vm_instruction;

			//------------------------------------
			// symbol value to save at runtime ...
			InstructionSourceInfo instruction_info=compiler_instruction->instruction_source_info;

			// Save str_symbol that was created on compiler process, and is destroyed when compiler finish.
			instruction_info.ptr_str_symbol_name=compiler_instruction->instruction_source_info.ptr_str_symbol_name;

			sf->instruction_source_infos.push(new InstructionSourceInfo(instruction_info));
		}

		if(lookup_sorted_table_local_variables != NULL){

			// update variables symbol...
			Vector<Symbol *> *local_vars_dst=new Vector<Symbol *>();

			for(int i=0; i < sf->local_variables->length(); i++){
				int idx_var=order_local_vars.get(i);
				Symbol *s=(Symbol *)sf->local_variables->get(idx_var);
				s->idx_position=lookup_sorted_table_local_variables[idx_var];
				local_vars_dst->push(s);
			}

			delete sf->local_variables;

			// set vector with all local variables sorted
			sf->local_variables=local_vars_dst;


			free(lookup_sorted_table_local_variables);
		}

		ok=TRUE;

lbl_exit_pop_function:

		compiler_pop_current_function(compiler_data);

		// update min stk needed
		if(sf->min_code_stack_needed<sum_stk_load_stk){
			sf->min_code_stack_needed=sum_stk_load_stk;
		}

		return ok;
	}
}
