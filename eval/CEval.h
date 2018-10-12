/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




#define DEFAULT_NO_FILENAME					"no_file"


namespace zetscript{

	class  CEval{

	public:


		bool eval(const char *s);

		static void	init();

		static void destroySingletons();

	private:

		enum EXPRESSION_TOKEN:char{
			UNKNOW_TOKEN=0,
			SYMBOL_TOKEN,
			CONSTANT_TOKEN,
			PUNCTUATOR_TOKEN,
			OBJECT_TOKEN

		};


		struct tInfoAsmOpCompiler{
			ASM_OPERATOR operator_type;
			unsigned char index_op1; // index/type/etc
			intptr_t  index_op2; // usually a pointer or index
			unsigned short var_type;
			unsigned int pre_post_op_type;
			unsigned int scope_type;
			unsigned int runtime_prop;

			tInfoAsmOpCompiler(){
				operator_type=ASM_OPERATOR::END_FUNCTION;
				index_op1=ZS_UNDEFINED_IDX;
				index_op2=ZS_UNDEFINED_IDX;

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

		typedef struct {
			KEYWORD_TYPE id;
			const char *str;
			char * (* eval_fun)(const char *, int &, CScope *,tInfoFunctionCompile *, bool &);
		} tKeywordInfo;



		// singleton

		static map<string,tInfoConstantValue *> *constant_pool;

		static const char * current_parsing_filename;
		static int current_idx_parsing_filename;
		static tKeywordInfo defined_keyword[MAX_KEYWORD];
		static tDirectiveInfo defined_directive[MAX_DIRECTIVES];

		static tPunctuatorInfo defined_operator_punctuator[MAX_PUNCTUATORS];



		//---------------------------------------------------------------------------------------------------------------------------------------
		// CONSTANT TOOLS

		static tInfoConstantValue * getConstant(const string & const_name);
		static tInfoConstantValue * addConstant(const string & const_name, void *obj, unsigned short properties);




		static KEYWORD_TYPE 	isKeyword(const char *c);
		static DIRECTIVE_TYPE 	isDirective(const char *c);

		static bool evalNumberToken(const char *c, int & m_line, string & value, bool & error);


		//-----------------------------------------------
		//
		//static char * 	 isClassMember(const char *s,int & m_line, string & _class_name, string & var_name, bool & error, KEYWORD_TYPE ikw);

		// string generic utils...
		static bool   printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line);
		static bool   isVariableNameExpressionOk(const string & symbol, int m_line);
		static char *  getVariableToken(const char *s, const string & symbol);
		static bool	  isEndSymbolToken(char *s,char pre=0);
		static char * evalSymbol(const char *s, int m_line, string & value, SYMBOL_TYPE & ttv, vector<tInfoAsmOpCompiler *> 	**	asm_op=NULL);


		static PUNCTUATOR_TYPE   evalOperationalPunctuator(const char *s);

		static PUNCTUATOR_TYPE   evalOperatorPunctuator(const char *s);
		static PUNCTUATOR_TYPE   evalSpecialPunctuator(const char *s);
		static PUNCTUATOR_TYPE   evalPunctuator(const char *s);


		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS

		static bool   isCharacterEndExpression(char c);

		// AST core functions ...
		static char * evalExpression(const char *s, int & m_line, CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op);
		static char * evalExpression_Recursive(const char *s, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt);


		// NEW EXPRESSION...
		static char * evalExpressionNew(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op);

		// FUNCTION OBJECT...
		static char * evalExpressionFunctionObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op);

		//STRUCT OBJECT...
		static char * evalExpressionStructObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op);

		//VECTOR OBJECT...
		static char * evalExpressionVectorObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op);

		// GENERIC VECTOR/FUNCTION ARGS
		static char * evalExpressionArgs(char c1, char c2,const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------
		// Class

		static char * isClassMember(const char *s,int & m_line, short & idxScopeClass, bool & error);
		static char * evalKeywordClass(const char *s,int & m_line,  CScope *scope_info, bool & error);

		// eval block { }
		static char * evalBlock(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		static char * evalKeyword		(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordIf		(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordFor	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordWhile	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordDoWhile(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalBreak			(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalContinue		(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalDefaultCase	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordSwitch	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordVar	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordReturn	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordDelete	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);


		static char * evalKeywordFunction(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile ** ifc);


		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------


		static char * eval_Recursive(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);


	};
}
