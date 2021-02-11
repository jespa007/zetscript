#include "eval.h"

namespace zetscript{

	namespace eval{

		char * eval_keyword_class_attrib(EvalData *eval_data, const char *s, int & line	, ScriptClass *sc	);

		//------------------------------------------------------------------------------------------------------------------------------------------
		//
		// CLASS
		//

		char * eval_keyword_class(EvalData *eval_data,const char *s,int & line, Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			int class_line;
			std::string class_name;
			std::string base_class_name="";
			ScriptClass *sc;
			Keyword key_w;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_CLASS){

				char *test_attrib_s=NULL;
				int  test_attrib_line=-1;


				if(scope_info->scope_parent!=NULL){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"class keyword is not allowed");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

				// check for symbol's name
				aux_p=get_name_identifier_token(
						eval_data
						,aux_p
						,line
						,class_name
				);

				// try to register class...
				class_line = line;

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(strncmp(aux_p, "extends",7)==0 ){ // extension class detected
					IGNORE_BLANKS(aux_p,eval_data,aux_p+7,line);
					aux_p=get_name_identifier_token(
							eval_data
							,aux_p
							,line
							,base_class_name
					);

					IGNORE_BLANKS(aux_p,eval_data,aux_p, line);
				}

				// register class
				try{
					sc=eval_data->script_class_factory->registerClass(
						__FILE__
						, __LINE__
						, class_name
						,base_class_name
					);
				}catch(std::exception &ex){
					eval_data->error=true;
					eval_data->error_str=ex.what();
					return NULL;
				}

				ZS_PRINT_DEBUG("registered class \"%s\" line %i ",class_name.c_str(), class_line);

				if(*aux_p == '{' ){

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					// TODO: Register class and baseof
					// register info class ...
					// check for named functions or vars...
					while(*aux_p != '}' && *aux_p != 0){
						/*char *test_attrib=aux_p;
						if((test_attrib=eval_keyword_class_attrib(
							eval_data
							,aux_p
							, line
							,sc->symbol_class.scope // pass class scope
						))==NULL){*/

							// 1st. check whether eval a keyword...
							key_w = is_keyword(aux_p);

							switch(key_w){
							// functions
							case Keyword::KEYWORD_STATIC:
							case Keyword::KEYWORD_FUNCTION:

									aux_p = eval_keyword_function(
										eval_data
										,aux_p
										, line
										,sc->symbol_class.scope // pass class scope
									);
									break;
							case Keyword::KEYWORD_UNKNOWN: // supposes a member function
								if((test_attrib_s=eval_keyword_class_attrib(eval_data
										,aux_p
										, test_attrib_line
										,sc
								))!=NULL){
									aux_p=test_attrib_s;
									line=test_attrib_line;
								}else{
									if(eval_data->error){
										return NULL;
									}
									// function member without function keyword
									aux_p=eval_keyword_function(eval_data
											,aux_p
											, line
											,sc->symbol_class.scope
									);
								}
								break;
							case Keyword::KEYWORD_VAR:
							case Keyword::KEYWORD_CONST: // const symbol
									aux_p = eval_keyword_var(
										eval_data
										,aux_p
										, line
										,sc->symbol_class.scope // pass class scope
									);

									if(aux_p != NULL){ // particular case where var declaration ends with ';'
										if(*aux_p == ';'){
											aux_p++;
										}
									}

									break;
							default:
								EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"unexpected keyword \"%s\" in class declaration \"%s\"",eval_data_keywords[key_w].str,class_name.c_str());
							}

							if(aux_p == NULL){
								return NULL;
							}
						/*}else{ // parsed detected an attrib
							aux_p = test_attrib;
						}*/
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					}

					if(*aux_p != '}'){
						EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,class_line ,"expected '}' to end class declaration \"%s\"",class_name.c_str());
					}

