namespace zetscript{
	namespace eval{

		void push_function(EvalData *eval_data,ScriptFunction *sf);
		int  pop_function(EvalData *eval_data);
		char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * eval_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info,bool return_on_break_or_continue_keyword = false);
		Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_function=false);
		void eval_check_scope(EvalData *eval_data, Scope *scope, unsigned idx_instruction_start);
		void inc_jmp_codes(EvalData *eval_data, int idx_start_instruction, int idx_end_instruction, unsigned inc_value);

		char * is_class_member_extension(EvalData *eval_data,const char *s,int & line,ScriptClass **sc,std::string & member_symbol){

			char *aux_p = (char *)s;
			std::string class_name;
			*sc=NULL;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check whwther the function is anonymous or not.
			aux_p=get_identifier_token(
					eval_data
					,aux_p
					,line
					,class_name);

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

				if((*sc=GET_SCRIPT_CLASS(eval_data,class_name)) != NULL){
					aux_p=get_identifier_token(
							eval_data
							,aux_p+2
							,line
							,member_symbol
					);
					return aux_p;
				}else{
					EVAL_ERROR(eval_data->current_parsing_file,line,"class %s not found",class_name.c_str());
				}
			}
			return NULL;
		}

		char * eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			std::string symbol_value;
			Keyword key_w;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_DELETE){

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

				aux_p=get_identifier_token(
						eval_data
						,aux_p
						,line
						,symbol_value
				);

				 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				return aux_p;
			}

			return NULL;
		}

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
			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_CLASS){

				if(scope_info->scope_parent!=NULL){
					EVAL_ERROR(eval_data->current_parsing_file,line,"class keyword is not allowed");
				}

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

				// check for symbol's name
				aux_p=get_identifier_token(
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
					aux_p=get_identifier_token(
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
						__FILE__
						, __LINE__
						, class_name
						,base_class_name
					);
				}catch(std::exception &ex){
					eval_data->error=true;
					eval_data->error_str=ex.what();
					return NULL;
				}

				ZS_PRINT_DEBUG("registered class \"%s\" line %i ",class_name.c_str(), class_line);

				if(*aux_p == '{' ){

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					// TODO: Register class and baseof
					// register info class ...
					// check for named functions or vars...
					while(*aux_p != '}' && *aux_p != 0){

						// 1st. check whether eval a keyword...
						key_w = is_keyword(aux_p);
						if(key_w == Keyword::KEYWORD_UNKNOWN){
								aux_p = eval_keyword_function(
										eval_data
										,aux_p
										, line
										,sc->symbol.scope // pass class scope

								);
						}else{
							EVAL_ERROR(eval_data->current_parsing_file,line,"unexpected keyword \"%s\" in class declaration \"%s\"",eval_info_keywords[key_w].str,class_name.c_str());
						}
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					}

					if(*aux_p != '}'){
						EVAL_ERROR(eval_data->current_parsing_file,class_line ,"expected '}' to end class declaration \"%s\"",class_name.c_str());
					}

					return aux_p+1;

				}else{
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '{' to start class declaration\"%s\"",class_name.c_str());
				}
			}

			return NULL;
		}
		//
		//--------------------------------------------------------------------------------------------------------------------------------------------------------
		//
		//  KEYWORDS
		//
		char * eval_keyword_function(
				EvalData *eval_data
				, const char *s
				, int & line
				, Scope *scope_info
				, bool check_anonymous_function
				, std::string * resulted_function_name
			){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_var=NULL;
			Keyword key_w;
			std::vector<FunctionParam> args;
			std::string conditional_str;
			ScriptClass *sc=NULL; // if NULL it suposes is the main
			Symbol *symbol_sf=NULL;

			Symbol * irv=NULL;

			static int n_anonymous_function=0;
			std::string function_name="";
			//Scope *scope=scope_info;
			size_t advance_chars=0;
			bool is_anonymous=false;


			// check for keyword ...
			if(scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_CLASS_MAIN
				&& scope_info->scope_base == scope_info
				&& scope_info->scope_parent == NULL // is function member
				){
				key_w = Keyword::KEYWORD_FUNCTION;
				sc=scope_info->script_class;
			}
			else{
				key_w = is_keyword(aux_p);
				advance_chars=strlen(eval_info_keywords[key_w].str);
			}

			if(key_w == Keyword::KEYWORD_FUNCTION){

				std::string arg_value;
				FunctionParam arg_info;

				Scope *scope_function =eval_new_scope(eval_data,scope_info,true); // push current scope
				ScriptFunction *sf=NULL;

				// advance keyword...
				aux_p += advance_chars;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				bool named_function = *aux_p!='(';

				if(named_function){ // is named function..

					if(check_anonymous_function){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected anonymous function");
					}

					// function cannot be declared within main scope
					if(scope_info != MAIN_SCOPE(eval_data) && sc == NULL){ // function within a function (not function member)
						EVAL_ERROR(eval_data->current_parsing_file,line,"named functions are only allowed in main scope. You can only use anonymous functions");
					}

					if(sc==NULL){ // check if function member declaration
					   end_var=is_class_member_extension( // is function class extensions (example A::function1(){ return 0;} )
							eval_data
							,aux_p
							,line
							,&sc
							,function_name
					   );
					}

					// not member function, so is normal function ...
					if(end_var == NULL){ // global function
						// check whwther the function is anonymous with a previous arithmetic operation ....
						end_var=get_identifier_token(
								eval_data
								,aux_p
								,line
								,function_name
						);
						// copy value
						zs_strutils::copy_from_ptr_diff(function_name,aux_p,end_var);
					}
					aux_p=end_var;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				}
				else{ // name anonymous function
					if(check_anonymous_function==false){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Anonymous functions should be used on expression");
					}

					is_anonymous=true;

					if(
							scope_info->script_class != SCRIPT_CLASS_MAIN(eval_data)
						 && scope_info->scope_parent != NULL
					){
						sc=scope_info->script_class;
					}
				}

				// eval function args...
				if(*aux_p != '('){ // push arguments...
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected open '(' for function");
				}

				// save scope pointer for function args ...
				aux_p++;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				bool var_args=false;

				// grab words separated by ,
				while(*aux_p != 0 && *aux_p != ')' && !var_args){
					arg_info.by_ref=false;
					arg_info.var_args=false;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					if(args.size()>0){
						if(*aux_p != ','){
							EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ',' ");
						}
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
					}

					if(*aux_p == ')' || *aux_p == ','){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected arg");
					}

					// capture line where argument is...
					arg_info.line=line;

					if(*aux_p=='.' && *(aux_p+1)=='.' && *(aux_p+2)=='.'){// is_keyword(aux_p)==KEYWORD_REF){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+3,line);
						arg_info.by_ref =true;
					}else if(is_keyword(aux_p)==KEYWORD_VARIABLE_ARGS){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[KEYWORD_REF].str),line);
						var_args=arg_info.var_args =true;
					}



					//int m_start_arg=line;
					end_var=get_identifier_token(
							 eval_data
							,aux_p
							,line
							,arg_value
					);

					// copy value
					zs_strutils::copy_from_ptr_diff(arg_value,aux_p,end_var);

					// ok register symbol into the object function ...
					arg_info.arg_name=arg_value;
					args.push_back(arg_info);
					aux_p=end_var;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				}

				aux_p++;
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p != '{'){
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '{' as function block");
				}

				// register function ...
				if(is_anonymous){ // register named function...
					function_name="_@afun_"+(scope_info->script_class!=SCRIPT_CLASS_MAIN(eval_data)?scope_info->script_class->symbol.name:"")+"_"+zs_strutils::int_to_str(n_anonymous_function++);
				}


				if(resulted_function_name!=NULL){ // save function...
					*resulted_function_name=function_name;
				}

				//--- OP
				if(sc!=NULL){ // register as variable member...
					symbol_sf=sc->registerMemberFunction(
							 eval_data->current_parsing_file
							,line
							,function_name
							,args
					);
				}
				else{ // register as local variable in the function...
					symbol_sf=eval_data->current_function->script_function->registerLocalFunction(
						 scope_info
						, eval_data->current_parsing_file
						, line
						, function_name
						, args
					);

					if(scope_info->script_class != SCRIPT_CLASS_MAIN(eval_data)){ // is a function that was created within a member function...
						symbol_sf->properties|=SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS;
					}

				}

				sf=(ScriptFunction *)symbol_sf->ref_ptr;

				// register args as part of stack...
				for(unsigned i=0; i < args.size(); i++){
					try{
						sf->registerLocalVariable(
								scope_function
								,eval_data->current_parsing_file
								,args[i].line
								,args[i].arg_name
						);
					}catch(std::exception & ex){
						eval_data->error=true;
						eval_data->error_str=ex.what();
						return NULL;
					}
				}

				push_function(eval_data,sf);

				// ok let's go to body..
				aux_p = eval_block(
						eval_data
						,aux_p
						,line
						,scope_function);

				pop_function(eval_data);
			}
			return aux_p;
		}

		char *  eval_keyword_return(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			Keyword key_w;
			std::string s_aux;

			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_RETURN){ // possible variable...
				//PASTNode child_node=NULL;
				aux_p += strlen(eval_info_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// save starting point before process the expression...
				if((aux_p = eval_expression(
						eval_data
						,aux_p
						, line
						, scope_info
						,&eval_data->current_function->instructions
						//,std::vector<char>{';'}
				))!= NULL){

					eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_RET));

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					return aux_p;
				}
			}
			return NULL;
		}

		void link_breaks(EvalData *eval_data){
			// capture breaks and link jmp positions...
			if(eval_data->break_jmp_instructions.size() > 0){ // capture break_jmp_instructions...
				size_t idx_instruction = eval_data->current_function->instructions.size();
				for(unsigned i=0; i < eval_data->break_jmp_instructions.size(); i++){
					eval_data->break_jmp_instructions[i]->vm_instruction.value_op2=idx_instruction;
				}
				eval_data->break_jmp_instructions.clear();
			}
		}

		void link_continues(EvalData *eval_data, unsigned idx_instruction){
			// capture breaks and link jmp positions...
			if(eval_data->continue_jmp_instructions.size() > 0){ // capture breaks...
				for(unsigned i=0; i < eval_data->continue_jmp_instructions.size(); i++){
					eval_data->continue_jmp_instructions[i]->vm_instruction.value_op2=idx_instruction;
				}
				eval_data->continue_jmp_instructions.clear();
			}
		}

		char * eval_keyword_while(EvalData *eval_data,const char *s,int & line, Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;
			std::string start_symbol;
			Keyword key_w;
			int idx_instruction_conditional_while;
			EvalInstruction *ei_jnt; // conditional to end block

			// check for keyword ...
			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_WHILE){

				aux_p += strlen(eval_info_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// evaluate conditional line ...
				if(*aux_p != '('){
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '(' while ");
				}

				// save current instruction to use later...
				idx_instruction_conditional_while=(int)(eval_data->current_function->instructions.size());

				end_expr = eval_expression(
						eval_data
						,aux_p+1
						,line
						,scope_info
						,&eval_data->current_function->instructions
						,std::vector<char>{')'}
				);

				// insert instruction if evaluated expression
				eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JNT));


				zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);

				IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);

				if(*aux_p != '{'){
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected while-block open block ('{') ");
				}

				aux_p=eval_block(
						eval_data
						,aux_p
						,line
						,scope_info
				);

				// insert jmp instruction to begin condition while...
				eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_JMP,ZS_IDX_UNDEFINED,idx_instruction_conditional_while));

				// update jnt instruction to jmp after jmp instruction...
				ei_jnt->vm_instruction.value_op2=eval_data->current_function->instructions.size();

				// catch all breaks in the while...
				link_breaks(eval_data);

				// catch all breaks in the while...
				link_continues(eval_data,idx_instruction_conditional_while);

				return aux_p;
			}

			return NULL;
		}

		char * eval_keyword_do_while(EvalData *eval_data,const char *s,int & line, Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;
			std::string start_symbol;
			Keyword key_w;
			int idx_do_while_start;
			int idx_do_while_conditional;


			// check for keyword ...
			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_DO_WHILE){

				aux_p += strlen(eval_info_keywords[key_w].str);

				//1st evaluate body ..
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// save current instruction to use later...
				idx_do_while_start=(int)(eval_data->current_function->instructions.size());


				if(*aux_p != '{'){
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected open block ('{') in do-while expression");
				}

				aux_p=eval_block(
						eval_data
						,aux_p
						,line
						,scope_info
				);

					// Finally evaluate conditional line ...
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// check for keyword ...
				key_w = is_keyword(aux_p);

				if(key_w!=KEYWORD_WHILE){
					EVAL_ERROR(eval_data->current_parsing_file,line,"expected while keyword");
				}

				aux_p += strlen(eval_info_keywords[key_w].str);

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p == '('){

					idx_do_while_conditional=(int)(eval_data->current_function->instructions.size());

					if((end_expr = eval_expression(
							eval_data
							,aux_p+1
							,line
							,scope_info
							,&eval_data->current_function->instructions
							,std::vector<char>{')'}
					)) != NULL){
						zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);
					}else{
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ')' do-while expression");
					}
				}else{
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '(' do-while expression");
				}
				// insert jmp instruction to begin condition while...
				eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_JT,ZS_IDX_UNDEFINED,idx_do_while_start));

				// catch all breaks in the while...
				link_breaks(eval_data);

				// catch all continues and evaluates bottom...
				link_continues(eval_data,idx_do_while_conditional);

				return end_expr+1;
			}
			return NULL;
		}

		char * eval_keyword_if_else(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;//,*start_symbol;
			Keyword key_w;
			//std::vector<EvalInstruction *> ei_jmps;
			EvalInstruction *if_jnt;
			std::vector<EvalInstruction *> else_end_jmp;
			EvalInstruction *ei_aux;
			bool end=true;

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_IF){
				do{
					end=true;
					//ei_jmps.clear();
					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p != '('){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '(' if");
					}

					// eval conditional expression
					end_expr = eval_expression(
							eval_data
							,aux_p+1
							,line
							,scope_info
							,&eval_data->current_function->instructions
							,std::vector<char>{')'}
					);

					// insert instruction if evaluated expression
					eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JNT));
					if_jnt=ei_aux;
					//ei_jmps.push_back(ei_aux);

					IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);
					if(*aux_p != '{'){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected if-block open block ('{')");

					}

					aux_p=eval_block(eval_data,aux_p
						,line
						,scope_info
					);

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					bool else_key = false;
					if((key_w = is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
						else_key = (key_w == Keyword::KEYWORD_ELSE);
					}

					if(!else_key){ // it finish if-else
						if_jnt->vm_instruction.value_op2=eval_data->current_function->instructions.size();
					}else{ // do else

						// we should insert jmp to end conditional chain if/else...
						eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JMP));
						if_jnt->vm_instruction.value_op2=eval_data->current_function->instructions.size();

						else_end_jmp.push_back(ei_aux);

						aux_p += strlen(eval_info_keywords[key_w].str);

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						bool if_key = false;
						if((key_w = is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
							if_key = (key_w == Keyword::KEYWORD_IF);
						}

						if(if_key){ // else if
							end=false;
						}else{ // only else, expect {

							if(*aux_p != '{'){
								EVAL_ERROR(eval_data->current_parsing_file,line,"Expected else-block open block ('{')");
							}

							// eval else block
							aux_p=eval_block(
									eval_data
									,aux_p
									,line
									,scope_info
							);

						}


					}

				}while(!end); // loop

				for(unsigned i=0; i < else_end_jmp.size(); i++){
					else_end_jmp[i]->vm_instruction.value_op2=eval_data->current_function->instructions.size();
				}
				return aux_p;
			}
			return NULL;
		}

		char * eval_keyword_for(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			Keyword key_w;
			unsigned int idx_instruction_for_start;
			EvalInstruction *ei_jnt; // conditional to end block
			std::vector<EvalInstruction *> post_operations;

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_FOR){

				aux_p += strlen(eval_info_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p == '('){ // ready ...

					int idx_instruction_start_for=(int)(eval_data->current_function->instructions.size());

					// save scope pointer ...
					Scope *new_scope =eval_new_scope(eval_data,scope_info); // push current scope


					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

					// 1. Iterator ...
					if(*aux_p != ';'){ // there's some var Init...
						// Init node ...
						Keyword key_w = is_keyword(aux_p);

						if(key_w == KEYWORD_VAR){
							aux_p = eval_keyword_var_or_const(
									eval_data
									,aux_p
									,line
									,new_scope
							);
						}
						else{
							EVAL_ERROR(eval_data->current_parsing_file,line,"Expected For 'var' keyword");
						}
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					key_w = is_keyword(aux_p);
					if(key_w == Keyword::KEYWORD_IN){

						IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[Keyword::KEYWORD_IN].str),line);

						aux_p = eval_expression(
								eval_data
								,(const char *)aux_p
								,line
								,new_scope
								,&eval_data->current_function->instructions
						);

						// init it and vector/dictionary
						eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_IT_INI));
					}
					else{ // expects conditional and post (i.e for(;;) )
						if(*aux_p != ';'){
							EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ';'");
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						if(*aux_p != ';'){ // conditional...
							char * end_p=NULL;
							IGNORE_BLANKS(end_p,eval_data,aux_p+1,line);

							if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

								idx_instruction_for_start=(int)(eval_data->current_function->instructions.size());

								aux_p = eval_expression(
										eval_data
										,(const char *)aux_p
										,line
										,new_scope
										,&eval_data->current_function->instructions
								);

								eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JNT));
							}
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						if(*aux_p != ';'){
							EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ';'");

						}
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						if(*aux_p != ')' ){ // finally do post op...

							if(*aux_p == ',' ){
								EVAL_ERROR(eval_data->current_parsing_file,line,"Unexpected ) ");
							}

							do{
								aux_p = eval_expression(
										eval_data
										,aux_p
										,line
										,new_scope
										,&post_operations
								);

								if(*aux_p == ',' ){
									IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
								}else{
									if(*aux_p != ')' ){
										EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
									}
								}

							}while(*aux_p != ')' && *aux_p != 0);
						}
					}

					if(*aux_p != ')'){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
					if(*aux_p != '{'){
						EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '{' for begin block");
					}

					// eval block ...
					aux_p=eval_block(
							eval_data
							,aux_p
							,line
							,new_scope
					);

					// catch all continues and set all jmps after processing block but before post operation...
					link_continues(eval_data,(int)(eval_data->current_function->instructions.size()));

					// insert post operations...
					eval_data->current_function->instructions.insert(
						eval_data->current_function->instructions.end()
						,post_operations.begin()
						,post_operations.end()
					);

					// insert jmp instruction to begin condition for...
					eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_JMP,ZS_IDX_UNDEFINED,idx_instruction_for_start));

					// update jnt instruction to jmp after jmp instruction...
					ei_jnt->vm_instruction.value_op2=eval_data->current_function->instructions.size();

					// catch all breaks in the while...
					link_breaks(eval_data);

					// true: We treat declared variables into for as another scope.
					eval_check_scope(eval_data,new_scope,idx_instruction_start_for);
					return aux_p;

				}else{
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '(' for");
				}
			}

			return NULL;
		}

		char * eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			char *aux_p = (char *)s;
			Scope *scope_case=NULL;
			std::string val;
			Keyword key_w;//,key_w2;
			int idx_start_switch;
			std::vector<EvalInstruction *> 	ei_cases; // stores all conditional instructions at begin
			std::vector<EvalInstruction *>  ei_break_jmps; // breaks or if condition not satisfies nothing (there's no default)

			// check for keyword ...
			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_SWITCH){

				aux_p += strlen(eval_info_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				if(*aux_p == '('){
						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						// evaluate switch condition expression ...
						aux_p = eval_expression(
							eval_data
							,aux_p
							,line
							,scope_info
							,&eval_data->current_function->instructions
							,std::vector<char>{')'}
						);

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						if(*aux_p == '{'){

							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							idx_start_switch = (int)(eval_data->current_function->instructions.size());
							int n_default=0;

							while(*aux_p != '}' && *aux_p != 0){

								bool is_default=false;
								// search for case or default...
								key_w = is_keyword(aux_p);
								if(key_w == KEYWORD_CASE){

									TokenNode token_node;
									PreOperator pre_operator=PreOperator::PRE_OPERATOR_UNKNOWN;
									int idx_current_instruction=(int)(eval_data->current_function->instructions.size());

									// ignore case
									IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);


									if((pre_operator = is_pre_operator(aux_p))!=PreOperator::PRE_OPERATOR_UNKNOWN){
										IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_pre_operators[pre_operator].str),line);
									}

									// capture constant value (should be a constant -not a identifier in any case-)
									aux_p=eval_symbol(
										eval_data
										,aux_p
										,line
										,&token_node
										,pre_operator
										,PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
									);

									if(token_node.token_type != TOKEN_TYPE_LITERAL){
										EVAL_ERROR(eval_data->current_parsing_file,line,"'case' only accepts literals");
									}

									/*std::vector<EvalInstruction *> case_instructions={
											token_node.instructions[0]
											,new EvalInstruction(
											BYTE_CODE_JE
											,ZS_IDX_UNDEFINED
											,idx_current_instruction
									)};*/

									// insert a pair of instructions...
									ei_cases.push_back(token_node.instructions[0]);
									ei_cases.push_back(new EvalInstruction(
											BYTE_CODE_JE
											,ZS_IDX_UNDEFINED
											,idx_current_instruction
									));

								}else if(key_w == KEYWORD_DEFAULT){
									if(n_default > 0){
										EVAL_ERROR(eval_data->current_parsing_file,line,"there's an already 'default' case");
									}

									is_default=true;
									n_default++;
									IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

								}else{
									EVAL_ERROR(eval_data->current_parsing_file,line,"Expected 'case' or 'default' keyword");
								}

								if(*aux_p!=':'){
									EVAL_ERROR(eval_data->current_parsing_file,line,"Expected ':' ");
								}

								// ignore :
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

								if((is_keyword(aux_p) == Keyword::KEYWORD_CASE)){ // another case ...
									continue;
								}

								aux_p=eval_recursive(
									eval_data
									,aux_p
									, line
									, scope_info
									, true // cancel eval when break or case is found...
								);

								if((is_keyword(aux_p) == Keyword::KEYWORD_BREAK)){ // it cuts current expression to link breaks...
									IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[Keyword::KEYWORD_BREAK].str),line);
									EvalInstruction *ei_break_jmp=new EvalInstruction(BYTE_CODE_JMP);
									eval_data->current_function->instructions.push_back(ei_break_jmp);
									ei_break_jmps.push_back(ei_break_jmp);
								}else if(is_default){
									EVAL_ERROR(eval_data->current_parsing_file,line,"'default' case needs to have a 'break' at the end");
								}

								IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

							}

							int idx_current_instruction=(int)(eval_data->current_function->instructions.size());

							if(*aux_p != '}'){
								EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '}' switch");
							}

							// insert all cases found first
							eval_data->current_function->instructions.insert(
									eval_data->current_function->instructions.begin()+idx_start_switch,
									ei_cases.begin(),
									ei_cases.end()
							);

							for(unsigned i=0; i < ei_break_jmps.size(); i++){
								ei_break_jmps[i]->vm_instruction.value_op2=idx_current_instruction;
							}


							// update all jmp acording number of cases found...
							inc_jmp_codes(eval_data, idx_start_switch, idx_current_instruction, (int)(ei_cases.size()));




							return aux_p+1;
						}
						else{
							EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '{' begin switch block");
						}
				}
				else{
					EVAL_ERROR(eval_data->current_parsing_file,line,"Expected '(' switch ");
				}
			}

			return NULL;
		}

		ConstantValue * perform_const_operation(EvalData *eval_data,int line,ByteCode byte_code,ConstantValue *stk_op1, ConstantValue *stk_op2){

			float op2=0;
			float result_op=0;
			ConstantValue *stk_int_calc_result=NULL;
			const char *file=eval_data->current_parsing_file;
			//int line=(*it)->instruction_source_info.line;

			if(stk_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){
				result_op=((zs_int)stk_op1->stk_value);
			}else if(stk_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				result_op=*((float *)(&stk_op1->stk_value));
			}else{
				EVAL_ERROR(file,line,"Constant operations should be number");
			}

			if(stk_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){
				op2=((zs_int)stk_op2->stk_value);
			}else if(stk_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				op2=*(float *)((zs_int)stk_op2->stk_value);
			}else{
				EVAL_ERROR(file,line,"Constant operations should be number");
			}

			stk_int_calc_result=(ConstantValue *)malloc(sizeof(ConstantValue));

			// which operation ?
			switch(byte_code){
			case BYTE_CODE_NOT:
				result_op=-result_op;
				break;
			case BYTE_CODE_ADD:
				result_op+=op2;
				break;
			case BYTE_CODE_MUL:
				result_op*=op2;
				break;
			case BYTE_CODE_DIV:
				if(op2==0){
					EVAL_ERROR(file,line,"divide by 0");
				}
				result_op/=op2;
				break;
			case BYTE_CODE_MOD:
				if(op2==0){
					EVAL_ERROR(file,line,"divide by 0");
				}
				result_op=fmod(result_op,op2);
				break;
			default:
				EVAL_ERROR(file,line,"const instruction %i not implemented",byte_code);
				break;
			}

			if((stk_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT) && (stk_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT)){
				*stk_int_calc_result={(void *)((zs_int)result_op),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT};
			} // float
			if((stk_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT) || (stk_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){
				memcpy(&stk_int_calc_result->stk_value,&result_op,sizeof(float));
				stk_int_calc_result->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
			}

			return stk_int_calc_result;
		}

		char * eval_keyword_var_or_const(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass
			char *aux_p = (char *)s;
			Keyword key_w;
			char *start_var,*end_var;
			std::string s_aux,variable_name;
			//bool end=false;
			bool allow_for_in=true;
			int start_line=0;

			key_w = is_keyword(aux_p);


			if(key_w == Keyword::KEYWORD_VAR || key_w == Keyword::KEYWORD_CONST){ // possible variable...
				bool is_constant = key_w == Keyword::KEYWORD_CONST;
				if(is_constant){ // scope_info will be global scope...
					scope_info = MAIN_SCOPE(eval_data);
				}

				aux_p += strlen(eval_info_keywords[key_w].str);
				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
				bool new_variable=false;


				do{ // JE: added multivar feature.

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					start_var=aux_p;
					start_line = line;

					line = start_line;

					// check whwther the function is anonymous with a previous arithmetic operation ....
					end_var=get_identifier_token(
							eval_data,
							aux_p,
							line,
							variable_name
					);

					ZS_PRINT_DEBUG("registered symbol \"%s\" line %i ",variable_name.c_str(), line);

					Keyword keyw = is_keyword(variable_name.c_str());

					if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...
						EVAL_ERROR(eval_data->current_parsing_file,line,"Cannot use symbol name as reserverd symbol \"%s\"",eval_info_keywords[keyw].str);
					}

					// register symbol...
					if(is_constant == false){ // do not register as variable...
						eval_data->current_function->script_function->registerLocalVariable(
								scope_info
								,eval_data->current_parsing_file
								, line
								, variable_name
						);
					}

					// advance identifier length chars
					aux_p=end_var;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p == '='){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
						allow_for_in=false;

						std::vector<EvalInstruction *>	 		constant_instructions;
						// try to evaluate expression...
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						aux_p = eval_expression(
							eval_data
							,is_constant?(aux_p+1):start_var
							,start_line
							,scope_info
							,is_constant?&constant_instructions:&eval_data->current_function->instructions
							//,std::vector<char>{','}
						);

						/*if(is_constant){ // resolve constant_expression

							ConstantValue *stk_op1,*stk_op2,*stk_int_calc_result;
							std::vector<zs_int> stack; // constant/vectors or dictionaries...
							std::vector<zs_int> inter_calc_stack; // constant/vectors or dictionaries...

							// let's fun evalute, an expression throught its op codes...
							EvalInstruction **it=&constant_instructions[0];
							size_t size=constant_instructions.size();
							for(unsigned i=0; i < size; i++,it++){
								Instruction *instruction=&(*it)->vm_instruction;
								if(instruction->byte_code == BYTE_CODE_LOAD_TYPE_CONSTANT){
									stack.push_back(instruction->value_op2);
								}else{ // expect operation ?

									if(stack.size()<2){
										EVAL_ERROR(eval_data->current_parsing_file,(*it)->instruction_source_info.line,"internal error expected >= 3 stacks for constant");
									}

									stk_op1=(ConstantValue *)stack[stack.size()-2];
									stk_op2=(ConstantValue *)stack[stack.size()-1];
									stack.pop_back(); // op2
									stack.pop_back(); // op1

									stk_int_calc_result=perform_const_operation(
										eval_data
										,(*it)->instruction_source_info.line
										,instruction->byte_code
										,stk_op1
										,stk_op2
									);

									stack.push_back((zs_int)stk_int_calc_result);
									inter_calc_stack.push_back((zs_int)stk_int_calc_result);
								}
							}

							if(stack.size()!=1){
								EVAL_ERROR(eval_data->current_parsing_file,(*it)->instruction_source_info.line,"internal error: final stack should be 1");
							}

							stk_int_calc_result = (ConstantValue *)stack[0];

							if(stk_int_calc_result->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){
								printf("constant %i\n",(int)((zs_int)stk_int_calc_result->stk_value));
							}

							if(stk_int_calc_result->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
								printf("constant %f\n",*((float *)&stk_int_calc_result->stk_value));
							}

							if(stk_int_calc_result->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
								printf("constant %s\n",((std::string *)stk_int_calc_result)->c_str());
							}

							for(unsigned i=0; i < constant_instructions.size();i++){
								delete constant_instructions[i];
							}

							for(unsigned i=0; i < inter_calc_stack.size();i++){
								free((void *)inter_calc_stack[i]);
							}
						}*/

						line = start_line;
					}
					new_variable=false;
					if(*aux_p == ','){
						new_variable=true;
						aux_p++;
					}
				}while(new_variable);

				return aux_p;
			}
			return NULL;
		}

		char *eval_keyword_break(EvalData *eval_data,const char *s, int & line, Scope *scope_info){
			char *aux_p = (char *)s;
			Keyword key_w;

			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_BREAK){

				int last_line_ok = line;
				EvalInstruction *jmp_instruction;

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

				// insert jmp instruction
				eval_data->current_function->instructions.push_back(
					jmp_instruction=new EvalInstruction(BYTE_CODE_JMP)
				);

				// remmember jmp break...
				eval_data->break_jmp_instructions.push_back(
					jmp_instruction
				);
			}
			return aux_p;
		}

		char *eval_keyword_continue(EvalData *eval_data,const char *s, int & line, Scope *scope_info){
			char *aux_p = (char *)s;
			Keyword key_w;

			key_w = is_keyword(aux_p);

			if(key_w == Keyword::KEYWORD_CONTINUE){

				int last_line_ok = line;
				EvalInstruction *jmp_instruction;

				IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

				// insert jmp instruction
				eval_data->current_function->instructions.push_back(
					jmp_instruction=new EvalInstruction(BYTE_CODE_JMP)
				);

				// remmember jmp break...
				eval_data->continue_jmp_instructions.push_back(
					jmp_instruction
				);
			}

			return aux_p;
		}

		char *eval_keyword(EvalData *eval_data,const char *s, int & line, Scope *scope_info, Keyword keyw){
			char *aux_p= (char *)s;

			// check if condition...
			if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...

				switch(keyw){
				case KEYWORD_FUNCTION:
					return  eval_keyword_function(eval_data,s,line,scope_info);
					break;
				case KEYWORD_CLASS:
					return  eval_keyword_class(eval_data,s,line,scope_info);
					break;
				default:
					if(eval_info_keywords[keyw].eval_fun != NULL){
						return (*eval_info_keywords[keyw].eval_fun)(eval_data,s,line,scope_info);
					}else{
						THROW_RUNTIME_ERROR("Not implemented");
					}
					break;
				}
			}
			return NULL; // is not keyword
		}
	}
}
