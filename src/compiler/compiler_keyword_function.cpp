/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "compiler.h"

namespace zetscript{
	//------------------------------------------------------------------------------------------------------------------------------------------
	//
	// FUNCTION
	//

	static int k_anonymous_function=0;

	String compiler_anonymous_function_name(const String &pre_name="",const String &post_name=""){
		return "@"+(pre_name==""?"":pre_name)+"_afun_"+(post_name==""?"":post_name+"_")+Integer::toString(k_anonymous_function++);
	}


	void compiler_generate_byte_code_field_initializer(
			CompilerData *_compiler_data
			, ScriptFunction *_sf
			, int _line
			,const String & _symbol_name
			, Vector<CompilerInstruction *> *_compiler_instructions
	){

		// 1. allocate for  sf->instructions_len + (compiler_data->current_function->instructions.length() + 1)
		PtrInstruction new_instructions=NULL;
		size_t new_instructions_len=0;
		size_t new_instructions_total_bytes=0;
		Instruction * start_ptr=NULL;
		int n_elements_to_add=_compiler_instructions->length();

		n_elements_to_add=n_elements_to_add+2; // +2 for push_stk + store at the end

		if(_sf->instructions == NULL){
			n_elements_to_add=n_elements_to_add+1; // +1 for end instruction
		}

		new_instructions_len = _sf->instructions_len+(n_elements_to_add);
		new_instructions_total_bytes=new_instructions_len* sizeof(Instruction);
		new_instructions=(Instruction *)ZS_MALLOC(new_instructions_total_bytes);


		start_ptr=new_instructions+_sf->instructions_len;

		// 2. copy current block to new
		if(_sf->instructions_len>0){
			memcpy(new_instructions,_sf->instructions,_sf->instructions_len*sizeof(Instruction));
			start_ptr--; // start from end instruction
		}


		// 3. copy compiler instructions
		for(int i=0; i < _compiler_instructions->length(); i++){
			CompilerInstruction *compiler_instruction = _compiler_instructions->get(i);
			// save instruction ...
			*start_ptr=compiler_instruction->vm_instruction;

			//------------------------------------
			// symbol value to save at runtime ...
			InstructionSourceInfo instruction_info=compiler_instruction->instruction_source_info;

			// Save str_symbol that was created on compiler process, and is destroyed when compiler finish.
			instruction_info.ptr_str_symbol_name=compiler_instruction->instruction_source_info.ptr_str_symbol_name;

			// add instruction source information...
			_sf->instruction_source_infos.push(new InstructionSourceInfo(instruction_info));

			start_ptr++;

			delete compiler_instruction; // do not need compiler instruction any more

		}

		// 4. add load/store/reset stack
		*start_ptr++=Instruction(
				BYTE_CODE_PUSH_STK_THIS_VARIABLE
			,(uint8_t)ZS_UNDEFINED_IDX
			,ZS_UNDEFINED_IDX
			,INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT
		);
		_sf->instruction_source_infos.push(new InstructionSourceInfo(
			compiler_instruction_source_info(
				_compiler_data
				,_compiler_data->current_parsing_file
				,_line
				,_symbol_name
		)));


		*start_ptr++=Instruction(BYTE_CODE_STORE,1);
		_sf->instruction_source_infos.push(NULL);

		if(_sf->instructions != NULL){
			free(_sf->instructions); // deallocate last allocated instructions
		}

		_sf->instructions=new_instructions;
		_sf->instructions_len=new_instructions_len;

		_compiler_instructions->clear();
	}

