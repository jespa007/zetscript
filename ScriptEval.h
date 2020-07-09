/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




namespace zetscript{

	namespace ScriptEval{

		bool Eval(const char *s, const char *_source_filename=NULL, int _line=1);
/*
		 void IniVars();

		 //int					GetIdxScopeFromSymbolRef(const std::string & symbol_ref);
		 void pushFunction(ScriptFunction *sf);
		 void popFunction();

		 KeywordType 	IsKeywordType(const char *c);
		 DirectiveType 	IsDirectiveType(const char *c);

		 char * EvalLiteralNumber(const char *c, int & line, std::string & value, bool & error);

		// LINK
		 //void linkSymbols();
		 std::string * GetCompiledSymbol(const std::string & s);


		//-----------------------------------------------
		//
		// char * 	 IsClassMember(const char *s,int & line, std::string & _class_name, std::string & var_name, bool & error, KeywordType ikw);

		// std::string generic utils...
		 bool 	IsLiteral(const std::string & s);
		 bool   	isIdentifierNameExpressionOk(const std::string & symbol, int line);
		 char *  	GetIdentifierToken(const char *s, std::string & symbol);
		 bool	  	IsEndSymbolToken(char *s,char pre=0);
		 bool 	IsAccessPunctuator(char *s);
		 char * 	EvalSymbol(const char *s, int line, ScriptEval::TokenNode *  token);
		 bool     EndExpression(const char * s);

		 SeparatorType  		EvalSeparatorType(const char *s);
		 OperatorType   		EvalOperatorType(const char *s);
		 PreOperatorType   	EvalPreOperatorType(const char *s);
		 IdentityOperatorType   evalPrePostIdentityOperator(const char *s);

		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS

		 void LinkAccessorSymbol(TokenNode & node);

		// AST core functions ...

		/// Turn tokens into byte code throught semi AST algorithm ...
		 ByteCode PunctuatorToInstruction(OperatorType op);

		/// Make operator precedence from the AST of tokens built during EvalExpression.
		 bool  MakeOperatorPrecedence(std::vector<TokenNode> * vExpressionTokens,std::vector<OpCodeInstructionEval> *instruction,int idx_start,int idx_end,bool & error);

		/// Turn expression in Tokens (with some byte code instructions inside)...
		 char * EvalExpression(const char *s, int & line, Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);


		/// BYTE_CODE_NEW EXPRESSION...
		 char * EvalNewObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);

		/// FUNCTION OBJECT...
		 char * EvalFunctionObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 	*	instruction);

		/// STRUCT OBJECT...
		 char * EvalDictionaryObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);

		/// VECTOR OBJECT...
		 char * EvalVectorObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);

		/// GENERIC VECTOR/FUNCTION ARGS
		 char * EvalExpressionArgs(char c1, char c2,const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------
		// Class

		 char * IsClassMember(const char *s,int & line, ScriptClass **sc,std::string & member_symbol, bool & error);
		 char * EvalKeywordTypeClass(const char *s,int & line,  Scope *scope_info, bool & error);

		// eval block { }
		 char * EvalBlock(const char *s,int & line,  Scope *scope_info, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		 char * evalKeyword		(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordIf		(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordFor	(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordWhile	(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordDoWhile(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalBreak			(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalContinue		(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalDefaultCase	(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordSwitch	(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordVar	(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordReturn	(const char *s, int & line, Scope *scope_info, bool & error);
		 char * evalKeywordDelete	(const char *s, int & line, Scope *scope_info, bool & error);

		 char * evalKeywordFunction(const char *s,int & line,  Scope *scope_info, bool & error);

		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------



		 char * eval_Recursive(const char *s, int & line, Scope *scope_info, bool & error);

	};*/
	}
}
