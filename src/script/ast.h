#pragma once


#define MAX_EXPRESSION_LENGTH 8192



enum TYPE_GROUP{
	GROUP_0=0, // +,-,||
	GROUP_1, // *,/,==,>,<,<=,>=
	GROUP_2, // &&
	GROUP_3, // !
	MAX_GROUPS
};



enum TYPE_KEYWORD{
	NONE=0,
	IF,
	ELSE,
	FOR,
	WHILE,
	VAR,
	SWITCH,
	CASE,
	DEFAULT,
	BREAK,
	RETURN,
	FUNCTION,
	CLASS,
	THIS,
	MAX_KEYWORD
};

typedef struct{
	TYPE_KEYWORD id;
	const char *str;
}tInfoKeyword;

class tASTNode;
class CScope;
typedef tASTNode *PASTNode;

class tASTNode{
public:


	string token;
	string 	value;
	string type_ptr;
	string type_class;
	int definedValueline;
	PASTNode parent;
	PASTNode left,right;

	tASTNode(){
		definedValueline=-1;
		token="";
		value="";
		parent=left=right=NULL;
	}

};



extern tInfoKeyword str_op[MAX_KEYWORD];
tInfoKeyword * is_keyword(const char *c);
bool IS_WORD(const char *s);
char * GET_END_WORD(const char *s);

PASTNode generateAST(const char *s, int m_line);

