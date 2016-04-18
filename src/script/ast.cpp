#include "zg_script.h"

#define MAX_EXPRESSION_LENGHT 2096


#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_ast_cr print_info_cr
#else
#define print_ast_cr(s,...)
#endif



tInfoKeyword str_op[MAX_KEYWORD]{
		{NONE_KEYWORD, "none"},
		{IF_KEYWORD,"if"},
		{ELSE_KEYWORD,"else"},
		{FOR_KEYWORD,"for"},
		{WHILE_KEYWORD,"while"},
		{VAR_KEYWORD,"var"},
		{SWITCH_KEYWORD,"switch"},
		{CASE_KEYWORD,"case"},
		{BREAK_KEYWORD,"break"},
		{DEFAULT_KEYWORD,"default"},
		{FUNCTION_KEYWORD,"function"},
		{RETURN_KEYWORD,"return"},
		{CLASS_KEYWORD,"class"},
		{THIS_KEYWORD,"this"}
};

char * token_group0(char *c){
	char *aux=c;
	// try number operators...
	if(*c=='+' || *c=='-' || *c=='^' || (*c=='=' && *(c+1)!='=') ){
		aux++;
		return aux;
	}

	if( (*c=='&' && *(c+1)!='&') // ==
	){
		aux++;
		return aux;
	}

	if( (*c=='|' && *(c+1)!='|') // ==
	){
		aux++;
		return aux;
	}

	// try boolean operators...
	if(
			(*c=='|' && *(c+1)=='|') // ||
	){
		aux+=2;
		return aux;
	}

	if(
			(*c=='<' && *(c+1)=='<') // ||
	){
		aux+=2;
		return aux;
	}

	if(
			(*c=='>' && *(c+1)=='>') // ||
	){
		aux+=2;
		return aux;
	}
	return 0;
}

char * token_group1(char *c){
	char *aux=c;


	// try boolean operators...
	if( (*c=='&' && *(c+1)=='&') // ==
	){
		aux+=2;
		return aux;
	}

	// try number operators...
	if(*c=='*' || *c=='/' || *c=='%' ){
		aux++;
		return aux;
	}

	return 0;
}

char * token_group2(char *c){
	char *aux=c;

	// try boolean operators...
	if( (*c=='=' && *(c+1)=='=') // ==
	 || (*c=='<' && *(c+1)=='=') // <=
	 || (*c=='>' && *(c+1)=='=') // >=

	){
		aux+=2;
		return aux;
	}

	// try number operators...
	if( *c=='>' || *c=='<'){
		aux++;
		return aux;
	}
	return 0;
}

char * token_group3(char *c){
	char *aux=c;
	// try boolean operators...
	if( (*c=='!') // ==
	){
		aux++;
		return aux;
	}
	return 0;
}

char * is_token(char *c){
	char *aux;

	if(*c == ';'){ // semicolon
		return c+1;
	}

	if(*c == ':'){ // semicolon
		return c+1;
	}

	if((aux=token_group0(c))!=0){
		return aux;
	}else if((aux=token_group1(c))!=0) {
		return aux;
	}else if((aux=token_group2(c))!=0) {
		return aux;
	}else if((aux=token_group3(c))!=0) {
		return aux;
	}
	return 0;
}

// to string utils ...

