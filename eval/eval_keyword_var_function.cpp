#include "eval.h"

namespace zetscript{

	namespace eval{

		static int n_anonymous_function=0;

		//
		//--------------------------------------------------------------------------------------------------------------------------------------------------------
		//
		//  VAR/FUNCTION
		//
		char * eval_keyword_var(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass
			// check for keyword ...
			char *aux_p = (char *)s;
			Keyword key_w = is_keyword(s);
			std::vector<EvalInstruction *> member_var_init_instructions;

			if(key_w == Keyword::KEYWORD_VAR || key_w == Keyword::KEYWORD_CONST){ // possible variable...
				bool is_static = false,
					 is_constant = false,
				 	 is_class_scope=false,
				 	 end=false;

				Scope *scope_var=scope_info;
				ScriptFunction *sf_field_initializer=NULL;


				char *start_var,*end_var;
				ScriptClass *sc=NULL;
				std::string s_aux,variable_name,pre_variable_name="";
				std::string error;
				Symbol *symbol_variable,*symbol_member_variable=NULL;
				is_constant=key_w == Keyword::KEYWORD_CONST;
				Operator ending_op;

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

				// check class scope...
				if(scope_var->script_class->idx_class != IDX_BUILTIN_TYPE_CLASS_MAIN
					&& scope_var->scope_base == scope_var
					&& scope_var->scope_parent == NULL // is function member
				){ // class members are defined as functions
					sc=scope_var->script_class;
					is_class_scope=true;
				}

				if(is_constant){ // scope_var will be global scope...
					if(!(sc!=NULL || scope_var == MAIN_SCOPE(eval_data))){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"\"const\" is allowed only in class or global");
					}

					// always static or constant are global symbols...
					scope_var = MAIN_SCOPE(eval_data);

					if(sc!=NULL){
						pre_variable_name=sc->symbol_class.name+"::";
					}
				}else if(sc != NULL){
					sf_field_initializer=sc->sf_field_initializer;
				}


				do{
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					start_var=aux_p;
					end_var=NULL;
					ScriptClass *sc_var_member_extension=sc;

					if(sc==NULL){
						if((end_var=is_class_member_extension( // is function class extensions (example A::function1(){ return 0;} )
							eval_data
							,aux_p
							,line
							,&sc_var_member_extension
							,variable_name
					   ))!=NULL){
							if(is_class_scope==true){
								EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected ::");
							}
							sf_field_initializer=sc_var_member_extension->sf_field_initializer;
						}else{ // NULL check if error
							if(eval_data->error){
								return NULL;
							}
						}
					}

					if(end_var==NULL){
						// check whwther the function is anonymous with a previous arithmetic operation ....
						if((end_var=get_name_identifier_token(
								eval_data,
								aux_p,
								line,
								variable_name
						))==NULL){
							return NULL;
						}
					}

					ZS_PRINT_DEBUG("registered symbol \"%s\" line %i ",variable_name.c_str(), line);

					Keyword keyw = is_keyword(variable_name.c_str());

					if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Cannot use symbol name as reserverd symbol \"%s\"",eval_data_keywords[keyw].str);
					}

					// register symbol...
					try{

						bool is_local_or_const=is_constant || sc_var_member_extension==NULL;
						bool is_const_member_or_var_member=sc_var_member_extension != NULL;

						if(is_local_or_const){ // is constant or is local (sc_var_member_extension==NULL)
							symbol_variable=eval_data->current_function->script_function->registerLocalVariable(
								scope_var
								, eval_data->current_parsing_file
								, line
								, pre_variable_name+variable_name
							);
						}

						if( is_const_member_or_var_member){
							symbol_member_variable=sc_var_member_extension->registerMemberVariable(
								error
								,eval_data->current_parsing_file
								,line
								,variable_name
								,is_constant?SYMBOL_PROPERTY_CONST | SYMBOL_PROPERTY_STATIC :0
							);

							if(symbol_member_variable==NULL){
								EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"%s",error.c_str());
							}

							if(is_constant == true){
								symbol_member_variable->ref_ptr=symbol_variable->idx_position;
							}

						}
					}catch(std::exception & ex){
						EVAL_ERROR("%s",ex.what());
					}