	Symbol *compiler_new_inline_anonymous_function(CompilerData *compiler_data,Vector<CompilerInstruction *> *compiler_instructions){

		String name=compiler_anonymous_function_name("","defval");
		Instruction *start_ptr=NULL;
		size_t instructions_len=(compiler_instructions->length()+2); // additional +2 operations byte_code_ret and byte_code_end_function
		size_t instructions_total_bytes=instructions_len*sizeof(Instruction);

		Symbol * symbol_sf=ZS_MAIN_FUNCTION(compiler_data)->registerLocalFunction(
			 ZS_MAIN_SCOPE(compiler_data)
			, compiler_data->current_parsing_file
			, -1
			, name
		);

		ScriptFunction *sf=(ScriptFunction *)symbol_sf->ref_ptr;

		ScriptScope *new_scope_info = compiler_new_scope_function(compiler_data,ZS_MAIN_SCOPE(compiler_data));
		sf->scope=new_scope_info;

		// fill all instructions
		start_ptr=sf->instructions=(Instruction *)ZS_MALLOC(instructions_total_bytes); // +1 is for return

		sf->instructions_len=instructions_len;

		for(int i=0; i < compiler_instructions->length(); i++){
			CompilerInstruction *instruction = compiler_instructions->get(i);
			InstructionSourceInfo instruction_info=instruction->instruction_source_info;

			// save instruction ...
			*start_ptr=instruction->vm_instruction;

			if(instruction->vm_instruction.byte_code == BYTE_CODE_LOAD_STRING){
				start_ptr->byte_code = BYTE_CODE_NEW_STRING;
				start_ptr->properties=INSTRUCTION_PROPERTY_STRING;
			}

			// Save str_symbol that was created on compiler process, and is destroyed when compiler finish.
			instruction_info.ptr_str_symbol_name=instruction->instruction_source_info.ptr_str_symbol_name;

			sf->instruction_source_infos.push(
				new InstructionSourceInfo(instruction_info)
			);

			start_ptr++;

			delete instruction;
		}

		// add return in the end...
		start_ptr->byte_code=BYTE_CODE_RET;
		start_ptr->value_op1= INSTRUCTION_VALUE_OP1_NOT_DEFINED;
		start_ptr->value_op2=ZS_UNDEFINED_IDX;
		sf->instruction_source_infos.push(NULL);

		compiler_instructions->clear();


		return symbol_sf;
	}

	char * compiler_keyword_function(
			CompilerData *compiler_data
			, const char *s
			, int & line
			, ScriptScope *scope_info
			, uint16_t properties // allow_anonymous_function attrib /anonymous, etc
			, Symbol ** result_symbol_function
			, const String & custom_symbol_name

		){

		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		ScriptType *sc=NULL; // if NULL it suposes is the main
		char *aux_p = (char *)s;
		Keyword key_w=compiler_is_keyword(aux_p);
		bool is_static = false;

		// check if static...
		if(key_w==Keyword::KEYWORD_STATIC){

			is_static=true;
			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);
		}

		// check type particular case

