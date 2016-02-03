#pragma once


#define MAX_EXPRESSION_LENGTH 8192

class tASTOperator;
typedef tASTOperator *PASTOperator;

class tASTOperator{
public:


	string token;
	string 	value;
	PASTOperator parent;
	PASTOperator left,right;

	tASTOperator(){
		token="";
		value="";
		parent=left=right=NULL;
	}

};


enum TYPE_GROUP{
	GROUP_0=0, // +,-,||
	GROUP_1, // *,/,==,>,<,<=,>=
	GROUP_2, // &&
	GROUP_3, // !
	MAX_GROUPS
};

int generateAsmCode(PASTOperator op, int & numreg);
PASTOperator generateAST(const char *s, TYPE_GROUP type_group=GROUP_0, PASTOperator parent=NULL);
