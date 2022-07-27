/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{
	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// FUNCTION
	//

	static int k_anonymous_function=0;

	zs_string eval_anonymous_function_name(const zs_string &pre_name="",const zs_string &post_name=""){
		return "_"+(pre_name==""?"":pre_name)+"@afun_"+(post_name==""?"":post_name+"_")+zs_strutils::zs_int_to_str(k_anonymous_function++);
	}


	void eval_generate_byte_code_field_initializer(EvalData *eval_data, ScriptFunction *sf, zs_vector<EvalInstruction *> *eval_instructions, Symbol *symbol_member_var){

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
			sf->instruction_source_infos.push_back(new InstructionSourceInfo(instruction_info));

			start_ptr++;

			delete eval_instruction; // do not need eval instruction any more

		}

		// 4. add load/store/reset stack
		idx_position=start_ptr-new_instructions;
		*start_ptr++=Instruction(
			BYTE_CODE_PUSH_STK_MEMBER_VAR
			, ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED
			,symbol_member_var->idx_position
		);
		sf->instruction_source_infos.push_back(new InstructionSourceInfo(
			eval_data->current_parsing_file
			,symbol_member_var->line
			,get_mapped_name(eval_data,symbol_member_var->name)
		));


		*start_ptr++=Instruction(BYTE_CODE_STORE,1);
		sf->instruction_source_infos.push_back(NULL);

		if(sf->instructions != NULL){
			free(sf->instructions); // deallocate last allocated instructions
		}

		sf->instructions=new_instructions;
		sf->instructions_len=new_instructions_len;

		eval_instructions->clear();
	}

	Symbol *eval_new_inline_anonymous_function(EvalData *eval_data,zs_vector<EvalInstruction *> *eval_instructions){

		zs_string name_script_function=eval_anonymous_function_name("","defval");
		Instruction *start_ptr=NULL;
		size_t instructions_len=(eval_instructions->count+2); // additional +2 operations byte_code_ret and byte_code_end_function
		size_t instructions_total_bytes=instructions_len*sizeof(Instruction);

		Symbol * symbol_sf=MAIN_FUNCTION(eval_data)->registerLocalFunction(
			 MAIN_SCOPE(eval_data)
			, eval_data->current_parsing_file
			, -1
			, name_script_function
		);

		ScriptFunction *sf=(ScriptFunction *)symbol_sf->ref_ptr;

		Scope *new_scope_info = eval_new_scope(eval_data,MAIN_SCOPE(eval_data),true);
		sf->scope_script_function=new_scope_info;

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

			sf->instruction_source_infos.push_back(
				new InstructionSourceInfo(instruction_info)
			);

			start_ptr++;

			delete instruction;
		}

		// add return in the end...
		start_ptr->byte_code=BYTE_CODE_RET;
		start_ptr->value_op1= ZS_IDX_INSTRUCTION_OP1_NOT_DEFINED;
		start_ptr->value_op2=ZS_IDX_UNDEFINED;
		sf->instruction_source_infos.push_back(NULL);

		eval_instructions->clear();


		return symbol_sf;
	}

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

		// check type particular case

		//Keyword key_w;
		//
		// check for keyword ...
		if(scope_info->script_type_owner->idx_script_type != IDX_TYPE_CLASS_MAIN
			&& ((  scope_info->scope_base == scope_info
			      && scope_info->scope_parent == NULL
			    )
			   || (properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY)
			  )
			   // is function member
			){ // type members are defined as functions
			key_w=eval_is_keyword(aux_p);
			if(key_w != Keyword::KEYWORD_FUNCTION){ // make it optional
				key_w = Keyword::KEYWORD_FUNCTION;
			}
			else{
				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);
			}
			sc=scope_info->script_type_owner;
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
			int params_len=0;

			//bool var_args=false;
			char *end_var = NULL;
			zs_string param_value;
			zs_vector<ScriptFunctionParam *> script_function_params;
			zs_string conditional_str;
			Symbol *symbol_sf=NULL;
			zs_string name_script_function="";
			ScriptFunction *sf = NULL;

			// advance keyword...
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			bool named_function = *aux_p!='(';

			if(named_function){ // is named function..

				if(sc==NULL){ // check if function member declaration
				   end_var=is_class_member_extension( // is function type extensions (example A::function1(){ return 0;} )
						eval_data
						,aux_p
						,line
						,&sc
						,name_script_function
				   );

				   if(sc!=NULL){ // scope is the type
					   scope_info=sc->scope_script_type;
				   }
				}

				// not member function, so is normal function ...
				if(end_var == NULL){ // global function
					// check whwther the function is anonymous with a previous arithmetic operation ....
					end_var=get_name_identifier_token(
							eval_data
							,aux_p
							,line
							,name_script_function
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



				// anonymous functions are always in main scope
				/*if(
						scope_info->script_type != SCRIPT_TYPE_MAIN(eval_data)
					 && scope_info->scope_parent != NULL
				){
					sc=scope_info->script_type;
				}*/
			}

			// eval function args...
			if(*aux_p != '('){ // expected (
				zs_string error;
				if(is_special_char(aux_p)){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error %s: unexpected '%c' "
					,scope_info->script_type_owner != SCRIPT_TYPE_MAIN(eval_data->script_type_factory)?zs_strutils::format(
							"declaring function member '%s::%s'"
							,scope_info->script_type_owner->str_script_type.c_str()
							,(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)?"anonymous_function":name_script_function.c_str()
							).c_str():"declaring function"
							,*aux_p

					);
				}else{

					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Syntax error %s: expected function start argument declaration '(' "
							,scope_info->script_type_owner != SCRIPT_TYPE_MAIN(eval_data->script_type_factory)?zs_strutils::format(
									"declaring function member '%s::%s'"
									,scope_info->script_type_owner->str_script_type.c_str()
									,(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)?"anonymous_function":name_script_function.c_str()
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
						EVAL_ERROR_FILE_LINE_GOTOF(
							eval_data->current_parsing_file
							,line
							,eval_keyword_function_params
							,"Syntax error: expected function argument separator ','"
						);
					}
					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
				}

				if(*aux_p == ')' || *aux_p == ','){
					EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,line
						,eval_keyword_function_params
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
						EVAL_ERROR_FILE_LINE_GOTO(
							eval_data->current_parsing_file
							,line
							,eval_keyword_function_params
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
					EVAL_ERROR_FILE_LINE_GOTOF(
						eval_data->current_parsing_file
						,line
						,eval_keyword_function_params
						,"Expected ')' after variable argument declaration"
					);
				}
				else if(*aux_p=='='){ // default argument...

					if(param_info.properties & MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF ){
						EVAL_ERROR_FILE_LINE_GOTOF(
							eval_data->current_parsing_file
							,line
							,eval_keyword_function_params
							,"Arguments by reference cannot set a default argument"
						);
					}

					zs_vector<EvalInstruction *> ei_instructions_default;
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
						EVAL_ERROR_FILE_LINE_GOTOF(
							eval_data->current_parsing_file
							,line
							,eval_keyword_function_params
							,"Syntax error:  expected expression after '='"
						);
					}

					// copy evaluated instruction
					// convert instruction to stk_element
					if(ei_instructions_default.count == 1){
						Instruction *instruction=&((EvalInstruction *)ei_instructions_default.items[0])->vm_instruction;
						// trivial default values that can be accomplished by single stack element.
						switch(instruction->byte_code){
						case BYTE_CODE_LOAD_UNDEFINED:
							param_info.default_param_value=k_stk_undefined;
							break;
						case BYTE_CODE_LOAD_NULL:
							param_info.default_param_value={0,STK_PROPERTY_NULL};
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
						Symbol *sf_aux=eval_new_inline_anonymous_function(eval_data,&ei_instructions_default);
						param_info.default_param_value={(zs_int)sf_aux,STK_PROPERTY_FUNCTION};
					}

					// finally delete all evaluated code
					for(unsigned i=0; i < ei_instructions_default.count; i++){
						delete (EvalInstruction *)ei_instructions_default.items[i];
					}

				}

				script_function_params.push_back(
						new ScriptFunctionParam(param_info)
				);
			}

			// register function ...
			if(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS){ // register named function...
				if(custom_symbol_name != ""){
					name_script_function=custom_symbol_name;
				}else{
					name_script_function=eval_anonymous_function_name();//sc!=NULL?sc->str_script_type:"");
				}
			}

			params=ScriptFunctionParam::createArrayFromVector(&script_function_params);
			params_len=script_function_params.count;

			// remove collected script function params
			for(unsigned i=0; i < script_function_params.count; i++){
				delete (ScriptFunctionParam *)script_function_params.items[i];
			}

			aux_p++;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p != '{'){
				if(params != NULL){
					delete [] params;
				}
				EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,"Syntax error:  expected '{' as function block");
			}

			//--- OP
			if(sc!=NULL){ // register as variable member...
				try{
					symbol_sf=sc->registerMemberFunction(
							name_script_function
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
						, name_script_function
						, &params
						, params_len
					);
				}catch(std::exception & ex){
					if(params != NULL){
						delete [] params;
					}

					EVAL_ERROR_FILE_LINEF(eval_data->current_parsing_file,line,ex.what());
				}

				if((properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)==0){
					if(scope_info->script_type_owner != SCRIPT_TYPE_MAIN(eval_data->script_type_factory)){ // is a function that was created within a member function...
						((ScriptFunction *)(symbol_sf->ref_ptr))->properties|=FUNCTION_PROPERTY_MEMBER_FUNCTION;
					}
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

		if(key_w != Keyword::KEYWORD_RETURN){ // possible variable...
			return NULL;
		}

		//PASTNode child_node=NULL;
		aux_p += strlen(eval_data_keywords[key_w].str);
		bool end=false;

		// save starting point before process the expression...
		do{

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if((aux_p = eval_sub_expression(
					eval_data
					,aux_p
					, line
					, scope_info
					,&eval_data->current_function->eval_instructions
					,NULL
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION
			))== NULL){
				return NULL;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				//return aux_p;


			if(*aux_p==','){
				aux_p++;
			}else{
				end=true;
			}

		}while(!end);

		eval_data->current_function->eval_instructions.push_back(
			new EvalInstruction(BYTE_CODE_RET)
		);

		return aux_p;
	}
}