		//Keyword key_w;
		//
		// check for keyword ...
		if(scope_info->owner_type->id != SCRIPT_TYPE_ID_CLASS_MAIN
			&& ((  scope_info->scope_base == scope_info
			      && scope_info->scope_parent == NULL
			    )
			   || (properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_MEMBER_PROPERTY)
			  )
			   // is function member
			){ // type members are defined as functions
			key_w=compiler_is_keyword(aux_p);
			if(key_w != Keyword::KEYWORD_FUNCTION){ // make it optional
				key_w = Keyword::KEYWORD_FUNCTION;
			}
			else{
				IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);
			}
			sc=scope_info->owner_type;
		}
		else{
			key_w = compiler_is_keyword(aux_p);
			if(key_w == Keyword::KEYWORD_FUNCTION){
				IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);
			}
		}


		if(key_w == Keyword::KEYWORD_FUNCTION || is_static){
			FunctionParam *params=NULL;
			int params_len=0;

			//bool var_args=false;
			char *end_var = NULL;
			String param_value;
			Vector<FunctionParam *> script_function_params;
			String conditional_str;
			Symbol *symbol_sf=NULL;
			String name="";
			ScriptFunction *sf = NULL;

			// advance keyword...
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			bool named_function = *aux_p!='(';

			if(named_function){ // is named function..

				if(sc==NULL){ // check if function member declaration

				   end_var=is_class_member_extension( // is function type extensions (example A::function1(){ return 0;} )
						compiler_data
						,aux_p
						,line
						,&sc
						,name
				   );

				   if(compiler_data->error){
					   return NULL;
				   }


				   if(sc!=NULL){ // scope is the type
					   scope_info=sc->scope;
				   }
				}

				// not member function, so is normal function ...
				if(end_var == NULL){ // global function
					// check whwther the function is anonymous with a previous arithmetic operation ....
					end_var=get_name_identifier_token(
							compiler_data
							,aux_p
							,line
							,name
					);

					if(end_var == NULL){
						return NULL;
					}
				}

				aux_p=end_var;
				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
			}
			else{ // name anonymous function

				if((properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)==0){
					// it return NULL telling to no compiler function here. It will perform in expression instead (it also will create anonymous in there)
					return NULL;
				}
			}

			// compiler function args...
			if(*aux_p != '('){ // expected (
				String error;
				if(is_special_char(aux_p)){
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Syntax error %s: unexpected '%c' "
					,scope_info->owner_type != ZS_TYPE_MAIN(compiler_data->script_types_factory)?String::format(
							"declaring function member '%s::%s'"
							,scope_info->owner_type->name.toConstChar()
							,(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)?"anonymous_function":name.toConstChar()
							).toConstChar():"declaring function"
							,*aux_p

					);
				}else{

					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Syntax error %s: expected function start argument declaration '(' "
							,scope_info->owner_type != ZS_TYPE_MAIN(compiler_data->script_types_factory)?String::format(
									"declaring function member '%s::%s'"
									,scope_info->owner_type->name.toConstChar()
									,(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)?"anonymous_function":name.toConstChar()
									).toConstChar():"declaring function"

					);
				}
			}

			// save scope pointer for function args ...
			IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);

			while(*aux_p != 0 && *aux_p != ')'){
				FunctionParam param_info;

				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
				Keyword kw_arg=Keyword::KEYWORD_UNKNOWN;

				if(script_function_params.length()>0){
					if(*aux_p != ','){
						EVAL_ERROR_FILE_LINE_GOTOF(
							compiler_data->current_parsing_file
							,line
							,compiler_keyword_function_params
							,"Syntax error: expected function argument separator ','"
						);
					}
					IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
				}

				if(*aux_p == ')' || *aux_p == ','){
					EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,line
						,compiler_keyword_function_params
						,"Syntax error: expected argument name"
					);
				}

				// capture line where argument is...
				param_info.line=line;



				if(*aux_p=='.' && *(aux_p+1)=='.' && *(aux_p+2)=='.'){// compiler_is_keyword(aux_p)==KEYWORD_REF){
					IGNORE_BLANKS(aux_p,compiler_data,aux_p+3,line);
					param_info.properties|=MSK_FUNCTION_ARG_PROPERTY_VAR_ARGS;
				}else{
					switch(kw_arg=compiler_is_keyword(aux_p)){
					case Keyword::KEYWORD_UNKNOWN:
						break;
					case KEYWORD_REF:
						IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[KEYWORD_REF].str),line);
						param_info.properties|=MSK_FUNCTION_ARG_PROPERTY_BY_REF;
						break;
					default:
						EVAL_ERROR_FILE_LINE_GOTO(
							compiler_data->current_parsing_file
							,line
							,compiler_keyword_function_params
							,"Syntax error: unexpected keyword '%s'"
							,compiler_data_keywords[kw_arg].str
						);
						break;
					}
				}

				//int m_start_arg=line;
				end_var=get_name_identifier_token(
					 compiler_data
					,aux_p
					,line
					,param_value
				);

				if(end_var == NULL){
					goto compiler_keyword_function_params;
				}

				// copy value
				param_value=compiler_copy_const_char_diff(aux_p,end_var);
				// ok register symbol into the object function ...
				param_info.name=String(param_value);


				aux_p=end_var;

				IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

				if((param_info.properties & MSK_FUNCTION_ARG_PROPERTY_VAR_ARGS) && *aux_p!=')'){
					EVAL_ERROR_FILE_LINE_GOTOF(
						compiler_data->current_parsing_file
						,line
						,compiler_keyword_function_params
						,"Expected ')' after variable argument declaration"
					);
				}
				else if(*aux_p=='='){ // default argument...

					if(param_info.properties & MSK_FUNCTION_ARG_PROPERTY_BY_REF ){
						EVAL_ERROR_FILE_LINE_GOTOF(
							compiler_data->current_parsing_file
							,line
							,compiler_keyword_function_params
							,"Arguments by reference cannot set a default argument"
						);
					}

					Vector<CompilerInstruction *> ei_instructions_default;
					bool create_anonymous_function_return_expression=false;

					IGNORE_BLANKS(aux_p,compiler_data,aux_p+1,line);
					// create inline expression
					aux_p=compiler_expression(
						compiler_data
						,aux_p
						,line
						,ZS_MAIN_SCOPE(compiler_data)
						,&ei_instructions_default
					);

					if(aux_p==NULL){
						goto compiler_keyword_function_params;
					}

					if(ei_instructions_default.length() == 0){ // expected expression
						EVAL_ERROR_FILE_LINE_GOTOF(
							compiler_data->current_parsing_file
							,line
							,compiler_keyword_function_params
							,"Syntax error:  expected expression after '='"
						);
					}

					// copy compileruated instruction
					// convert instruction to stk_element
					if(ei_instructions_default.length() == 1){
						Instruction *instruction=&((CompilerInstruction *)ei_instructions_default.get(0))->vm_instruction;
						// trivial default values that can be accomplished by single stack element.
						switch(instruction->byte_code){
						case BYTE_CODE_LOAD_UNDEFINED:
							param_info.default_param_value=k_stk_undefined;
							break;
						case BYTE_CODE_LOAD_NULL:
							param_info.default_param_value={0,STACK_ELEMENT_PROPERTY_NULL};
							break;
						case BYTE_CODE_LOAD_INT:
							param_info.default_param_value={instruction->value_op2,STACK_ELEMENT_PROPERTY_INT};
							break;
						case BYTE_CODE_LOAD_FLOAT:
							param_info.default_param_value={instruction->value_op2,STACK_ELEMENT_PROPERTY_FLOAT};
							break;
						case BYTE_CODE_LOAD_BOOL:
							param_info.default_param_value={instruction->value_op2,STACK_ELEMENT_PROPERTY_BOOL};
							break;
						default: // else is an object so we'll create a function in order to return object or complex expression
							create_anonymous_function_return_expression=true;
							break;
						}
					}else{ //non trivial expression so we will create an anonymous function
						create_anonymous_function_return_expression=true;
					}

					if(create_anonymous_function_return_expression==true){
						Symbol *sf_aux=compiler_new_inline_anonymous_function(compiler_data,&ei_instructions_default);
						param_info.default_param_value={(zs_int)sf_aux,STACK_ELEMENT_PROPERTY_FUNCTION};
					}

					// finally delete all compileruated code
					for(int i=0; i < ei_instructions_default.length(); i++){
						delete ei_instructions_default.get(i);
					}

				}

				script_function_params.push(
						new FunctionParam(param_info)
				);
			}

			// register function ...
			if(properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS){ // register named function...
				if(custom_symbol_name != ""){
					name=custom_symbol_name;
				}else{
					name=compiler_anonymous_function_name();//sc!=NULL?sc->name:"");
				}
			}

			params=FunctionParam::createArrayFromArray(&script_function_params);
			params_len=script_function_params.length();

			// remove collected script function params
			for(int i=0; i < script_function_params.length(); i++){
				delete (FunctionParam *)script_function_params.get(i);
			}

			aux_p++;
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			if(*aux_p != '{'){
				if(params != NULL){
					delete [] params;
				}
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,"Syntax error:  expected '{' as function block");
			}

			//--- OP
			if(sc!=NULL){ // register as variable member...
				try{
					symbol_sf=sc->registerMemberFunction(
							name
							,&params
							,params_len
							,is_static?SCRIPT_FUNCTION_PROPERTY_STATIC:SCRIPT_FUNCTION_PROPERTY_MEMBER_FUNCTION
							,SCRIPT_TYPE_ID_CLASS_MAIN
							,0
							,compiler_data->current_parsing_file
							,line
					);

				}catch(std::exception & ex){
					if(params != NULL){
						delete [] params;
					}
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,ex.what());
				}
			}
			else{ // register as local variable in the function...
				try{
					symbol_sf=compiler_data->current_function->script_function->registerLocalFunction(
						 scope_info
						, compiler_data->current_parsing_file
						, line
						, name
						, &params
						, params_len
					);
				}catch(std::exception & ex){
					if(params != NULL){
						delete [] params;
					}

					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line,ex.what());
				}

				if((properties & EVAL_KEYWORD_FUNCTION_PROPERTY_IS_ANONYMOUS)==0){
					if(scope_info->owner_type != ZS_TYPE_MAIN(compiler_data->script_types_factory)){ // is a function that was created within a member function...
						((ScriptFunction *)(symbol_sf->ref_ptr))->properties|=SCRIPT_FUNCTION_PROPERTY_MEMBER_FUNCTION;
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

			compiler_push_function(compiler_data,sf);

			// ok let's go to body..
			if((aux_p = compiler_block_function(
					compiler_data
					,aux_p
					,line
					,scope_info
					,sf
					,params
					,params_len
				)
			)==NULL){
				// deallocate current function
				compiler_pop_current_function(compiler_data);
				return NULL;
			}

			compiler_pop_and_compile_function(compiler_data);

			return aux_p;
	//---------------------------------------------
	// CONTROL ERROR
	compiler_keyword_function_params:
			// unallocate script function params
			for(int h=0; h < script_function_params.length(); h++){
				delete (FunctionParam *)script_function_params.get(h);
			}
			script_function_params.clear();
			return NULL;
	//---------------------------------------------

		}

		return NULL;
	}

	char *  compiler_keyword_return(CompilerData *compiler_data,const char *s,int & line,  ScriptScope *scope_info){
		// PRE: **ast_node_to_be_compileruated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		Keyword key_w;
		String s_aux;

		key_w = compiler_is_keyword(aux_p);

		if(key_w != Keyword::KEYWORD_RETURN){ // possible variable...
			return NULL;
		}

		//PASTNode child_node=NULL;
		aux_p += strlen(compiler_data_keywords[key_w].str);
		bool end=false;

		// save starting point before process the expression...
		do{
			Vector<CompilerInstruction *> partial_ex;
			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);

			if((aux_p = compiler_sub_expression(
					compiler_data
					,aux_p
					, line
					, scope_info
					,&partial_ex
					,NULL
					,EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION
			))== NULL){
				return NULL;
			}

			IGNORE_BLANKS(aux_p,compiler_data,aux_p,line);
				//return aux_p;

			// global variables should not deref object references due they are not incs its references through
			// calling functions
			if(partial_ex.length()==1){
				CompilerInstruction *ei_arg=partial_ex.get(0);
				ByteCode byte_code_aux=ei_arg->vm_instruction.byte_code;

				// If byte code is a global var load (find var is also global) set as push stk to
				// avoid deref global objects
				if(byte_code_aux ==BYTE_CODE_LOAD_GLOBAL){
					ei_arg->vm_instruction.byte_code=BYTE_CODE_PUSH_STK_GLOBAL_IRGO;
				}else if(byte_code_aux ==BYTE_CODE_FIND_VARIABLE){
					ei_arg->vm_instruction.value_op1=INSTRUCTION_VALUE_OP1_PUSH_STK_GLOBAL_IRGO;
				}
			}

			compiler_data->current_function->compiler_instructions.concat(partial_ex);
			// next
			if(*aux_p==','){
				aux_p++;
			}else{
				end=true;
			}

		}while(!end);

		compiler_data->current_function->compiler_instructions.push(
			new CompilerInstruction(BYTE_CODE_RET)
		);

		return aux_p;
	}
}