					// advance identifier length chars
					aux_p=end_var;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p == '='){

						int aux_line=line;
						bool is_var_member_or_const=sc_var_member_extension!=NULL || is_constant == true;
						bool is_var_member=sf_field_initializer!=NULL && is_constant==false;

						if((aux_p = eval_expression(
							eval_data
							,is_var_member_or_const?aux_p+1:start_var
							,aux_line
							,scope_var
							,is_var_member?&member_var_init_instructions:&eval_data->current_function->instructions
							,{}
							,0 //is_constant==true||sf_field_initializer ? 0:EVAL_EXPRESSION_ON_MAIN_BLOCK
						))==NULL){
							goto error_eval_keyword_var;
						}

						if(is_var_member){ // check load and set find

							eval_generate_byte_code_field_initializer(eval_data,sf_field_initializer,&member_var_init_instructions,symbol_member_variable);

							// set variable as member
							//(Instruction )((uint8_t *)sf_field_initializer->instructions+sf_field_initializer->instructions_len)

							for(size_t i=0; i < member_var_init_instructions.size(); i++){
								delete member_var_init_instructions[i];
							}

							member_var_init_instructions.clear();
						}
						else{
							if(is_constant){ // make ptr as constant after variable is saved
								EvalInstruction *eval_instruction;
								eval_data->current_function->instructions.push_back(eval_instruction=new EvalInstruction(
									BYTE_CODE_LOAD_GLOBAL
								));

								eval_instruction->vm_instruction.value_op2=symbol_variable->idx_position;
								eval_instruction->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data, pre_variable_name+variable_name);
								eval_instruction->symbol.name=pre_variable_name+variable_name;
								eval_instruction->symbol.scope=MAIN_SCOPE(eval_data);

								eval_data->current_function->instructions.push_back(new EvalInstruction(
									BYTE_CODE_STORE_CONST
								));
							}

							// finally we insert reset stack
							eval_data->current_function->instructions.push_back(new EvalInstruction(
								BYTE_CODE_RESET_STACK
							));
						}

						line = aux_line;
					}
					else if(is_constant){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Uninitialized constant symbol %s%s"
								,sc_var_member_extension!=NULL?zs_strutils::format("::%s",sc->symbol_class.name.c_str()).c_str():""
								,variable_name.c_str());
					}

					if(*aux_p == ',' /*|| *aux_p==')'*/){ // is new variable
						aux_p++;

					}else{
						end=true;
					}

				}while(!end); // is new variable

				// after variable declaration is expected to have any keyword but is not valid any operator,
				if((ending_op=is_operator(aux_p))!=Operator::OPERATOR_UNKNOWN){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected '%s' within variable initialization",eval_data_operators[ending_op].str)
				}

				return aux_p;
			}

