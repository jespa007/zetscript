#pragma once


#define MAX_EXPRESSION_LENGTH 8192

class tASTNode;
typedef tASTNode *PASTNode;

class tASTNode{
public:


	string token;
	string 	value;
	string type_ptr;
	string type_class;
	PASTNode parent;
	PASTNode left,right;

	tASTNode(){
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

int generateAsmCode(PASTNode op, int & numreg);
PASTNode generateAST(const char *s, TYPE_GROUP type_group=GROUP_0, PASTNode parent=NULL);
bool isVarDeclarationStatment(const char *statment, bool & error);
