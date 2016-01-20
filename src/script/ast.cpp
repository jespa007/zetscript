#include "zg_script.h"



bool token_group0(char c){	return c=='+' || c=='-';}
bool token_group1(char c){	return c=='*' || c=='/' || c=='%';}

bool is_token(char c){	return token_group0(c) || token_group1(c);}
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
	while((*aux)!=0 && !((*aux)==' ' || (*aux)=='\t' || (*aux)=='\n' || (*aux)=='\r') && !is_token(*aux)) {
		aux++;
	}
	return aux;
}
//------------------------------------------------------------------------------------------------------------
inline char * GET_CLOSED_PARENTHESIS(const char *s){
	bool end = false;
	char *aux = (char *)s;
	
	if(*aux != '('){
		printf("Error first character must be a parenthesis!\n" );
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
	int index=0;
	char *aux=(char *)s;
	char *start_value, * end_value;
	char *start_expression,*end_expression ;
	char value[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
	tASTOperator *op=new tASTOperator;
	bool theres_a_token=false;
	bool first_start_parenthesis=false;
	
	aux=IGNORE_SPACES(aux);
	
	if(*aux==0){
		printf("no expression entry\n");
		return NULL;
	}	
	
	if(type_group>=MAX_GROUPS) {
		printf("max groups \n");
		return NULL;
	}
	
		
	printf("new expression eval:\"%s\" group:%i\n",s,type_group);
	
	//if(type_group==GROUP_0) 
	{ // search for operator +/-...
		start_expression=aux;
		bool theres_some_token=false;
		bool end = false;
		char *expr_op=0;
		
		// searching for operator!
		printf("searching for operator type %i...\n",type_group);
	
		while(*aux!=0 && !end){
	
			printf("checkpoint1:%c\n",*aux);
			// 1. ignore spaces...
			aux=IGNORE_SPACES(aux);
			
			if(is_token(*aux)){
				printf("unexpected token %c...\n",*aux);
				return NULL;
			}
			
			printf("checkpoint2:%c\n",*aux);
			
			start_value=aux;
			if(*aux=='('){ // exp within ()
			
				printf("try find parenthesis close\n");
				end_expression = GET_CLOSED_PARENTHESIS(aux);
				
				if(end_expression==0){
					return NULL;
				}
				
				printf("checkpoint4:%c\n",*end_expression);
				
				if(*end_expression==0){
						printf("')' not found\n");
						return NULL;
				}else{ // advance )
					end_expression++;
				}				
			}else{ // value 
				end_expression = GET_END_WORD(aux);
			}
			
			aux=end_expression;
			
			printf("checkpoint3:%c\n",*aux);
			
			aux=IGNORE_SPACES(aux);
			
			if(*aux!=0){

				expr_op=aux;
				
				if(is_token(*expr_op)){
					theres_some_token|=true;
					
					switch(type_group){
					case  GROUP_0:	end = token_group0(*expr_op);break;
					case GROUP_1:	end = token_group1(*expr_op);break;
					default: break;
					}					
				}
				aux++; // advance operator...
			}
		}
		
		if(!end) {// there's no any operators \"type_group\"...
			if(!theres_some_token ){ // only we have a value (trivial)
			
				  if( *start_expression!='('){
					printf("trivial value %s\n",s);
					op->expr.left=op->expr.right=NULL;
					op->expr.value=s; // assign its value ...
					return op;
				  }else{
					printf("START:%c\n",*start_expression);
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
			printf("operator \"%c\" found we can evaluate left and right branches!!\n",*expr_op);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};
			char eval_right[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s,expr_op-s); // copy its left side...
			op->expr.left=generateAST(eval_left,type_group);

			strncpy(eval_right,expr_op+1,strlen(expr_op)); // copy its right side...
			op->expr.right=generateAST(eval_right,type_group);
			
			if(op->expr.right == NULL || op->expr.left == NULL){ // some wrong was happened
				return NULL;
			}
			
			op->token = *expr_op;		
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
		printf("MOV\tE[%i],%s\n",numreg,op->expr.value.c_str());
		
		// try parse value...
		if(CNumber::Parse(op->expr.value)!=NULL){
			printf("%s detected as number\n",op->expr.value.c_str());
		}
		else if(op->expr.value[0]=='\"' && op->expr.value[op->expr.value.size()-1]=='\"'){
			printf("%s detected as string\n",op->expr.value.c_str());
		}
		else if(CBoolean::Parse(op->expr.value)!=NULL){
			printf("%s detected as boolean\n",op->expr.value.c_str());
		}else{
			printf("ERROR: %s is unkown variable\n",op->expr.value.c_str());
			return -1;
		}
		
		r=numreg;	
	}else{ 

		int right=0, left=0;
	
		left=generateAsmCode(op->expr.left,numreg);
		right=generateAsmCode(op->expr.right,numreg);
		
		if(left == -1 || right == -1){ // error!
			return -1;
		}
		
		r=numreg;
		printf("%c\tE[%i],E[%i],E[%i]\n",op->token,numreg,left,right);
				
	}
	numreg++;
	
	return r;
}		

		
		



