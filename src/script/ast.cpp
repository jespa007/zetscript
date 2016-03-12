#include "zg_script.h"



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
//------------------------------------------------------------------------------------------------------------
/*inline char * CStringUtils::IGNORE_BLANKS(const char *s){
	char *aux=(char *)s;
	while((*aux)!=0 && ((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r')) {
		aux++;
	}
	return aux;
}*/
//------------------------------------------------------------------------------------------------------------
inline char * GET_END_WORD(const char *s){
	char *aux=(char *)s;
	while((*aux)!=0 && !((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r') && (is_token(aux)==0)) {
		aux++;
	}
	return aux;
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
PASTNode generateAST(const char *s, int & m_line, TYPE_GROUP type_group,PASTNode parent ){
	//int index=0;
	char *aux=(char *)s;
	char *s_effective_start=(char *)s;
	char *start_expression,*end_expression ;
	string pre_token="";
	string post_token="";
	bool eval_preoperator=true;
	bool eval_postoperator=true;


	tASTNode *op=new tASTNode;
	op->parent=parent;

	
	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
	
	if(*aux==0){
		print_error_cr("no expression entry");
		return NULL;
	}	
	
	if(type_group>=MAX_GROUPS) {
		printf("max groups \n");
		return NULL;
	}
	
		
	print_info_cr("new expression eval:\"%s\" group:%i",s,type_group);
	
	//if(type_group==GROUP_0) 
	{ // search for operator +/-...
		start_expression=aux;
		bool theres_some_token=false;
		char * expr_op_end = 0;
		char *expr_op=0;
		
		// searching for operator!
		print_info_cr("searching for operator type %i...",type_group);
	
		while(*aux!=0 && (expr_op_end==0)){
	
			printf("checkpoint1:%c\n",*aux);
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

							print_info_cr("preoperator %s!",str_op);
							pre_token = str_op;
							aux=end_expression; // ignore first token...
							s_effective_start=end_expression;
							start_expression=aux;
						}else{ // just ignore it
							aux=end_expression; // ignore first token...
						}
				}
				else{
					print_error_cr("unexpected token %c...",*aux);
					return NULL;
				}
			}
			
			print_info_cr("checkpoint2:%c",*aux);
			
			//start_value=aux;
			if(*aux=='('){ // exp within ()
			
				print_info_cr("try find parenthesis close");
				end_expression = GET_CLOSED_PARENTHESIS(aux);
				
				if(end_expression==0){
					return NULL;
				}
				
				print_info_cr("checkpoint4:%c",*end_expression);
				
				if(*end_expression==0){
						print_error_cr("')' not found");
						return NULL;
				}else{ // advance )
					end_expression++;
				}				
			}else{ // value 
				end_expression = GET_END_WORD(aux);
			}
			
			aux=end_expression;
			
			print_info_cr("checkpoint3:%c\n",*aux);
			
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			
			// eval post operators...
			if(*aux != 0 && eval_postoperator){
				char *aux2;
				if((aux2=postoperator_token(aux, m_line))!=0){
					char str_op[10]={0};
					strncpy(str_op,aux,aux2-aux);
					print_info_cr("postperator %s!",str_op);
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
				}

				aux=adv_op; // advance operator...
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
					print_info_cr("trivial value %s",subexpr);
					op->left=op->right=NULL;
					op->value=GET_STR_WITHOUT_SPACES(subexpr); // assign its value ...


					if(pre_token!=""){ // generate a prenode operator..
						op=preOperator(pre_token,op);
					}
					else if(post_token!=""){ // generate a post node operator..
						op=postOperator(post_token,op);
					}

					return op;
				  }else{ // parenthesis!
					print_info_cr("START:%c",*start_expression);
					char subexpr[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
					if((end_expression-start_expression-2)> MAX_EXPRESSION_LENGTH){
						printf("expression too long\n");
						return NULL;
					}
					
					strncpy(subexpr,start_expression+1,end_expression-start_expression-2); // copy sub expression
					printf("expr:%s\n",subexpr);
					PASTNode p_gr=generateAST(subexpr,m_line,GROUP_0,op);

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
					printf("try to generate group1 expression: %s\n",s_effective_start);
					return generateAST(s,m_line,(TYPE_GROUP)(((int)type_group)+1),op);
			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...
			char operator_str[10]={0};
			strncpy(operator_str,expr_op,expr_op_end-expr_op);


			print_info_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};
			char eval_right[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s_effective_start,expr_op-s_effective_start); // copy its left side...
			op->left=generateAST(eval_left,m_line,type_group,op);

			if(pre_token!=""){
				op->left=preOperator(pre_token,op->left);
			}

			strncpy(eval_right,expr_op_end,strlen(expr_op)); // copy its right side...

			op->right=generateAST(eval_right,m_line,type_group,op);
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

	
int generateAsmCode(PASTNode op, CLocalScope *lc, int & numreg, bool & error){

	int r=0;
	if(op==NULL){
		return -1;
	}
	
	if(op->left==NULL && op->right==NULL){ // trivial case value itself...

		print_info_cr("CONST \tE[%i],%s\n",numreg,op->value.c_str());
		if(!lc->insertLoadValueInstruction(op->value, op->type_ptr)){
			error|=true;
			return -1;

		}

		r=numreg;
	}else{ 

		int right=0, left=0;
	
		left=generateAsmCode(op->left,lc,numreg,error);

		if(error) return -1;

		right=generateAsmCode(op->right,lc,numreg,error);
		
		if(error) return -1;

		r=numreg;

		if(left !=-1 && right!=-1){ // 2 ops

			// particular case if operator is =
			if(op->token == "="){
				// the variable can only assigned if the type is the same or if the type is undefined.
				// check if left operand is registered variable...
				CObject * var_obj = lc->getRegisteredVariable(op->left->value,false);
				if(var_obj == NULL){
					print_error_cr("undeclared variable \"%s\"");
					error|=true;
					return -1;
				}else{ // ok is declared ... let's see if undefined variable or is the same type ...
					bool is_undefined = dynamic_cast<CUndefined *>(var_obj) != NULL;
					string ptr_class_type =  lc->getUserTypeResultCurrentStatmentAtInstruction(right);

					if(ptr_class_type=="unknow"){
						error|=true;
						return -1;
					}

					if(is_undefined ||
							var_obj->getPointerClassStr() == ptr_class_type
						){

						if(is_undefined){ // create object ...

							var_obj = CFactoryContainer::getInstance()->newObjectByClassPtr(ptr_class_type);
							if(var_obj!=NULL){
								lc->defineVariable(op->left->value,var_obj);
								print_info_cr("%s defined as %s",op->left->value.c_str(),ptr_class_type.c_str());
							}else{
								print_error_cr("ERRRRRRRRRROR %s is not registered",ptr_class_type.c_str());
								error|=true;
								return -1;
							}
						}




					    // set value the operator = must be defined !
						if(!lc->insertMovVarInstruction(var_obj,right)){
							print_error_cr("ERRRRRRRRRROR 2");
							error|=true;
							return -1;
						}

						print_info_cr("MOV \tV(%s),E[%i]\n",op->left->value.c_str(),right);


					}else{
						print_error_cr("\"%s\" was instanced as \"%s\" and cannot be change type as \"%s\"",op->left->value.c_str(),var_obj->getPointerClassStr().c_str(),op->type_ptr.c_str());
						error|=true;
						return -1;
					}
				}
			}
			else{
				print_info_cr("%s\tE[%i],E[%i],E[%i]",op->token.c_str(),numreg,left,right);
				if(!lc->insertOperatorInstruction(op->token,left,right)){
					error|=true;
					return -1;
				}
			}

		}else if(right!=-1){ // one op..
			print_info_cr("%s\tE[%i],E[%i] !!!",op->token.c_str(),numreg,right);
			if(!lc->insertOperatorInstruction(op->token,right)){
				error|=true;
				return -1;
			}

		}else if(left!=-1){ // one op..
			print_info_cr("%s\tE[%i],E[%i] !!!",op->token.c_str(),numreg,left);
			if(!lc->insertOperatorInstruction(op->token,left)){
				error|=true;
				return -1;
			}

		}else{ // ERROR
			print_error_cr("ERROR both ops ==0!");
			error|=true;
			return -1;
		}
				
	}
	numreg++;
	
	return r;
}		

		
		



