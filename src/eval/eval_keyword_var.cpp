/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"



namespace zetscript{

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	//  VAR/FUNCTION
	//

	char * eval_keyword_var(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, uint16_t properties){
		// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass
		// check for keyword ...
		char *aux_p = (char *)s;
		Keyword key_w = eval_is_keyword(s);
		std::vector<EvalInstruction *> ei_member_var_init;

		if(key_w == Keyword::KEYWORD_VAR || key_w == Keyword::KEYWORD_CONST){ // possible variable...
			bool  is_constant = false,
				 is_class_scope=false,
				 end=false;

			Scope *scope_var=scope_info;



			char *start_var=NULL,*end_var=NULL;
			int start_line=0;
			ScriptType *sc=NULL;
			std::string s_aux="",variable_name="";
			std::string error="";
			Symbol *symbol_variable=NULL,*symbol_member_variable=NULL;
			is_constant=key_w == Keyword::KEYWORD_CONST;
			Operator ending_op=Operator::OPERATOR_UNKNOWN;

			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

			// check type scope...
			if(scope_var->script_type_owner->idx_script_type != IDX_TYPE_CLASS_MAIN
				&& scope_var->scope_base == scope_var
				&& scope_var->scope_parent == NULL // is function member
			){ // type members are defined as functions
				sc=scope_var->script_type_owner;
				is_class_scope=true;
			}

			do{
				int test_line=0;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				start_var=aux_p;
				start_line=line;
				end_var=NULL;
				std::string pre_variable_name="";
				ScriptFunction *sf_field_initializer=NULL;
				ScriptType *sc_var_member_extension=sc;

				if(sc_var_member_extension==NULL){
					if((end_var=is_class_member_extension( // is function type extensions (example A::function1(){ return 0;} )
						eval_data
						,start_var
						,start_line
						,&sc_var_member_extension
						,variable_name
				   ))!=NULL){
						if(is_class_scope==true){
							EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Unexpected ::");
						}
						line=start_line;
						sf_field_initializer=sc_var_member_extension->sf_field_initializer;
					}else{ // NULL check if error
						if(eval_data->error){
							return NULL;
						}
					}
				}else{
					sf_field_initializer=sc_var_member_extension->sf_field_initializer;
				}

				if((sc_var_member_extension!=NULL) && (is_constant==true)){
					scope_var = MAIN_SCOPE(eval_data);
					pre_variable_name=std::string(sc_var_member_extension->str_script_type)+"::";
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

				ZS_LOG_DEBUG("registered symbol '%s' line %i ",variable_name.c_str(), line);

				Keyword keyw = eval_is_keyword(variable_name.c_str());

				if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected keyword '%s' after var",eval_data_keywords[keyw].str);
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
						try{
							symbol_member_variable=sc_var_member_extension->registerMemberVariable(
								variable_name
								,is_constant?SYMBOL_PROPERTY_CONST | SYMBOL_PROPERTY_STATIC : 0
								,eval_data->current_parsing_file
								,line
							);

						}catch(std::exception & ex){
							EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"%s",ex.what());
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
				test_line=line;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p == '='){

					test_line=line;
					bool is_var_member_or_const=sc_var_member_extension!=NULL || is_constant == true;
					bool is_var_member=sf_field_initializer!=NULL && is_constant==false;

					if((aux_p = eval_expression(
						eval_data
						,is_var_member_or_const?aux_p+1:start_var
						,test_line
						,scope_var
						,is_var_member?&ei_member_var_init:&eval_data->current_function->eval_instructions
						,NULL
						,properties
					))==NULL){
						goto error_eval_keyword_var;
					}

					if(is_var_member){ // check load and set find

						eval_generate_byte_code_field_initializer(eval_data,sf_field_initializer,&ei_member_var_init,symbol_member_variable);

						ei_member_var_init.clear();
					}
					else{
						if(is_constant){ // make ptr as constant after variable is saved

							// unset last reset stack
							EvalInstruction *eval_instruction=(EvalInstruction *)eval_data->current_function->eval_instructions[eval_data->current_function->eval_instructions.size()-1];
							eval_instruction->vm_instruction.properties&=~INSTRUCTION_PROPERTY_RESET_STACK;

							// add instruction push
							eval_data->current_function->eval_instructions.push_back(
									eval_instruction=new EvalInstruction(
											BYTE_CODE_PUSH_STK_GLOBAL
									)
							);

							eval_instruction->vm_instruction.value_op2=symbol_variable->idx_position;
							eval_instruction->instruction_source_info.ptr_str_symbol_name=eval_get_mapped_name(eval_data, pre_variable_name+variable_name);
							eval_instruction->symbol_name=pre_variable_name+variable_name;
							eval_instruction->symbol_scope=MAIN_SCOPE(eval_data);

							eval_data->current_function->eval_instructions.push_back(
									new EvalInstruction(
										BYTE_CODE_STORE_CONST
										,1
										,ZS_IDX_UNDEFINED
										,INSTRUCTION_PROPERTY_RESET_STACK
									)
							);
						}
					}

					line = test_line;
				}
				else if(is_constant){
					EVAL_ERROR_FILE_LINE(
						eval_data->current_parsing_file
						,test_line
						,"Uninitialized constant symbol %s%s"
						,sc_var_member_extension!=NULL
							?
							zs_strutils::format("::%s",sc->str_script_type.c_str()).c_str()
							:
							""
						,variable_name.c_str()
					);
				}

				if(*aux_p == ',' /*|| *aux_p==')'*/){ // is new variable
					aux_p++;

				}else{
					end=true;
				}

			}while(!end); // is new variable

			// after variable declaration is expected to have any keyword but is not valid any operator,
			if((ending_op=is_operator(aux_p))!=Operator::OPERATOR_UNKNOWN){
				if((((properties & EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR)==EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR) && (ending_op == Operator::OPERATOR_IN))==false){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected '%s' within variable initialization",eval_data_operators[ending_op].str)
				}
			}

			return aux_p;
		}

error_eval_keyword_var:
		for(unsigned i=0; i < ei_member_var_init.size(); i++){
			delete (EvalInstruction *)ei_member_var_init[i];
		}

		ei_member_var_init.clear();

		return NULL;
	}



}


