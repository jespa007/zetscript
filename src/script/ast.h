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
	PUNCTUATOR_NODE,
	EXPRESSION_NODE,
	KEYWORD_NODE,
	FUNCTION_ARGS_DECL_NODE,
	FUNCTION_ARGS_CALL_NODE,
	ARRAY_INDEX_NODE,
	ARRAY_NODE,
	SYMBOL_NODE,
	IF_NODE,
	BODY_NODE,
	ELSE_NODE,
	FOR_NODE,
	FUNCTION_NODE,
	WHILE_NODE,
	CONDITIONAL_NODE,
	PRE_FOR_NODE,
	POST_FOR_NODE,
	MAX_NODE_TYPE
};


enum KEYWORD_TYPE{
	UNKNOWN_KEYWORD=0,
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

	UNKNOWN_PUNCTUATOR=0,

	//--------------------------------
	// OPERATORS

	PLUS_PUNCTUATOR=1,
	MINUS_PUNCTUATOR,
	MUL_PUNCTUATOR,
	DIV_PUNCTUATOR,
	MOD_PUNCTUATOR,

	ASSIGN_PUNCTUATOR,

	BINARY_XOR_PUNCTUATOR,
	BINARY_AND_PUNCTUATOR,
	BINARY_OR_PUNCTUATOR,
	SHIFT_LEFT_PUNCTUATOR,
	SHIFT_RIGHT_PUNCTUATOR,


	LOGIC_AND_PUNCTUATOR,
	LOGIC_OR_PUNCTUATOR,
	LOGIC_EQUAL_PUNCTUATOR,
	LOGIC_NOT_EQUAL_PUNCTUATOR,
	LOGIC_GT_PUNCTUATOR,
	LOGIC_LT_PUNCTUATOR,
	LOGIC_GET_PUNCTUATOR,
	LOGIC_LET_PUNCTUATOR,

	LOGIC_NOT_PUNCTUATOR,


	PRE_INC_PUNCTUATOR,
	PRE_DEC_PUNCTUATOR,

	POST_INC_PUNCTUATOR,
	POST_DEC_PUNCTUATOR,


	MAX_OPERATOR_PUNCTUATORS,


	//---------------------------
	// SPECIAL CHARACTERS

	COMA_PUNCTUATOR=1,
	SEMICOLON_PUNCTUATOR,
	FIELD_PUNCTUATOR,
	OPEN_PARENTHESIS_PUNCTUATOR,
	CLOSE_PARENTHESIS_PUNCTUATOR,

	OPEN_BRAKET_PUNCTUATOR,
	CLOSE_BRAKET_PUNCTUATOR,

	OPEN_SQUARE_BRAKET_PUNCTUATOR,
	CLOSE_SQUARE_BRAKET_PUNCTUATOR,

	MAX_SPECIAL_PUNCTUATORS


};





class tASTNode;
typedef tASTNode *PASTNode;

typedef struct{
	KEYWORD_TYPE id;
	const char *str;
	char * (* parse_fun )(const char *,int & ,  CScriptFunction *, PASTNode *);
}tInfoKeyword;

typedef struct{
	PUNCTUATOR_TYPE id;
	const char *str;
	bool (* parse_fun )(const char *);
}tInfoPunctuator;


enum{
	LEFT_NODE=0,
	RIGHT_NODE
};








class tASTNode{
public:

	NODE_TYPE node_type;
	tInfoKeyword *keyword_info;
	tInfoPunctuator *operator_info;
	string 	value_symbol;
	string type_ptr;
	string type_class;
	int definedValueline;
	PASTNode parent;
	vector<PASTNode> children; //left,right;

	tASTNode(int preallocate_num_nodes=0){
		node_type = UNKNOWN_NODE;
		keyword_info = NULL;
		definedValueline=-1;
		operator_info=NULL;
		value_symbol="";
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

	static tInfoKeyword defined_keyword[MAX_KEYWORD];
	static tInfoPunctuator defined_operator_punctuator[MAX_OPERATOR_PUNCTUATORS];
	static tInfoPunctuator defined_special_punctuator[MAX_SPECIAL_PUNCTUATORS];

	static void createSingletons();
	static void destroySingletons();

	static tInfoKeyword * isKeyword(const char *c);


	/**
	 * Given starting char, try find an expression ended with ; and return new char pointer.
	 * @s: current char
	 * @m_line: current line
	 * @node: nodes
	 */
	static char * generateAST_Recursive(const char *s, int m_line, CScriptFunction *sf, PASTNode *node);
	static PASTNode generateAST(const char *s, CScriptFunction *sf);

private:


	// string generic utils...
	static char *getSymbolName(const char *s,int & m_startLine);
	static char * getEndWord(const char *s);


	static PASTNode preNode(tInfoPunctuator * punctuator,PASTNode affected_op);
	//static PASTNode postOperator(tInfoPunctuator * punctuator,PASTNode affected_op);
	static char * parseExpression(const char *s, int m_line, CScriptFunction *sf, PASTNode * node  );
	static char * parseExpression_Recursive(const char *s, int m_line, CScriptFunction *sf, PASTNode *node,GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL );

	static char * parseBlock(const char *s,int & m_line,  CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated);



	// Punctuators...
	static bool parsePlusPunctuator(const char *s);
	static bool parseMinusPunctuator(const char *s);
	static bool parseMulPunctuator(const char *s);
	static bool parseDivPunctuator(const char *s);
	static bool parseModPunctuator(const char *s);

	static bool parseAssignPunctuator(const char *s);

	static bool parseBinaryXorPunctuator(const char *s);
	static bool parseBinaryAndPunctuator(const char *s);
	static bool parseBinaryOrPunctuator(const char *s);
	static bool parseShiftLeftPunctuator(const char *s);
	static bool parseShiftRightPunctuator(const char *s);

	static bool parseLogicAndPunctuator(const char *s);
	static bool parseLogicOrPunctuator(const char *s);
	static bool parseLogicEqualPunctuator(const char *s);
	static bool parseLogicNotEqualPunctuator(const char *s);
	static bool parseLogicGreatherThanPunctuator(const char *s);
	static bool parseLogicLessThanPunctuator(const char *s);
	static bool parseLogicGreatherEqualThanPunctuator(const char *s);
	static bool parseLessEqualThanPunctuator(const char *s);
	static bool parseNotPunctuator(const char *s);
	static bool parseNotEqualPunctuator(const char *s);

	static tInfoPunctuator *checkPreOperatorPunctuator(const char *s);
	static tInfoPunctuator *checkPostOperatorPunctuator(const char *s);

	static bool parseIncPunctuator(const char *s);
	static bool parseDecPunctuator(const char *s);



	static tInfoPunctuator * parsePunctuatorGroup0(const char *s);
	static tInfoPunctuator * parsePunctuatorGroup1(const char *s);
	static tInfoPunctuator * parsePunctuatorGroup2(const char *s);
	static tInfoPunctuator * parsePunctuatorGroup3(const char *s);

	static tInfoPunctuator *  isOperatorPunctuator(const char *s);
	static tInfoPunctuator *  isSpecialPunctuator(const char *s);
	static tInfoPunctuator * isPunctuator(const char *s);


	// keyword...

	static char * parseKeyWord(const char *s, int & m_start_line, CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated);

	static char * parseIf(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseFor(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseWhile(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseSwitch(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseVar(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseReturn(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);
	static char * parseFunction(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated);


	static char * parseArgs(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated, char c1, char c2);

	static char * generateAST_Recursive(const char *s, int m_line, CScriptFunction *sf, bool & error, PASTNode *node_to_be_evaluated);

};

