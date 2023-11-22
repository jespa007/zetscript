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
		zs_string name;
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
				,name
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
				 name
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

		ZS_LOG_DEBUG("registered type '%s' line %i ",name.c_str(), class_line);

		if(*aux_p != '{' ){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected 'extends' or '{' to after class declaration'%s'",name.c_str());
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
							,sc->scope // pass type scope
						);
						break;
				case Keyword::KEYWORD_UNKNOWN: // supposes a member function
					// function member without function keyword
					aux_p=eval_keyword_function(eval_data
							,aux_p
							, line
							,sc->scope
					);

					break;
				case Keyword::KEYWORD_VAR:
				case Keyword::KEYWORD_CONST: // const symbol
						aux_p = eval_keyword_var(
							eval_data
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
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"unexpected keyword '%s' in class declaration '%s'",eval_data_keywords[key_w].str,name.c_str());
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
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,class_line ,"expected '}' to end class declaration '%s'",name.c_str());
		}

		return aux_p+1;
	}

	char * is_class_member_extension(EvalData *eval_data,const char *s,int & line,ScriptType **sc,zs_string & member_symbol){

		char *aux_p = (char *)s;
		zs_string name;
		*sc=NULL;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check whwther the function is anonymous or not.
		aux_p=get_name_identifier_token(
				eval_data
				,aux_p
				,line
				,name);

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

			if((*sc=GET_SCRIPT_TYPE(eval_data->script_type_factory,name)) == NULL){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Error access '%s::%s'. Type '%s' not defined"
					,name.c_str()
					,member_symbol.c_str()
					,name.c_str()
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

			if((tst_op_aux=is_operator(aux_p))!=ZS_OPERATOR_UNKNOWN){
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

			if((tst_op_aux=is_operator(aux_p))!=ZS_OPERATOR_UNKNOWN){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error 'delete': unexpected operator '%s' after delete identifier ",eval_data_operators[tst_op_aux].str);
			}

			// get last instruction...
			eval_instruction = eval_data->current_function->eval_instructions.get(eval_data->current_function->eval_instructions.size()-1);
			ByteCode  byte_code=eval_instruction->vm_instruction.byte_code;
			if(byte_code==BYTE_CODE_FIND_VARIABLE){
				eval_instruction->vm_instruction.properties|=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}else if(ByteCodeHelper::isLoadVarType(byte_code)){
				eval_instruction->vm_instruction.byte_code=ByteCodeHelper::loadVarTypeToPushStk(byte_code);
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
		zs_string class_property_name=sc->name;
		Scope *scope_info=sc->scope;

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
			
			zs_string name;
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
							,name
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
						, ZS_EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY | ZS_EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
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
								break;
							}else{
								EVAL_ERROR_FILE_LINE(
									eval_data->current_parsing_file
									,line
									,"Property '%s' has already a '%s' implemented"
									,property_name.c_str()
									,it->name
								);
							}
						}

						it++;
					}

					if(it->name==NULL){
						// find setter

						MetamethodMemberSetterInfo _mp_info=mp->metamethod_members.getSetterInfo(name.c_str());

						if(_mp_info.metamethod!=METAMETHOD_INVALID){
							if(_mp_info.setters->size() == 0){
								mp->metamethod_members.addSetter(_mp_info.metamethod,symbol);
							}else{
								EVAL_ERROR_FILE_LINE(
									eval_data->current_parsing_file
									,line
									,"Property '%s' has already a setter '%s'"
									,property_name.c_str()
									,_mp_info.metamethod_name
								);
							}
						}else{

							// metamethod not found !

							zs_string list_valid_metamethods="";


							list_valid_metamethods+=zs_string("- '")+MetamethodHelper::getSymbolName(METAMETHOD_SET)+"'\n";
							list_valid_metamethods+=zs_string("- '_get'\n");

							// get all member list
							const Metamethod *it_mm=MetamethodMembers::member_list;
							while(*it_mm!=0){
								if(*it_mm!=METAMETHOD_SET){
									list_valid_metamethods+=zs_string("- '")+ MetamethodHelper::getSymbolName(*it_mm)+"'\n";
								}
								it_mm++;
							}


							// get all member setter listMetamethodMembers::MetamethodHelper::member_setter_list
							const Metamethod *it_setters=MetamethodMembers::member_setter_list;
							while(*it_setters!=0){
								if(*it_setters!=METAMETHOD_SET){
									list_valid_metamethods+=zs_string("- '")+MetamethodHelper::getSymbolName(*it_setters)+"'\n";
								}
								it_setters++;
							}


							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"unexpected metamethod '%s' in property '%s::%s'. Valid metamethods are the following:\n\n%s\n"
								,name.c_str()
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





