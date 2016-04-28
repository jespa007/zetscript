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

char *GET_SYMBOL_NAME(const char *s,int & m_startLine){

	char *aux_p=(char *)s;
	char *end_token=is_token((char *)s);
	char *token;

	if(end_token != NULL){
		token = CStringUtils::copyStringFromInterval(aux_p,end_token);
		print_error_cr("Unexpected token '%s' at line",token,m_startLine);
		return NULL;
	}

	if(*aux_p!=0 && (
	   ('a' <= *aux_p && *aux_p <='z') ||
	   ('A' <= *aux_p && *aux_p <='Z'))
	){ // let's see it has right chars...

		aux_p++;
		while((*aux_p!=0) && (('a' <= *aux_p && *aux_p <='z') ||
			  ('0' <= *aux_p && *aux_p <='9') ||
			  (*aux_p=='_') ||
			  ('A' <= *aux_p && *aux_p <='Z'))){
			aux_p++;
		}
	}else{
		print_error_cr("Symbol name cannot start with char %c",*aux_p);
		return NULL;
	}

	return aux_p;
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
	str_op[KEYWORD_TYPE::NONE_KEYWORD] = {NONE_KEYWORD, "none",NULL};
	str_op[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var",parseVar};
	str_op[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",parseIf},
	str_op[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else",NULL};
	str_op[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for",parseFor};
	str_op[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while",parseWhile};

	str_op[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch",parseSwitch};
	str_op[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case",NULL};
	str_op[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break",NULL};
	str_op[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default",NULL};
	str_op[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",parseFunction};
	str_op[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return",parseReturn};
	str_op[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",NULL};
	str_op[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this",NULL};
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
				*aux == ';'  || // semicolon
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
char * CAst::parseExpression_Recursive(const char *s, int m_line, bool & error, CScriptFunction *sf, PASTNode *op, GROUP_TYPE type_group,PASTNode parent ){
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
	 // search for operator +/-...
		start_expression=aux;
		bool theres_some_token=false;
		char * expr_op_end = 0;
		char *expr_op=0;
		
		// searching for operator!
		print_ast_cr("searching for operator type %i...",type_group);
	
		while(*aux!=0 && *aux!=';' && *aux!=','  && *aux!=')'  && *aux!=']' && (expr_op_end==0)){
	
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


				// usually we have to take care about special op symbols
				if((key_w =is_keyword(aux)) != NULL){

					if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){ // function object ...

						PASTNode function_node;
						PASTNode function_args;

						aux = parseFunction(aux,m_define_symbol_line,sf,&function_node);
						if(aux == NULL){
							return NULL;
						}

						aux = CStringUtils::IGNORE_BLANKS(aux, m_line);

						if(*aux == '('){ // parse args within '(' ')'...
							aux = parseArgs(aux,m_define_symbol_line,sf,&function_args, '(', ')');

							if(aux == NULL){
								return NULL;
							}

							aux = CStringUtils::IGNORE_BLANKS(aux, m_line);

						}

					}
					else{
						print_error_cr("Error expected ; before %s at line %i",key_w, m_define_symbol_line);
						error = true;
						return NULL;
					}
				}else{

					PASTNode vector_args;
					PASTNode index_access;

					if(*aux == '['){ // vector object ...
						aux = parseArgs(aux,m_define_symbol_line,sf,&vector_args, '[', ']');

						if(aux == NULL){
							return NULL;
						}

						aux = CStringUtils::IGNORE_BLANKS(aux+1, m_line);


						if(*aux == '['){ // parse args within '(' ')'...
							aux = parseArgs(aux,m_define_symbol_line,sf,&index_access, '[', ']');

							if(aux == NULL){
								return NULL;
							}

							aux = CStringUtils::IGNORE_BLANKS(aux, m_line);

						}



						if(aux == NULL){
							return NULL;
						}
					}
					else{

						end_expression = GET_END_WORD(aux);
					}
				}
			}
			
			aux=end_expression;
			
			print_ast_cr("checkpoint3:%c\n",*aux);
			
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			
			// eval post operators...
			if(*aux != 0  && *aux!=';' && *aux!=','  && *aux!=')'  && *aux!=']' && eval_postoperator){
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

			if(*aux!=0 && *aux!=';'  && *aux!=','  && *aux!=')'  && *aux!=']' ){ // is not end expression

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

				*op=new tASTNode(2); // always preallocate 2 nodes (left and right)
				(*op)->parent=parent;
			
				  if( *start_expression!='('){
					  char subexpr[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
					  strncpy(subexpr,start_expression,end_expression-start_expression); // copy sub expression

					print_ast_cr("trivial value %s at line %i",subexpr, m_define_symbol_line);
					(*op)->children[LEFT_NODE]=(*op)->children[RIGHT_NODE]=NULL;
					(*op)->value=GET_STR_WITHOUT_SPACES(subexpr); // assign its value ...
					(*op)->definedValueline=m_define_symbol_line;


					if(pre_token!=""){ // generate a prenode operator..
						 if(*subexpr == 0){
							 print_error_cr("expected symbol before %s at line %i",pre_token.c_str(),m_line);
							 error = true;
							 return NULL;
						 }

						 (*op)=preOperator(pre_token,*op);
					}
					else if(post_token!=""){ // generate a post node operator..

						 if(*subexpr == 0){
							 print_error_cr("expected symbol after %s at line %i",pre_token.c_str(),m_line);
							 error = true;
							 return NULL;
						 }


						 (*op)=postOperator(post_token,*op);
					}

					return end_expression;
				  }else{ // parenthesis!
					print_ast_cr("START:%c",*start_expression);
					string subexpr = CStringUtils::copyStringFromInterval(start_expression+1,end_expression-1);
					print_ast_cr("expr:%s\n",subexpr.c_str());

					PASTNode p_gr=NULL;

					parseExpression_Recursive(subexpr.c_str(),m_line,error,sf,&p_gr, GROUP_0,(*op));
					if(error){
						return NULL;
					}


					if(pre_token!=""){
						p_gr = preOperator(pre_token,p_gr);
					}

					// set node ...
					*op = p_gr;

					return end_expression;
				}
			}
			else{ // there's a token, so let's perform generate its AST
					// reset pretoken...
					pre_token="";
					print_ast_cr("try to generate group1 expression: %s\n",s_effective_start);
					return parseExpression_Recursive(s,m_effective_start_line,error,sf,op,(GROUP_TYPE)(((int)type_group)+1),parent);
			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...

			*op=new tASTNode(2); // always preallocate 2 nodes (left and right)
			(*op)->parent=parent;

			char operator_str[10]={0};
			strncpy(operator_str,expr_op,expr_op_end-expr_op);



			print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};
			char eval_right[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s_effective_start,expr_op-s_effective_start); // copy its left side...
			parseExpression_Recursive(eval_left,m_effective_start_line,error,sf,&(*op)->children[LEFT_NODE],type_group,*op);

			if(error){
				return NULL;
			}

			if(pre_token!=""){
				(*op)->children[LEFT_NODE]=preOperator(pre_token,(*op)->children[LEFT_NODE]);
			}

			strncpy(eval_right,expr_op_end,strlen(expr_op)); // copy its right side...

			parseExpression_Recursive(eval_right,m_line,error,sf,&(*op)->children[RIGHT_NODE],type_group,(*op));
			if(error){
				return NULL;
			}


			if(!strcmp(operator_str,"-")) {
				(*op)->children[RIGHT_NODE]=preOperator("-",(*op)->children[RIGHT_NODE]);
				strcpy(operator_str,"+");
			}
			
			if((*op)->children[RIGHT_NODE] == NULL && (*op)->children[LEFT_NODE] == NULL){ // some wrong was happened
				return NULL;
			}
			
			(*op)->token = operator_str;
		}
		

	return expr_op_end;
}

char * CAst::parseExpression(const char *s, int m_line, bool & error, CScriptFunction *sf , PASTNode * ast_node_to_be_evaluated ){

	// PRE: s is current string to parse. This function tries to parse an expression like i+1; and generates binary ast.
	// If this functions finds ';' then the function will generate ast.

	char *end_exp=NULL;


	end_exp=parseExpression_Recursive(s,m_line,error,sf,ast_node_to_be_evaluated);

	if(*end_exp == ';' || *end_exp == ','){

		end_exp=end_exp+1;
	}
	else{
		print_error_cr("expected ; or , at line %i",m_line);
		error = true;
		return NULL;
	}


	return end_exp;
}

//---------------------------------------------------------------------------------------------------------------
// PARSE KEYWORDS




char * CAst::parseArgs(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated, char c1,char c2){
	char *aux_p = (char *)s;
	int m_startLine = m_line;
	bool error = false;
	PASTNode   node_arg_expression;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...

	if(*aux_p == c1){
		aux_p++;
		do{
			aux_p = parseExpression(aux_p,m_startLine,error,sf,&node_arg_expression);
			if(aux_p == NULL || error == true){
				return NULL;
			}

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		}while(*aux_p != ',' && *aux_p != c2 && *aux_p != 0);

		if(*aux_p != c2){
			return NULL;
		}
		return aux_p;
	}
	return NULL;
}

char * CAst::parseFunction(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*value_symbol,*end_var;
	tInfoKeyword *key_w;
	int m_startLine = m_line;
	PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
	string conditional_str;
	bool error = false;
	CScope::tInfoRegisteredVar * irv=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){
		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = FUNCTION_KEYWORD;


		(*ast_node_to_be_evaluated)->children.push_back(args_node = new tASTNode);
		(*ast_node_to_be_evaluated)->children.push_back(body_node = new tASTNode);

		args_node->node_type = ARGS_NODE;
		body_node->node_type = BODY_NODE;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		end_var=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		(*ast_node_to_be_evaluated)->definedValueline = m_startLine;

		if(*aux_p!='('){ // is named function..

			// check whwther the function is anonymous or not.
			end_var=GET_SYMBOL_NAME(aux_p,m_startLine);

			if(end_var != NULL){


				if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;


				// check whether parameter name's matches with some global variable...
				if((irv=sf->getScope()->getCurrentScope()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
					print_error_cr("Function name \"%s\" defined at line %i is ambiguos with symbol defined at %i", value_symbol, m_startLine,irv->m_line);
					return NULL;
				}

				(*ast_node_to_be_evaluated)->value = value_symbol;
			}else{
				return NULL;
			}

			aux_p=end_var;

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		}



		// parse function args...

		if(*aux_p == '('){ // push arguments...

			aux_p++;

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
			// grab words separated by ,

			while(*aux_p != 0 && *aux_p != ')'){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

				if(*aux_p == ')' || *aux_p == ','){
					print_error_cr("Expected arg at line %i ",m_startLine);
					return NULL;
				}

				end_var=GET_SYMBOL_NAME(aux_p,m_startLine);

				if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;

				// check if repeats...
				for(unsigned k = 0; k < args_node->children.size(); k++){
					if(args_node->children[k]->value == value_symbol){
						print_error_cr("Repeated argument '%s' argument at line %i ",value_symbol,m_startLine);
						return NULL;
					}
				}

				// check whether parameter name's matches with some global variable...
				if((irv=sf->getScope()->getCurrentScope()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
					print_error_cr("Ambiguos symbol argument \"%s\" at line %i name with var defined at %i", value_symbol, m_startLine,irv->m_line);
					return NULL;

				}


				aux_p=end_var;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

				if(*aux_p != ')'){

					if(*aux_p != ','){
						print_error_cr("Expected ',' at line ",m_startLine);
						return NULL;
					}

					aux_p++;
				}


				// PUSH NEW ARG...
				arg_node = new tASTNode;
				arg_node->value=value_symbol;
				args_node->children.push_back(arg_node);



			}

			if(*aux_p != ')'){
				print_error_cr("Expected ')' at line ",m_startLine);
				return NULL;
			}

			aux_p++;
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

			if(*aux_p != '{'){
				print_error_cr("Expected '{' at line ",m_startLine);
				return NULL;
			}

			// ok let's go to body..
			if((aux_p = parseBlock(aux_p,m_startLine,sf,error,&body_node)) != NULL){

				if(!error){

					sf->getScope()->registerSymbol((*ast_node_to_be_evaluated)->value,(*ast_node_to_be_evaluated)->definedValueline);

					m_line = m_startLine;
					return aux_p;
				}
			}

		}
		else{
			print_error_cr("Unclosed function defined at line %i",m_startLine);
		}
	}
	return NULL;
}

char *  CAst::parseReturn(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	tInfoKeyword *key_w;
	bool error=false;
	PASTNode expression_node=NULL; // in case there's = expression.

	string s_aux;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...

		aux_p += strlen(key_w->str);

		aux_p = parseExpression(aux_p, m_startLine, error, sf, &expression_node);

		if(aux_p != NULL){

			*ast_node_to_be_evaluated = new tASTNode;
			(*ast_node_to_be_evaluated)->node_type = NODE_TYPE::KEYWORD_NODE;
			(*ast_node_to_be_evaluated)->keyword_type = KEYWORD_TYPE::RETURN_KEYWORD;

			(*ast_node_to_be_evaluated)->children.push_back(expression_node);

			m_line = m_startLine;

			return aux_p;

		}
	}
	return NULL;
}

char * CAst::parseWhile(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tInfoKeyword *key_w;
	int m_startLine = m_line;
	PASTNode conditional=NULL, while_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::WHILE_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = WHILE_KEYWORD;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,')',m_startLine)) != NULL){

				if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
					return NULL;
				}

				conditional_str=start_symbol;

				parseExpression((const char *)conditional_str.c_str(),m_startLine,error,sf,&conditional);

				if(error){
					return NULL;
				}

				conditional->node_type = CONDITIONAL_NODE;
				(*ast_node_to_be_evaluated)->children.push_back(conditional);

				aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_startLine);
				if(*aux_p != '{'){
					print_error_cr("Expected while-block open block ('{') %i",m_startLine);
					return NULL;
				}

				if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&while_node))!= NULL){
					if(!error){
						while_node->node_type = WHILE_NODE;
						(*ast_node_to_be_evaluated)->children.push_back(while_node);
						m_line = m_startLine;
						return aux_p;
					}
				}

			}else{
				print_error_cr("Expected ')' while %i",m_startLine);
				return NULL;
			}

		}else{
			print_error_cr("Expected '(' while %i",m_startLine);
			return NULL;
		}
	}
	return NULL;
}

char * CAst::parseIf(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tInfoKeyword *key_w;
	int m_startLine = m_line;
	PASTNode conditional=NULL, if_node=NULL, else_node=NULL;
	string conditional_str;
	bool error = false;


	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);


	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::IF_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = IF_KEYWORD;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,')',m_startLine)) != NULL){

				if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
					return NULL;
				}

				conditional_str=start_symbol;

				parseExpression((const char *)conditional_str.c_str(),m_startLine,error,sf,&conditional);

				if(error){
					return NULL;
				}

				conditional->node_type = CONDITIONAL_NODE;
				(*ast_node_to_be_evaluated)->children.push_back(conditional);

				aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_startLine);
				if(*aux_p != '{'){
					print_error_cr("Expected if-block open block ('{') %i",m_startLine);
					return NULL;
				}

				if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&if_node))!= NULL){
					if(!error){
						if_node->node_type = IF_NODE;
						(*ast_node_to_be_evaluated)->children.push_back(if_node);

						aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

						bool else_key = false;
						if((key_w = is_keyword(aux_p)) != NULL){
							else_key = (key_w->id == KEYWORD_TYPE::ELSE_KEYWORD);
						}

						if(else_key){
							aux_p += strlen(key_w->str);

							aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
							if(*aux_p != '{'){
								print_error_cr("Expected else-block open block ('{') %i",m_startLine);
								return NULL;
							}

							if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&else_node))!= NULL){
									if(!error){

										else_node->node_type = ELSE_NODE;
										(*ast_node_to_be_evaluated)->children.push_back(else_node);
										m_startLine = m_line;
										return aux_p;
									}
									else{

									}
							}

						}else{
							m_line = m_startLine;
							return aux_p;
						}
					}
				}
			}else{
				print_error_cr("Expected ')' if %i",m_startLine);
				return NULL;
			}

		}else{
			print_error_cr("Expected '(' if %i",m_startLine);
			return NULL;
		}
	}
	return NULL;
}

