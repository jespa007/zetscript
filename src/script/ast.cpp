#include "zg_script.h"


#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_ast_cr print_ast_cr
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

tInfoKeyword * is_keyword(const char *c){

	int m_line=0;

	char *str=CStringUtils::IGNORE_BLANKS(c,m_line);

	for(int i = 0; i < MAX_KEYWORD; i++){
		int size = strlen(str_op[i].str);
		char *aux = str+size;
		if((strncmp(str,str_op[i].str,size)==0) && (
				*aux == 0  || // carry end
				*aux == ' '  || // space
				is_token(aux)!=NULL ||
				*aux == '\n' || // carry return
				*aux == '(' || // carry return
				*aux == ')' || // carry return
			   (*aux == '/' && *(aux+1) == '*')) //start block comment
			   ){
			return &str_op[i];
		}
	}

	return 0;
}

//------------------------------------------------------------------------------------------------------------
/*inline char * CStringUtils::IGNORE_BLANKS(const char *s){
	char *aux=(char *)s;
	while((*aux)!=0 && ((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r')) {
		aux++;
	}
	return aux;
}*/
//------------------------------------------------------------------------------------------------------------
char * GET_END_WORD(const char *s){
	char *aux=(char *)s;
	while((*aux)!=0 && !((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r') && (is_token(aux)==0)) {
		aux++;
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


PASTNode preOperator(string token,PASTNode affected_op){ // can be -,+,! etc...
	tASTNode *op=new tASTNode;
	op->token = token;
	op->left=affected_op;
	return op;
}


PASTNode postOperator(string token,PASTNode affected_op){ // can be -,+,! etc...
	string token1 = "post_"+token;

	return preOperator(token1,affected_op);
}




//------------------------------------------------------------------------------------------------------------
PASTNode generateAST_Recursive(const char *s, int m_line, bool & error, TYPE_GROUP type_group=TYPE_GROUP::GROUP_0,PASTNode parent=NULL ){
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


	tASTNode *op=new tASTNode;
	op->parent=parent;

	
	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
	
	if(*aux==0){
		print_error_cr("no expression entry");
		error = true;
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
					op->left=op->right=NULL;
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
					PASTNode p_gr=generateAST_Recursive(subexpr,m_line,error,GROUP_0,op);
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
					return generateAST_Recursive(s,m_effective_start_line,error,(TYPE_GROUP)(((int)type_group)+1),op);
			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...
			char operator_str[10]={0};
			strncpy(operator_str,expr_op,expr_op_end-expr_op);


			print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};
			char eval_right[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s_effective_start,expr_op-s_effective_start); // copy its left side...
			op->left=generateAST_Recursive(eval_left,m_effective_start_line,error,type_group,op);

			if(error){
				return NULL;
			}

			if(pre_token!=""){
				op->left=preOperator(pre_token,op->left);
			}

			strncpy(eval_right,expr_op_end,strlen(expr_op)); // copy its right side...

			op->right=generateAST_Recursive(eval_right,m_line,error,type_group,op);
			if(error){
				return NULL;
			}


			if(!strcmp(operator_str,"-")) {
				op->right=preOperator("-",op->right);
				strcpy(operator_str,"+");
			}

			
			if(op->right == NULL && op->left == NULL){ // some wrong was happened
				return NULL;
			}
			
			op->token = operator_str;
		}
		
	}
	return op;
}

PASTNode generateAST(const char *s, int m_line ){
	bool error=false;
	return generateAST_Recursive(s,m_line,error);
}

		
		