char * GET_END_WORD(const char *s){

	char *aux=(char *)s;

	if(*aux == '\"'){
		aux++;
		while((*aux)!=0 && !((*aux)=='\n') && !((*aux)=='\"')) {
			aux++;
		}

		if(*aux != '\"') {
			print_error_cr("Error \" not closed");
			return NULL;
		}
		aux++;

	}else{
		while((*aux)!=0 && !((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r') && (is_token(aux)==0)) {
			aux++;
		}
	}
	return aux;
}

bool IS_WORD(const char *s){
	char *aux=(char *)s;
	while((*aux)!=0 && !((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r') && (is_token(aux)==0)) {
		aux++;
	}
	return *aux==0;
}


tInfoKeyword CAst::str_op[MAX_KEYWORD];

void CAst::createSingletons(){
	str_op[KEYWORD_TYPE::NONE_KEYWORD] = {NONE_KEYWORD, "none"};
	str_op[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var"};
	str_op[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if"},
	str_op[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else"};
	str_op[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for"};
	str_op[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while"};

	str_op[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch"};
	str_op[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case"};
	str_op[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break"};
	str_op[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default"};
	str_op[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function"};
	str_op[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return"};
	str_op[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class"};
	str_op[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this"};
}

void CAst::destroySingletons(){

}



tInfoKeyword * CAst::is_keyword(const char *c){

	int m_line=0;

	char *str=CStringUtils::IGNORE_BLANKS(c,m_line);

	for(int i = 0; i < MAX_KEYWORD; i++){
		int size = strlen(str_op[i].str);
		char *aux = str+size;
		if((strncmp(str,str_op[i].str,size)==0) && (
				*aux == 0  || // carry end
				*aux == ' '  || // space
				*aux == '\t'  || // tab
				is_token(aux)!=NULL ||
				*aux == '\n' || // carry return
				*aux == '(' || // open parenthesis
				*aux == ')' || // close parenthesis
			   (*aux == '/' && *(aux+1) == '*')) //start block comment
			   ){
			return &str_op[i];
		}
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
string GET_STR_WITHOUT_SPACES(const string & s){
	string f="";

	for(unsigned i =0; i < s.size(); i++){
		if(s[i]!=' ')
			f+=s[i];
	}

	return f;
}

//------------------------------------------------------------------------------------------------------------
inline char * GET_CLOSED_PARENTHESIS(const char *s){
	//bool end = false;
	char *aux = (char *)s;
	
	if(*aux != '('){
		print_error_cr("Error first character must be a parenthesis!" );
		return NULL;
	}
	int num_levels=1; // because first is a parenthesis;
	do{
		aux++;
		if(*aux==')'){
			num_levels--;
		}else if(*aux=='('){
			num_levels++;
		}
	}while((*aux) != 0 && num_levels>0);
		
	return aux;
}


char *preoperator_token( char *c, int & m_line){
	char *aux=CStringUtils::IGNORE_BLANKS(c,m_line);

	// detection ++ operator.
	if(*aux=='+' && *(aux+1)=='+'){
		aux+=2;
		return aux;
	}

	// detection -- operator.
	if(*aux=='-' && *(aux+1)=='-'){
		aux+=2;
		return aux;
	}

	if(*aux=='+' || *aux=='-' || *aux=='!'){
		aux++;
		return aux;
	}

	return 0;
}

char *postoperator_token( char *c, int & m_line){
	char *aux=CStringUtils::IGNORE_BLANKS(c,m_line);

	// detection ++ operator.
	if(*aux=='+' && *(aux+1)=='+'){
		aux+=2;
		return aux;
	}

	// detection -- operator.
	if(*aux=='-' && *(aux+1)=='-'){
		aux+=2;
		return aux;
	}

	return 0;
}

PASTNode CAst::preOperator(string token,PASTNode affected_op){ // can be -,+,! etc...
	PASTNode op=new tASTNode(2);
	op->token = token;
	op->children[LEFT_NODE]=affected_op;
	return op;
}

PASTNode CAst::postOperator(string token,PASTNode affected_op){ // can be -,+,! etc...
	string token1 = "post_"+token;

	return preOperator(token1,affected_op);
}
//------------------------------------------------------------------------------------------------------------
PASTNode CAst::parseExpression_Recursive(const char *s, int m_line, bool & error, GROUP_TYPE type_group,PASTNode parent ){
	//int index=0;
	char *aux=(char *)s;
	char *s_effective_start=(char *)s;
	char *start_expression,*end_expression ;
	string pre_token="";
	string post_token="";
	bool eval_preoperator=true;
	bool eval_postoperator=true;

	int m_effective_start_line=m_line;
	int m_define_symbol_line;


	PASTNode op=new tASTNode(2); // always preallocate 2 nodes (left and right)
	op->parent=parent;
	
	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
	
	if(*aux==0){
		return NULL;
	}	
	
	if(type_group>=MAX_GROUPS) {
		print_error_cr("max groups");
		error = true;
		return NULL;
	}

	print_ast_cr("new expression eval:\"%s\" group:%i at line %i",s,type_group, m_line);
	
	//if(type_group==GROUP_0) 
	{ // search for operator +/-...
		start_expression=aux;
		bool theres_some_token=false;
		char * expr_op_end = 0;
		char *expr_op=0;
		
		// searching for operator!
		print_ast_cr("searching for operator type %i...",type_group);
	
		while(*aux!=0 && (expr_op_end==0)){
	
			print_ast_cr("checkpoint1:%c\n",*aux);
			// 1. ignore spaces...
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			
			if(is_token(aux)!=0){

				if((end_expression=preoperator_token(aux, m_line))!=0){
						if(eval_preoperator || (!eval_preoperator&&(eval_postoperator))){ // first token! Save the operator to apply on the result expression.
							char str_op[10]={0};

							if(eval_preoperator){ // It can't do preoperator and postoperator at the same time.
								eval_postoperator=false;
							}

							strncpy(str_op,aux,end_expression-aux);

							print_ast_cr("preoperator %s!",str_op);
							pre_token = str_op;
							aux=end_expression; // ignore first token...
							s_effective_start=end_expression;
							m_effective_start_line = m_line;
							start_expression=aux;
						}else{ // just ignore it
							aux=end_expression; // ignore first token...
						}
				}
				else{
					print_error_cr("unexpected token %c at line %i",*aux,m_line);
					error = true;
					return NULL;
				}
			}
			
			print_ast_cr("checkpoint2:%c",*aux);
			
			//start_value=aux;
			if(*aux=='('){ // exp within ()
				print_ast_cr("try find parenthesis close");
				end_expression = GET_CLOSED_PARENTHESIS(aux);
				
				if(end_expression==0){
					return NULL;
				}
				
				print_ast_cr("checkpoint4:%c",*end_expression);
				
				if(*end_expression==0){
						print_error_cr("')' not found");
						error = true;
						return NULL;
				}else{ // advance )
					end_expression++;
				}				
			}else{ // value 

				tInfoKeyword *key_w;
				m_define_symbol_line = m_line;
				end_expression = GET_END_WORD(aux);

				// usually we have to take care about special op symbols
				if((key_w =is_keyword(end_expression)) != NULL){
					print_error_cr("Error expected ; before %s at line %i",key_w, m_define_symbol_line);
					error = true;
					return NULL;
				}
			}
			
			aux=end_expression;
			
			print_ast_cr("checkpoint3:%c\n",*aux);
			
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			
			// eval post operators...
			if(*aux != 0 && eval_postoperator){
				char *aux2;
				if((aux2=postoperator_token(aux, m_line))!=0){
					char str_op[10]={0};
					strncpy(str_op,aux,aux2-aux);
					print_ast_cr("postperator %s!",str_op);
					post_token = str_op;

					aux=aux2;
				}
				aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			}

			if(*aux!=0){

				expr_op=aux;
				char *adv_op;
				if((adv_op=is_token(expr_op))!=0){
					theres_some_token|=true;
					
					switch(type_group){
					case  GROUP_0:	expr_op_end = token_group0(expr_op);break;
					case GROUP_1:	expr_op_end = token_group1(expr_op);break;
					case GROUP_2:	expr_op_end = token_group2(expr_op);break;
					case GROUP_3:	expr_op_end = token_group3(expr_op);break;
					default: break;
					}

					aux=adv_op; // advance operator...
				}
			}

			if(!eval_preoperator){
				eval_postoperator=false;
			}

			eval_preoperator = false; // set it false due is not first.
		}
		
		if(expr_op_end==0) {// there's no any operators \"type_group\"...
			if(!theres_some_token ){ // only we have a value (trivial)
			
				  if( *start_expression!='('){
					  char subexpr[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
					  strncpy(subexpr,start_expression,end_expression-start_expression); // copy sub expression

					print_ast_cr("trivial value %s at line %i",subexpr, m_define_symbol_line);
					op->children[LEFT_NODE]=op->children[RIGHT_NODE]=NULL;
					op->value=GET_STR_WITHOUT_SPACES(subexpr); // assign its value ...
					op->definedValueline=m_define_symbol_line;


					if(pre_token!=""){ // generate a prenode operator..
						 if(*subexpr == 0){
							 print_error_cr("expected symbol before %s at line %i",pre_token.c_str(),m_line);
							 error = true;
							 return NULL;
						 }

						op=preOperator(pre_token,op);
					}
					else if(post_token!=""){ // generate a post node operator..

						 if(*subexpr == 0){
							 print_error_cr("expected symbol after %s at line %i",pre_token.c_str(),m_line);
							 error = true;
							 return NULL;
						 }


						op=postOperator(post_token,op);
					}

					return op;
				  }else{ // parenthesis!
					print_ast_cr("START:%c",*start_expression);
					char subexpr[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
					if((end_expression-start_expression-2)> MAX_EXPRESSION_LENGTH){
						print_error_cr("expression too long\n");
						error = true;
						return NULL;
					}
					
					strncpy(subexpr,start_expression+1,end_expression-start_expression-2); // copy sub expression
					print_ast_cr("expr:%s\n",subexpr);
					PASTNode p_gr=parseExpression_Recursive(subexpr,m_line,error,GROUP_0,op);
					if(error){
						return NULL;
					}


					if(pre_token!=""){
						return preOperator(pre_token,p_gr);
					}else{
						return p_gr;
					}
				}
			}
			else{ // there's a token, so let's perform generating its AST
					// reset pretoken...
					pre_token="";
					print_ast_cr("try to generate group1 expression: %s\n",s_effective_start);
					return parseExpression_Recursive(s,m_effective_start_line,error,(GROUP_TYPE)(((int)type_group)+1),op);
			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...
			char operator_str[10]={0};
			strncpy(operator_str,expr_op,expr_op_end-expr_op);


			print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};
			char eval_right[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s_effective_start,expr_op-s_effective_start); // copy its left side...
			op->children[LEFT_NODE]=parseExpression_Recursive(eval_left,m_effective_start_line,error,type_group,op);

			if(error){
				return NULL;
			}

			if(pre_token!=""){
				op->children[LEFT_NODE]=preOperator(pre_token,op->children[LEFT_NODE]);
			}

			strncpy(eval_right,expr_op_end,strlen(expr_op)); // copy its right side...

			op->children[RIGHT_NODE]=parseExpression_Recursive(eval_right,m_line,error,type_group,op);
			if(error){
				return NULL;
			}


			if(!strcmp(operator_str,"-")) {
				op->children[RIGHT_NODE]=preOperator("-",op->children[RIGHT_NODE]);
				strcpy(operator_str,"+");
			}

			
			if(op->children[RIGHT_NODE] == NULL && op->children[LEFT_NODE] == NULL){ // some wrong was happened
				return NULL;
			}
			
			op->token = operator_str;
		}
		
	}
	return op;
}

char * CAst::parseExpression(const char *s, int m_line, bool & error, PASTNode * node  ){

	// PRE: s is current string to parse. This function tries to parse an expression like i+1; and generates binary ast.
	// If this functions finds ';' then the function will generate ast.

	char *end_expr=NULL;
	char *aux_p =(char *)s;
	char *expression;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p, ';',m_line)) != NULL){

			if((expression=CStringUtils::copyStringFromInterval(aux_p,end_expr)) == NULL){
				error = true;
				return NULL;
			}

			*node=parseExpression_Recursive(expression,m_line,error);

			end_expr=end_expr+1;
	}
	else{
		print_error_cr("expected ; at line %i",m_line);
		error = true;
		return NULL;
	}

	return end_expr;
}

//---------------------------------------------------------------------------------------------------------------
// PARSE KEYWORDS


char * CAst::parseFor(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	char *aux_p = (char *)s;
	CScope *_localScope =  sf->getScope()->getCurrentScope(); // gets current evaluating scope...
	tInfoKeyword *key_w;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);


	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id != KEYWORD_TYPE::FOR_KEYWORD){
		aux_p += strlen(key_w->str);
	}


	return NULL;

}


char * CAst::parseSwitch(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	char *aux_p = (char *)s;
	char *end_expr, *end_symbol,*start_symbol;
	PASTNode case_default_node;
	char *value_to_eval;
	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	tInfoKeyword *key_w;
	tInfoKeyword *key_w2;
	int m_startLine = m_line;
	bool error=false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);


	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id != KEYWORD_TYPE::SWITCH_KEYWORD){
		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p,')',m_startLine)) != NULL){
				value_to_eval = CStringUtils::copyStringFromInterval(aux_p+1, end_expr-1);

				//--- create node
				*ast_node_to_be_evaluated = new tASTNode;

				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_type = SWITCH_KEYWORD;
				(*ast_node_to_be_evaluated)->value = value_to_eval;

				aux_p = end_expr+1;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

				if(*aux_p == '{'){

					// ok try to get cases and default nodes ...
					aux_p++;
					aux_p = CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

					while(*aux_p!='}' && *aux_p!=0){

						key_w = is_keyword(aux_p);
						if(key_w){
							switch(key_w->id){
							case CASE_KEYWORD:
								aux_p += strlen(key_w->str);

								// get the symbol...
								aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
								start_symbol=aux_p;
								end_symbol = GET_END_WORD(start_symbol);
								aux_p=end_symbol;

								aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

								if(*aux_p == ':'){
									value_to_eval = CStringUtils::copyStringFromInterval(aux_p+1, end_expr-1);
									aux_p++;
								}
								else{
									print_error_cr("Expected  : case at %i",m_startLine);
								}


								break;
							case DEFAULT_KEYWORD:
								aux_p += strlen(key_w->str);
								aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
								if(*aux_p == ':'){
									aux_p++;
								}
								else{
									print_error_cr("Expected  : default at %i",m_startLine);
								}

								break;
							default:
								print_error_cr("Expected case or default in switch %i",m_startLine);
								return NULL;
								break;
							}

							_localScope->pushScope();

							// eval block...
							if((aux_p=generateAST_Recursive(aux_p, m_startLine, sf, error, &case_default_node))==NULL){
								return NULL;
							}

							key_w2 = is_keyword(aux_p);
							if(key_w2->id == BREAK_KEYWORD){
								aux_p += strlen(key_w->str);
								CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

								if(*aux_p == ';'){ // the new scope ...
									case_default_node->node_type = KEYWORD_NODE;
									case_default_node->keyword_type=key_w->id;
									case_default_node->value=value_to_eval;
								}
								else{
									print_error_cr("Expected break %i",m_startLine);
									return NULL;
								}

							}else{
								print_error_cr("Expected break %i",m_startLine);
								return NULL;

							}


							_localScope->popScope();

						}
						else{
							print_error_cr("Expected case or default in switch %i",m_startLine);
							return NULL;
						}

						aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
					}

					if(*aux_p == '}'){



						return aux_p + 1;
					}
					else{
						print_error_cr("Expected '}' switch %i",m_startLine);
					}
				}
				else{
					print_error_cr("Expected '{' switch %i",m_startLine);
				}

			}
			else{
				print_error_cr("Expected ')' switch %i",m_startLine);
			}
		}
		else{
			print_error_cr("Expected '(' switch %i",m_startLine);
		}


	}


	return NULL;

}


char * CAst::parseVar(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	CScope *_currentScope =  sf->getScope()->getCurrentScope(); // gets current evaluating scope...
	tInfoKeyword *key_w;
	char *start_var,*end_var, *symbol_name;
	bool error=false;
	PASTNode expression_node=NULL; // in case there's = expression.

	string s_aux;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		if(*aux_p!=0 && (
		   ('a' <= *aux_p && *aux_p <='z') ||
		   ('A' <= *aux_p && *aux_p <='Z'))
		){ // let's see it has right chars...
			start_var=aux_p;
			aux_p++;
			while((*aux_p!=0 || *aux_p!= '=') && (('a' <= *aux_p && *aux_p <='z') ||
				  ('0' <= *aux_p && *aux_p <='9') ||
				  (*aux_p=='_') ||
				  ('A' <= *aux_p && *aux_p <='Z'))){
				aux_p++;
			}
			end_var=aux_p;

			if((symbol_name=CStringUtils::copyStringFromInterval(start_var,end_var)) == NULL){
				return NULL;
			}


			if((key_w = is_keyword(symbol_name)) != NULL){
				print_error_cr("unexpected keyword \"%s\" at line %i",stat, m_line);
				return NULL;
			}

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if((*aux_p == ';' || *aux_p == '=')){


				print_info_cr("registered \"%s\" variable: ",symbol_name);

				if(!_currentScope->registerVariable(symbol_name,m_startLine)){
					return NULL;
				}

				if(*aux_p == '='){
					// copy expression and try to evaluate it...
					end_var = parseExpression(aux_p+1,m_startLine,error,&expression_node);

					if(error){
						return NULL;
					}else{
						aux_p = end_var;
					}
				}

				(*ast_node_to_be_evaluated) = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_type = VAR_KEYWORD;


				if(expression_node != NULL){
					(*ast_node_to_be_evaluated)->children.push_back(expression_node);
				}

				// rewind line when the variable was declared because we pass the expression "variable=expression"
				m_line = m_startLine;

				return aux_p;
			}
			else{
				print_error_cr("variable cannot cannot contain char '%c' at line",*aux_p, m_line);
			}

		}else{
			print_error_cr("variable cannot start with char %c",*aux_p);
		}
	}
	return NULL;
}


char * CAst::parseBlock(const char *s,int & m_line,  CScriptFunction *sf, bool & error,PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;

	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	CScope *_ant, *_post;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...

	if(*aux_p == '{'){
		aux_p++;

		// up scope ...
		_ant = _localScope->getCurrentScope();
		_localScope = _localScope->pushScope();
		_post = _localScope->getCurrentScope();

		if((aux_p = generateAST_Recursive(aux_p, m_startLine,sf,error,ast_node_to_be_evaluated)) != NULL){
			if(error){
				return NULL;
			}

			if(*aux_p != '}'){
				error = true;
				print_error_cr("Expected } ");
				return NULL;
			}

			_localScope->popScope();

			(*ast_node_to_be_evaluated)->node_type = BLOCK_NODE;

			m_line = m_startLine;
			return aux_p+1;
		}
	}

	return NULL;
}

char *CAst::parseKeyWord(const char *s, int & m_start_line, CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p= (char *)s;
	int m_line = m_start_line;


	tInfoKeyword *keyw=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

	// check if condition...
	keyw = is_keyword(aux_p);

	if(keyw != NULL){ // a keyword was detected...

		aux_p += strlen(keyw->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

		switch(keyw->id){

		default:
			return NULL;

		case KEYWORD_TYPE::VAR_KEYWORD:

			if((aux_p = parseVar(s,m_line,sf, ast_node_to_be_evaluated)) != NULL){
				return aux_p;
			}

			break;
		case KEYWORD_TYPE::FUNCTION_KEYWORD:
			break;
		case KEYWORD_TYPE::FOR_KEYWORD:
			break;
		case KEYWORD_TYPE::WHILE_KEYWORD:
			break;
		case KEYWORD_TYPE::SWITCH_KEYWORD:

			if((aux_p = parseSwitch(s,m_line,sf, ast_node_to_be_evaluated)) != NULL){
				return aux_p;
			}
			break;
		case KEYWORD_TYPE::IF_KEYWORD:
			break;
		case KEYWORD_TYPE::ELSE_KEYWORD:
			break;
		case KEYWORD_TYPE::CASE_KEYWORD:
			break;

		}

		// something wrong was happen..
		error = true;

	}


	return NULL;
}



char * CAst::generateAST_Recursive(const char *s, int m_line, CScriptFunction *sf, bool & error, PASTNode *node_to_be_evaluated){

	// PRE: **node must be created and is i/o ast pointer variable where to write changes.

	tInfoKeyword *keyw=NULL;
	char *aux = (char *)s;
	char *end_expr;
	*node_to_be_evaluated = new tASTNode;

	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

	while(*aux != 0 ){
		PASTNode child_node = NULL;


		// 1st. check whether parse a keyword...
		if((end_expr = parseKeyWord(aux, m_line, sf, error, &child_node)) == NULL){

			// If was unsuccessful then try to parse expression.
			if(error){
				return NULL;
			}

			// 2nd. check whether parse a block
			if((end_expr = parseBlock(aux,m_line, sf, error, &child_node))==NULL){

				// If was unsuccessful then try to parse expression.
				if(error){
					return NULL;
				}

				// 2nd. try expression
				if((end_expr = parseExpression(aux,m_line,error, &child_node)) == NULL){ // something wrong was happen.
					return NULL;
				}
			}

		}

		(*node_to_be_evaluated)->children.push_back(child_node);
		aux=end_expr;
		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

		keyw = is_keyword(aux);

		if(keyw != NULL){
			if(keyw->id == BREAK_KEYWORD){
				char *aux2 = aux + strlen(keyw->str);
				aux2=CStringUtils::IGNORE_BLANKS(aux2, m_line);
				if(*aux2==';'){
					return aux; // return break;
				}
			}
		}else{
			if(*aux=='}'){
				return aux;
			}
		}

	}

	return NULL;
}


PASTNode CAst::generateAST(const char *s, CScriptFunction *sf){

	int m_line = 1;
	bool error=false;
	PASTNode node = NULL;


	if(generateAST_Recursive(s, m_line,sf,error,&node) != NULL){
		node->node_type = MAIN_NODE;
		return node;
	}else{
		if(node != NULL){
			delete node;
		}
	}

	return NULL;
}



