/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{



	char * eval_keyword_class_property(EvalData *eval_data, const char *s, int & line	, ScriptType *sc	);

	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// CLASS
	//

	char * eval_keyword_class(EvalData *eval_data,const char *s,int & line, Scope *scope_info){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		int class_line;
		zs_string str_script_type;
		zs_string base_class_name="";
		ScriptType *sc;
		Keyword key_w;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);


		if(key_w != Keyword::KEYWORD_CLASS){
			return NULL;
		}

		if(scope_info->scope_parent!=NULL){
			EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"class keyword is not allowed");
		}

		IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

		// check for symbol's name
		aux_p=get_name_identifier_token(
				eval_data
				,aux_p
				,line
				,str_script_type
		);

		// try to register type...
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

		// register type
		try{
			sc=eval_data->script_type_factory->registerScriptType(
				 str_script_type
				,base_class_name
				,0
				,eval_data->current_parsing_file
				, line
			);
		}catch(zs_exception &ex){
			eval_data->error=true;
			eval_data->str_error=ex.what();
			eval_data->error_line=ex.getLine();
			eval_data->error_file=ex.getFilename();
			return NULL;
		}catch(std::exception &ex){
			eval_data->error=true;
			eval_data->str_error=ex.what();
			return NULL;
		}

		ZS_LOG_DEBUG("registered type '%s' line %i ",str_script_type.c_str(), class_line);

		if(*aux_p != '{' ){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected 'extends' or '{' to after class declaration'%s'",str_script_type.c_str());
		}

		IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

		// TODO: Register type and baseof
		// register info type ...
		// check for named functions or vars...
		while(*aux_p != '}' && *aux_p != 0){
			char *test_attrib=aux_p;
			int test_line_attrib=line;
			if((test_attrib=eval_keyword_class_property(
				eval_data
				,aux_p
				,line
				,sc // pass type
			))==NULL){

				line=test_line_attrib; // restore line

				if(eval_data->error){
					return NULL;
				}

				// 1st. check whether eval a keyword...
				key_w = eval_is_keyword(aux_p);

				switch(key_w){
				// functions
				case Keyword::KEYWORD_STATIC:
				case Keyword::KEYWORD_FUNCTION:

						aux_p = eval_keyword_function(
							eval_data
							,aux_p
							, line
							,sc->scope_script_type // pass type scope
						);
						break;
				case Keyword::KEYWORD_UNKNOWN: // supposes a member function
					// function member without function keyword
					aux_p=eval_keyword_function(eval_data
							,aux_p
							, line
							,sc->scope_script_type
					);

					break;
				case Keyword::KEYWORD_VAR:
				case Keyword::KEYWORD_CONST: // const symbol
						aux_p = eval_keyword_var(
							eval_data
							,aux_p
							, line
							,sc->scope_script_type // pass type scope
						);

						if(aux_p != NULL){ // particular case where var declaration ends with ';'
							if(*aux_p == ';'){
								aux_p++;
							}
						}

						break;
				default:
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"unexpected keyword '%s' in class declaration '%s'",eval_data_keywords[key_w].str,str_script_type.c_str());
				}

				if(aux_p == NULL){
					return NULL;
				}
			}else{ // parsed detected an attrib
				aux_p = test_attrib;
			}
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
		}

		if(*aux_p != '}'){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,class_line ,"expected '}' to end class declaration '%s'",str_script_type.c_str());
		}

		// link unreferenced forward declared functions
		for(int i=0; i < sc->scope_script_type->symbol_functions->size(); i++){
			Symbol  *symbol_sf=(Symbol *)(sc->scope_script_type->symbol_functions->items[i]);
			ScriptFunction *sf=(ScriptFunction *)symbol_sf->ref_ptr;
			Instruction *it=sf->instructions;
			if(it != NULL){
				while(it->byte_code!=BYTE_CODE_END_FUNCTION){
					if((it->byte_code==BYTE_CODE_THIS_CALL) && (it->value_op2==ZS_IDX_UNDEFINED)){
						// search function and link its idx_position
						zs_string str_name_unreferenced_this_call=SFI_GET_SYMBOL_NAME(sf,it);

						for(int j = 0; j < sc->scope_script_type->symbol_functions->size(); j++){
							Symbol *sv=(Symbol *)sc->scope_script_type->symbol_functions->items[j];
							if(
								   ( sv->name == str_name_unreferenced_this_call )
							){
								it->value_op2=(zs_int)sv;
								break;

							}
						}

					}

					it++;
				}
			}
		}


		return aux_p+1;


	}

	char * is_class_member_extension(EvalData *eval_data,const char *s,int & line,ScriptType **sc,zs_string & member_symbol){

		char *aux_p = (char *)s;
		zs_string str_script_type;
		*sc=NULL;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check whwther the function is anonymous or not.
		aux_p=get_name_identifier_token(
				eval_data
				,aux_p
				,line
				,str_script_type);

		if(aux_p == NULL){
			return NULL;
		}

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...
			aux_p=get_name_identifier_token(
					eval_data
					,aux_p+2
					,line
					,member_symbol
			);

			if((*sc=GET_SCRIPT_TYPE(eval_data->script_type_factory,str_script_type)) == NULL){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Error access '%s::%s'. Type '%s' not defined"
					,str_script_type.c_str()
					,member_symbol.c_str()
					,str_script_type.c_str()
				);
			}

			return aux_p;
		}
		return NULL;
	}

	char * eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		zs_string symbol_value;
		Keyword key_w;
		EvalInstruction *eval_instruction;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_DELETE){
			Operator tst_op_aux;

			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

			if((tst_op_aux=is_operator(aux_p))!=OPERATOR_UNKNOWN){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'delete': unexpected '%s' after delete ",eval_data_operators[tst_op_aux].str);
			}

			key_w=eval_is_keyword(aux_p);
			if(key_w!=Keyword::KEYWORD_UNKNOWN && key_w!=Keyword::KEYWORD_THIS){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'delete': unexpected '%s' after delete ",eval_data_keywords[key_w].str);
			}

			if((aux_p = eval_expression(
					eval_data
					,aux_p
					,line
					,scope_info
					,&eval_data->current_function->eval_instructions
					,{}
					,EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL
			))==NULL){
				// delete unusued vars_for
				return NULL;
			}

			if((tst_op_aux=is_operator(aux_p))!=OPERATOR_UNKNOWN){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'delete': unexpected operator '%s' after delete identifier ",eval_data_operators[tst_op_aux].str);
			}

			// get last instruction...
			eval_instruction = (EvalInstruction *)eval_data->current_function->eval_instructions.items[eval_data->current_function->eval_instructions.size()-1];
			ByteCode  byte_code=eval_instruction->vm_instruction.byte_code;
			if(byte_code==BYTE_CODE_FIND_VARIABLE){
				eval_instruction->vm_instruction.properties|=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}else if(byte_code_is_load_var_type(byte_code)){
				eval_instruction->vm_instruction.byte_code=byte_code_load_var_type_to_push_stk(byte_code);
			}

			eval_data->current_function->eval_instructions.push_back(new EvalInstruction(BYTE_CODE_DELETE));
			eval_data->current_function->eval_instructions.push_back(new EvalInstruction(BYTE_CODE_RESET_STACK));

			return aux_p;
		}

		return NULL;
	}

	char * eval_keyword_class_property(
			EvalData *eval_data
			, const char *s
			, int & line
			, ScriptType *sc
		){

		char *aux_p = (char *)s;
		zs_string property_name="";
		int attrib_start_line;
		char *end_var = NULL;
		zs_string class_property_name=sc->str_script_type;
		Scope *scope_info=sc->scope_script_type;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		attrib_start_line = line;

		end_var=get_name_identifier_token(
				eval_data
				,aux_p
				,line
				,property_name
		);


		if(end_var == NULL){
			return NULL;
		}

		IGNORE_BLANKS(aux_p,eval_data,end_var,line);

		if(*aux_p=='{'){ // is a type property

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			Symbol *symbol=NULL;
			Symbol *symbol_attrib=NULL;
			MemberProperty *mp=NULL;
			
			zs_string name_script_function;
			zs_string error;

			try{
				symbol_attrib=sc->registerMemberProperty(
						 property_name
						,eval_data->current_parsing_file
						,attrib_start_line

				);
			}catch(std::exception & ex){

				EVAL_ERROR_FILE_LINE(
					eval_data->current_parsing_file
					,attrib_start_line
					,ex.what()
					, NULL
				);

			}

			mp=(MemberProperty *)symbol_attrib->ref_ptr;
			//Scope *scope_function =eval_new_scope(eval_data,scope_info); // push current scope

			// here we only expect to have _set and _get functions

			while(*aux_p != '}' && *aux_p != 0){

					// 1st. check whether eval a keyword...
					Keyword key_w = eval_is_keyword(aux_p);

					if(key_w == KEYWORD_FUNCTION){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
					}

					end_var=get_name_identifier_token(
							eval_data
							,aux_p
							,line
							,name_script_function
					);

					if(end_var == NULL){
						return NULL;
					}

					// metamethod property
					aux_p=end_var;

					if((aux_p = eval_keyword_function(
						eval_data
						,aux_p
						, line
						,scope_info // pass type scope
						, EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY | EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
						,&symbol
						,name_script_function+"@"+property_name
					))==NULL){
						return NULL;
					}

					if(name_script_function == "_get"){
						if(mp->metamethod_members.getter==NULL){
							mp->metamethod_members.getter=symbol;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a getter"
								,property_name.c_str()
							);
						}
					}else if(name_script_function == "_post_inc"){
						if(mp->metamethod_members.post_inc==NULL){
							mp->metamethod_members.post_inc=symbol;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a post increment (aka i++) metamethod"
								,property_name.c_str()
							);
						}
					}else if(name_script_function == "_post_dec"){
						if(mp->metamethod_members.post_dec==NULL){
							mp->metamethod_members.post_dec=symbol;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a post decrement (aka i--) metamethod"
								,property_name.c_str()
							);
						}
					}else if(name_script_function == "_pre_inc"){
						if(mp->metamethod_members.pre_inc==NULL){
							mp->metamethod_members.pre_inc=symbol;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already  a pre increment (aka ++i) metamethod"
								,property_name.c_str()
							);
						}
					}else if(name_script_function == "_pre_dec"){
						if(mp->metamethod_members.pre_dec==NULL){
							mp->metamethod_members.pre_dec=symbol;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a pre decrement (aka --i) metamethod"
								,property_name.c_str()
							);
						}
					}else if(name_script_function == "_neg"){
						if(mp->metamethod_members.neg==NULL){
							mp->metamethod_members.neg=symbol;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property \"%s\" has already a neg (aka -) metamethod"
								,property_name.c_str()
							);
						}
					}else{ // find setter
						MetamethodMemberSetterInfo _mp_info=mp->metamethod_members.getSetterInfo(name_script_function.c_str());

						if(_mp_info.byte_code_metamethod!=BYTE_CODE_METAMETHOD_INVALID){
							if(_mp_info.setters->size() == 0){
								mp->metamethod_members.addSetter(_mp_info.byte_code_metamethod,symbol);
							}else{
								EVAL_ERROR_FILE_LINE(
									eval_data->current_parsing_file
									,line
									,"Property '%s' has already a setter '%s'"
									,property_name.c_str()
									,_mp_info.str_byte_code_metamethod
								);
							}
						} else{

							zs_string list_valid_metamethods="";




							// get all member list
							const ByteCodeMetamethod *it_mm=MetamethodMembers::byte_code_metamethod_member_list;
							while(*it_mm!=0){
								list_valid_metamethods+=zs_string("- '")+ byte_code_metamethod_to_symbol_str(*it_mm)+"'\n";
								it_mm++;
							}


							// get all member setter listMetamethodMembers::byte_code_metamethod_member_setter_list
							const ByteCodeMetamethod *it_setters=MetamethodMembers::byte_code_metamethod_member_setter_list;
							while(*it_setters!=0){
								list_valid_metamethods+=zs_string("- '")+byte_code_metamethod_to_symbol_str(*it_setters)+"'\n";
								it_setters++;
							}


							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"unexpected metamethod '%s' in property '%s::%s'. Valid metamethods are the following:\n\n%s\n"
								,name_script_function.c_str()
								,class_property_name.c_str()
								,property_name.c_str()
								,list_valid_metamethods.c_str()
							);
						}
					}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
			}

			if(*aux_p != '}'){
				EVAL_ERROR_FILE_LINE(
					eval_data->current_parsing_file,attrib_start_line
					,"expected '}' to end in property '%s::%s'"
					,class_property_name.c_str()
					,property_name.c_str()
				);
			}

			return aux_p+1;

		}

		return NULL;

	}

}





