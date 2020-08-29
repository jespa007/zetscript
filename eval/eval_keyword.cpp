namespace zetscript{
	namespace eval{

		void push_function(EvalData *eval_data,ScriptFunction *sf);
		void pop_function(EvalData *eval_data);
		char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * eval_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info,bool return_on_break_or_continue_keyword = false);
		Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent);
		void eval_check_scope(EvalData *eval_data, Scope *scope, unsigned idx_instruction_start);
		void inc_jmp_codes(EvalData *eval_data, int idx_start_instruction, int idx_end_instruction, unsigned inc_value);
		//Scope * evalPopScope(EvalData *eval_data, Scope *current_scope);

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
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"class %s not found",class_name.c_str());
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

			if(key_w != Keyword::KEYWORD_UNKNOWN){
				if(key_w == Keyword::KEYWORD_DELETE){

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

					aux_p=get_identifier_token(
							eval_data
							,aux_p
							,line
							,symbol_value
					);

					 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					 if(*aux_p != ';'){
						 THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ;");
					 }

					return aux_p;
				}
			}
			return NULL;
		}

		char * eval_keyword_class(EvalData *eval_data,const char *s,int & line, Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			//Scope *class_scope_info=NULL;
			int class_line;
			std::string class_name;
			std::string base_class_name="";
			ScriptClass *sc;
			Keyword key_w;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){

				if(key_w == Keyword::KEYWORD_CLASS){

					if(scope_info->scope_parent!=NULL){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"class keyword is not allowed");
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
					sc=eval_data->script_class_factory->registerClass(
							__FILE__
							, __LINE__
							, class_name
							,base_class_name
					);

					ZS_PRINT_DEBUG("registered class \"%s\" line %i ",class_name.c_str(), class_line);

					if(*aux_p == '{' ){

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						// TODO: Register class and baseof
						// register info class ...
						// check for named functions or vars...
						while(*aux_p != '}' && *aux_p != 0){

							// 1st. check whether eval a keyword...
							key_w = is_keyword(aux_p);
							if(key_w == Keyword::KEYWORD_UNKNOWN){ // only expects function name
									aux_p = eval_keyword_function(
											eval_data
											,aux_p
											, line
											,sc->symbol.scope // pass class scope

									);
							}else{
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"unexpected keyword \"%s\"",eval_info_keywords[key_w].str);
							}
							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
						}

						if(*aux_p != '}'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
						}

						return aux_p+1;

					}else{
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '{'");
					}
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

			){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_var=NULL;
			Keyword key_w;
			std::vector<FunctionParam> args;
			std::string conditional_str;
			ScriptClass *sc=NULL;
			Symbol *symbol_sf=NULL;

			Symbol * irv=NULL;

			// we deduce we are in class whether is not main class and scope idx is pointing at scope base
			bool is_class=
					scope_info->script_class->idx_class != IDX_BUILTIN_TYPE_CLASS_MAIN
					&& scope_info->scope_base == scope_info
					;

			static int n_anonymous_function=0;
			std::string function_name="";
			Scope *scope=scope_info;
			size_t advance_chars=0;


			// check for keyword ...
			if(is_class){ // within class supposes is a function already
				key_w = Keyword::KEYWORD_FUNCTION;
			}
			else{
				key_w = is_keyword(aux_p);
				advance_chars=strlen(eval_info_keywords[key_w].str);
			}

			if(key_w != Keyword::KEYWORD_UNKNOWN){

				if(key_w == Keyword::KEYWORD_FUNCTION){

					// advance keyword...
					aux_p += advance_chars;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					bool named_function = *aux_p!='(';

					if(named_function){ // is named function..

						if(is_class){
							sc=scope_info->script_class;
						}else if((end_var=is_class_member_extension( // is function class extensions (example A::function1(){ return 0;} )
								eval_data
								,aux_p
								,line
								,&sc
								,function_name
						))!=NULL){ // check if particular case extension attribute class
							// current scope is changed by class scope...
							scope = sc->symbol.scope;
							//symbol_value = (char *)class_member.c_str();
						}else{
							if(is_class){ // get class from idx_scope

							}
						}

						if(end_var == NULL){ // global function
							// check whwther the function is anonymous with a previous arithmetic operation ....
							end_var=get_identifier_token(
									eval_data
									,aux_p
									,line
									,function_name
							);

						}

						// copy value
						zs_strutils::copy_from_ptr_diff(function_name,aux_p,end_var);

						aux_p=end_var;
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					}
					else{ //function node
						// TODO manage function object
						printf("TODO manage function object\n");
					}

					// eval function args...
					if(*aux_p == '('){ // push arguments...

						aux_p++;
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
						std::string arg_value;
						FunctionParam arg_info;

						// grab words separated by ,
						while(*aux_p != 0 && *aux_p != ')'){
							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
							//char *arg_name;


							if(args.size()>0){
								if(*aux_p != ','){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ',' ");
								}
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
							}

							if(*aux_p == ')' || *aux_p == ','){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected arg");
							}

							// capture line where argument is...
							arg_info.line=line;

							//int m_start_arg=line;
							end_var=get_identifier_token(
									 eval_data
									,aux_p
									,line
									,arg_value
							);

							// copy value
							zs_strutils::copy_from_ptr_diff(arg_value,aux_p,end_var);

							// check if repeats...
							for(unsigned k = 0; k < args.size(); k++){
								if(args[k].arg_name == arg_value){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Repeated argument '%s' argument ",arg_value.c_str());
								}
							}

							// check whether parameter name's matches with some global variable...
							if((irv=scope->getSymbol(arg_value.c_str())) != NULL){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Ambiguous symbol argument \"%s\" name with var defined at %i", arg_value.c_str(), -1);
							}
								// ok register symbol into the object function ...


							arg_info.arg_name=arg_value;
							args.push_back(arg_info);


							aux_p=end_var;
							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
						}

						aux_p++;
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						if(*aux_p != '{'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '{'");
						}

						// register function ...
						if(!named_function){ // register named function...
							function_name="_afun_"+zs_strutils::int_to_str(n_anonymous_function++);
						}
						//--- OP
						if(sc!=NULL){ // register as variable member...
							symbol_sf=sc->registerFunctionMember(
									 eval_data->current_parsing_file
									,line
									,function_name
									,args
							);
						}
						else{ // register as local variable in the function...
							symbol_sf=eval_data->current_function->script_function->registerFunction(
									scope_info
									, eval_data->current_parsing_file
									, line
									, function_name
									,args
							);
						}

						push_function(eval_data,(ScriptFunction *)symbol_sf->ref_ptr);

						// ok let's go to body..
						aux_p = eval_block(
								eval_data
								,aux_p
								,line
								,scope);

						pop_function(eval_data);
					}
					else{
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line," Expected '('");
					}
				}else{
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected operator or function operator");
				}
			}
			return aux_p;
		}

		char *  eval_keyword_return(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			Keyword key_w;
			std::string s_aux;

			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){

				if(key_w == Keyword::KEYWORD_RETURN){ // possible variable...
					//PASTNode child_node=NULL;
					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					if((aux_p = eval_expression(
							eval_data
							,aux_p
							, line
							, scope_info
							,&eval_data->current_function->instructions
					))!= NULL){

						if(*aux_p!=';'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ';'");
						}

						eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_RET));

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						return aux_p;
					}
				}
			}
			return NULL;
		}

		void link_breaks(EvalData *eval_data){
			// capture breaks and link jmp positions...
			if(eval_data->break_jmp_instructions.size() > 0){ // capture break_jmp_instructions...
				unsigned idx_instruction = eval_data->current_function->instructions.size();
				for(unsigned i=0; i < eval_data->break_jmp_instructions.size(); i++){
					eval_data->break_jmp_instructions[i]->vm_instruction.value_op2=idx_instruction;
					delete eval_data->break_jmp_instructions[i]; // delete because is not needed anymore
				}

				eval_data->break_jmp_instructions.clear();
			}
		}

		void link_continues(EvalData *eval_data, unsigned idx_instruction){
			// capture breaks and link jmp positions...
			if(eval_data->continue_jmp_instructions.size() > 0){ // capture breaks...
				for(unsigned i=0; i < eval_data->continue_jmp_instructions.size(); i++){
					eval_data->continue_jmp_instructions[i]->vm_instruction.value_op2=idx_instruction;
					delete eval_data->continue_jmp_instructions[i]; // delete because is not needed anymore
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
			unsigned int idx_instruction_conditional_while;
			EvalInstruction *ei_jnt; // conditional to end block


			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){
				if(key_w == Keyword::KEYWORD_WHILE){

					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					// evaluate conditional line ...
					if(*aux_p == '('){

						// save current instruction to use later...
						idx_instruction_conditional_while=eval_data->current_function->instructions.size();


						end_expr = eval_expression(
								eval_data
								,aux_p+1
								,line
								,scope_info
								,&eval_data->current_function->instructions
						);

						if(*end_expr != ')'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
						}

						// insert instruction if evaluated expression
						eval_data->current_function->instructions.push_back(ei_jnt=new EvalInstruction(BYTE_CODE_JNT));


						zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);

						IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);

						if(*aux_p != '{'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected while-block open block ('{') ");
						}

						aux_p=eval_block(
								eval_data
								,aux_p
								,line
								,scope_info
						);

					}else{
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '(' while ");
					}

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
			}
			return NULL;
		}

		char * eval_keyword_do_while(EvalData *eval_data,const char *s,int & line, Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;
			std::string start_symbol;
			Keyword key_w;
			unsigned int idx_do_while_start;
			unsigned int idx_do_while_conditional;


			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){
				if(key_w == Keyword::KEYWORD_DO_WHILE){

					aux_p += strlen(eval_info_keywords[key_w].str);

					//1st evaluate body ..
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					// save current instruction to use later...
					idx_do_while_start=eval_data->current_function->instructions.size();


					if(*aux_p != '{'){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected open block ('{') in do-while expression");
					}
					if((aux_p=eval_block(
							eval_data
							,aux_p
							,line
							,scope_info
					))!= NULL){

						// Finally evaluate conditional line ...
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						// check for keyword ...
						key_w = is_keyword(aux_p);

						if(key_w!=KEYWORD_WHILE){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected while keyword");
						}

						aux_p += strlen(eval_info_keywords[key_w].str);

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						if(*aux_p == '('){

							idx_do_while_conditional=eval_data->current_function->instructions.size();

							if((end_expr = eval_expression(
									eval_data
									,aux_p+1
									,line
									,scope_info
									,&eval_data->current_function->instructions
							)) != NULL){
								if(*end_expr != ')'){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
								}

								zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr);

							}else{
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')' do-while expression");
							}

						}else{
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '(' do-while expression");
						}

						// insert jmp instruction to begin condition while...
						eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_JT,ZS_IDX_UNDEFINED,idx_do_while_start));

						// catch all breaks in the while...
						link_breaks(eval_data);

						// catch all continues and evaluates bottom...
						link_continues(eval_data,idx_do_while_conditional);


						return end_expr+1;
					}
				}
			}
			return NULL;
		}

		char * eval_keyword_if_else(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;//,*start_symbol;
			Keyword key_w;
			std::vector<EvalInstruction *> ei_jmps;
			EvalInstruction *ei_aux;

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){
				if(key_w == Keyword::KEYWORD_IF){

					do{

						aux_p += strlen(eval_info_keywords[key_w].str);
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						if(*aux_p != '('){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '(' if");
						}

						// eval conditional expression
						end_expr = eval_expression(
								eval_data
								,aux_p+1
								,line
								,scope_info
								,&eval_data->current_function->instructions
						);

						if(*end_expr != ')'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
						}


						// insert instruction if evaluated expression
						eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JNT));
						ei_jmps.push_back(ei_aux);

						IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);
						if(*aux_p != '{'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected if-block open block ('{')");

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

						if(else_key){

							// we should insert jmp to end conditional chain if/else...
							eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JMP));
							ei_jmps.push_back(ei_aux);

							aux_p += strlen(eval_info_keywords[key_w].str);

							if(*aux_p != '{'){
								aux_p++;
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

							bool if_key = false;
							if((key_w = is_keyword(aux_p)) != Keyword::KEYWORD_UNKNOWN){
								if_key = (key_w == Keyword::KEYWORD_IF);
							}

							if(!if_key){ // not if, only else

								if(*aux_p != '{'){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected else-block open block ('{')");
								}

								// eval else block
								aux_p=eval_block(
										eval_data
										,aux_p
										,line
										,scope_info
								);

								return aux_p;
							} // else keep up parsing if nodes case ...
						}else{ // end if expression

							// update all collected jump/jnt to current instruction...
							for(unsigned i=0; i < ei_jmps.size(); i++){
								// insert instruction if evaluated expression
								ei_jmps[i]->vm_instruction.value_op2=eval_data->current_function->instructions.size();
							}

							return aux_p;
						}
					}while(true); // loop
				}
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

			if(key_w != Keyword::KEYWORD_UNKNOWN){
				if(key_w == Keyword::KEYWORD_FOR){

					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p == '('){ // ready ...

						unsigned idx_instruction_start_for=eval_data->current_function->instructions.size();

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
								aux_p = aux_p - 1; // redirect aux_p to ';'
							}
							else{
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected 'var' keyword");
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
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ';'");
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*aux_p != ';'){ // conditional...
								char * end_p=NULL;
								IGNORE_BLANKS(end_p,eval_data,aux_p+1,line);

								if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

									idx_instruction_for_start=eval_data->current_function->instructions.size();

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
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ';'");

							}
							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*aux_p != ')' ){ // finally do post op...

								if(*aux_p == ',' ){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Unexpected ) ");
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
											THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
										}
									}

								}while(*aux_p != ')' && *aux_p != 0);
							}
						}

						if(*aux_p != ')'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')'");
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						if(*aux_p != '{'){
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '{' for-block");
						}

						// eval block ...
						aux_p=eval_block(
								eval_data
								,aux_p
								,line
								,new_scope
						);

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

						// catch all continues and evaluates bottom...
						link_continues(eval_data,idx_instruction_for_start);


						// true: We treat declared variables into for as another scope.
						eval_check_scope(eval_data,new_scope,idx_instruction_start_for);
						return aux_p;

					}else{
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '(' for");
					}
				}
			}
			return NULL;
		}

		char * eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			Scope *scope_case=NULL;
			std::string val;
			Keyword key_w;//,key_w2;
			unsigned idx_start_switch;
			std::vector<EvalInstruction *> 	ei_cases; // stores all conditional instructions at begin
			//std::vector<EvalInstruction *> 	ei_jt; // holds instructions to set jmps on each case
			std::vector<EvalInstruction *>  ei_break_jmps; // breaks or if condition not satisfies nothing (there's no default)

			// check for keyword ...
			key_w = is_keyword(aux_p);

			if(key_w != Keyword::KEYWORD_UNKNOWN){
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
							);

							if(*aux_p != ')'){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ')' switch");
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*aux_p == '{'){

								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

								idx_start_switch = eval_data->current_function->instructions.size();
								int n_default=0;

								while(*aux_p != '}' && *aux_p != 0){

									bool is_default=false;
									// search for case or default...
									key_w = is_keyword(aux_p);
									if(key_w == KEYWORD_CASE){

										TokenNode token_node;
										int idx_current_instruction=eval_data->current_function->instructions.size();
										std::vector<EvalInstruction *> case_instructions=std::vector<EvalInstruction *>{
											 token_node.instructions[0]		// load value to compare
											,new EvalInstruction(
													BYTE_CODE_JE
													,ZS_IDX_UNDEFINED
													,idx_current_instruction
											) // je instruction...
										};

										IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

										// capture constant value (should not be identifier in any case)
										aux_p=eval_symbol(
											eval_data
											,aux_p
											,line
											,&token_node
											,PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
										);

										if(token_node.token_type != TOKEN_TYPE_LITERAL){
											THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"'case' only accepts literals");
										}

										// insert a pair of instructions...
										ei_cases.insert(
												ei_cases.end(),
												case_instructions.begin(),
												case_instructions.end()
										);

									}else if(key_w == KEYWORD_DEFAULT){
										if(n_default > 0){
											THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"there's an already 'default' case");
										}

										is_default=true;
										n_default++;
										IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

									}else{
										THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected 'case' or 'default' keyword");
									}

									if(*aux_p!=':'){
										THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ':' ");
									}

									// ignore :
									IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line)

									aux_p=eval_recursive(
										eval_data
										,aux_p
										, line
										, scope_info
										, true // cancel eval when break or case is found...
									);

									if((is_keyword(aux_p) == Keyword::KEYWORD_BREAK)){ // it cuts current expression to link breaks...
										EvalInstruction *ei_break_jmp=new EvalInstruction(BYTE_CODE_JMP);
										eval_data->current_function->instructions.push_back(ei_break_jmp);
										ei_break_jmps.push_back(ei_break_jmp);
									}else if(is_default){
										THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"'default' case needs to have a 'break' at the end");
									}

									IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

								}

								unsigned idx_current_instruction=eval_data->current_function->instructions.size();

								if(*aux_p != '}'){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '}' switch");
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
								inc_jmp_codes(eval_data, idx_start_switch, idx_current_instruction, ei_cases.size());




								return aux_p+1;
							}
							else{
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '{' switch");
							}
					}
					else{
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '(' switch ");
					}
				}
			}
			return NULL;
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

			if(key_w != Keyword::KEYWORD_UNKNOWN){
				if(key_w == Keyword::KEYWORD_VAR || key_w == Keyword::KEYWORD_CONST){ // possible variable...
					bool is_constant = key_w == Keyword::KEYWORD_CONST;
					if(is_constant){ // scope_info will be global scope...
						scope_info = MAIN_SCOPE(eval_data);
					}

					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					while(*aux_p != ';' && *aux_p != 0){ // JE: added multivar feature.

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

						Keyword keyw = is_keyword(variable_name.c_str());

						if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Cannot use symbol name as reserverd symbol \"%s\"",eval_info_keywords[keyw].str);
						}

						aux_p=end_var;
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;


						if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
							allow_for_in=false;

							// register symbol...
							eval_data->current_function->script_function->registerVariable(
									scope_info
									,eval_data->current_parsing_file
									, line
									, variable_name
							);

							// if = then eval expression
							if(*aux_p == '='){ // only for variables (not class members)
								std::vector<EvalInstruction *>	 		constant_instructions;
								// try to evaluate expression...
								IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

								aux_p = eval_expression(
									eval_data
									,is_constant?(aux_p+1):start_var
									,start_line
									,scope_info
									,is_constant?&constant_instructions:&eval_data->current_function->instructions
								);

								if(is_constant){ // resolve constant_expression
									StackElement result;

									THROW_RUNTIME_ERROR("const expression not implemented");
									/*try{
										ConstantValue result=
										eval_data->zs->registerConstantValue(variable_name,result);
									}catch(std::exception & ex){
										THROW_RUNTIME_ERROR("error evaluating constant expression");
									}*/

									for(unsigned i=0; i < constant_instructions.size();i++){
										delete constant_instructions[i];
									}
								}

								line = start_line;
							}
							//---
							ZS_PRINT_DEBUG("registered symbol \"%s\" line %i ",variable_name.c_str(), line);
						}
						else{

							Keyword keyw = is_keyword(aux_p);
							if(keyw == Keyword::KEYWORD_IN){ // in keyword was detected (return to evalr)...
								if(!allow_for_in){
									THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"'in' keyword should be used within for (example: for ( var e in v) {...} )", *aux_p);
								}

								return aux_p; // return in
							}
							else{
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"unexpected char '%c'", *aux_p);
							}
						}

						// ignores ';' or ','
						if(*aux_p == ','){
							aux_p++;
						}
					}

					if(*aux_p != ';'){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected ';'");
					}

					return aux_p+1;
				}
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


				if(*aux_p != ';'){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,last_line_ok,"Expected ';'");
				}

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

				if(*aux_p != ';'){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,last_line_ok,"Expected ';'");
				}
			}

			return aux_p;

		}

		/*char *eval_keyword_default(EvalData *eval_data,const char *s, int & line, Scope *scope_info){
			char *aux_p=(char *)s;
			std::string value_to_eval;
			TokenNode token_node;

			//std::vector<EvalInstruction> *tokenCompiled = NULL;

			IGNORE_BLANKS(aux_p,eval_data,aux_p, line);

			Keyword keyw = is_keyword(aux_p);

			if(keyw == Keyword::KEYWORD_CASE){ // a keyword was detected...

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// get the symbol...
				if(*aux_p=='-'){
					// INSERT NEGATE OP.
				}
				aux_p++;

				aux_p=eval_symbol(
					eval_data
					,aux_p
					,line
					,&token_node
					,PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
				);
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p != ':'){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected  ':' ");
			}
			return aux_p+1;
		}*/

		char *eval_keyword(EvalData *eval_data,const char *s, int & line, Scope *scope_info, Keyword & keyw){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p= (char *)s;

			keyw=Keyword::KEYWORD_UNKNOWN;//,keyw2nd=Keyword::KEYWORD_UNKNOWN;

			IGNORE_BLANKS(aux_p,eval_data,aux_p, line);

			// check if condition...
			keyw = is_keyword(aux_p);

			if(keyw != Keyword::KEYWORD_UNKNOWN){ // a keyword was detected...

				switch(keyw){
				/*case KEYWORD_CASE:
				case KEYWORD_DEFAULT:
					return eval_keyword_default(eval_data,s,line,scope_info);
					break;*/
				case KEYWORD_FUNCTION:
					return  eval_keyword_function(eval_data,s,line,scope_info);
					break;
				case KEYWORD_CLASS:
					return  eval_keyword_class(eval_data,s,line,scope_info);
					break;
				case KEYWORD_NEW:
					return  eval_object_new(eval_data,s,line,scope_info,&eval_data->current_function->instructions);
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
