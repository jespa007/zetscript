namespace zetscript{
	namespace eval{

		void pushFunction(EvalData *eval_data,ScriptFunction *sf);
		void popFunction(EvalData *eval_data);
		char * evalBlock(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error, bool function_entry=false);
		char * evalRecursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info,  bool & error);
		Scope * evalNewScope(EvalData *eval_data, Scope *scope_parent,bool entry_function=false);
		void evalCheckScope(EvalData *eval_data, Scope *scope);
		//Scope * evalPopScope(EvalData *eval_data, Scope *current_scope);

		char * isClassMemberExtension(EvalData *eval_data,const char *s,int & line,ScriptClass **sc,std::string & member_symbol, bool & error){

			char *aux_p = (char *)s;
			std::string class_name;
			*sc=NULL;

			error = false;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check whwther the function is anonymous or not.
			if((aux_p=getIdentifierToken(eval_data,aux_p,class_name))==NULL){
				 writeError(eval_data->current_parsing_file,line ,"Expected class symbol");
				 return NULL;
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

				if((*sc=GET_SCRIPT_CLASS(eval_data,class_name)) != NULL){

					if((aux_p=getIdentifierToken(eval_data,aux_p+2,member_symbol))==NULL){
						 writeError(eval_data->current_parsing_file,line ,"Expected class member symbol");
						 return NULL;
					}
					return aux_p;
				}else{
					error=true;
				}
			}
			return NULL;
		}

		char * evalKeywordTypeDelete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			std::string symbol_value;
			KeywordType key_w;

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_DELETE){

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);


					if((aux_p=getIdentifierToken(eval_data,aux_p,symbol_value))==NULL){
						 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
						 return NULL;
					}

					 IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					 if(*aux_p != ';'){
						 writeError(eval_data->current_parsing_file,line,"Expected ;");
						 return NULL;
					 }

					return aux_p;
				}
			}
			return NULL;
		}

		char * evalKeywordTypeClass(EvalData *eval_data,const char *s,int & line, Scope *scope_info, bool & error){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			//Scope *class_scope_info=NULL;
			int class_line;
			std::string class_name;
			std::string base_class_name="";
			ScriptClass *sc;
			KeywordType key_w;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

				if(key_w == KeywordType::KEYWORD_TYPE_CLASS){

					if(scope_info->scope_parent!=NULL){
						writeError(eval_data->current_parsing_file,line,"class keyword is not allowed");
						return NULL;
					}

					IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[key_w].str),line);

					// check for symbol's name
					if((aux_p=getIdentifierToken(eval_data,aux_p,class_name))==NULL){
						 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
						 return NULL;
					}

					// try to register class...
					class_line = line;

					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(strncmp(aux_p, "extends",7)==0 ){ // extension class detected

						IGNORE_BLANKS(aux_p,eval_data,aux_p+7,line);

						if((aux_p=getIdentifierToken(eval_data,aux_p,base_class_name))==NULL){
							 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
							 return NULL;
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p, line);
					}


					// register class
					if((sc=eval_data->script_class_factory->registerClass(
							__FILE__
							, __LINE__
							, class_name
							,base_class_name
					))==NULL){
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
							key_w = isKeywordType(aux_p);
							if(key_w == KeywordType::KEYWORD_TYPE_UNKNOWN){ // only expects function name
								/*switch(key_w){
								default:
									writeError(eval_data->current_parsing_file,line,"Expected \"var\" or \"function\" keyword");
									return NULL;
									break;
								case KeywordType::KEYWORD_TYPE_FUNCTION:*/

									if((aux_p = evalKeywordTypeFunction(
											eval_data
											,aux_p
											, line
											,sc->symbol.scope // pass class scope
											,error
									)) == NULL){
										return NULL;
									}
								/*	break;
								case KeywordType::KEYWORD_TYPE_VAR:
									if((aux_p = evalKeywordTypeVar(
											eval_data
											,aux_p
											, line
											,class_scope_info
											,error
									)) == NULL){
										return NULL;
									}
									break;
								}*/
							}else{
								writeError(eval_data->current_parsing_file,line,"unexpected \"%s\"",eval_info_keywords[key_w].str);
								return NULL;
							}
							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
						}

						if(*aux_p != '}'){
							writeError(eval_data->current_parsing_file,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
							return NULL;
						}

						/*aux_p=IGNORE_BLANKS(eval_data,aux_p+1,line);

						if(*aux_p != ';'){
							writeError(eval_data->current_parsing_file,class_line ,"class \"%s\" not end with ;",class_name.c_str());
							return NULL;
						}*/

						return aux_p+1;

					}else{
						writeError(eval_data->current_parsing_file,line,"Expected '{'");
						return NULL;
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

		char * evalKeywordTypeFunction(
				EvalData *eval_data
				, const char *s
				, int & line
				, Scope *scope_info
				, bool & error
//				, ScriptClass *sc
			){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_var=NULL;
			KeywordType key_w;
			std::vector<FunctionParam> args;
			std::string conditional_str;
			ScriptClass *sc=NULL;
			ScriptFunction *sf=NULL;

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
				key_w = KeywordType::KEYWORD_TYPE_FUNCTION;
			}
			else{
				key_w = isKeywordType(aux_p);
				advance_chars=strlen(eval_info_keywords[key_w].str);
			}

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

				if(key_w == KeywordType::KEYWORD_TYPE_FUNCTION){

					// advance keyword...
					aux_p += advance_chars;
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					bool named_function = *aux_p!='(';

					if(named_function){ // is named function..

						if(is_class){
							sc=scope_info->script_class;
						}else if((end_var=isClassMemberExtension( // is function class extensions (example A::function1(){ return 0;} )
								eval_data
								,aux_p
								,line
								,&sc
								,function_name
								,error
						))!=NULL){ // check if particular case extension attribute class
							// current scope is changed by class scope...
							scope = sc->symbol.scope;
							//symbol_value = (char *)class_member.c_str();
						}else{
							if(is_class){ // get class from idx_scope

							}
						}

						if(error){ // isClassMemberExtension error
							return NULL;
						}

						if(end_var == NULL){ // global function
							// check whwther the function is anonymous with a previous arithmetic operation ....
							if((end_var=getIdentifierToken(
									eval_data
									,aux_p
									,function_name
							)) == NULL){
								 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
								 return NULL;
							}

						}

						// copy value
						if(!zs_strutils::copy_from_ptr_diff(function_name,aux_p,end_var)){
								return NULL;
						}

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
									writeError(eval_data->current_parsing_file,line,"Expected ',' ");
									return NULL;
								}
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
							}

							if(*aux_p == ')' || *aux_p == ','){
								writeError(eval_data->current_parsing_file,line,"Expected arg");
								return NULL;
							}

							// capture line where argument is...
							arg_info.line=line;

							//int m_start_arg=line;
							if((end_var=getIdentifierToken(
									 eval_data
									,aux_p
									,arg_value
							)) == NULL){
								 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
								 return NULL;
							}

							// copy value
							if(!zs_strutils::copy_from_ptr_diff(arg_value,aux_p,end_var)){
									return NULL;
							}


							// check if repeats...
							for(unsigned k = 0; k < args.size(); k++){
								if(args[k].arg_name == arg_value){
									writeError(eval_data->current_parsing_file,line,"Repeated argument '%s' argument ",arg_value.c_str());
									return NULL;
								}
							}

							// check whether parameter name's matches with some global variable...
							if((irv=scope->getSymbol(arg_value.c_str())) != NULL){
								writeError(eval_data->current_parsing_file,line,"Ambiguous symbol argument \"%s\" name with var defined at %i", arg_value.c_str(), -1);
								return NULL;
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
							writeError(eval_data->current_parsing_file,line,"Expected '{'");
							return NULL;
						}

						// register function ...
						if(!named_function){ // register named function...
							function_name="_afun_"+zs_strutils::int_to_str(n_anonymous_function++);
						}
						//--- OP
						if(sc!=NULL){ // register as variable member...
							sf=sc->registerFunctionMember(
									 eval_data->current_parsing_file
									,line
									,function_name
									,args
							);
						}
						else{ // register as local variable in the function...
							sf=eval_data->current_function->script_function->addFunction(
									scope_info
									, eval_data->current_parsing_file
									, line
									, function_name
									,args
							);
						}

						pushFunction(eval_data,sf);

						// ok let's go to body..
						aux_p = evalBlock(
								eval_data
								,aux_p
								,line
								,scope
								,error
								,true);

						popFunction(eval_data);
					}
					else{
						writeError(eval_data->current_parsing_file,line," Expected '('");
					}
				}else{
					writeError(eval_data->current_parsing_file,line,"Expected operator or function operator");
				}
			}
			return aux_p;
		}

		char *  evalKeywordTypeReturn(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			KeywordType key_w;
			std::string s_aux;

			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

				if(key_w == KeywordType::KEYWORD_TYPE_RETURN){ // possible variable...
					//PASTNode child_node=NULL;
					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					if((aux_p = evalExpression(
							eval_data
							,aux_p
							, line
							, scope_info
							,&eval_data->current_function->instructions
					))!= NULL){

						if(*aux_p!=';'){
							writeError(eval_data->current_parsing_file,line,"Expected ';'");
							return NULL;
						}

						eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_RET));

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						return aux_p;
					}
				}
			}
			return NULL;
		}

		char * evalKeywordTypeWhile(EvalData *eval_data,const char *s,int & line, Scope *scope_info,  bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;
			std::string start_symbol;
			KeywordType key_w;
			std::string conditional_str;
			error = false;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_WHILE){

					aux_p += strlen(eval_info_keywords[key_w].str);
					// evaluate conditional line ...
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					if(*aux_p == '('){

						if((end_expr = evalExpression(
								eval_data
								,aux_p+1
								,line
								,scope_info
								,&eval_data->current_function->instructions
						)) != NULL){

							if(*end_expr != ')'){
								writeError(eval_data->current_parsing_file,line,"Expected ')'");
								return NULL;
							}

							if(!zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr)){
								return NULL;
							}

							IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);
							if(*aux_p != '{'){
								writeError(eval_data->current_parsing_file,line,"Expected while-block open block ('{') ");
								return NULL;
							}
							if((aux_p=evalBlock(
									eval_data
									,aux_p
									,line
									,scope_info
									,error
							))!= NULL){
								if(!error){
									return aux_p;
								}
							}
						}else{
							writeError(eval_data->current_parsing_file,line,"Expected ')' while ");
							return NULL;
						}

					}else{
						writeError(eval_data->current_parsing_file,line,"Expected '(' while ");
						return NULL;
					}
				}
			}
			return NULL;
		}

		char * evalKeywordTypeDoWhile(EvalData *eval_data,const char *s,int & line, Scope *scope_info,  bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;
			std::string start_symbol;
			KeywordType key_w;
			std::string conditional_str;
			error = false;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_DO_WHILE){


					aux_p += strlen(eval_info_keywords[key_w].str);

					//1st evaluate body ..
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
					if(*aux_p != '{'){
						writeError(eval_data->current_parsing_file,line,"Expected open block ('{') in do-while expression");
						return NULL;
					}
					if((aux_p=evalBlock(
							eval_data
							,aux_p
							,line
							,scope_info
							,error
					))!= NULL){
						if(!error){

							// Finally evaluate conditional line ...
							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

							// check for keyword ...
							key_w = isKeywordType(aux_p);

							if(key_w!=KEYWORD_TYPE_WHILE){
								writeError(eval_data->current_parsing_file,line,"expected while keyword");
								return NULL;
							}

							aux_p += strlen(eval_info_keywords[key_w].str);

							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

							if(*aux_p == '('){

								if((end_expr = evalExpression(
										eval_data
										,aux_p+1
										,line
										,scope_info
										,&eval_data->current_function->instructions
								)) != NULL){
									if(*end_expr != ')'){
										writeError(eval_data->current_parsing_file,line,"Expected ')'");
										return NULL;
									}
									if(!zs_strutils::copy_from_ptr_diff(start_symbol,aux_p+1, end_expr)){
										return NULL;
									}
								}else{
									writeError(eval_data->current_parsing_file,line,"Expected ')' do-while expression");
									return NULL;
								}

							}else{
								writeError(eval_data->current_parsing_file,line,"Expected '(' do-while expression");
								return NULL;
							}

							return end_expr+1;
						}
					}
				}
			}
			return NULL;
		}

		char * evalKeywordTypeIf(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr;//,*start_symbol;
			int dl=-1;
			KeywordType key_w;
			//std::string conditional_str;
			error = false;
			std::vector<EvalInstruction *> ei_jmps;
			EvalInstruction *ei_aux;
			//EvalInstruction ei_jmp_else_if;
			//int conditional_line=0;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_IF){

					do{

						aux_p += strlen(eval_info_keywords[key_w].str);
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						if(*aux_p != '('){
							writeError(eval_data->current_parsing_file,line,"Expected '(' if");
							return NULL;
						}

						// eval conditional expression
						if((end_expr = evalExpression(
								eval_data
								,aux_p+1
								,line
								,scope_info
								,&eval_data->current_function->instructions
						)) == NULL){
							writeError(eval_data->current_parsing_file,line,"Expected ')' if ");
							return NULL;
						}

						if(*end_expr != ')'){
							writeError(eval_data->current_parsing_file,line,"Expected ')'");
							return NULL;
						}


						// insert instruction if evaluated expression
						eval_data->current_function->instructions.push_back(ei_aux=new EvalInstruction(BYTE_CODE_JNT));
						ei_jmps.push_back(ei_aux);

						/*if(IGNORE_BLANKS(eval_data,aux_p+1,dl)==end_expr){
							writeError(eval_data->current_parsing_file,line,"no conditional expression");
							return NULL;
						}

						if(!zs_strutils::copy_from_ptr_diff(conditional_str,aux_p+1, end_expr)){
							return NULL;
						}*/

						IGNORE_BLANKS(aux_p,eval_data,end_expr+1,line);
						if(*aux_p != '{'){
							writeError(eval_data->current_parsing_file,line,"Expected if-block open block ('{')");
							return NULL;
						}

						if((aux_p=evalBlock(eval_data,aux_p
								,line
								,scope_info
								,error
								))== NULL){
							return NULL;
						}

						if(error){
							return NULL;
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						bool else_key = false;
						if((key_w = isKeywordType(aux_p)) != KeywordType::KEYWORD_TYPE_UNKNOWN){
							else_key = (key_w == KeywordType::KEYWORD_TYPE_ELSE);
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
							if((key_w = isKeywordType(aux_p)) != KeywordType::KEYWORD_TYPE_UNKNOWN){
								if_key = (key_w == KeywordType::KEYWORD_TYPE_IF);
							}

							if(!if_key){ // not if, only else

								if(*aux_p != '{'){
									writeError(eval_data->current_parsing_file,line,"Expected else-block open block ('{')");
									return NULL;
								}

								// eval else block
								if((aux_p=evalBlock(
										eval_data
										,aux_p
										,line
										,scope_info
										,error
								))!= NULL){
										if(!error){
											return aux_p;
										}
										else{
											return NULL;
										}
								}else{
									return NULL;
								}
							} // else keep up parsing if nodes case ...
						}else{ // end if expression

							// update all collected jump/jnt to current instruction...
							for(unsigned i=0; i < ei_jmps.size(); i++){
								// insert instruction if evaluated expression
								ei_jmps[i]->vm_instruction.value_op2=eval_data->current_function->instructions.size();
							}

							// concatenate collected jmps
							eval_data->current_function->jmp_instructions.insert(
									   eval_data->current_function->jmp_instructions.end()
									,  ei_jmps.begin()
									, ei_jmps.end()
							);

							return aux_p;
						}
					}while(true); // loop
				}
			}
			return NULL;
		}

		char * evalKeywordTypeFor(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			KeywordType key_w;
			error=false;
			std::string eval_for;


			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_FOR){

					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p == '('){ // ready ...

						// save scope pointer ...
						Scope *new_scope =evalNewScope(eval_data,scope_info); // push current scope


						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

						if(*aux_p != ';'){ // there's some var Init...
							// Init node ...
							KeywordType key_w = isKeywordType(aux_p);

							if(key_w == KEYWORD_TYPE_VAR){
								if((aux_p = evalKeywordTypeVar(
										eval_data
										,aux_p
										,line
										,new_scope
										,error
								))==NULL){
									return NULL;
								}
								aux_p = aux_p - 1; // redirect aux_p to ';'
							}
							else{

								writeError(eval_data->current_parsing_file,line,"Expected 'var' keyword");
								return NULL;
							}
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

						key_w = isKeywordType(aux_p);
						if(key_w == KeywordType::KEYWORD_TYPE_IN){

							//PASTNode node_for_in_right_op_expression=NULL;

							IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_info_keywords[KeywordType::KEYWORD_TYPE_IN].str),line);


							if((aux_p = evalExpression(
									eval_data
									,(const char *)aux_p
									,line
									,new_scope
									,&eval_data->current_function->instructions
							)) == NULL){
								return NULL;
							}
						}
						else{ // expects conditional and post (i.e for(;;) )
							if(*aux_p != ';'){
								writeError(eval_data->current_parsing_file,line,"Expected ';'");
								return NULL;

							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*aux_p != ';'){ // conditional...
								char * end_p=NULL;
								IGNORE_BLANKS(end_p,eval_data,aux_p+1,line);

								if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

									if((aux_p = evalExpression(
											eval_data
											,(const char *)aux_p
											,line
											,new_scope
											,&eval_data->current_function->instructions
									)) == NULL){
										return NULL;
									}
								}
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

							if(*aux_p != ';'){
								writeError(eval_data->current_parsing_file,line,"Expected ';'");
								return NULL;

							}
							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*aux_p != ')' ){ // finally do post op...

								if(*aux_p == ',' ){
									writeError(eval_data->current_parsing_file,line,"Unexpected ) ");
									return NULL;
								}

								do{
									if((aux_p = evalExpression(
											eval_data
											,aux_p
											,line
											,new_scope
											,&eval_data->current_function->instructions
									))==NULL){
										return NULL;
									}

									if(*aux_p == ',' ){
										IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
									}else{
										if(*aux_p != ')' ){
											writeError(eval_data->current_parsing_file,line,"Expected ')'");
											return NULL;
										}
									}

								}while(*aux_p != ')' && *aux_p != 0);
							}
						}

						if(*aux_p != ')'){
							writeError(eval_data->current_parsing_file,line,"Expected ')'");
							return NULL;
						}

						IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
						if(*aux_p != '{'){
							writeError(eval_data->current_parsing_file,line,"Expected '{' for-block");
							return NULL;
						}

						// eval block ...
						if((aux_p=evalBlock(
								eval_data
								,aux_p
								,line
								,new_scope
								,error
						))!= NULL){ // true: We treat declared variables into for as another scope.
							if(!error){
								evalCheckScope(eval_data,new_scope);
								return aux_p;
							}
						}
					}else{
						writeError(eval_data->current_parsing_file,line,"Expected '(' for");
						return NULL;
					}
				}
			}
			return NULL;
		}

		char * evalKeywordTypeSwitch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info,  bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			Scope *scope_case=NULL;
			std::string val;
			KeywordType key_w;//,key_w2;

			error=false;
			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_SWITCH){

					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					if(*aux_p == '('){
							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
							// evaluate switch condition expression ...
							if((aux_p = evalExpression(
								eval_data
								,aux_p
								,line
								,scope_info
								,&eval_data->current_function->instructions
							))==NULL){
								return NULL;
							}

							if(*aux_p != ')'){
								writeError(eval_data->current_parsing_file,line,"Expected ')' switch");
								error = true;
								return NULL;
							}

							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*aux_p == '{'){

								aux_p++;

								if((aux_p=evalRecursive(
									eval_data
									,aux_p
									, line
									, scope_info
									, error))==NULL
								){
									return NULL;
								}

								IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

								if(*aux_p != '}'){
									writeError(eval_data->current_parsing_file,line,"Expected '}' switch");
									return NULL;
								}

								return aux_p+1;
							}
							else{
								writeError(eval_data->current_parsing_file,line,"Expected '{' switch");
								return NULL;
							}
					}
					else{
						writeError(eval_data->current_parsing_file,line,"Expected '(' switch ");
						return NULL;
					}
				}
			}
			return NULL;
		}

		char * evalKeywordTypeVar(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error){

			// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass

			char *aux_p = (char *)s;
			KeywordType key_w;
			char *start_var,*end_var;
			std::string s_aux,variable_name;
			bool end=false;
			bool allow_for_in=true;
			int start_line=0;

			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_VAR){ // possible variable...

					aux_p += strlen(eval_info_keywords[key_w].str);
					IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

					while(*aux_p != ';' && *aux_p != 0 && !end){ // JE: added multivar feature.

						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
						start_var=aux_p;
						start_line = line;
						//sc=NULL;

						/*if(sc_come_from != NULL){ // it comes from class declaration itself
							sc=sc_come_from;
						}
						else{ // check if type var ClasS::v1 or v1
							if((end_var=isClassMemberExtension(
									eval_data,
									aux_p,
									line,
									&sc,
									variable_name,
									error
							))==NULL){ // causal variable
								if(error){
									return NULL;
								}
								else{*/ // get normal name...

									line = start_line;

									// check whwther the function is anonymous with a previous arithmetic operation ....
									if((end_var=getIdentifierToken(
											eval_data,
											aux_p,
											variable_name
									))==NULL){
										writeError(eval_data->current_parsing_file,line,"Expected symbol");
										return NULL;
									}
								//}
							//}
						//}

						KeywordType keyw = isKeywordType(variable_name.c_str());

						if(keyw != KeywordType::KEYWORD_TYPE_UNKNOWN){ // a keyword was detected...
							writeError(eval_data->current_parsing_file,line,"Cannot use symbol name as reserverd symbol \"%s\"",eval_info_keywords[keyw].str);
							return NULL;
						}

						aux_p=end_var;
						IGNORE_BLANKS(aux_p,eval_data,aux_p,line);
						//}
						bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;
						/*if(sc!=NULL && *aux_p == '='){
							writeError(eval_data->current_parsing_file,line,"Variable member is not assignable on its declaration. Should be initialized within constructor.");
							return NULL;
						}*/

						if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
							allow_for_in=false;
							if(*aux_p == '='){ // only for variables (not class members)
								// try to evaluate expression...
								IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

								if((aux_p = evalExpression(
									eval_data
									,start_var
									,start_line
									,scope_info
									,&eval_data->current_function->instructions
								)) == NULL){
									return NULL;
								}
								line = start_line;
							}
							 // define as many vars is declared within ','

							//--- OP
							/*if(sc!=NULL){ // register as variable member...
								sc->registerVariable(eval_data->current_parsing_file, line, variable_name);
							}
							else{ */// register as local variable in the function...
								eval_data->current_function->script_function->addSymbol(
										scope_info
										,eval_data->current_parsing_file
										, line
										, variable_name
								);
							//}
							//---
							ZS_PRINT_DEBUG("registered symbol \"%s\" line %i ",variable_name.c_str(), line);
						}
						else{

							KeywordType keyw = isKeywordType(variable_name.c_str());
							if(keyw == KeywordType::KEYWORD_TYPE_IN){ // in keyword was detected (return to evalr)...
								if(!allow_for_in){
									writeError(eval_data->current_parsing_file,line,"'in' keyword should be used with an uninitialized variable (example: for ( var e in v) {...} )", *aux_p);
									return NULL;
								}
								end=true;
							}
							else{
								error=true;
								writeError(eval_data->current_parsing_file,line,"unexpected '%c'", *aux_p);
								return NULL;
							}
						}

						// ignores ';' or ','
						if(*aux_p == ',')
							aux_p++;
					}

					if(*aux_p == ';'){
						aux_p++;
					}
					else{
						writeError(eval_data->current_parsing_file,line,"Expected ';'");
						return NULL;
					}
					return aux_p;
				}
			}
			return NULL;
		}

		char *evalKeywordTypeBreak(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool & error){
			// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
			char *aux_p=(char *)s;

			fprintf(stderr,"find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n");

			{ // ok break is valid in current scope...

				writeError(eval_data->current_parsing_file,line,"\"break\" allowed within loop or case-switch statements");
				error = true;
				return NULL;
			}

			if(*aux_p != ';'){
				writeError(eval_data->current_parsing_file,line,"expected ';'");
				error = true;
				return NULL;
			}
		}

		char *evalKeywordTypeContinue(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool & error){
			// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
			char *aux_p=(char*)s;
			{ // ok break is valid in current scope...

				writeError(eval_data->current_parsing_file,line,"\"continue\" allowed within loop or case-switch statements");
				error = true;
				return NULL;
			}

			if(*aux_p != ';'){
				writeError(eval_data->current_parsing_file,line,"expected ';'");
				error = true;
				return NULL;
			}
		}

		char *evalKeywordTypeDefault(EvalData *eval_data,const char *s, int & line, Scope *scope_info,  bool & error){
			char *aux_p=(char *)s;
			std::string value_to_eval;
			TokenNode token_node;

			//std::vector<EvalInstruction> *tokenCompiled = NULL;

			IGNORE_BLANKS(aux_p,eval_data,aux_p, line);

			KeywordType keyw = isKeywordType(aux_p);

			if(keyw == KeywordType::KEYWORD_TYPE_CASE){ // a keyword was detected...

				IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

				// get the symbol...
				if(*aux_p=='-'){
					// INSERT NEGATE OP.
				}
				aux_p++;

				aux_p=evalSymbol(
					eval_data
					,aux_p
					,line
					,&token_node
				);

				if(aux_p==NULL){ return NULL;}
			}

			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			if(*aux_p != ':'){
				writeError(eval_data->current_parsing_file,line,"Expected  ':' ");
				return NULL;
			}
			return aux_p+1;
		}

		char *evalKeywordType(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p= (char *)s;

			KeywordType keyw=KeywordType::KEYWORD_TYPE_UNKNOWN;//,keyw2nd=KeywordType::KEYWORD_TYPE_UNKNOWN;

			IGNORE_BLANKS(aux_p,eval_data,aux_p, line);

			// check if condition...
			keyw = isKeywordType(aux_p);

			if(keyw != KeywordType::KEYWORD_TYPE_UNKNOWN){ // a keyword was detected...

				switch(keyw){
				case KEYWORD_TYPE_CASE:
				case KEYWORD_TYPE_DEFAULT:
					return evalKeywordTypeDefault(eval_data,s,line,scope_info,error);

				case KEYWORD_TYPE_FUNCTION:
					if((aux_p = evalKeywordTypeFunction(eval_data,s,line,scope_info,error)) != NULL){
						return aux_p;
					}
					error = true;
					return NULL;
				case KEYWORD_TYPE_CLASS:
					if((aux_p = evalKeywordTypeClass(eval_data,s,line,scope_info,error)) != NULL){
						return aux_p;
					}
					error = true;
					return NULL;
				case KEYWORD_TYPE_NEW:
					if((aux_p = evalNewObject(eval_data,s,line,scope_info,&eval_data->current_function->instructions)) != NULL){
						return aux_p;
					}
					error = true;
					return NULL;
				default:
					if(eval_info_keywords[keyw].eval_fun != NULL){
						return  (*eval_info_keywords[keyw].eval_fun)(eval_data,s,line,scope_info,error);
					}
					writeError(eval_data->current_parsing_file,line,"Not implemented");
					error = true;
					return NULL;
				}
			}
			return NULL;
		}

	}
}
