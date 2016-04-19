#pragma once


#define MAX_EXPRESSION_LENGTH 8192

enum GROUP_TYPE{
	GROUP_0=0, // +,-,||
	GROUP_1, // *,/,==,>,<,<=,>=
	GROUP_2, // &&
	GROUP_3, // !
	MAX_GROUPS
};


enum NODE_TYPE{
	UNKNOWN_NODE=0,
	MAIN_NODE=1,
	EXPRESSION_NODE,
	KEYWORD_NODE,
	BLOCK_NODE,
	ARGUMENTS_NODE,
	SYMBOL_NODE,
	IF_NODE,
	CASE_NODE,
	ELSE_NODE,
	FOR_NODE,
	CONDITIONAL_NODE,
	FOR_PRE_NODE,
	FOR_POST_NODE,
	MAX_NODE_TYPE
};


enum KEYWORD_TYPE{
	NONE_KEYWORD=0,
	IF_KEYWORD,
	ELSE_KEYWORD,
	FOR_KEYWORD,
	WHILE_KEYWORD,
	VAR_KEYWORD,
	SWITCH_KEYWORD,
	CASE_KEYWORD,
	DEFAULT_KEYWORD,
	BREAK_KEYWORD,
	RETURN_KEYWORD,
	FUNCTION_KEYWORD,
	CLASS_KEYWORD,
	THIS_KEYWORD,
	MAX_KEYWORD
};

enum PUNCTUATOR_TYPE{
};

typedef struct{
	KEYWORD_TYPE id;
	const char *str;
}tInfoKeyword;


enum{
LEFT_NODE=0,
RIGHT_NODE
};

bool IS_WORD(const char *s);
char * GET_END_WORD(const char *s);



class tASTNode;
typedef tASTNode *PASTNode;

class tASTNode{
public:

	NODE_TYPE node_type;
	KEYWORD_TYPE keyword_type;
	string token;
	string 	value;
	string type_ptr;
	string type_class;
	int definedValueline;
	PASTNode parent;
	vector<PASTNode> children; //left,right;

	tASTNode(int preallocate_num_nodes=0){
		node_type = UNKNOWN_NODE;
		keyword_type = KEYWORD_TYPE::NONE_KEYWORD;
		definedValueline=-1;
		token="";
		value="";
		parent=NULL;

		if(preallocate_num_nodes > 0){
			for(int i = 0; i < preallocate_num_nodes; i++){
				children.push_back(new tASTNode());
			}
		}
	}

};

class CAst{
public:

	static tInfoKeyword str_op[MAX_KEYWORD];

	static void createSingletons();
	static void destroySingletons();



	static tInfoKeyword * is_keyword(const char *c);


	/**
	 * Given starting char, try find an expression ended with ; and return new char pointer.
	 * @s: current char
	 * @m_line: current line
	 * @node: nodes
	 */
	static char * generateAST_Recursive(const char *s, int m_line, CScriptFunction *sf, PASTNode *node);
	static PASTNode generateAST(const char *s, CScriptFunction *sf);

private:

	static PASTNode preOperator(string token,PASTNode affected_op);
	static PASTNode postOperator(string token,PASTNode affected_op);
	static char * parseExpression(const char *s, int m_line, bool & error, PASTNode * node  );
	static PASTNode parseExpression_Recursive(const char *s, int m_line, bool & error, GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL );

	static char * parseIf(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseFor(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseSwitch(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseBlock(const char *s,int & m_line,  CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated);
	static char * parseVar(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseKeyWord(const char *s, int & m_start_line, CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated);
	static char * generateAST_Recursive(const char *s, int m_line, CScriptFunction *sf, bool & error, PASTNode *node_to_be_evaluated);

};

