/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"



namespace zetscript{

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	//  VAR/FUNCTION
	//

	char * compiler_keyword_var(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info, uint16_t properties){
		// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass
		// check for keyword ...
		char *aux_p = (char *)s;
		Keyword key_w = compiler_is_keyword(s);
		Vector<CompilerInstruction *> ei_member_var_init;

		if(key_w == Keyword::KEYWORD_VAR || key_w == Keyword::KEYWORD_CONST){ // possible variable...
			bool  is_constant = false,
				 is_class_scope=false,
				 end=false;

			ScriptScope *scope_var=scope_info;

			char *start_var=NULL,*end_var=NULL;
			int start_line=0;
			bool is_static=scope_info==ZS_MAIN_SCOPE(compiler_data);
			ScriptType *script_type=NULL;
			String s_aux="",variable_name="";
			String error="";
			Symbol *symbol_variable=NULL,*symbol_member_variable=NULL;
			is_constant=key_w == Keyword::KEYWORD_CONST;
			Operator ending_op=Operator::ZS_OPERATOR_UNKNOWN;

			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

			// check type scope...
			if(scope_var->owner_type->id != SCRIPT_TYPE_ID_CLASS_MAIN
				&& scope_var->scope_base == scope_var
				&& scope_var->scope_parent == NULL // is function member
			){ // type members are defined as functions
				script_type=scope_var->owner_type;
				is_class_scope=true;
				is_static=true;
			}

			do{
				int test_line=0;
				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
				start_var=aux_p;
				start_line=line;
				end_var=NULL;
				String pre_variable_name="";
				ScriptFunction *sf_field_initializer=NULL;
				ScriptType *sc_var_member_extension=script_type;

				if(sc_var_member_extension==NULL){
					if((end_var=is_class_member_extension( // is function type extensions (example A::function1(){ return 0;} )
						compiler_data
						,start_var
						,start_line
						,&sc_var_member_extension
						,variable_name
				   ))!=NULL){
						if(is_class_scope==true){
							EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Unexpected ::");
						}
						line=start_line;
						sf_field_initializer=sc_var_member_extension->sf_field_initializer;
					}else{ // NULL check if error
						if(compiler_data->error){
							return NULL;
						}
					}
				}else{
					sf_field_initializer=sc_var_member_extension->sf_field_initializer;
				}

				if((sc_var_member_extension!=NULL) && (is_constant==true)){
					scope_var = ZS_MAIN_SCOPE(compiler_data);
					pre_variable_name=String(sc_var_member_extension->name)+"::";
				}

				if(end_var==NULL){
					// check whwther the function is anonymous with a previous arithmetic operation ....
					if((end_var=get_name_identifier_token(
							compiler_data,
							aux_p,
							line,
							variable_name
					))==NULL){
						return NULL;
					}
				}

				ZS_LOG_DEBUG("registered symbol '%s' line %i ",variable_name.toConstChar(), line);

				Keyword keyw = compiler_is_keyword(variable_name.toConstChar());

				if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Unexpected keyword '%s' after var",compiler_data_keywords[keyw].str);
				}

				// register symbol...
				try{

					bool is_local_or_const=is_constant || sc_var_member_extension==NULL;
					//bool is_const_member_or_var_member=sc_var_member_extension != NULL;

					if(is_local_or_const){ // is constant or is local (sc_var_member_extension==NULL)
						symbol_variable=compiler_data->current_function->script_function->registerLocalVariable(
							scope_var
							, compiler_data->current_parsing_file
							, line
							, pre_variable_name+variable_name
						);

						// static constant
						if(sc_var_member_extension!=NULL){
							try{
								symbol_member_variable=sc_var_member_extension->registerMemberVariable(
									variable_name
									,SYMBOL_PROPERTY_CONST | SYMBOL_PROPERTY_STATIC
									,compiler_data->current_parsing_file
									,line
								);

							}catch(std::exception & ex){
								EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"%s",ex.what());
							}

							if(is_constant == true){
								symbol_member_variable->ref_ptr=symbol_variable->idx_position;
							}
						}
					}

				}catch(std::exception & ex){
					EVAL_ERROR("%s",ex.what());
				}


				// advance identifier length chars
				aux_p=end_var;
				test_line=line;
				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

				if(*aux_p == '='){

					test_line=line;
					bool is_var_member_or_const=sc_var_member_extension!=NULL || is_constant == true;
					bool is_var_member=sf_field_initializer!=NULL && is_constant==false;

					if((aux_p = compiler_expression(
						compiler_data
						,is_var_member_or_const?aux_p+1:start_var
						,test_line
						,scope_var
						,is_var_member?&ei_member_var_init:&compiler_data->current_function->compiler_instructions
						,NULL
						,properties
					))==NULL){
						goto error_compiler_keyword_var;
					}

					if(is_var_member){ // check load and set find

						compiler_generate_byte_code_field_initializer(
							compiler_data
							,sf_field_initializer
							,line
							,variable_name
							,&ei_member_var_init
						);

						ei_member_var_init.clear();
					}
					else{
						if(is_constant){ // make ptr as constant after variable is saved

							// unset last reset stack
							CompilerInstruction *compiler_instruction=(CompilerInstruction *)compiler_data->current_function->compiler_instructions.get(compiler_data->current_function->compiler_instructions.length()-1);
							compiler_instruction->vm_instruction.properties&=~INSTRUCTION_PROPERTY_RESET_STACK;

							// add instruction push
							compiler_data->current_function->compiler_instructions.push(
									compiler_instruction=new CompilerInstruction(
											is_static?BYTE_CODE_PUSH_STK_GLOBAL:BYTE_CODE_PUSH_STK_LOCAL
									)
							);

							compiler_instruction->vm_instruction.value_op2=symbol_variable->idx_position;
							compiler_instruction->instruction_source_info.ptr_str_symbol_name=get_mapped_name(compiler_data, pre_variable_name+variable_name);
							compiler_instruction->symbol_name=pre_variable_name+variable_name;
							compiler_instruction->symbol_scope=scope_var;

							compiler_data->current_function->compiler_instructions.push(
									new CompilerInstruction(
										BYTE_CODE_STORE_CONST
										,1
										,ZS_UNDEFINED_IDX
										,INSTRUCTION_PROPERTY_RESET_STACK
									)
							);
						}

					}

					line = test_line;
				}
				else if(is_constant){
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,test_line,"Uninitialized constant symbol %s%s"
							,sc_var_member_extension!=NULL?String::format("::%s",script_type->name.toConstChar()).toConstChar():""
							,variable_name.toConstChar());
				}

				if(*aux_p == ',' /*|| *aux_p==')'*/){ // is new variable
					aux_p++;

				}else{
					end=true;
				}

			}while(!end); // is new variable

			// after variable declaration is expected to have any keyword but is not valid any operator,
			if((ending_op=is_operator(aux_p))!=Operator::ZS_OPERATOR_UNKNOWN){
				if((((properties & EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR)==EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR) && (ending_op == Operator::ZS_OPERATOR_IN))==false){
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Unexpected '%s' within variable initialization",compiler_data_operators[ending_op].str)
				}
			}

			return aux_p;
		}

error_compiler_keyword_var:
		for(int i=0; i < ei_member_var_init.length(); i++){
			delete ei_member_var_init.get(i);
		}

		ei_member_var_init.clear();

		return NULL;
	}



}


