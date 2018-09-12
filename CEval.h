#pragma once

namespace zetscript{

	class CEval{
	public:

		static CEval * getInstance();

		char * evalCode(const char *s, int & m_line, CScope *scope_info);

	private:

		typedef struct{
			string symbol_name;
		}tExpressionToken;

		typedef tStackElement tInfoConstantValue;

		struct tInfoAsmOpCompiler{
			ASM_OPERATOR operator_type;
			unsigned char index_op1; // index/type/etc
			intptr_t  index_op2; // usually a pointer or index
			short idxAstNode;
			unsigned short var_type;
			unsigned int pre_post_op_type;
			unsigned int scope_type;
			unsigned int runtime_prop;

			tInfoAsmOpCompiler(){
				operator_type=ASM_OPERATOR::END_FUNCTION;
				index_op1=ZS_UNDEFINED_IDX;
				index_op2=ZS_UNDEFINED_IDX;
				idxAstNode=ZS_UNDEFINED_IDX;

				var_type=0;
				pre_post_op_type=0;
				scope_type = 0;
				runtime_prop = 0;
			}

		};

		typedef struct {
			vector<tInfoAsmOpCompiler *> asm_op;
		}tContinueInstructionScope,tBreakInstructionScope;

		struct tInfoFunctionCompile{
			vector<tInfoAsmOpCompiler *> 			asm_op;
			CScriptFunctionObject 				*  	function_info_object;

			tInfoFunctionCompile(CScriptFunctionObject	* _function_info_object){
				function_info_object = _function_info_object;
			}

			~tInfoFunctionCompile(){

				for(unsigned j = 0;  j< asm_op.size();j++){
					delete asm_op[j];
				}

			}
		};

		static KEYWORD_TYPE 	isKeyword(const char *c);
		static DIRECTIVE_TYPE 	isDirective(const char *c);

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
		static bool parseAddAssignPunctuator(const char *s);
		static bool parseSubAssignPunctuator(const char *s);
		static bool parseMulAssignPunctuator(const char *s);
		static bool parseDivAssignPunctuator(const char *s);
		static bool parseModAssignPunctuator(const char *s);


		static bool parseBinaryXorPunctuator(const char *s);
		static bool parseBinaryAndPunctuator(const char *s);
		static bool parseBinaryOrPunctuator(const char *s);
		static bool parseShiftLeftPunctuator(const char *s);
		static bool parseShiftRightPunctuator(const char *s);

		static bool parseLogicAndPunctuator(const char *s);
		static bool parseLogicOrPunctuator(const char *s);
		static bool parseLogicEqualPunctuator(const char *s);
		static bool parseInstanceOfPunctuator(const char *s);

		static bool parseLogicNotEqualPunctuator(const char *s);
		static bool parseLogicGreatherThanPunctuator(const char *s);
		static bool parseLogicLessThanPunctuator(const char *s);
		static bool parseLogicGreatherEqualThanPunctuator(const char *s);
		static bool parseLessEqualThanPunctuator(const char *s);
		static bool parseNotPunctuator(const char *s);

		static bool parseIncPunctuator(const char *s);
		static bool parseDecPunctuator(const char *s);


		static PUNCTUATOR_TYPE checkPreOperatorPunctuator(const char *s);
		static PUNCTUATOR_TYPE checkPostOperatorPunctuator(const char *s);

		static PUNCTUATOR_TYPE   parseArithmeticPunctuator(const char *s);
		static PUNCTUATOR_TYPE   isOperatorPunctuator(const char *s);
		static PUNCTUATOR_TYPE   isSpecialPunctuator(const char *s);
		static PUNCTUATOR_TYPE  isPunctuator(const char *s);

		static char * 	 isClassMember(const char *s,int & m_line, string & _class_name, string & var_name, PASTNode & _class_node, bool & error, KEYWORD_TYPE ikw);

		// string generic utils...
		static char *getSymbolName(const char *s,int & m_startLine);
		static char * getEndWord(const char *s, int m_line);



		static CEval *eval;

		tKeywordInfo defined_keyword[MAX_KEYWORD];
		tDirectiveInfo defined_directive[MAX_DIRECTIVES];
		tPunctuatorInfo defined_operator_punctuator[MAX_PUNCTUATORS];

		map<string,tInfoConstantValue *> *constant_pool;

		CEval();


		char * evalFunction(const char *s,int & m_line,  CScope *scope_info);
		char * evalBlock(const char *s,int & m_line,  CScope *scope_info, bool & error, bool push_scope);
		static char * parseBlock(const char *s,int & m_line,  CScope *scope_info, bool & error, PASTNode *ast_node_to_be_evaluated, PASTNode parent=NULL,bool push_scope=true);


		// keyword...

		static char * parseKeyWord(const char *s, int & m_start_line, CScope *scope_info, bool & error);

		static char * parseStruct_Recursive(const char *s,int & m_line,  CScope *scope_info);
		static char * parseStruct(const char *s,int & m_line,  CScope *scope_info);
		static char * parseIf(const char *s,int & m_line,  CScope *scope_info);
		static char * parseFor(const char *s,int & m_line,  CScope *scope_info);
		//static char * parseForeach(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated);
		static char * parseWhile(const char *s,int & m_line,  CScope *scope_info);
		static char * parseDoWhile(const char *s,int & m_line,  CScope *scope_info);
		static char * parseSwitch(const char *s,int & m_line, CScope *scope_info);
		static char * parseVar(const char *s,int & m_line,  CScope *scope_info);
		static char * parseConst(const char *s,int & m_line,  CScope *scope_info);
		static char * parseMemberVar(const char *s,int & m_line,  CScope *scope_info);
		static char * parseReturn(const char *s,int & m_line,  CScope *scope_info);
		static char * parseFunction(const char *s,int & m_line,  CScope *scope_info);
		static char * parseNew(const char *s,int & m_line,  CScope *scope_info);
		static char * parseDelete(const char *s,int & m_line,  CScope *scope_info);

		static char * parseClass(const char *s,int & m_line,  CScope *scope_info);

		static char * parseArgs(char c1, char c2,const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated);
		static bool isMarkEndExpression(char c);

		/**
		 * this functions tries to evaluate expression that was get from getSymbolValue and didn't know as trivial expression like (), function(), etc.
		 * Must be evaluated later with this function.
		 */
		char *   deduceExpression(const char *str, int & m_line, CScope *scope_info);
		/**
		 * Try to get symbol. It can be trivial (i.e values or names) or not trivial ( inline functions, if-else, etc). At the end, the parse will perform to
		 * parse non-trivial symbols with a special function.
		 */
		char * getSymbolValue(
				const char *current_string_ptr,
				int & m_line,
				CScope *scope_info,
				string & symbol_name,
				int & m_definedSymbolLine,
				PUNCTUATOR_TYPE pre_operator,
				PUNCTUATOR_TYPE & post_operator,

		);
		// AST core functions ...
		char * evalExpression(const char *s, int & m_line, CScope *scope_info);
		char * generateExpressionTokens(const char *s, int & m_line, vector<tExpressionToken> *et);
		char * evalBody_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error);
	};

}
