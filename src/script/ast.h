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


enum ASM_OPERATOR{
	UNKNOW=0,
	MOV=1, // mov expression to var
	LOAD, // primitive value like number/string or boolean...
	EQU,  // ==
	LT,  // <
	LTE,  // <=
	NOT, // !
	GT,  // >
	GTE, // >=
	NEG, // !
	ADD, // +
	INC, // ++
	AND, // &&
	OR,  // ||
	DIV, // /
	MUL, // *
	MOD,  // %
	CAT  // str+str


};

int generateAsmCode(PASTNode op, int & numreg, bool & error);
PASTNode generateAST(const char *s, TYPE_GROUP type_group=GROUP_0, PASTNode parent=NULL);

