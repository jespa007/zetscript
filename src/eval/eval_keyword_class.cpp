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
		zs_string type_name;
		zs_string base_class_name="";
		ScriptType *sc;
		Keyword key_w;
		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check for keyword ...
		key_w = eval_is_keyword(aux_p);


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
					,type_name
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
				sc=eval_data->script_type_factory->registerClass(
					 type_name
					,base_class_name
					,__FILE__
					, __LINE__
				);
			}catch(std::exception &ex){
				eval_data->error=true;
				eval_data->error_str=ex.what();
				return NULL;
			}

			ZS_LOG_DEBUG("registered class '%s' line %i ",type_name.c_str(), class_line);

			if(*aux_p == '{' ){

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				// TODO: Register class and baseof
				// register info class ...
				// check for named functions or vars...
				while(*aux_p != '}' && *aux_p != 0){
					char *test_attrib=aux_p;
					int test_line_attrib=line;
					if((test_attrib=eval_keyword_class_property(
						eval_data
						,aux_p
						,line
						,sc // pass class
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
									,sc->class_scope // pass class scope
								);
								break;
						case Keyword::KEYWORD_UNKNOWN: // supposes a member function
							// function member without function keyword
							aux_p=eval_keyword_function(eval_data
									,aux_p
									, line
									,sc->class_scope
							);

							break;
						case Keyword::KEYWORD_VAR:
						case Keyword::KEYWORD_CONST: // const symbol
								aux_p = eval_keyword_var(
									eval_data
									,aux_p
									, line
									,sc->class_scope // pass class scope
								);

								if(aux_p != NULL){ // particular case where var declaration ends with ';'
									if(*aux_p == ';'){
										aux_p++;
									}
								}

								break;
						default:
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"unexpected keyword '%s' in class declaration '%s'",eval_data_keywords[key_w].str,type_name.c_str());
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
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,class_line ,"expected '}' to end class declaration '%s'",type_name.c_str());
				}

				return aux_p+1;

			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Expected 'extends' or '{' to after class declaration'%s'",type_name.c_str());
			}
		}
		return NULL;
	}

	char * is_class_member_extension(EvalData *eval_data,const char *s,int & line,ScriptType **sc,zs_string & member_symbol){

		char *aux_p = (char *)s;
		zs_string type_name;
		*sc=NULL;

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		// check whwther the function is anonymous or not.
		aux_p=get_name_identifier_token(
				eval_data
				,aux_p
				,line
				,type_name);

		if(aux_p == NULL){
			return NULL;
		}

		IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

			if((*sc=GET_SCRIPT_CLASS(eval_data,type_name)) != NULL){
				aux_p=get_name_identifier_token(
						eval_data
						,aux_p+2
						,line
						,member_symbol
				);

				return aux_p;
			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"class %s not found",type_name.c_str());
			}
		}
		return NULL;
	}

	char * eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		zs_string symbol_value;
		Keyword key_w;
		Symbol *symbol_found=NULL;
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
			eval_instruction = (EvalInstruction *)eval_data->current_function->eval_instructions.items[eval_data->current_function->eval_instructions.count-1];
			ByteCode  byte_code=eval_instruction->vm_instruction.byte_code;
			if(byte_code==BYTE_CODE_FIND_VARIABLE){
				eval_instruction->vm_instruction.properties|=INSTRUCTION_PROPERTY_USE_PUSH_STK;
			}else if(byte_code_is_load_var_type(byte_code)){
				eval_instruction->vm_instruction.byte_code=byte_code_load_var_type_to_push_stk(byte_code);
			}

			eval_data->current_function->eval_instructions.push_back((zs_int)(new EvalInstruction(BYTE_CODE_DELETE)));
			eval_data->current_function->eval_instructions.push_back((zs_int)(new EvalInstruction(BYTE_CODE_RESET_STACK)));

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
		char *end_var = NULL;
		zs_string property_name="";
		int attrib_start_line;
		zs_string class_property_name=sc->type_name;
		Scope *scope_info=sc->class_scope;

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

		if(*aux_p=='{'){ // is a class property

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			Symbol *symbol=NULL;
			Symbol *symbol_attrib=NULL;
			MemberProperty *mp=NULL;
			char *end_var;
			zs_string function_name;
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
							,function_name
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
						,scope_info // pass class scope
						, EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY | EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
						,&symbol
						,function_name+"@"+property_name
					))==NULL){
						return NULL;
					}

					if(function_name == "_get"){
						if(mp->metamethod_members.getter==NULL){
							mp->metamethod_members.getter=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a getter"
								,property_name.c_str()
							);
						}
					}else if(function_name == "_post_inc"){
						if(mp->metamethod_members.post_inc==NULL){
							mp->metamethod_members.post_inc=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a post increment (aka i++) metamethod"
								,property_name.c_str()
							);
						}
					}else if(function_name == "_post_dec"){
						if(mp->metamethod_members.post_dec==NULL){
							mp->metamethod_members.post_dec=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a post decrement (aka i--) metamethod"
								,property_name.c_str()
							);
						}
					}else if(function_name == "_pre_inc"){
						if(mp->metamethod_members.pre_inc==NULL){
							mp->metamethod_members.pre_inc=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already  a pre increment (aka ++i) metamethod"
								,property_name.c_str()
							);
						}
					}else if(function_name == "_pre_dec"){
						if(mp->metamethod_members.pre_dec==NULL){
							mp->metamethod_members.pre_dec=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property '%s' has already a pre decrement (aka --i) metamethod"
								,property_name.c_str()
							);
						}
					}else if(function_name == "_neg"){
						if(mp->metamethod_members.neg==NULL){
							mp->metamethod_members.neg=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Property \"%s\" has already a neg (aka -) metamethod"
								,property_name.c_str()
							);
						}
					}else{ // find setter
						MetamethodMemberSetterInfo _mp_info=mp->metamethod_members.getSetterInfo(function_name.c_str());

						if(_mp_info.byte_code_metamethod!=BYTE_CODE_METAMETHOD_INVALID){
							if(_mp_info.setters->count == 0){
								mp->metamethod_members.addSetter(_mp_info.byte_code_metamethod,(ScriptFunction *)symbol->ref_ptr);
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

							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"unexpected metamethod '%s' in property '%s::%s'"
								,function_name.c_str()
								,class_property_name.c_str()
								,property_name.c_str()
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





