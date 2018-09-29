/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




#define DEFAULT_NO_FILENAME					"no_file"

namespace zetscript{

	class  CEval{

	public:
		static const char * current_parsing_filename;
		static int current_idx_parsing_filename;
		static tKeywordInfo defined_keyword[MAX_KEYWORD];
		static tDirectiveInfo defined_directive[MAX_DIRECTIVES];

		static tPunctuatorInfo defined_operator_punctuator[MAX_PUNCTUATORS];

		static char * eval_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error);
		/**
		 * Get CASTNode Node by its idx, regarding current state.
		 */

		static void 					init();



		static int						getCursorCompile();
		static void						resetCursorCompile();


		static const char * getSymbolValueConstCharByIdx(int idx);


		static void destroySingletons();


	private:


		static KEYWORD_TYPE 	isKeyword(const char *c);
		static DIRECTIVE_TYPE 	isDirective(const char *c);

		//-----------------------------------------------
		//
		static char * 	 isClassMember(const char *s,int & m_line, string & _class_name, string & var_name, bool & error, KEYWORD_TYPE ikw);

		// string generic utils...
		static char *getSymbolName(const char *s,int & m_line);
		static char * getEndWord(const char *s, int m_line);

		static bool printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line);



		static PUNCTUATOR_TYPE checkPreOperatorPunctuator(const char *s);
		static PUNCTUATOR_TYPE checkPostOperatorPunctuator(const char *s);


		static PUNCTUATOR_TYPE   isOperatorPunctuator(const char *s);
		static PUNCTUATOR_TYPE   isSpecialPunctuator(const char *s);
		static PUNCTUATOR_TYPE   isPunctuator(const char *s);


		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS

		// AST core functions ...
		static char * evalExpression(const char *s, int & m_line, CScope *scope_info);
		static char * evalExpression_Recursive(const char *s, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt);

		static char * evalExpressionSymbol(
				const char *current_string_ptr,
				int & m_line,
				CScope *scope_info,
				CASTNode **node,
				int & m_definedSymbolLine,
				PUNCTUATOR_TYPE pre_operator,
				PUNCTUATOR_TYPE & post_operator

		);

		// Punctuators...
		static bool evalExpressionPlusPunctuator(const char *s);
		static bool evalExpressionMinusPunctuator(const char *s);
		static bool evalExpressionMulPunctuator(const char *s);
		static bool evalExpressionDivPunctuator(const char *s);
		static bool evalExpressionModPunctuator(const char *s);

		static bool evalExpressionFieldPunctuator(const char *s);
		static bool evalExpressionInlineIfPunctuator(const char *s);
		static bool evalExpressionInlineElsePunctuator(const char *s);

		static bool evalExpressionAssignPunctuator(const char *s);
		static bool evalExpressionAddAssignPunctuator(const char *s);
		static bool evalExpressionSubAssignPunctuator(const char *s);
		static bool evalExpressionMulAssignPunctuator(const char *s);
		static bool evalExpressionDivAssignPunctuator(const char *s);
		static bool evalExpressionModAssignPunctuator(const char *s);


		static bool evalExpressionBinaryXorPunctuator(const char *s);
		static bool evalExpressionBinaryAndPunctuator(const char *s);
		static bool evalExpressionBinaryOrPunctuator(const char *s);
		static bool evalExpressionShiftLeftPunctuator(const char *s);
		static bool evalExpressionShiftRightPunctuator(const char *s);

		static bool evalExpressionLogicAndPunctuator(const char *s);
		static bool evalExpressionLogicOrPunctuator(const char *s);
		static bool evalExpressionLogicEqualPunctuator(const char *s);
		static bool evalExpressionInstanceOfPunctuator(const char *s);

		static bool evalExpressionLogicNotEqualPunctuator(const char *s);
		static bool evalExpressionLogicGreatherThanPunctuator(const char *s);
		static bool evalExpressionLogicLessThanPunctuator(const char *s);
		static bool evalExpressionLogicGreatherEqualThanPunctuator(const char *s);
		static bool evalExpressionLessEqualThanPunctuator(const char *s);
		static bool evalExpressionNotPunctuator(const char *s);

		static bool evalExpressionIncPunctuator(const char *s);
		static bool evalExpressionDecPunctuator(const char *s);


		static PUNCTUATOR_TYPE   evalExpressionArithmeticPunctuator(const char *s);

		static bool   isMarkEndExpression(char c);


		// NEW EXPRESSION...
		static char * evalExpressionNew(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated);


		// FUNCTION OBJECT...
		static char * evalExpressionFunctionObject(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated);
		static char * evalExpressionFunctionArrayAccess_Recursive(const char *str, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt,PASTNode parent=NULL);
		static char * evalExpressionFunctionArrayAccess(const char *str, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt,PASTNode parent);

		//STRUCT OBJECT...
		static char * evalExpressionStructObject_Recursive(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt);
		static char * evalExpressionStructObject(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt);

		//VECTOR OBJECT...
		static char * evalExpressionVectorObject_Recursive(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt);
		static char * evalExpressionVectorObject(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------

		// eval block { }
		static char * evalBlock(const char *s,int & m_line,  CScope *scope_info, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		static char * evalKeyWord(const char *s, int & m_start_line, CScope *scope_info, bool & error);

		static char * evalKeyWordIf(const char *s,int & m_line,  CScope *scope_info);
		static char * evalKeyWordFor(const char *s,int & m_line,  CScope *scope_info);

		static char * evalKeyWordWhile(const char *s,int & m_line,  CScope *scope_info);
		static char * evalKeyWordDoWhile(const char *s,int & m_line,  CScope *scope_info);
		static char * evalKeyWordSwitch(const char *s,int & m_line, CScope *scope_info);
		static char * evalKeyWordVar(const char *s,int & m_line,  CScope *scope_info);

		static char * evalKeyWordMemberVar(const char *s,int & m_line,  CScope *scope_info);
		static char * evalKeyWordReturn(const char *s,int & m_line,  CScope *scope_info);
		static char * evalKeyWordFunction(const char *s,int & m_line,  CScope *scope_info);

		static char * evalKeyWordDelete(const char *s,int & m_line,  CScope *scope_info);

		static char * evalKeyWordClass(const char *s,int & m_line,  CScope *scope_info);



		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------


		static char * evalArgs(char c1, char c2,const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated);


	};
}
