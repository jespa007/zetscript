#pragma once


#define MAX_EXPRESSION_LENGTH 8192

class tASTNode;
class CLocalScope;
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
	PRE_INC, // ++
	POST_INC, // ++
	PRE_DEC, // ++
	POST_DEC, // ++
	LOGIC_AND, // &&
	LOGIC_OR,  // ||
	DIV, // /
	MUL, // *
	MOD,  // %
	CAT,  // str+str
	AND, // bitwise logic and
	OR, // bitwise logic or
	XOR, // logic xor
	SHL, // shift left
	SHR, // shift right
	PUSH_SCOPE,
	POP_SCOPE,
	JNT // goto if not true ... goes end to conditional.


};

int generateAsmCode(PASTNode op, CLocalScope *lc, int & numreg, bool & error);
PASTNode generateAST(const char *s, int & m_line,TYPE_GROUP type_group=GROUP_0, PASTNode parent=NULL);