error_eval_keyword_var:
			for(size_t i=0; i < member_var_init_instructions.size(); i++){
				delete member_var_init_instructions[i];
			}

			member_var_init_instructions.clear();

			return NULL;
		}


		//------------------------------------------------------------------------------------------------------------------------------------------
		//
		// FUNCTION
		//
		char * eval_keyword_function(
				EvalData *eval_data
				, const char *s
				, int & line
				, Scope *scope_info
				, bool check_anonymous_function
				, std::string * resulted_function_name
			){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			ScriptClass *sc=NULL; // if NULL it suposes is the main
			char *aux_p = (char *)s;
			Keyword key_w=is_keyword(aux_p);
			bool is_static = false;

			// check if static...
			if(key_w==Keyword::KEYWORD_STATIC){

				is_static=true;
				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
			}
			//Keyword key_w;
			//
			// check for keyword ...
			if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_CLASS_MAIN
				&& scope_info->scope_base == scope_info
				&& scope_info->scope_parent == NULL // is function member
				){ // class members are defined as functions
				key_w=is_keyword(aux_p);
				if(key_w != Keyword::KEYWORD_FUNCTION){ // make it optional
					key_w = Keyword::KEYWORD_FUNCTION;
				}
				else{
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
				}
				sc=scope_info->script_class;
			}
			else{
				key_w = is_keyword(aux_p);
				if(key_w == Keyword::KEYWORD_FUNCTION){
					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
				}
			}


			if(key_w == Keyword::KEYWORD_FUNCTION){
				FunctionParam arg_info;
				bool var_args=false;
				int n_arg=0;
				char *end_var = NULL;
				std::string arg_value;
				std::string error;
				//size_t advance_chars=0;


				std::vector<FunctionParam> args={};
				std::string conditional_str;
				Symbol *symbol_sf=NULL;

				Symbol * irv=NULL;


				std::string function_name="";
				//Scope *scope=scope_info;
				bool is_anonymous=false;

				Scope *scope_function =eval_new_scope(eval_data,scope_info,true); // push current scope
				ScriptFunction *sf=NULL;

				// advance keyword...
				//aux_p += advance_chars;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				bool named_function = *aux_p!='(';

				if(named_function){ // is named function..

					if(check_anonymous_function){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected anonymous function");
					}

					// function cannot be declared within main scope
					if(scope_info != MAIN_SCOPE(eval_data) && sc == NULL){ // function within a function (not function member)
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"named functions are only allowed in main scope. You can only use anonymous functions");
					}

					if(sc==NULL){ // check if function member declaration
					   end_var=is_class_member_extension( // is function class extensions (example A::function1(){ return 0;} )
							eval_data
							,aux_p
							,line
							,&sc
							,function_name
					   );
					}

					// not member function, so is normal function ...
					if(end_var == NULL){ // global function
						// check whwther the function is anonymous with a previous arithmetic operation ....
						end_var=get_name_identifier_token(
								eval_data
								,aux_p
								,line
								,function_name
						);
						// copy value
						zs_strutils::copy_from_ptr_diff(function_name,aux_p,end_var);
					}
					aux_p=end_var;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				}
				else{ // name anonymous function
					if(check_anonymous_function==false){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Anonymous functions should be used on expression");
					}

					is_anonymous=true;

					if(
							scope_info->script_class != SCRIPT_CLASS_MAIN(eval_data)
						 && scope_info->scope_parent != NULL
					){
						sc=scope_info->script_class;
					}
				}

				// eval function args...
				if(*aux_p != '('){ // push arguments...
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected function start argument declaration '(' ");
				}

				// save scope pointer for function args ...
				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				while(*aux_p != 0 && *aux_p != ')' && !var_args){
					arg_info.by_ref=false;
					arg_info.var_args=false;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					if(args.size()>0){
						if(*aux_p != ','){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected function argument separator ','");
						}
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
					}

					if(*aux_p == ')' || *aux_p == ','){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error: expected argument name");
					}

					// capture line where argument is...
					arg_info.line=line;

					if(*aux_p=='.' && *(aux_p+1)=='.' && *(aux_p+2)=='.'){// is_keyword(aux_p)==KEYWORD_REF){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+3,line);
						var_args=arg_info.var_args =true;
					}else if(is_keyword(aux_p)==KEYWORD_REF){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[KEYWORD_REF].str),line);
						arg_info.by_ref =true;
					}

					//int m_start_arg=line;
					end_var=get_name_identifier_token(
						 eval_data
						,aux_p
						,line
						,arg_value
					);

					// copy value
					zs_strutils::copy_from_ptr_diff(arg_value,aux_p,end_var);
					// ok register symbol into the object function ...
					arg_info.arg_name=arg_value;
					args.push_back(arg_info);

					aux_p=end_var;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				}

				aux_p++;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p != '{'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error:  expected '{' as function block");
				}

				// register function ...
				if(is_anonymous){ // register named function...
					function_name="_@afun_"+(scope_info->script_class!=SCRIPT_CLASS_MAIN(eval_data)?scope_info->script_class->symbol_class.name:"")+"_"+zs_strutils::zs_int_to_str(n_anonymous_function++);
				}


				if(resulted_function_name!=NULL){ // save function...
					*resulted_function_name=function_name;
				}

				//--- OP
				if(sc!=NULL){ // register as variable member...
					symbol_sf=sc->registerMemberFunction(
							error
							,eval_data->current_parsing_file
							,line
							,function_name
							,args
							,is_static?SYMBOL_PROPERTY_STATIC:0
					);

					if(symbol_sf == NULL){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,error.c_str());
					}

				}
				else{ // register as local variable in the function...
					symbol_sf=eval_data->current_function->script_function->registerLocalFunction(
						 scope_info
						, eval_data->current_parsing_file
						, line
						, function_name
						, args
					);

					if(scope_info->script_class != SCRIPT_CLASS_MAIN(eval_data)){ // is a function that was created within a member function...
						symbol_sf->properties|=SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS;
					}
				}

				sf=(ScriptFunction *)symbol_sf->ref_ptr;

				// register args as part of stack...
				for(unsigned i=0; i < args.size(); i++){
					try{
						sf->registerLocalVariable(
								scope_function
								,eval_data->current_parsing_file
								,args[i].line
								,args[i].arg_name
						);
					}catch(std::exception & ex){
						eval_data->error=true;
						eval_data->error_str=ex.what();
						return NULL;
					}
				}

				eval_push_function(eval_data,sf);

				// ok let's go to body..
				aux_p = eval_block(
						eval_data
						,aux_p
						,line
						,scope_function);

				eval_pop_function(eval_data);
			}
			return aux_p;
		}

		char *  eval_keyword_return(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			Keyword key_w;
			std::string s_aux;

			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_RETURN){ // possible variable...
				//PASTNode child_node=NULL;
				aux_p += strlen(eval_data_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// save starting point before process the expression...
				if((aux_p = eval_expression(
						eval_data
						,aux_p
						, line
						, scope_info
						,&eval_data->current_function->instructions
						,{}
						,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION
						//,std::vector<char>{';'}
				))!= NULL){

					eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_RET));

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					return aux_p;
				}
			}
			return NULL;
		}
	}
}

