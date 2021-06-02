#include "eval.h"

namespace zetscript{



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
					 class_name
					,base_class_name
					,__FILE__
					, __LINE__
				);
			}catch(std::exception &ex){
				eval_data->error=true;
				eval_data->error_str=ex.what();
				return NULL;
			}

			ZS_LOG_DEBUG("registered class \"%s\" line %i ",class_name.c_str(), class_line);

			if(*aux_p == '{' ){

				IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

				// TODO: Register class and baseof
				// register info class ...
				// check for named functions or vars...
				while(*aux_p != '}' && *aux_p != 0){
					char *test_attrib=aux_p;
					int test_line_attrib=line;
					if((test_attrib=eval_keyword_class_attrib(
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
									,sc->symbol_class.scope // pass class scope
								);
								break;
						case Keyword::KEYWORD_UNKNOWN: // supposes a member function
							// function member without function keyword
							aux_p=eval_keyword_function(eval_data
									,aux_p
									, line
									,sc->symbol_class.scope
							);

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
					}else{ // parsed detected an attrib
						aux_p = test_attrib;
					}
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
		key_w = eval_is_keyword(aux_p);

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

		IGNORE_BLANKS(aux_p,eval_data,end_var,line);

		if(*aux_p=='{'){ // is a class attribute

			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
			Symbol *symbol=NULL;
			Symbol *symbol_attrib=NULL;
			MemberAttribute *ma=NULL;
			char *end_var;
			std::string function_name;
			std::string error;

			try{
				symbol_attrib=sc->registerMemberAttribute(
						 attrib_name
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

			ma=(MemberAttribute *)symbol_attrib->ref_ptr;
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

					// set getter/setter
					aux_p=end_var;

					if((aux_p = eval_keyword_function(
						eval_data
						,aux_p
						, line
						,scope_info // pass class scope
						, EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_ATTRIB | EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS
						,&symbol
						,attrib_name+"@"+function_name
					))==NULL){
						return NULL;
					}

					if(function_name == "_set"){
						if(ma->setters.count == 0){
							ma->addSetter((ScriptFunction *)symbol->ref_ptr);
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Attribute \"%s\" has already a setter"
								,attrib_name.c_str()
							);
						}
					}else if(function_name == "_get"){
						if(ma->getter==NULL){
							ma->getter=(ScriptFunction *)symbol->ref_ptr;
						}else{
							EVAL_ERROR_FILE_LINE(
								eval_data->current_parsing_file
								,line
								,"Attribute \"%s\" has already a getter"
								,attrib_name.c_str()
							);
						}
					}else{

						EVAL_ERROR_FILE_LINE(
							eval_data->current_parsing_file
							,line
							,"unexpected metamethod \"%s\" in attribute \"%s::%s\""
							,function_name.c_str()
							,class_attribute_name.c_str()
							,attrib_name.c_str()
						);
					}

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
			}

			if(*aux_p != '}'){
				EVAL_ERROR_FILE_LINE(
					eval_data->current_parsing_file,attrib_start_line
					,"expected '}' to end in attribute \"%s::%s\""
					,class_attribute_name.c_str()
					,attrib_name.c_str()
				);
			}

			return aux_p+1;

		}

		return NULL;

	}

}