char * CAst::parseFor(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *aux_p2;
	tInfoKeyword *key_w;
	char *end_expr,*start_symbol;
	int m_startLine = m_line;
	bool error=false;
	PASTNode block_for = NULL;
	string eval_for;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::FOR_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = FOR_KEYWORD;

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){ // ready ...

			struct{
				char next_char;
				NODE_TYPE node_type;
			}info_for[3]={
					{';',PRE_FOR_NODE},
					{';',CONDITIONAL_NODE},
					{')',POST_FOR_NODE}
			};

			for(int i = 0; i < 3; i++){

				PASTNode for_node;
				if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,info_for[i].next_char,m_startLine)) != NULL){

					if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					eval_for = start_symbol;

					aux_p2=CStringUtils::IGNORE_BLANKS(eval_for.c_str(),m_startLine);

					if(*aux_p2 != 0){ // check is not void
						parseExpression((const char *)aux_p2,m_startLine,error,sf,&for_node);

						if(error){
							return NULL;
						}

						for_node->node_type = info_for[i].node_type;
						(*ast_node_to_be_evaluated)->children.push_back(for_node);

					}
				}else{
					print_error_cr("Expected '%c' line %i",info_for[i].next_char,m_startLine);
				}

				aux_p = end_expr+1;
			}

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
			if(*aux_p != '{'){
				print_error_cr("Expected '{' for-block at line %i",m_startLine);
				return NULL;
			}

			// parse block ...
			if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&block_for))!= NULL){
					if(!error){

						block_for->node_type = FOR_NODE;
						(*ast_node_to_be_evaluated)->children.push_back(block_for);
						m_line = m_startLine;
						return aux_p;
					}
					else{

					}
			}
		}else{
			print_error_cr("Expected '(' for %i",m_startLine);
			return NULL;
		}
	}

	return NULL;
}


