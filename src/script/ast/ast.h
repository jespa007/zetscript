#pragma once

#include "script/CScriptClassFactory.h"

#define MAX_EXPRESSION_LENGTH 8192

enum GROUP_TYPE{
	GROUP_0=0, // +,-,||
	GROUP_1, // *,/,==,>,<,<=,>=
	GROUP_2, // &&
	GROUP_3, // !
	MAX_GROUPS
};









class CScope;








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
	static bool generateAST(const char *s, tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated);



private:


	// string generic utils...
	static char *getSymbolName(const char *s,int & m_startLine);
	static char * getEndWord(const char *s, int m_line);


	//static PASTNode preNodePunctuator(tInfoPunctuator * punctuator,PASTNode affected_op);
	//static PASTNode postOperator(tInfoPunctuator * punctuator,PASTNode affected_op);

	static bool printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line);

	// Punctuators...
	static bool parsePlusPunctuator(const char *s);
	static bool parseMinusPunctuator(const char *s);
	static bool parseMulPunctuator(const char *s);
	static bool parseDivPunctuator(const char *s);
	static bool parseModPunctuator(const char *s);

	static bool parseFieldPunctuator(const char *s);
	static bool parseInlineIfPunctuator(const char *s);
	static bool parseInlineElsePunctuator(const char *s);

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


	// AST core functions ...
	static char * generateAST_Recursive(const char *s, int & m_line, tInfoRegisteredFunctionSymbol *sf, bool & error, PASTNode *node_to_be_evaluated=NULL, bool allow_breaks = false);
	static char * parseExpression(const char *s, int & m_line, tInfoRegisteredFunctionSymbol *sf, PASTNode * ast_node_to_be_evaluated=NULL);
	static char * parseExpression_Recursive(const char *s, int & m_line, tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL,GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL);

	/**
	 * this functions tries to evaluate expression that was get from getSymbolValue and didn't know as trivial expression like (), function(), etc.
	 * Must be evaluated later with this function.
	 */
	static char *   deduceExpression(const char *str, int & m_line, tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL, PASTNode parent=NULL);


	// parse block { }
	static char * parseBlock(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, bool & error, PASTNode *ast_node_to_be_evaluated=NULL, bool push_scope=true);


	// keyword...

	static char * parseKeyWord(const char *s, int & m_start_line, tInfoRegisteredFunctionSymbol *sf, bool & error, PASTNode *ast_node_to_be_evaluated=NULL);

	static char * parseIf(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseFor(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseWhile(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseSwitch(const char *s,int & m_line, tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseVar(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseReturn(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseFunction(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseNew(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static char * parseDelete(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);

	static char * parseClass(const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf,PASTNode *ast_node_to_be_evaluated);


	static char * parseArgs(char c1, char c2,const char *s,int & m_line,  tInfoRegisteredFunctionSymbol *sf, PASTNode *ast_node_to_be_evaluated=NULL);
	static bool isMarkEndExpression(char c);

	/**
	 * Try to get symbol. It can be trivial (i.e values or names) or not trivial ( inline functions, if-else, etc). At the end, the parse will perform to
	 * parse non-trivial symbols with a special function.
	 */
	static char * getSymbolValue(
			const char *current_string_ptr,
			int & m_line,
			tInfoRegisteredFunctionSymbol *sf,
			string & symbol_name,
			int & m_definedSymbolLine,
			tInfoPunctuator *pre_operator,
			tInfoPunctuator **post_operator,
			bool & is_symbol_trivial
	);

};

