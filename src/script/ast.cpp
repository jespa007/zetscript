#include "zg_script.h"



char * token_group0(char *c){
	char *aux=c;
	// try number operators...
	if(*c=='+' || *c=='-'){
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

char * is_token(char *c){
	char *aux;
	if((aux=token_group0(c))!=0){
		return aux;
	}else if((aux=token_group1(c))!=0) {
		return aux;
	}else if((aux=token_group2(c))!=0) {
		return aux;
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------
inline char * IGNORE_SPACES(const char *s){
	char *aux=(char *)s;
	while((*aux)!=0 && ((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r')) {
		aux++;
	}
	return aux;
}
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
//------------------------------------------------------------------------------------------------------------
PASTOperator generateAST(const char *s, TYPE_GROUP type_group){
	//int index=0;
	char *aux=(char *)s;
	//char *start_value, * end_value;
	char *start_expression,*end_expression ;

	//char value[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
	tASTOperator *op=new tASTOperator;
	//bool theres_a_token=false;
	//bool first_start_parenthesis=false;
	
	aux=IGNORE_SPACES(aux);
	
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
			aux=IGNORE_SPACES(aux);
			
			if(is_token(aux)!=0){
				print_error_cr("unexpected token %c...",*aux);
				return NULL;
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
			
			aux=IGNORE_SPACES(aux);
			
			if(*aux!=0){

				expr_op=aux;
				char *adv_op;
				if((adv_op=is_token(expr_op))!=0){
					theres_some_token|=true;
					
					switch(type_group){
					case  GROUP_0:	expr_op_end = token_group0(expr_op);break;
					case GROUP_1:	expr_op_end = token_group1(expr_op);break;
					case GROUP_2:	expr_op_end = token_group2(expr_op);break;
					default: break;
					}
				}

				aux=adv_op; // advance operator...
			}
		}
		
		if(expr_op_end==0) {// there's no any operators \"type_group\"...
			if(!theres_some_token ){ // only we have a value (trivial)
			
				  if( *start_expression!='('){
					print_info_cr("trivial value %s",s);
					op->expr.left=op->expr.right=NULL;
					op->expr.value=GET_STR_WITHOUT_SPACES(s); // assign its value ...
					return op;
				  }else{
					print_info_cr("START:%c",*start_expression);
					char subexpr[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
					if((end_expression-start_expression-2)> MAX_EXPRESSION_LENGTH){
						printf("expression too long\n");
						return NULL;
					}
					
					strncpy(subexpr,start_expression+1,end_expression-start_expression-2); // copy sub expression					
					printf("expr:%s\n",subexpr);
					return generateAST(subexpr,GROUP_0);
				  
				}
			}
			else{ // there's a token, so let's perform generating its AST
					printf("try to generate group1 expression: %s\n",s);
					return generateAST(s,(TYPE_GROUP)(((int)type_group)+1));
			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...
			char operator_str[10]={0};
			strncpy(operator_str,expr_op,expr_op_end-expr_op);


			print_info_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};
			char eval_right[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s,expr_op-s); // copy its left side...
			op->expr.left=generateAST(eval_left,type_group);

			strncpy(eval_right,expr_op_end,strlen(expr_op)); // copy its right side...
			op->expr.right=generateAST(eval_right,type_group);
			
			if(op->expr.right == NULL || op->expr.left == NULL){ // some wrong was happened
				return NULL;
			}
			
			op->token = operator_str;
		}
		
	}

	return op;
}

		
		
int generateAsmCode(PASTOperator op, int & numreg){
	
	int r=0;
	if(op==NULL){
		return r;
	}
	
	if(op->expr.left==NULL && op->expr.right==NULL){ // trivial case value itself...



		print_info_cr("MOV\tE[%i],%s\n",numreg,op->expr.value.c_str());
		
		CZG_Script::getInstance()->insertMovInstruction(op->expr.value);
		
		r=numreg;	
	}else{ 

		int right=0, left=0;
	
		left=generateAsmCode(op->expr.left,numreg);
		right=generateAsmCode(op->expr.right,numreg);
		
		if(left == -1 || right == -1){ // error!
			return -1;
		}
		
		r=numreg;
		print_info_cr("%s\tE[%i],E[%i],E[%i]",op->token.c_str(),numreg,left,right);

		CZG_Script::getInstance()->insertOperatorInstruction(op->token,left,right);
				
	}
	numreg++;
	
	return r;
}		

		
		