char * CAst::parseSwitch(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr, *end_symbol,*start_symbol;
	PASTNode case_default_keyword_node,case_body_node,case_value_node;
	char *value_to_eval;
	string val;
	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	tInfoKeyword *key_w,*key_w2;
	int m_startLine = m_line;
	bool error=false;
	int n_cases;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::SWITCH_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = SWITCH_KEYWORD;

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		if(*aux_p == '('){
			//if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p,')',m_startLine)) != NULL){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

				end_expr = GET_END_WORD(aux_p);

				value_to_eval = CStringUtils::copyStringFromInterval(aux_p, end_expr);

				if(!value_to_eval) {return NULL;}

				//--- create node
				(*ast_node_to_be_evaluated)->value = value_to_eval;

				//---
				aux_p=CStringUtils::IGNORE_BLANKS(end_expr,m_startLine);

				if(*aux_p != ')'){
					print_error_cr("Expected ')' switch %i",m_startLine);
					error = true;
					return NULL;
				}

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

				if(*aux_p == '{'){

					// ok try to get cases and default nodes ...
					aux_p++;
					aux_p = CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);


					while(*aux_p!='}' && *aux_p!=0){
							n_cases = 0;
							// init node ..
							case_default_keyword_node = new tASTNode;

							while((key_w = is_keyword(aux_p)) != NULL){ // acumulative cases /defaults...

								switch(key_w->id){
								case DEFAULT_KEYWORD:
								case CASE_KEYWORD:

									val = "default";
									aux_p += strlen(key_w->str);
									aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

									if(key_w->id == CASE_KEYWORD){


										// get the symbol...

										start_symbol=aux_p;
										end_symbol = GET_END_WORD(start_symbol);
										aux_p=end_symbol;

										value_to_eval = CStringUtils::copyStringFromInterval(start_symbol, end_symbol);

										if(value_to_eval==NULL){ return NULL;}

										val = value_to_eval;

										aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
									}



									if(*aux_p == ':'){

										// check if repeated...
										for(unsigned j=0; j <(*ast_node_to_be_evaluated)->children.size(); j++ ){
											for(unsigned h=0; h <(*ast_node_to_be_evaluated)->children[j]->children.size(); h++ ){
												if((*ast_node_to_be_evaluated)->children[j]->children[h]->value == val){
													print_error_cr("Symbol %s repeteaded in switch at line %i",val.c_str(),m_startLine);
													return NULL;
												}
											}
										}

										case_value_node = new tASTNode;
										case_value_node->node_type = KEYWORD_NODE;
										case_value_node->keyword_type = CASE_KEYWORD;
										case_value_node->value = val;
										case_default_keyword_node->children.push_back(case_value_node);

										aux_p++;
										n_cases++;
									}
									else{
										print_error_cr("Expected  : case at %i",m_startLine);
										return NULL;
									}


									break;

									aux_p += strlen(key_w->str);
									aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
									if(*aux_p == ':'){
										n_cases++;
										aux_p++;
									}
									else{
										print_error_cr("Expected  : default at %i",m_startLine);
										return NULL;
									}

									break;
								default:
									print_error_cr("Expected case or default in switch %i",m_startLine);
									return NULL;
									break;
								}

								aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
							}

							if(n_cases == 0){
								print_error_cr("Expected case or default in switch %i",m_startLine);
								return NULL;
							}

							_localScope->pushScope();


							case_body_node = NULL;

							// eval block...
							if((aux_p=generateAST_Recursive(aux_p, m_startLine, sf, error, &case_body_node))==NULL){
								return NULL;
							}

							if(error){
								return NULL;
							}

							key_w2 = is_keyword(aux_p);
							if(key_w2->id == BREAK_KEYWORD){
								aux_p += strlen(key_w2->str);
								CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

								if(*aux_p == ';'){ // the new scope ...
									case_default_keyword_node->node_type = KEYWORD_NODE;
									//case_default_keyword_node->keyword_type = key_w->id; // think about
									case_default_keyword_node->value=value_to_eval;

									if(case_body_node != NULL){
										case_body_node->node_type = BODY_NODE;
									}

									// stack body ...
									case_default_keyword_node->children.push_back(case_body_node);

									// stack cases/default with body...
									(*ast_node_to_be_evaluated)->children.push_back(case_default_keyword_node);
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

							aux_p =CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);
						}

					if(*aux_p == '}'){
						m_line = m_startLine;
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

		start_var=aux_p;


		end_var=GET_SYMBOL_NAME(aux_p,m_startLine);

		if(end_var != NULL){

			if((symbol_name=CStringUtils::copyStringFromInterval(start_var,end_var)) == NULL){
				return NULL;
			}

			aux_p=end_var;

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if((*aux_p == ';' || *aux_p == '=')){

				print_info_cr("registered \"%s\" variable: ",symbol_name);

				if(!_currentScope->registerSymbol(symbol_name,m_startLine)){
					return NULL;
				}

				if(*aux_p == '='){

					// try to evaluate expression...
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

					end_var = parseExpression(aux_p+1,m_startLine,error,sf,&expression_node);

					if(error){
						return NULL;
					}else{
						aux_p = end_var;
					}
				}else{ // ignores ';'
					aux_p++;
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

		}
	}
	return NULL;
}


char * CAst::parseBlock(const char *s,int & m_line,  CScriptFunction *sf, bool & error,PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;

	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	//CScope *_ant, *_post;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...

	if(*aux_p == '{'){
		aux_p++;

		_localScope->pushScope();

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

			(*ast_node_to_be_evaluated)->node_type = BODY_NODE;

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

	tInfoKeyword *keyw=NULL,*keyw2nd=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

	// check if condition...
	keyw = is_keyword(aux_p);

	if(keyw != NULL){ // a keyword was detected...

		aux_p+=strlen(keyw->str);
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

		// check if another kwyword is put ...
		if((keyw2nd = is_keyword(aux_p))!= NULL){
			print_error_cr("unexpected keyword \"%s\" at line %i",keyw2nd->str, m_line);
			error = true;
			return NULL;
		}

		if(str_op[keyw->id].parse_fun != NULL){

			if((aux_p = str_op[keyw->id].parse_fun(s,m_line,sf, ast_node_to_be_evaluated)) != NULL){
				return aux_p;
			}

			error = true;

		}else{
			print_error_cr("%s has no parse function implemented yet!",str_op[keyw->id].str);
		}

    	// something wrong was happen..

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

		if(*aux == '}'){ // trivial cases...
			return aux;
		}else{
			keyw = is_keyword(aux);
			if(keyw!= NULL){
				if(keyw->id == KEYWORD_TYPE::BREAK_KEYWORD){
					return aux;
				}
			}
		}

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
				if((end_expr = parseExpression(aux,m_line,error, sf, &child_node)) == NULL){ // something wrong was happen.
					return NULL;
				}
			}

		}

		(*node_to_be_evaluated)->children.push_back(child_node);
		aux=end_expr;
		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

		/*keyw = is_keyword(aux);

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
		}*/

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



