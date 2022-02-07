/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{

	#define EVAL_KEYWORD_VAR_ALLOW_IN 0x1

	static int n_anonymous_function=0;

	zs_string eval_anonymous_function_name(const zs_string &pre_name=""){
		return "_@afun_"+(pre_name==""?"":pre_name+"_")+zs_strutils::zs_int_to_str(n_anonymous_function++);
	}


	void eval_generate_byte_code_field_initializer(EvalData *eval_data, ScriptFunction *sf, zs_vector *eval_instructions, Symbol *symbol_member_var){

		// 1. allocate for  sf->instructions_len + (eval_data->current_function->instructions.size() + 1)
		PtrInstruction new_instructions=NULL;
		unsigned idx_position=0;
		size_t new_instructions_len=0;
		size_t new_instructions_total_bytes=0;
		Instruction * start_ptr=NULL;
		int n_elements_to_add=eval_instructions->count;

		n_elements_to_add=n_elements_to_add+3; // +3 for load/store/reset stack

		if(sf->instructions == NULL){
			n_elements_to_add=n_elements_to_add+1; // +1 for end instruction
		}

		new_instructions_len = sf->instructions_len+(n_elements_to_add);
		new_instructions_total_bytes=new_instructions_len* sizeof(Instruction);
		new_instructions=(Instruction *)ZS_MALLOC(new_instructions_total_bytes);


		start_ptr=new_instructions+sf->instructions_len;

		// 2. copy current block to new
		if(sf->instructions_len>0){
			memcpy(new_instructions,sf->instructions,sf->instructions_len*sizeof(Instruction));
			start_ptr--; // start from end instruction
		}


		// 3. copy eval instructions
		for(unsigned i=0; i < eval_instructions->count; i++){
			EvalInstruction *eval_instruction = (EvalInstruction *)eval_instructions->items[i];
			// save instruction ...
			*start_ptr=eval_instruction->vm_instruction;

			//------------------------------------
			// symbol value to save at runtime ...
			InstructionSourceInfo instruction_info=eval_instruction->instruction_source_info;

			// Save str_symbol that was created on eval process, and is destroyed when eval finish.
			instruction_info.ptr_str_symbol_name=eval_instruction->instruction_source_info.ptr_str_symbol_name;

			// add instruction source information...
			sf->instruction_source_info.push_back((zs_int)(new InstructionSourceInfo(instruction_info)));

			start_ptr++;

			delete eval_instruction; // do not need eval instruction any more

		}

		// 4. add load/store/reset stack
		idx_position=start_ptr-new_instructions;
		*start_ptr++=Instruction(BYTE_CODE_PUSH_STK_MEMBER_VAR,IDX_ZS_UNDEFINED,symbol_member_var->idx_position);
		sf->instruction_source_info.push_back((zs_int)(new InstructionSourceInfo(
			eval_data->current_parsing_file
			,symbol_member_var->line
			,get_mapped_name(eval_data,symbol_member_var->name)
		)));


		*start_ptr++=Instruction(BYTE_CODE_STORE,1);
		sf->instruction_source_info.push_back(0);

		if(sf->instructions != NULL){
			free(sf->instructions); // deallocate last allocated instructions
		}

		sf->instructions=new_instructions;
		sf->instructions_len=new_instructions_len;

		eval_instructions->clear();
	}

	ScriptFunction *eval_new_inline_anonymous_function(EvalData *eval_data,zs_vector *eval_instructions){

		zs_string function_name=eval_anonymous_function_name("defval");
		Instruction *instructions=NULL,*start_ptr=NULL;
		size_t instructions_len=(eval_instructions->count+2); // additional +2 operations byte_code_ret and byte_code_end_function
		size_t instructions_total_bytes=instructions_len*sizeof(Instruction);

		Symbol * symbol_sf=MAIN_FUNCTION(eval_data)->registerLocalFunction(
			 MAIN_SCOPE(eval_data)
			, eval_data->current_parsing_file
			, -1
			, function_name
		);

		ScriptFunction *sf=(ScriptFunction *)symbol_sf->ref_ptr;

		// fill all instructions
		start_ptr=sf->instructions=(Instruction *)ZS_MALLOC(instructions_total_bytes); // +1 is for return

		sf->instructions_len=instructions_len;

		for(unsigned i=0; i < eval_instructions->count; i++){
			EvalInstruction *instruction = (EvalInstruction *)eval_instructions->items[i];
			InstructionSourceInfo instruction_info=instruction->instruction_source_info;

			// save instruction ...
			*start_ptr=instruction->vm_instruction;

			if(instruction->vm_instruction.byte_code == BYTE_CODE_LOAD_STRING){
				start_ptr->byte_code = BYTE_CODE_NEW_STRING;
				start_ptr->properties=INSTRUCTION_PROPERTY_STRING;
			}

			// Save str_symbol that was created on eval process, and is destroyed when eval finish.
			instruction_info.ptr_str_symbol_name=instruction->instruction_source_info.ptr_str_symbol_name;

			sf->instruction_source_info.push_back((zs_int)(
				new InstructionSourceInfo(instruction_info))
			);

			start_ptr++;

			delete instruction;
		}

		// add return in the end...
		start_ptr->byte_code=BYTE_CODE_RET;
		start_ptr->value_op1=IDX_ZS_UNDEFINED;
		start_ptr->value_op2=IDX_ZS_UNDEFINED;
		sf->instruction_source_info.push_back(0);

		eval_instructions->clear();


		return sf;
	}

	//
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	//  VAR/FUNCTION
	//

	char * eval_keyword_var(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, uint16_t properties){
		// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass
		// check for keyword ...
		char *aux_p = (char *)s;
		Keyword key_w = eval_is_keyword(s);
		zs_vector ei_member_var_init;

		if(key_w == Keyword::KEYWORD_VAR || key_w == Keyword::KEYWORD_CONST){ // possible variable...
			bool is_static = false,
				 is_constant = false,
				 is_class_scope=false,
				 end=false;

			Scope *scope_var=scope_info;
			ScriptFunction *sf_field_initializer=NULL;


			char *start_var=NULL,*end_var=NULL;
			int start_line=0;
			ScriptType *sc=NULL;
			zs_string s_aux,variable_name,pre_variable_name="";
			zs_string error="";
			Symbol *symbol_variable=NULL,*symbol_member_variable=NULL;
			is_constant=key_w == Keyword::KEYWORD_CONST;
			Operator ending_op=Operator::OPERATOR_UNKNOWN;

			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

			// check class scope...
			if(scope_var->script_type->idx_type != IDX_TYPE_CLASS_MAIN
				&& scope_var->scope_base == scope_var
				&& scope_var->scope_parent == NULL // is function member
			){ // class members are defined as functions
				sc=scope_var->script_type;
				is_class_scope=true;
			}

			if(is_constant){ // scope_var will be global scope...
				if(!(sc!=NULL || scope_var == MAIN_SCOPE(eval_data))){
					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"'const' is allowed only in class or global");
				}

				// always static or constant are global symbols...
				scope_var = MAIN_SCOPE(eval_data);

				if(sc!=NULL){
					pre_variable_name=sc->type_name+"::";
				}
			}else if(sc != NULL){
				sf_field_initializer=sc->sf_field_initializer;
			}


			do{
				int test_line=0;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				start_var=aux_p;
				start_line=line;
				end_var=NULL;
				ScriptType *sc_var_member_extension=sc;

				if(sc==NULL){
					if((end_var=is_class_member_extension( // is function class extensions (example A::function1(){ return 0;} )
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
							EvalInstruction *eval_instruction;
							eval_data->current_function->eval_instructions.push_back((zs_int)(
									eval_instruction=new EvalInstruction(
											BYTE_CODE_PUSH_STK_GLOBAL
									)
							));

							eval_instruction->vm_instruction.value_op2=symbol_variable->idx_position;
							eval_instruction->instruction_source_info.ptr_str_symbol_name=get_mapped_name(eval_data, pre_variable_name+variable_name);
							eval_instruction->symbol.name=pre_variable_name+variable_name;
							eval_instruction->symbol.scope=MAIN_SCOPE(eval_data);

							eval_data->current_function->eval_instructions.push_back((zs_int)(
									new EvalInstruction(
											BYTE_CODE_STORE_CONST,1
									)
							));
						}

					}

					line = test_line;
				}
				else if(is_constant){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,test_line,"Uninitialized constant symbol %s%s"
							,sc_var_member_extension!=NULL?zs_strutils::format("::%s",sc->type_name.c_str()).c_str():""
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
				if((((properties & EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR)==EVAL_KEYWORD_VAR_PROPERTY_ALLOW_IN_OPERATOR) && (ending_op == Operator::OPERATOR_IN))==false){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Unexpected '%s' within variable initialization",eval_data_operators[ending_op].str)
				}
			}

			return aux_p;
		}

error_eval_keyword_var:
		for(size_t i=0; i < ei_member_var_init.count; i++){
			delete (EvalInstruction *)ei_member_var_init.items[i];
		}

		ei_member_var_init.clear();

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
			, uint16_t properties // allow_anonymous_function attrib /anonymous, etc
			, Symbol ** result_symbol_function
			, const zs_string & custom_symbol_name

		){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		ScriptType *sc=NULL; // if NULL it suposes is the main
		char *aux_p = (char *)s;
		Keyword key_w=eval_is_keyword(aux_p);
		bool is_static = false;

		// check if static...
		if(key_w==Keyword::KEYWORD_STATIC){

			is_static=true;
			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
		}

		// check class particular case

		//Keyword key_w;
		//
		// check for keyword ...
		if(scope_info->script_type->idx_type != IDX_TYPE_CLASS_MAIN
			&& ((  scope_info->scope_base == scope_info
			      && scope_info->scope_parent == NULL
			    )
			   || (properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY)
			  )
			   // is function member
			){ // class members are defined as functions
			key_w=eval_is_keyword(aux_p);
			if(key_w != Keyword::KEYWORD_FUNCTION){ // make it optional
				key_w = Keyword::KEYWORD_FUNCTION;
			}
			else{
				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
			}
			sc=scope_info->script_type;
		}
		else{
			key_w = eval_is_keyword(aux_p);
			if(key_w == Keyword::KEYWORD_FUNCTION){
				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
			}
		}


		if(key_w == Keyword::KEYWORD_FUNCTION || is_static){
			ScriptFunctionParam param_info;
			ScriptFunctionParam *params=NULL;
			size_t params_len=0;

			//bool var_args=false;
			int n_arg=0;
			char *end_var = NULL;
			zs_string param_value;
			zs_string error;
			//size_t advance_chars=0;


			zs_vector script_function_params;
			zs_string conditional_str;
			Symbol *symbol_sf=NULL;

			Symbol * irv=NULL;


			zs_string function_name="";
			ScriptFunction *sf=NULL;
			// advance keyword...
			//aux_p += advance_chars;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			bool named_function = *aux_p!='(';

			if(named_function){ // is named function..

				// function cannot be declared within main scope
				/*if(scope_info != MAIN_SCOPE(eval_data) && sc == NULL){ // function within a function (not function member)
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"named functions are only allowed in main scope. You can only use anonymous functions");
				}
*/
				if(sc==NULL){ // check if function member declaration
				   end_var=is_class_member_extension( // is function class extensions (example A::function1(){ return 0;} )
						eval_data
						,aux_p
						,line
						,&sc
						,function_name
				   );

				   if(sc!=NULL){ // scope is the class
					   scope_info=sc->class_scope;
				   }
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

					if(end_var == NULL){
						return NULL;
					}
				}

				aux_p=end_var;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
			}
			else{ // name anonymous function

				if((properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)==0){
					// it return NULL telling to no eval function here. It will perform in expression instead (it also will create anonymous in there)
					return NULL;
				}

				if(
						scope_info->script_type != SCRIPT_CLASS_MAIN(eval_data)
					 && scope_info->scope_parent != NULL
				){
					sc=scope_info->script_type;
				}
			}

			// eval function args...
			if(*aux_p != '('){ // expected (
				zs_string error;
				if(is_special_char(aux_p)){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error %s: unexpected '%c' "
					,scope_info->script_type != SCRIPT_CLASS_MAIN(eval_data)?zs_strutils::format(
							"declaring function member in class '%s'"
							,scope_info->script_type->type_name.c_str()
							).c_str():"declaring function"
							,*aux_p

					);
				}else{

					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error %s: expected function start argument declaration '(' "
							,scope_info->script_type != SCRIPT_CLASS_MAIN(eval_data)?zs_strutils::format(
									"declaring function member in class '%s'"
									,scope_info->script_type->type_name.c_str()
									).c_str():"declaring function"

					);
				}
			}

			// save scope pointer for function args ...
			IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

			while(*aux_p != 0 && *aux_p != ')'){
				param_info=ScriptFunctionParam();
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				Keyword kw_arg=Keyword::KEYWORD_UNKNOWN;

				if(script_function_params.count>0){
					if(*aux_p != ','){
						EVAL_ERROR_FILE_LINE_AND_GOTOF(
							eval_keyword_function_params
							,eval_data->current_parsing_file
							,line
							,"Syntax error: expected function argument separator ','"
						);
					}
					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				}

				if(*aux_p == ')' || *aux_p == ','){
					EVAL_ERROR_FILE_LINE_AND_GOTOF(
						eval_keyword_function_params
						,eval_data->current_parsing_file
						,line
						,"Syntax error: expected argument name"
					);
				}

				// capture line where argument is...
				param_info.line=line;



				if(*aux_p=='.' && *(aux_p+1)=='.' && *(aux_p+2)=='.'){// eval_is_keyword(aux_p)==KEYWORD_REF){
					IGNORE_BLANKS(aux_p,eval_data,aux_p+3,line);
					param_info.properties|=MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS;
				}else{
					switch(kw_arg=eval_is_keyword(aux_p)){
					case Keyword::KEYWORD_UNKNOWN:
						break;
					case KEYWORD_REF:
						IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[KEYWORD_REF].str),line);
						param_info.properties|=MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF;
						break;
					default:
						EVAL_ERROR_FILE_LINE_AND_GOTO(
							eval_keyword_function_params
							,eval_data->current_parsing_file
							,line
							,"Syntax error: unexpected keyword '%s'"
							,eval_data_keywords[kw_arg].str
						);
						break;
					}
				}

				//int m_start_arg=line;
				end_var=get_name_identifier_token(
					 eval_data
					,aux_p
					,line
					,param_value
				);

				if(end_var == NULL){
					goto eval_keyword_function_params;
				}

				// copy value
				zs_strutils::copy_from_ptr_diff(param_value,aux_p,end_var);
				// ok register symbol into the object function ...
				param_info.name=zs_string(param_value);


				aux_p=end_var;

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if((param_info.properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS) && *aux_p!=')'){
					EVAL_ERROR_FILE_LINE_AND_GOTOF(
						eval_keyword_function_params
						,eval_data->current_parsing_file
						,line
						,"Expected ')' after variable argument declaration"
					);
				}
				else if(*aux_p=='='){ // default argument...

					if(param_info.properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF ){
						EVAL_ERROR_FILE_LINE_AND_GOTOF(
							eval_keyword_function_params
							,eval_data->current_parsing_file
							,line
							,"Arguments by reference cannot set a default argument"
						);
					}

					zs_vector ei_instructions_default;
					bool create_anonymous_function_return_expression=false;

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
					// create inline expression
					aux_p=eval_expression(
						eval_data
						,aux_p
						,line
						,MAIN_SCOPE(eval_data)
						,&ei_instructions_default
					);

					if(aux_p==NULL){
						goto eval_keyword_function_params;
					}

					if(ei_instructions_default.count == 0){ // expected expression
						EVAL_ERROR_FILE_LINE_AND_GOTOF(
							eval_keyword_function_params
							,eval_data->current_parsing_file
							,line
							,"Syntax error:  expected expression after '='"
						);
					}

					// copy evaluated instruction
					// convert instruction to stk_element
					if(ei_instructions_default.count == 1){
						Instruction *instruction=&((EvalInstruction *)ei_instructions_default.items[0])->vm_instruction;
						// trivial default values that can be accomplished by single stack element.
						switch(instruction->byte_code){
						case BYTE_CODE_LOAD_NULL:
							param_info.default_param_value=k_stk_null;
							break;
						case BYTE_CODE_LOAD_ZS_INT:
							param_info.default_param_value={instruction->value_op2,STK_PROPERTY_ZS_INT};
							break;
						case BYTE_CODE_LOAD_ZS_FLOAT:
							param_info.default_param_value={instruction->value_op2,STK_PROPERTY_ZS_FLOAT};
							break;
						case BYTE_CODE_LOAD_BOOL:
							param_info.default_param_value={instruction->value_op2,STK_PROPERTY_BOOL};
							break;
						default: // else is an object so we'll create a function in order to return object or complex expression
							create_anonymous_function_return_expression=true;
							break;
						}
					}else{ //non trivial expression so we will create an anonymous function
						create_anonymous_function_return_expression=true;
					}

					if(create_anonymous_function_return_expression==true){
						ScriptFunction *sf=eval_new_inline_anonymous_function(eval_data,&ei_instructions_default);
						param_info.default_param_value={(zs_int)sf,STK_PROPERTY_FUNCTION};
					}

					// finally delete all evaluated code
					for(unsigned i=0; i < ei_instructions_default.count; i++){
						delete (EvalInstruction *)ei_instructions_default.items[i];
					}

				}

				script_function_params.push_back((zs_int)(
						new ScriptFunctionParam(param_info)
				));
			}

			aux_p++;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p != '{'){
				EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error:  expected '{' as function block");
			}

			// register function ...
			if(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS){ // register named function...
				if(custom_symbol_name != ""){
					function_name=custom_symbol_name;
				}else{
					function_name=eval_anonymous_function_name(sc!=NULL?sc->type_name:"");
				}
			}

			params=ScriptFunctionParam::createArrayFromVector(&script_function_params);
			params_len=script_function_params.count;

			// remove collected script function params
			for(unsigned i=0; i < script_function_params.count; i++){
				delete (ScriptFunctionParam *)script_function_params.items[i];
			}

			//--- OP
			if(sc!=NULL){ // register as variable member...
				try{
					symbol_sf=sc->registerMemberFunction(
							function_name
							,&params
							,params_len
							,is_static?FUNCTION_PROPERTY_STATIC:FUNCTION_PROPERTY_MEMBER_FUNCTION
							,0
							,0
							,eval_data->current_parsing_file
							,line
					);
				}catch(std::exception & ex){
					if(params != NULL){
						delete [] params;
					}


					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,ex.what());
				}


			}
			else{ // register as local variable in the function...
				try{
					symbol_sf=eval_data->current_function->script_function->registerLocalFunction(
						 scope_info
						, eval_data->current_parsing_file
						, line
						, function_name
						, &params
						, params_len
					);
				}catch(std::exception & ex){
					if(params != NULL){
						delete [] params;
					}

					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,ex.what());
				}

				if(scope_info->script_type != SCRIPT_CLASS_MAIN(eval_data)){ // is a function that was created within a member function...
					((ScriptFunction *)(symbol_sf->ref_ptr))->properties|=FUNCTION_PROPERTY_MEMBER_FUNCTION;
				}
			}

			//-------------------------
			// IMPORTANT NOTE:
			// Due params var is NULL here because it was marked as assigned at ScriptFunction::updateParams. Reassign params variable again ...
			params=((ScriptFunction *)symbol_sf->ref_ptr)->params;
			//
			//-------------------------

			if(result_symbol_function != NULL){
				*result_symbol_function=symbol_sf;
			}

			sf=(ScriptFunction *)symbol_sf->ref_ptr;

			eval_push_function(eval_data,sf);

			// ok let's go to body..
			if((aux_p = eval_block(
					eval_data
					,aux_p
					,line
					,scope_info
					,sf
					,params
					,params_len
				)
			)==NULL){
				// deallocate current function
				eval_pop_current_function(eval_data);
				return NULL;
			}

			eval_pop_and_compile_function(eval_data);

			return aux_p;
	//---------------------------------------------
	// CONTROL ERROR
	eval_keyword_function_params:
			// unallocate script function params
			for(unsigned h=0; h < script_function_params.count; h++){
				delete (ScriptFunctionParam *)script_function_params.items[h];
			}
			script_function_params.clear();
			return NULL;
	//---------------------------------------------

		}

		return NULL;



	}

	char *  eval_keyword_return(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		Keyword key_w;
		zs_string s_aux;

		key_w = eval_is_keyword(aux_p);

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
					,&eval_data->current_function->eval_instructions
					,NULL
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION|EVAL_EXPRESSION_DO_NOT_RESET_STACK_LAST_CALL
			))!= NULL){

				eval_data->current_function->eval_instructions.push_back((zs_int)(
					new EvalInstruction(BYTE_CODE_RET)
				));

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				return aux_p;
			}
		}
		return NULL;
	}
}