					return aux_p+1;

				}else{
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected '{' to start class declaration\"%s\"",class_name.c_str());
				}
			}
			return NULL;
		}

		char * is_class_member_extension(EvalData *eval_data,const char *s,int & line,ScriptClass **sc,std::string & member_symbol){

			char *aux_p = (char *)s;
			std::string class_name;
			*sc=NULL;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check whwther the function is anonymous or not.
			aux_p=get_name_identifier_token(
					eval_data
					,aux_p
					,line
					,class_name);

			if(aux_p == NULL){
				return NULL;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

				if((*sc=GET_SCRIPT_CLASS(eval_data,class_name)) != NULL){
					aux_p=get_name_identifier_token(
							eval_data
							,aux_p+2
							,line
							,member_symbol
					);
					return aux_p;
				}else{
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"class %s not found",class_name.c_str());
				}
			}
			return NULL;
		}

		char * eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			std::string symbol_value;
			Keyword key_w;
			EvalInstruction *eval_instruction;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_DELETE){

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

				aux_p=get_name_identifier_token(
						eval_data
						,aux_p
						,line
						,symbol_value
				);

				eval_data->current_function->instructions.push_back(eval_instruction=new EvalInstruction(BYTE_CODE_FIND_VARIABLE));
				eval_instruction->symbol.name=symbol_value;
				eval_instruction->symbol.scope=scope_info;

				eval_instruction->instruction_source_info=InstructionSourceInfo(
					 eval_data->current_parsing_file
					 ,line
					 ,get_mapped_name(eval_data,symbol_value)
				);

				eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_DELETE));

				return aux_p;
			}

			return NULL;
		}

		char * eval_keyword_class_attrib(
				EvalData *eval_data
				, const char *s
				, int & line
				, ScriptClass *sc
			){

			char *aux_p = (char *)s;
			char *end_var = NULL;
			std::string attrib_name="";
			int attrib_start_line;
			std::string class_attribute_name=sc->symbol_class.name;
			Scope *scope_info=sc->symbol_class.scope;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			attrib_start_line = line;

			end_var=get_name_identifier_token(
					eval_data
					,aux_p
					,line
					,attrib_name
			);


			if(end_var == NULL){
				return NULL;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p=='{'){ // is a class attribute



				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				// here we only expect to have _set and _get functions

				while(*aux_p != '}' && *aux_p != 0){
					/*char *test_attrib=aux_p;
					if((test_attrib=eval_keyword_class_attrib(
						eval_data
						,aux_p
						, line
						,sc->symbol_class.scope // pass class scope
					))==NULL){*/
						//sc->registerMemberAttrib();

						// 1st. check whether eval a keyword...
						Keyword key_w = is_keyword(aux_p);

						switch(key_w){
						// functions
						case Keyword::KEYWORD_FUNCTION:
						case Keyword::KEYWORD_UNKNOWN:
							aux_p = eval_keyword_function(
								eval_data
								,aux_p
								, line
								,sc->symbol_class.scope // pass class scope
							);
							break;
						default:
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"unexpected keyword \"%s\" in attribute \"%s::%s\""
									,eval_data_keywords[key_w].str
									,class_attribute_name.c_str()
									,attrib_name.c_str()
									);
						}

						if(aux_p == NULL){
							return NULL;
						}
					/*}else{ // parsed detected an attrib
						aux_p = test_attrib;
					}*/
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				}

				if(*aux_p != '}'){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,attrib_start_line ,"expected '}' to end in attribute \"%s::%s\""
							,class_attribute_name.c_str()
							,attrib_name.c_str()
							);
				}

				return aux_p+1;

			}

			return NULL;

			// copy value
			//zs_strutils::copy_from_ptr_diff(function_name,aux_p,end_var);

			// it starts with name + {

		/*	fprintf(stderr,"===============================\n");
			fprintf(stderr,"eval_keyword_class_attrib not implemented yet\n");
			fprintf(stderr,"===============================\n");*/

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			/*ScriptClass *sc=NULL; // if NULL it suposes is the main
			char *aux_p = (char *)s;
			Keyword key_w=is_keyword(aux_p);
			bool is_static = false;
			char *end_var = NULL;
			std::string error;

			if(is_keyword(aux_p) == Keyword::KEYWORD_UNKNOWN){
				return NULL;
			}
			//Keyword key_w;
			//
			// check for keyword ...
			if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_MAIN
				&& scope_info->scope_base == scope_info
				&& scope_info->scope_parent == NULL // is function member
				){ // class members are defined as functions
				sc=scope_info->script_class;
			}
			else{
				return NULL;
			}



			Symbol *symbol_sf=NULL;
			Symbol * irv=NULL;


			std::string attrib_name="";
			//Scope *scope=scope_info;
			bool is_anonymous=false;

			if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_MAIN
				&& scope_info->scope_base == scope_info
				&& scope_info->scope_parent == NULL // is function member
				){
				sc=scope_info->script_class;
			}

			//std::string arg_value;
			//FunctionArg arg_info;

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
				function_name="_@afun_"+(scope_info->script_class!=SCRIPT_CLASS_MAIN(eval_data)?scope_info->script_class->symbol_class.name:"")+"_"+zs_strutils::int_to_str(n_anonymous_function++);
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

			pop_function(eval_data);

			return aux_p;*/

			return NULL;
		}

	}
}




