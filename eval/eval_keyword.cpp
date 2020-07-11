namespace zetscript{
	namespace eval{

		char * evalBlock(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * evalRecursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info,  bool & error);

		char * isClassMember(EvalData *eval_data,const char *s,int & line,ScriptClass **sc,std::string & member_symbol, bool & error){

			char *aux_p = (char *)s;
			std::string class_name;
			*sc=NULL;

			error = false;

			aux_p=ignoreBlanks(aux_p,line);

			// check whwther the function is anonymous or not.
			if((aux_p=getIdentifierToken(eval_data,aux_p,class_name))==NULL){
				 writeError(eval_data->current_parsing_file,line ,"Expected class symbol");
				 return NULL;
			}

			aux_p=ignoreBlanks(aux_p,line);

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

			aux_p=ignoreBlanks(aux_p,line);

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_DELETE){

					aux_p=ignoreBlanks(aux_p+strlen(eval_info_keywords[key_w].str),line);


					if((aux_p=getIdentifierToken(eval_data,aux_p,symbol_value))==NULL){
						 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
						 return NULL;
					}

					 aux_p=ignoreBlanks(aux_p,line);

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
			Scope *class_scope_info=NULL;
			int class_line;
			std::string class_name;
			std::string base_class_name="";
			ScriptClass *sc;
			KeywordType key_w;
			aux_p=ignoreBlanks(aux_p,line);

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

				if(key_w == KeywordType::KEYWORD_TYPE_CLASS){

					if(scope_info->getIdxScopeParent()!=ZS_UNDEFINED_IDX){
						writeError(eval_data->current_parsing_file,line,"class keyword is not allowed");
						return NULL;
					}

					aux_p=ignoreBlanks(aux_p+strlen(eval_info_keywords[key_w].str),line);

					// check for symbol's name
					if((aux_p=getIdentifierToken(eval_data,aux_p,class_name))==NULL){
						 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
						 return NULL;
					}

					// try to register class...
					class_line = line;


					aux_p=ignoreBlanks(aux_p,line);

					if(*aux_p == ':' ){

						aux_p=ignoreBlanks(aux_p+1,line);

						if((aux_p=getIdentifierToken(eval_data,aux_p,base_class_name))==NULL){
							 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
							 return NULL;
						}

						aux_p=ignoreBlanks(aux_p, line);
					}


					// register class
					if((sc=eval_data->script_class_factory->registerClass(__FILE__, __LINE__, class_name,base_class_name))==NULL){
						return NULL;
					}

					ZS_PRINT_DEBUG("registered class \"%s\" line %i ",class_name.c_str(), class_line);

					if(*aux_p == '{' ){

						aux_p=ignoreBlanks(aux_p+1,line);

						// TODO: Register class and baseof
						// register info class ...
						// check for named functions or vars...
						while(*aux_p != '}' && *aux_p != 0){

							// 1st. check whether eval a keyword...
							key_w = isKeywordType(aux_p);
							if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
								switch(key_w){
								default:
									writeError(eval_data->current_parsing_file,line,"Expected \"var\" or \"function\" keyword");
									return NULL;
									break;
								case KeywordType::KEYWORD_TYPE_FUNCTION:

									if((aux_p = evalKeywordTypeFunction(
											eval_data
											,aux_p
											, line
											,class_scope_info
											,error
									)) == NULL){
										return NULL;
									}
									break;
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
								}
							}else{
								writeError(eval_data->current_parsing_file,line,"Expected \"var\" or \"function\" keyword");
								return NULL;
							}
							aux_p=ignoreBlanks(aux_p,line);
						}

						if(*aux_p != '}'){
							writeError(eval_data->current_parsing_file,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
							return NULL;
						}

						aux_p=ignoreBlanks(aux_p+1,line);

						if(*aux_p != ';'){
							writeError(eval_data->current_parsing_file,class_line ,"class \"%s\" not end with ;",class_name.c_str());
							return NULL;
						}

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

		char * evalKeywordTypeFunction(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *symbol_value=NULL,*end_var=NULL;
			KeywordType key_w;
			std::vector<ParamArgInfo> arg;
			std::string conditional_str;
			ScriptClass *sc=NULL;

			Symbol * irv=NULL;
			std::string str_name,arg_value;
			static int n_anonymous_function=0;
			//std::string class_member,class_name,
			std::string function_name="";


			int idx_scope=ZS_UNDEFINED_IDX;
			Scope *body_scope=NULL;

			idx_scope=scope_info->idx_scope;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

				if(key_w == KeywordType::KEYWORD_TYPE_FUNCTION){

					// advance keyword...
					aux_p += strlen(eval_info_keywords[key_w].str);
					aux_p=ignoreBlanks(aux_p,line);

					bool named_function = *aux_p!='(';

					if(named_function){ // is named function..

						if((end_var=isClassMember(
								eval_data
								,aux_p
								,line
								,&sc
								,function_name
								,error
						))!=NULL){ // check if particular case extension attribute class
							idx_scope = sc->symbol_info.symbol->idx_scope;
							//symbol_value = (char *)class_member.c_str();
						}
						else{
							if(error){
								return NULL;
							}
							else{ // get normal name...

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
						}
						aux_p=end_var;
						aux_p=ignoreBlanks(aux_p,line);
					}
					else{ //function node
						// TODO manage function object
						printf("TODO manage function object\n");
					}

					// eval function args...
					if(*aux_p == '('){ // push arguments...

						aux_p++;
						aux_p=ignoreBlanks(aux_p,line);

						// grab words separated by ,
						while(*aux_p != 0 && *aux_p != ')'){
							aux_p=ignoreBlanks(aux_p,line);

							if(arg.size()>0){
								if(*aux_p != ','){
									writeError(eval_data->current_parsing_file,line,"Expected ',' ");
									return NULL;
								}
								aux_p=ignoreBlanks(aux_p+1,line);
							}

							if(*aux_p == ')' || *aux_p == ','){
								writeError(eval_data->current_parsing_file,line,"Expected arg");
								return NULL;
							}

							//int m_start_arg=line;
							if((end_var=getIdentifierToken(
									 eval_data
									,aux_p
									,arg_value
							)) == NULL){
								 writeError(eval_data->current_parsing_file,line ,"Expected symbol");
								 return NULL;
							}

							// check if repeats...
							for(unsigned k = 0; k < arg.size(); k++){
								if(arg[k].arg_name == arg_value){
									writeError(eval_data->current_parsing_file,line,"Repeated argument '%s' argument ",arg_value.c_str());
									return NULL;
								}
							}

							// check whether parameter name's matches with some global variable...
							if((irv=body_scope->getSymbol(symbol_value)) != NULL){
								writeError(eval_data->current_parsing_file,line,"Ambiguous symbol argument \"%s\" name with var defined at %i", symbol_value, -1);
								return NULL;
							}
								// ok register symbol into the object function ...
							ParamArgInfo arg_info;
							arg_info.arg_name=symbol_value;
							arg.push_back(arg_info);

							aux_p=end_var;
							aux_p=ignoreBlanks(aux_p,line);

							if(*aux_p != ')'){

							}
						}

						aux_p++;
						aux_p=ignoreBlanks(aux_p,line);

						if(*aux_p != '{'){
							writeError(eval_data->current_parsing_file,line,"Expected '{'");
							return NULL;
						}

						// ok let's go to body..
						if((aux_p = evalBlock(
								eval_data,
								aux_p,
								line,
								GET_SCOPE(eval_data,idx_scope),
								error
						)) != NULL){

							if(!error){
								// register function symbol...
								//int n_params=arg.size();

								/*if(named_function){ // register named function...
									if((irv=GET_SCOPE(idx_scope)->getSymbol(function_name,n_params)) != NULL){

										writeError(current_parsing_file,line,"Function name \"%s\" is already defined with same args at %s:%i", function_name.c_str(),irv->file.c_str(),irv->line);
										return NULL;
									}

									if((irv=GET_SCOPE(idx_scope)->registerSymbol(current_parsing_file,line,function_name,n_params))==NULL){
										return NULL;
									}

								}else{ // register anonymouse function at global scope...
									irv=GET_SCOPE(IDX_GLOBAL_SCOPE)->registerAnonymouseFunction(current_parsing_file,line,n_params);
								}*/

								if(!named_function){ // register named function...
									function_name="_afun_"+zs_strutils::intToString(n_anonymous_function++);
								}
								//--- OP
								if(sc!=NULL){ // register as variable member...
									sc->registerFunction(
											eval_data->current_parsing_file
											, line
											, function_name,arg
									);
								}
								else{ // register as local variable in the function...
									eval_data->evaluated_function_current->script_function->registerFunction(
											eval_data->current_parsing_file
											, line
											, function_name
											,arg
									);
								}

								//---


								GET_SCOPE(eval_data,idx_scope)->popScope();
								return aux_p;
							}
						}
					}
					else{
						writeError(eval_data->current_parsing_file,line," Expected '('");
					}
				}else{
					writeError(eval_data->current_parsing_file,line,"Expected operator or function operator");
				}
			}
			return NULL;
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
					aux_p=ignoreBlanks(aux_p,line);
					if((aux_p = evalExpression(
							eval_data
							,aux_p
							, line
							, scope_info
							,&eval_data->evaluated_function_current->evaluated_instruction
					))!= NULL){

						if(*aux_p!=';'){
							writeError(eval_data->current_parsing_file,line,"Expected ';'");
							return NULL;
						}
						aux_p=ignoreBlanks(aux_p+1,line);
						return aux_p;
					}
				}
			}
			return NULL;
		}

		char * evalKeywordTypeWhile(EvalData *eval_data,const char *s,int & line, Scope *scope_info,  bool & error){

			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			char *end_expr,*start_symbol;
			KeywordType key_w;
			Scope *_currentScope=NULL;

			//PASTNode conditional_expression=NULL, while_node=NULL;
			std::string conditional_str;
			error = false;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_WHILE){

					aux_p += strlen(eval_info_keywords[key_w].str);
					// evaluate conditional line ...
					aux_p=ignoreBlanks(aux_p,line);
					if(*aux_p == '('){

						if((end_expr = evalExpression(
								eval_data
								,aux_p+1
								,line
								,_currentScope
								,&eval_data->evaluated_function_current->evaluated_instruction
						)) != NULL){

							if(*end_expr != ')'){
								writeError(eval_data->current_parsing_file,line,"Expected ')'");
								return NULL;
							}

							if((start_symbol = zs_strutils::copyFromPointerDiff(aux_p+1, end_expr))==NULL){
								return NULL;
							}

							aux_p=ignoreBlanks(end_expr+1,line);
							if(*aux_p != '{'){
								writeError(eval_data->current_parsing_file,line,"Expected while-block open block ('{') ");
								return NULL;
							}
							if((aux_p=evalBlock(
									eval_data
									,aux_p
									,line
									,_currentScope
									,error
							))!= NULL){
								if(!error){
									scope_info->popScope();
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
			char *end_expr,*start_symbol;
			KeywordType key_w;
			Scope *_currentScope=NULL;
			std::string conditional_str;
			error = false;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_DO_WHILE){


					aux_p += strlen(eval_info_keywords[key_w].str);

					//1st evaluate body ..
					aux_p=ignoreBlanks(aux_p,line);
					if(*aux_p != '{'){
						writeError(eval_data->current_parsing_file,line,"Expected open block ('{') in do-while expression");
						return NULL;
					}
					if((aux_p=evalBlock(
							eval_data
							,aux_p
							,line
							,_currentScope
							,error
					))!= NULL){
						if(!error){

							// Finally evaluate conditional line ...
							aux_p=ignoreBlanks(aux_p,line);

							// check for keyword ...
							key_w = isKeywordType(aux_p);

							if(key_w!=KEYWORD_TYPE_WHILE){
								writeError(eval_data->current_parsing_file,line,"expected while keyword");
								return NULL;
							}

							aux_p += strlen(eval_info_keywords[key_w].str);

							aux_p=ignoreBlanks(aux_p,line);

							if(*aux_p == '('){

								if((end_expr = evalExpression(
										eval_data
										,aux_p+1
										,line
										,_currentScope
										,&eval_data->evaluated_function_current->evaluated_instruction
								)) != NULL){
									if(*end_expr != ')'){
										writeError(eval_data->current_parsing_file,line,"Expected ')'");
										return NULL;
									}
									if((start_symbol = zs_strutils::copyFromPointerDiff(aux_p+1, end_expr))==NULL){
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

							scope_info->popScope();
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
			char *end_expr,*start_symbol;
			int dl=-1;
			KeywordType key_w;
			std::string conditional_str;
			error = false;
			//int conditional_line=0;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_IF){

					do{

						aux_p += strlen(eval_info_keywords[key_w].str);
						aux_p=ignoreBlanks(aux_p,line);

						if(*aux_p != '('){
							writeError(eval_data->current_parsing_file,line,"Expected '(' if");
							return NULL;
						}

						if((end_expr = evalExpression(
								eval_data
								,aux_p+1
								,line
								,scope_info
								,&eval_data->evaluated_function_current->evaluated_instruction
						)) == NULL){
							writeError(eval_data->current_parsing_file,line,"Expected ')' if ");
							return NULL;
						}

						if(*end_expr != ')'){
							writeError(eval_data->current_parsing_file,line,"Expected ')'");
							return NULL;
						}

						if(ignoreBlanks(aux_p+1,dl)==end_expr){
							writeError(eval_data->current_parsing_file,line,"no conditional expression");
							return NULL;
						}

						if((start_symbol = zs_strutils::copyFromPointerDiff(aux_p+1, end_expr))==NULL){
							return NULL;
						}

						conditional_str=start_symbol;

						aux_p=ignoreBlanks(end_expr+1,line);
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

						aux_p=ignoreBlanks(aux_p,line);

						bool else_key = false;
						if((key_w = isKeywordType(aux_p)) != KeywordType::KEYWORD_TYPE_UNKNOWN){
							else_key = (key_w == KeywordType::KEYWORD_TYPE_ELSE);
						}

						if(else_key){
							aux_p += strlen(eval_info_keywords[key_w].str);

							if(*aux_p != '{'){
								aux_p++;
							}

							aux_p=ignoreBlanks(aux_p,line);

							bool if_key = false;
							if((key_w = isKeywordType(aux_p)) != KeywordType::KEYWORD_TYPE_UNKNOWN){
								if_key = (key_w == KeywordType::KEYWORD_TYPE_IF);
							}

							if(!if_key){

								if(*aux_p != '{'){
									writeError(eval_data->current_parsing_file,line,"Expected else-block open block ('{')");
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
										else{
											return NULL;
										}
								}else{
									return NULL;
								}
							} // else keep up parsing if nodes case ...
						}else{
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
			Scope *_currentScope=NULL;

			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_FOR){

					aux_p += strlen(eval_info_keywords[key_w].str);
					aux_p=ignoreBlanks(aux_p,line);

					if(*aux_p == '('){ // ready ...

						// save scope pointer ...
						_currentScope =scope_info->pushScope(); // push current scope


						aux_p=ignoreBlanks(aux_p+1,line);

						if(*aux_p != ';'){ // there's some var Init...
							// Init node ...
							KeywordType key_w = isKeywordType(aux_p);

							if(key_w == KEYWORD_TYPE_VAR){
								if((aux_p = evalKeywordTypeVar(
										eval_data
										,aux_p
										,line
										,
										_currentScope
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

						aux_p=ignoreBlanks(aux_p,line);

						key_w = isKeywordType(aux_p);
						if(key_w == KeywordType::KEYWORD_TYPE_IN){

							//PASTNode node_for_in_right_op_expression=NULL;

							aux_p=ignoreBlanks(aux_p+strlen(eval_info_keywords[KeywordType::KEYWORD_TYPE_IN].str),line);


							if((aux_p = evalExpression(
									eval_data
									,(const char *)aux_p
									,line
									,_currentScope
									,&eval_data->evaluated_function_current->evaluated_instruction
							)) == NULL){
								return NULL;
							}
						}
						else{ // expects conditional and post (i.e for(;;) )
							if(*aux_p != ';'){
								writeError(eval_data->current_parsing_file,line,"Expected ';'");
								return NULL;

							}

							aux_p=ignoreBlanks(aux_p+1,line);

							if(*aux_p != ';'){ // conditional...
								char * end_p=ignoreBlanks(aux_p+1,line);

								if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

									if((aux_p = evalExpression(
											eval_data
											,(const char *)aux_p
											,line
											,_currentScope
											,&eval_data->evaluated_function_current->evaluated_instruction
									)) == NULL){
										return NULL;
									}
								}
							}

							aux_p=ignoreBlanks(aux_p,line);

							if(*aux_p != ';'){
								writeError(eval_data->current_parsing_file,line,"Expected ';'");
								return NULL;

							}
							aux_p=ignoreBlanks(aux_p+1,line);

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
											,_currentScope
											,&eval_data->evaluated_function_current->evaluated_instruction
									))==NULL){
										return NULL;
									}

									if(*aux_p == ',' ){
										aux_p=ignoreBlanks(aux_p+1,line);
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

						aux_p=ignoreBlanks(aux_p+1,line);
						if(*aux_p != '{'){
							writeError(eval_data->current_parsing_file,line,"Expected '{' for-block");
							return NULL;
						}

						// eval block ...
						if((aux_p=evalBlock(
								eval_data
								,aux_p
								,line
								,_currentScope
								,error
						))!= NULL){ // true: We treat declared variables into for as another scope.
							if(!error){
								scope_info->popScope(); // push current scope
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
			Scope *current_scope=scope_info;

			error=false;
			// check for keyword ...
			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_SWITCH){

					current_scope=scope_info->pushScope();

					aux_p += strlen(eval_info_keywords[key_w].str);
					aux_p=ignoreBlanks(aux_p,line);

					if(*aux_p == '('){
							aux_p=ignoreBlanks(aux_p+1,line);
							// evaluate switch vale expression ...
							if((aux_p = evalExpression(
								eval_data
								,aux_p
								,line
								,scope_info
								,&eval_data->evaluated_function_current->evaluated_instruction
							))==NULL){
								return NULL;
							}

							if(*aux_p != ')'){
								writeError(eval_data->current_parsing_file,line,"Expected ')' switch");
								error = true;
								return NULL;
							}

							aux_p=ignoreBlanks(aux_p+1,line);

							if(*aux_p == '{'){

								aux_p++;

								if((aux_p=evalRecursive(
									eval_data
									,aux_p
									, line
									, current_scope
									, error))==NULL
								){
									return NULL;
								}

								aux_p=ignoreBlanks(aux_p,line);

								if(*aux_p != '}'){
									writeError(eval_data->current_parsing_file,line,"Expected '}' switch");
									return NULL;
								}

								scope_info->popScope();
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
			//char *symbol_value;
			bool end=false;
			bool allow_for_in=true;
			//short idxScopeClass=-1;
			//short idx_scope=scope_info->getScopePtrCurrent()->idx_scope;
			//bool is_class_member=false;
			int start_line=0;

			ScriptClass *sc=NULL;
			ScriptClass *sc_come_from=NULL;

			if(scope_info->idx_scope!=IDX_GLOBAL_SCOPE){
				sc_come_from=scope_info->getScriptClass();//) { // BYTE_CODE_NOT GLOBAL
			}
			//is_class_member = scope_info->getIdxScopeBase() == scope_info->getScopePtrCurrent()->idx_scope;

			key_w = isKeywordType(aux_p);

			if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
				if(key_w == KeywordType::KEYWORD_TYPE_VAR){ // possible variable...

					aux_p += strlen(eval_info_keywords[key_w].str);
					aux_p=ignoreBlanks(aux_p,line);

					while(*aux_p != ';' && *aux_p != 0 && !end){ // JE: added multivar feature.

						aux_p=ignoreBlanks(aux_p,line);
						start_var=aux_p;
						start_line = line;
						sc=NULL;

						if(sc_come_from != NULL){ // it comes from class declaration itself
							sc=sc_come_from;
						}
						else{ // check if type var ClasS::v1 or v1
							if((end_var=isClassMember(
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
								else{ // get normal name...

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
								}
							}
						}

						KeywordType keyw = isKeywordType(variable_name.c_str());

						if(keyw != KeywordType::KEYWORD_TYPE_UNKNOWN){ // a keyword was detected...
							writeError(eval_data->current_parsing_file,line,"Cannot use symbol name as reserverd symbol \"%s\"",eval_info_keywords[keyw].str);
							return NULL;
						}

						aux_p=end_var;
						aux_p=ignoreBlanks(aux_p,line);
						//}
						bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;
						if(sc!=NULL && *aux_p == '='){
							writeError(eval_data->current_parsing_file,line,"Variable member is not assignable on its declaration. Should be initialized within constructor.");
							return NULL;
						}

						if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
							allow_for_in=false;
							if(*aux_p == '='){ // only for variables (not class members)
								// try to evaluate expression...
								aux_p=ignoreBlanks(aux_p,line);

								if((aux_p = evalExpression(
									eval_data
									,start_var
									,start_line
									,scope_info
									,&eval_data->evaluated_function_current->evaluated_instruction
								)) == NULL){
									return NULL;
								}
								line = start_line;
							}
							 // define as many vars is declared within ','

							//--- OP
							if(sc!=NULL){ // register as variable member...
								sc->registerVariable(eval_data->current_parsing_file, line, variable_name);
							}
							else{ // register as local variable in the function...
								eval_data->evaluated_function_current->script_function->registerVariable(
										eval_data->current_parsing_file
										, line
										, variable_name
								);
							}
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

			//std::vector<EvaluatedInstruction> *tokenCompiled = NULL;

			aux_p=ignoreBlanks(aux_p, line);

			KeywordType keyw = isKeywordType(aux_p);

			if(keyw == KeywordType::KEYWORD_TYPE_CASE){ // a keyword was detected...

				aux_p=ignoreBlanks(aux_p,line);

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

			aux_p=ignoreBlanks(aux_p,line);

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

			aux_p=ignoreBlanks(aux_p, line);

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
