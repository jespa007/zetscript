/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"

namespace zetscript{



	char * compiler_keyword_class_property(CompilerData *compiler_data, const char *s, int & line	, ScriptType *sc	);

	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// CLASS
	//

	char * compiler_keyword_class(CompilerData *compiler_data,const char *s,int & line, ScriptScope *scope_info){
		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		int class_line;
		String name;
		String base_class_name="";
		ScriptType *sc;
		Keyword key_w;
		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);


		if(key_w != Keyword::KEYWORD_CLASS){
			return NULL;
		}

		if(scope_info->scope_parent!=NULL){
			EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"class keyword is not allowed");
		}

		IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

		// check for symbol's name
		aux_p=get_name_identifier_token(
				compiler_data
				,aux_p
				,line
				,name
		);

		// try to register type...
		class_line = line;

		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		if(strncmp(aux_p, "extends",7)==0 ){ // extension class detected
			IGNORE_BLANKS(aux_p,compiler_data,aux_p+7,line);
			aux_p=get_name_identifier_token(
					compiler_data
					,aux_p
					,line
					,base_class_name
			);

			IGNORE_BLANKS(aux_p,compiler_data,aux_p, line);
		}

		// register type
		try{
			sc=compiler_data->script_types_factory->registerType(
				 name
				,base_class_name
				,0
				,compiler_data->current_parsing_file
				, line
			);
		}catch(Exception &ex){
			compiler_data->error=true;
			compiler_data->str_error=ex.what();
			compiler_data->error_line=ex.getLine();
			compiler_data->error_file=ex.getFilename();
			return NULL;
		}catch(std::exception &ex){
			compiler_data->error=true;
			compiler_data->str_error=ex.what();
			return NULL;
		}

		ZS_LOG_DEBUG("registered type '%s' line %i ",name.toConstChar(), class_line);

		if(*aux_p != '{' ){
			EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Expected 'extends' or '{' to after class declaration'%s'",name.toConstChar());
		}

		IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

		// TODO: Register type and baseof
		// register info type ...
		// check for named functions or vars...
		while(*aux_p != '}' && *aux_p != 0){
			char *test_attrib=aux_p;
			int test_line_attrib=line;
			if((test_attrib=compiler_keyword_class_property(
				compiler_data
				,aux_p
				,line
				,sc // pass type
			))==NULL){

				line=test_line_attrib; // restore line

				if(compiler_data->error){
					return NULL;
				}

				// 1st. check whether compiler a keyword...
				key_w = compiler_is_keyword(aux_p);

				switch(key_w){
				// functions
				case Keyword::KEYWORD_STATIC:
				case Keyword::KEYWORD_FUNCTION:

						aux_p = compiler_keyword_function(
							compiler_data
							,aux_p
							, line
							,sc->scope // pass type scope
						);
						break;
				case Keyword::KEYWORD_UNKNOWN: // supposes a member function
					// function member without function keyword
					aux_p=compiler_keyword_function(compiler_data
							,aux_p
							, line
							,sc->scope
					);

					break;
				case Keyword::KEYWORD_VAR:
				case Keyword::KEYWORD_CONST: // const symbol
						aux_p = compiler_keyword_var(
							compiler_data
							,aux_p
							, line
							,sc->scope // pass type scope
						);

						if(aux_p != NULL){ // particular case where var declaration ends with ';'
							if(*aux_p == ';'){
								aux_p++;
							}
						}

						break;
				default:
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"unexpected keyword '%s' in class declaration '%s'",compiler_data_keywords[key_w].str,name.toConstChar());
				}

				if(aux_p == NULL){
					return NULL;
				}
			}else{ // parsed detected an attrib
				aux_p = test_attrib;
			}
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
		}

		if(*aux_p != '}'){
			EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,class_line ,"expected '}' to end class declaration '%s'",name.toConstChar());
		}

		return aux_p+1;
	}

	char * is_class_member_extension(CompilerData *compiler_data,const char *s,int & line,ScriptType **sc,String & member_symbol){

		char *aux_p = (char *)s;
		String name;
		*sc=NULL;

		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		// check whwther the function is anonymous or not.
		aux_p=get_name_identifier_token(
				compiler_data
				,aux_p
				,line
				,name);

		if(aux_p == NULL){
			return NULL;
		}

		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...
			aux_p=get_name_identifier_token(
					compiler_data
					,aux_p+2
					,line
					,member_symbol
			);

			if((*sc=ZS_GET_OBJECT_TYPE(compiler_data->script_types_factory,name)) == NULL){
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Error access '%s::%s'. ScriptType '%s' not defined"
					,name.toConstChar()
					,member_symbol.toConstChar()
					,name.toConstChar()
				);
			}

			return aux_p;
		}
		return NULL;
	}

	char * compiler_keyword_delete(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info){
		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		String symbol_value;
		Keyword key_w;
		CompilerInstruction *compiler_instruction;

		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		// check for keyword ...
		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_DELETE){
			Operator tst_op_aux;

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

			if((tst_op_aux=is_operator(aux_p))!=ZS_OPERATOR_UNKNOWN){
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Syntax error 'delete': unexpected '%s' after delete ",compiler_data_operators[tst_op_aux].str);
			}

			key_w=compiler_is_keyword(aux_p);
			if(key_w!=Keyword::KEYWORD_UNKNOWN && key_w!=Keyword::KEYWORD_THIS){
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Syntax error 'delete': unexpected '%s' after delete ",compiler_data_keywords[key_w].str);
			}

			if((aux_p = compiler_expression(
					compiler_data
					,aux_p
					,line
					,scope_info
					,&compiler_data->current_function->compiler_instructions
					,{}
					,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
			))==NULL){
				// delete unusued vars_for
				return NULL;
			}

			if((tst_op_aux=is_operator(aux_p))!=ZS_OPERATOR_UNKNOWN){
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Syntax error 'delete': unexpected operator '%s' after delete identifier ",compiler_data_operators[tst_op_aux].str);
			}

			// get last instruction...
			compiler_instruction = compiler_data->current_function->compiler_instructions.get(compiler_data->current_function->compiler_instructions.length()-1);
			ByteCode  byte_code=compiler_instruction->vm_instruction.byte_code;
			if(byte_code==BYTE_CODE_FIND_VARIABLE){
				compiler_instruction->vm_instruction.properties|=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}else if(compiler_is_byte_code_load_var_type(byte_code)){
				compiler_instruction->vm_instruction.byte_code=compiler_byte_code_load_var_type_to_push_stk(byte_code);
			}

			compiler_data->current_function->compiler_instructions.push(new CompilerInstruction(BYTE_CODE_DELETE));
			compiler_data->current_function->compiler_instructions.push(new CompilerInstruction(BYTE_CODE_RESET_STACK));

			return aux_p;
		}

		return NULL;
	}

	char * compiler_keyword_class_property(
			CompilerData *compiler_data
			, const char *s
			, int & line
			, ScriptType *sc
		){

		char *aux_p = (char *)s;
		String property_name="";
		int attrib_start_line;
		char *end_var = NULL;
		String class_property_name=sc->name;
		ScriptScope *scope_info=sc->scope;

		IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

		attrib_start_line = line;

		end_var=get_name_identifier_token(
				compiler_data
				,aux_p
				,line
				,property_name
		);


		if(end_var == NULL){
			return NULL;
		}

		IGNORE_BLANKS(aux_p,compiler_data,end_var,line);

		if(*aux_p=='{'){ // is a type property

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
			Symbol *symbol=NULL;
			Symbol *symbol_attrib=NULL;
			MemberProperty *mp=NULL;
			
			String name;
			String error;

			try{
				symbol_attrib=sc->registerMemberProperty(
						 property_name
						,compiler_data->current_parsing_file
						,attrib_start_line

				);
			}catch(std::exception & ex){

				EVAL_ERROR_FILE_LINE(
					compiler_data->current_parsing_file
					,attrib_start_line
					,ex.what()
					, NULL
				);

			}

			mp=(MemberProperty *)symbol_attrib->ref_ptr;
			//ScriptScope *scope_function =compiler_new_scope(compiler_data,scope_info); // push current scope

			// here we only expect to have _set and _get functions

			while(*aux_p != '}' && *aux_p != 0){

					// 1st. check whether compiler a keyword...
					Keyword key_w = compiler_is_keyword(aux_p);

					int line_start_property_metamethod=line;

					if(key_w == KEYWORD_FUNCTION){
						IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);
					}

					end_var=get_name_identifier_token(
							compiler_data
							,aux_p
							,line
							,name
					);

					if(end_var == NULL){
						return NULL;
					}

					// metamethod property
					aux_p=end_var;


					if((aux_p = compiler_keyword_function(
						compiler_data
						,aux_p
						, line
						,scope_info // pass type scope
						, EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY | EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
						,&symbol
						,name+"@"+property_name
					))==NULL){
						return NULL;
					}

					struct CheckSymbolInfo{
						const char *name;
						Symbol **symbol;

					}check_symbol_info[]={
							{"_get",&mp->metamethod_members.getter}
							,{"_postinc",&mp->metamethod_members.postinc}
							,{"_postdec",&mp->metamethod_members.postdec}
							,{"_preinc",&mp->metamethod_members.preinc}
							,{"_predec",&mp->metamethod_members.predec}
							//  _neg and bwc are disabled because is already defined on the value or new object
							// returned.
							//,{"_neg",&mp->metamethod_members.neg}
							//,{"_bwc",&mp->metamethod_members.bwc}
							,{NULL,NULL}
					};

					CheckSymbolInfo *it=check_symbol_info;

					while(it->name != NULL){
						if(name == it->name){
							if(*it->symbol==NULL){
								*it->symbol=symbol;


								// Member property getters SHOULD HAVE  0 parameter because in the vm it call a kind of fast call that
								// does not controls the parameters
								if(
									symbol->n_params != 0
								){
									EVAL_ERROR_FILE_LINE(
										compiler_data->current_parsing_file
										,line_start_property_metamethod
										,"Member property metamethod '%s::%s@%s' SHOULD HAVE 0 parameters"
										,class_property_name.toConstChar()
										,property_name.toConstChar()
										,it->name
									);
								}
								break;
							}else{
								EVAL_ERROR_FILE_LINE(
									compiler_data->current_parsing_file
									,line
									,"Member property metamethod '%s::%s@%s' has already a '%s' implemented"
									,class_property_name.toConstChar()
									,property_name.toConstChar()
									,it->name
								);
							}
						}

						it++;
					}

					if(it->name==NULL){
						// find setter

						MetamethodMemberSetterInfo _mp_info=mp->metamethod_members.getSetterInfo(name.toConstChar());

						if(_mp_info.metamethod!=METAMETHOD_INVALID){

							// Member property setters SHOULD HAVE  1 parameter because in the vm it call a kind of fast call that
							// does not controls the parameters
							if(
								symbol->n_params != 1
							){
								EVAL_ERROR_FILE_LINE(
									compiler_data->current_parsing_file
									,line_start_property_metamethod
									,"Member property metamethod '%s::%s@%s' SHOULD HAVE 1 parameter"
									,class_property_name.toConstChar()
									,property_name.toConstChar()
									,_mp_info.metamethod_name
								);
							}

							if(_mp_info.setters->length() == 0){
								mp->metamethod_members.addSetter(_mp_info.metamethod,symbol);
							}else{
								EVAL_ERROR_FILE_LINE(
									compiler_data->current_parsing_file
									,line
									,"Member property metamethod '%s::%s@%s' has already a setter '%s'"
									,class_property_name.toConstChar()
									,property_name.toConstChar()
									,_mp_info.metamethod_name
								);
							}
						}else{

							// metamethod not found !

							String list_valid_metamethods="";


							list_valid_metamethods+=String("- '")+MetamethodHelper::getMetamethodSymbolName(METAMETHOD_SET)+"'\n";
							list_valid_metamethods+=String("- '_get'\n");

							// get all member list
							const Metamethod *it_mm=MetamethodMembers::member_list;
							while(*it_mm!=0){
								if(*it_mm!=METAMETHOD_SET){
									list_valid_metamethods+=String("- '")+ MetamethodHelper::getMetamethodSymbolName(*it_mm)+"'\n";
								}
								it_mm++;
							}


							// get all member setter listMetamethodMembers::MetamethodHelper::member_setter_list
							const Metamethod *it_setters=MetamethodMembers::member_setter_list;
							while(*it_setters!=0){
								if(*it_setters!=METAMETHOD_SET){
									list_valid_metamethods+=String("- '")+MetamethodHelper::getMetamethodSymbolName(*it_setters)+"'\n";
								}
								it_setters++;
							}


							EVAL_ERROR_FILE_LINE(
								compiler_data->current_parsing_file
								,line
								,"unexpected metamethod '%s' in property '%s::%s'. Valid metamethods are the following:\n\n%s\n"
								,name.toConstChar()
								,class_property_name.toConstChar()
								,property_name.toConstChar()
								,list_valid_metamethods.toConstChar()
							);
						}
					}

				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
			}

			if(*aux_p != '}'){
				EVAL_ERROR_FILE_LINE(
					compiler_data->current_parsing_file,attrib_start_line
					,"expected '}' to end in property '%s::%s'"
					,class_property_name.toConstChar()
					,property_name.toConstChar()
				);
			}

			return aux_p+1;

		}

		return NULL;

	}

}





