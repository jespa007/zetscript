#pragma once


#define MAX_EXPRESSION_LENGTH 8192


typedef struct _tASTExpression *PASTExpression;
typedef struct _tASTOperator *PASTOperator;

typedef struct _tASTExpression{
	string 	value;
	PASTOperator left,right;
}tASTExpression;


typedef struct _tASTOperator{
	char token;
	tASTExpression expr;	
}tASTOperator;


enum TYPE_GROUP{
	GROUP_0=0, // +,-,||
	GROUP_1, // */% etc
	MAX_GROUPS
};

int generateAsmCode(PASTOperator op, int & numreg);
PASTOperator generateAST(const char *s, TYPE_GROUP type_group=GROUP_0);