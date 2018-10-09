/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "CZetScript.h"



#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr zs_print_debug_cr
#else
#define print_eval_cr(s,...)
#endif


namespace zetscript{


	void  		writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);

	CEval::tKeywordInfo CEval::defined_keyword[MAX_KEYWORD];
	tDirectiveInfo CEval::defined_directive[MAX_DIRECTIVES];
	tPunctuatorInfo CEval::defined_operator_punctuator[MAX_PUNCTUATORS];

	const char * CEval::current_parsing_filename=DEFAULT_NO_FILENAME;
	int CEval::current_idx_parsing_filename=-1;



	bool IS_SINGLE_COMMENT(char *str){

		if((*str!=0) && *str=='/'){
			return *(str+1)=='/';
		}
		return false;
	}

	bool IS_START_COMMENT(char *str){

		if((*str!=0) && *str=='/'){
			return *(str+1)=='*';
		}
		return false;
	}

	bool IS_END_COMMENT(char *str){

		if((*str!=0) && *str=='*'){
			return *(str+1)=='/';
		}
		return false;
	}

	char *ADVANCE_TO_END_COMMENT(char *aux_p, int &m_line){

		if(IS_START_COMMENT(aux_p)){
			aux_p+=2; //advance first
			bool end = false;
			while(*aux_p != 0  && !end){//!IS_END_COMMENT(aux_p) && *aux_p != 0){

				if(*aux_p == '*' && *(aux_p+1) == '/') {
					end=true;
				} // not end comment ... advance ...
				else {
					if(*aux_p=='\n'){
						m_line++;
					}
					aux_p++;
				}
			}
		}
		return aux_p;
	}

	char *IGNORE_BLANKS(const char *str, int &m_line) {
		char *aux_p = (char *)str;
		bool end = false;
		while(!end){
			end = true;
			while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

			if(IS_SINGLE_COMMENT(aux_p)) // ignore line
				while(*aux_p!=0 && *aux_p!='\n') aux_p++;

			else if(IS_START_COMMENT(aux_p)){
				// ignore until get the end of the comment...
				aux_p = ADVANCE_TO_END_COMMENT(aux_p, m_line);

				if(IS_END_COMMENT(aux_p))
					aux_p+=2;

				end=false;
			}
			// make compatible windows format...
			if(*aux_p == '\r')
				aux_p++;

			if(*aux_p == '\n') {
				m_line=m_line+1;
				end=false;
				aux_p++;
			}
		}
		return aux_p;
	}

	bool isPlusPunctuator(const char *s){
		if(*s=='+')
			return ((*(s+1) != '+') && (*(s+1) != '='));
		return false;
	}

	bool isMinusPunctuator(const char *s){
		if(*s=='-')
			return ((*(s+1) != '-') && (*(s+1) != '='));
		return false;
	}

	bool isMulPunctuator(const char *s){
		if(*s == '*')
			return ((*(s+1) != '='));
		return false;
	}

	bool isDivPunctuator(const char *s){
		if(*s == '/')
			return ((*(s+1) != '='));
		return false;
	}

	bool isModPunctuator(const char *s){
		if(*s == '%')
			return ((*(s+1) != '='));
		return false;
	}

	bool isFieldPunctuator(const char *s){
		return *s == '.';
	}

	bool isInlineIfPunctuator(const char *s){
		return *s == '?';
	}

	bool isInlineElsePunctuator(const char *s){
		return *s == ':';
	}

	bool isAssignPunctuator(const char *s){
		if(*s=='=')
			return (*(s+1) != '=');
		return false;
	}

	bool isAddAssignPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) == '=');
		return false;
	}

	bool isSubAssignPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) == '=');
		return false;
	}

	bool isMulAssignPunctuator(const char *s){
		if(*s=='*')
			return (*(s+1) == '=');
		return false;
	}

	bool isDivAssignPunctuator(const char *s){
		if(*s=='/')
			return (*(s+1) == '=');
		return false;
	}

	bool isModAssignPunctuator(const char *s){
		if(*s=='%')
			return (*(s+1) == '=');
		return false;
	}


	bool isBinaryXorPunctuator(const char *s){
		if(*s == '^')
			return ((*(s+1) != '='));
		return false;
	}

	bool isBinaryAndPunctuator(const char *s){
		if(*s=='&')
			return ((*(s+1) != '&')  && (*(s+1) != '='));
		return false;
	}

	bool isBinaryOrPunctuator(const char *s){
		if(*s=='|')
			return ((*(s+1) != '|') &&  (*(s+1) != '='));
		return false;
	}

	bool isShiftLeftPunctuator(const char *s){
		if(*s=='<')
			return (*(s+1) == '<');
		return false;
	}

	bool isShiftRightPunctuator(const char *s){
		if(*s=='>')
			return (*(s+1) == '>');
		return false;
	}

	bool isLogicAndPunctuator(const char *s){
		if(*s=='&')
			return (*(s+1) == '&');
		return false;
	}

	bool isLogicOrPunctuator(const char *s){
		if(*s=='|')
			return (*(s+1) == '|');
		return false;
	}

	bool isLogicEqualPunctuator(const char *s){
		if(*s=='=')
			return (*(s+1) == '=');
		return false;
	}

	bool isInstanceOfPunctuator(const char *s){
		return strncmp("instanceof",s,10) == 0;
	}

	bool isLogicNotEqualPunctuator(const char *s){
		if(*s=='!')
			return (*(s+1) == '=');
		return false;
	}

	bool isLogicGreatherThanPunctuator(const char *s){
		if( *s == '>')
			return (*(s+1) != '>');
		return false;
	}

	bool isLogicLessThanPunctuator(const char *s){
		if(*s == '<')
			return (*(s+1) != '<');

		return false;
	}

	bool isLogicGreatherEqualThanPunctuator(const char *s){
		if(*s=='>')
			return (*(s+1) == '=');
		return false;
	}

	bool isLessEqualThanPunctuator(const char *s){
		if(*s=='<')
			return (*(s+1) == '=');
		return false;
	}

	bool isNotPunctuator(const char *s){
		if(*s=='!')
			return (*(s+1) != '=');
		return false;
	}


	bool isIncPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) == '+');
		return false;
	}

	bool isDecPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) == '-');
		return false;
	}


	PUNCTUATOR_TYPE   CEval::evalOperatorPunctuator(const char *s){

		for(unsigned char  i = 0; i < MAX_OPERATOR_PUNCTUATORS; i++){
			if(defined_operator_punctuator[i].eval_fun != NULL){
				if(defined_operator_punctuator[i].eval_fun(s)){
					return defined_operator_punctuator[i].id;
				}
			}
		}
		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE   CEval::evalSpecialPunctuator(const char *s){

		for(unsigned char i = START_SPECIAL_PUNCTUATORS; i < MAX_SPECIAL_PUNCTUATORS; i++){

			if(*defined_operator_punctuator[i].str == *s){
				return defined_operator_punctuator[i].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}


	PUNCTUATOR_TYPE   CEval::evalOperationalPunctuator(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				SHIFT_LEFT_PUNCTUATOR, // <<
				SHIFT_RIGHT_PUNCTUATOR, // >>

				LOGIC_AND_PUNCTUATOR, // &&
				LOGIC_OR_PUNCTUATOR, // ||
				LOGIC_EQUAL_PUNCTUATOR, // =
				LOGIC_NOT_EQUAL_PUNCTUATOR, // !=
				LOGIC_GTE_PUNCTUATOR, // >=
				LOGIC_LTE_PUNCTUATOR, // <=

				INSTANCEOF_PUNCTUATOR, // instanceof

				// Then OPERATORS 1 char size
				ADD_PUNCTUATOR, // +
				SUB_PUNCTUATOR, // -
				MUL_PUNCTUATOR, // *
				DIV_PUNCTUATOR, // /
				MOD_PUNCTUATOR, // %

				ASSIGN_PUNCTUATOR, // =
				ADD_ASSIGN_PUNCTUATOR, // +=
				SUB_ASSIGN_PUNCTUATOR, // -=
				MUL_ASSIGN_PUNCTUATOR, // *=
				DIV_ASSIGN_PUNCTUATOR, // /=
				MOD_ASSIGN_PUNCTUATOR, // %=

				BINARY_XOR_PUNCTUATOR, // ^
				BINARY_AND_PUNCTUATOR, // &
				BINARY_OR_PUNCTUATOR, // |

				LOGIC_GT_PUNCTUATOR, // >
				LOGIC_LT_PUNCTUATOR, // <
				LOGIC_NOT_PUNCTUATOR // !
		};

		for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].eval_fun == NULL){
				THROW_RUNTIME_ERROR("internal: %s not have eval function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].eval_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}
		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;;
	}

	PUNCTUATOR_TYPE   CEval::evalPunctuator(const char *s){

		PUNCTUATOR_TYPE ip = isOperatorPunctuator(s);

		if(ip!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			return ip;
		}

		return isSpecialPunctuator(s);
	}

	bool CEval::endSymbol(char c, char pre){
		return evalPuntuator(c)!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR || c==' ' || c==0 || (c=='\"' && pre!='\\');
	}

	// to string utils ...
	char * CEval::getSymbol(const char *start_word, int m_line, string & value, SYMBOL_TYPE & ttv, vector<tInfoAsmOpCompiler *> 	**	asm_op){
		// PRE:

		char *start_str=(char *)start_word;
		char *aux=(char *)start_word;
		PUNCTUATOR_TYPE sp;
		KEYWORD_TYPE key_w;
		 bool is_possible_number=false;
		 int i=0;
		 bool start_digit = false;
		 ttv=SYMBOL_TYPE::UNKNOW_SYMBOL_TYPE;

		 // first solve embed objects...
		 if(*aux=='['){

		 }else if(*aux=='{'){

		 }else if(strncmp(aux,"function",8) && endSymbol(*(aux+8))){

		 }else { // try eval variable or constant integer ...
			value="";
			char pre=0;

			if(*aux=='\"'){

				aux++;
			}

			while(!endSymbol(*aux,pre)){
				pre=*aux;
				value += (*aux++);
			}

			// compile constant ...





			/*if(*aux == '\"'){ // string constant ...
						aux++;
						while((*aux)!=0 && !((*aux)=='\n') && !((*aux)=='\"' && *(aux-1) !='\\')) {
							aux++;
						}

						if(*aux != '\"') {
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Error \" not closed");
							return NULL;
						}
						aux++;
						ttv = SYMBOL_TYPE::CONSTANT_SYMBOL_TYPE;*/
		}


		//value = CZetScriptUtils::copyStringFromInterval(start_word,aux);

		return aux;
	}


	bool CEval::isVariableNameExpressionOk(const string & symbol, int m_line){

		char *aux_p = (char *)symbol.c_str();

		if(*aux_p!=0 && (
		   ('a' <= *aux_p && *aux_p <='z') ||
		   ('A' <= *aux_p && *aux_p <='Z') ||
		   *aux_p == '_'
		 )
		){ // let's see it has right chars...

			aux_p++;
			while((*aux_p!=0) && (('a' <= *aux_p && *aux_p <='z') ||
				  ('0' <= *aux_p && *aux_p <='9') ||
				  (*aux_p=='_') ||
				  ('A' <= *aux_p && *aux_p <='Z'))){
				aux_p++;
			}
		}else{
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line," Symbol name cannot begin with %c", *aux_p);
			return false;
		}

		return true;
	}

	KEYWORD_TYPE CEval::isKeyword(const char *c){
		int m_line=0;
		char *str=IGNORE_BLANKS(c,m_line);

		for(int i = 0; i < MAX_KEYWORD; i++){
			int size = strlen(defined_keyword[i].str);
			char *aux = str+size;
			if((strncmp(str,defined_keyword[i].str,size)==0) && (
					*aux == 0  || // carry end
					*aux == ' '  || // space

					*aux == '\t'  || // tab
					isOperatorPunctuator(aux)!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR ||
					isSpecialPunctuator(aux)!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR ||
					*aux == '\n' || // carry return

				   (*aux == '/' && *(aux+1) == '*')) //start block comment
				   ){
				return defined_keyword[i].id;
			}
		}
		return KEYWORD_TYPE::UNKNOWN_KEYWORD;
	}

	DIRECTIVE_TYPE CEval::isDirective(const char *c){
		int m_line=0;
		char *str=IGNORE_BLANKS(c,m_line);

		for(int i = 0; i < MAX_DIRECTIVES; i++){
			if(defined_directive[i].str){
				int size = strlen(defined_directive[i].str);

				if(strncmp(str,defined_directive[i].str,size)==0)
				{
					return defined_directive[i].id;
				}
			}
		}
		return DIRECTIVE_TYPE::UNKNOWN_DIRECTIVE;
	}
	//------------------------------------------------------------------------------------------------------------


	char * CEval::evalExpressionFunctionObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op){

		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

		return NULL;
	}


	char * CEval::evalExpressionStructObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			string symbol_value;
			char *end_p;
			PASTNode attr_node = NULL;
			int m_lineSymbol;

			if(*aux_p == '{'){ // go for final ...

				// this solve problem void structs...
				aux_p=IGNORE_BLANKS(aux_p+1,m_line);

				while (*aux_p != '}' && *aux_p != 0){

					m_lineSymbol = m_line;

					// expect word...
					if((end_p = getSymbol(aux_p, m_line,symbol_value))==NULL){
						 writeErrorMsg(CURRENT_PARSING_FILENAME,m_lineSymbol ,"Expected symbol after ','");
						 return NULL;
					 }

					 aux_p=IGNORE_BLANKS(end_p,m_line);

					 if(*aux_p != ':'){ // expected : ...
						 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ':'");
						 return NULL;
					 }

					 aux_p++;

					 // go to variable...
					 if((aux_p=evalExpression(aux_p,m_line,scope_info,NULL)) == NULL){  //IGNORE_BLANKS(aux_p+1,m_line);
						 return NULL;
					 }

					 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...


					 aux_p=IGNORE_BLANKS(aux_p,m_line);

					 if(*aux_p == ','){
						 aux_p=IGNORE_BLANKS(aux_p+1,m_line);
					 }
					 else if(*aux_p != '}' ){
						 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected '}' or ','");
						 return NULL;
					 }
				}
			}
			else{
				writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '{'");
			}
			return aux_p;
	}


	char * CEval::evalExpressionVectorObject(const char *s,int & m_line,  CScope *scope_info,  vector<tInfoAsmOpCompiler *> *	asm_op){

		char * aux_p=IGNORE_BLANKS(s,m_line);

		if(*aux_p != '['){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '['");
			return NULL;
		}

		aux_p=IGNORE_BLANKS(aux_p+1,m_line);
		unsigned v_elements=0;

		while(*aux_p!=0 && *aux_p != ']'){

			aux_p=IGNORE_BLANKS(aux_p,m_line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,",");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+1,m_line);
			}

			aux_p=evalExpression(aux_p,m_line,scope_info,NULL);

			v_elements++;
		}

		return aux_p;

	}

	char * CEval::evalExpressionNew(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;
		string symbol_value;

		KEYWORD_TYPE key_w;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::NEW_KEYWORD){
				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),m_line);
				// try get symbol ...

				if((end_p = getSymbol(aux_p, m_line,symbol_value))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected symbol");
					 return NULL;
				 }


				 aux_p=IGNORE_BLANKS(end_p,m_line);

				 if(*aux_p != '('){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' after \'%s\'",defined_keyword[key_w].str);
					 return NULL;
				 }

				 aux_p = evalExpressionArgs('(', ')',aux_p,m_line,scope_info,asm_op);
				 if(aux_p == NULL){
					 return NULL;
				 }

				return aux_p;
			}
		}
		return NULL;
	}


	bool CEval::isCharacterEndExpression(char c){
		return (c==0 || c==';' || c==',' ||  c==')'  || c==']' || c=='}');//|| c==':');
	}
	//-----------------------------------------------------------------------------------------------------------


	char * CEval::evalExpression(const char *s, int & m_line, CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op){
		// PRE: s is current string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.

		vector<CASTNode *> vt;
		PASTNode ast_node_to_be_evaluated=NULL;
		char *aux_p = s;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		while(!isCharacterEndExpression(*aux_p)){

			vector<tInfoAsmOpCompiler *> *tokenCompiled = NULL;
			PUNCTUATOR_TYPE pt = PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			SYMBOL_TYPE ttv= SYMBOL_TYPE::UNKNOW_SYMBOL_TYPE;
			string tokenValue;

			// check if punctuator...
			if((pt=isPunctuator(aux_p))!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){

			}else if(aux_p = getSymbol(aux_p,m_line,tokenValue,ttv,&tokenCompiled)){

			}


			// get symbol ?

			aux_p=IGNORE_BLANKS(aux_p,m_line);

		}




		// last character is in charge of who is calling evalExpression because there's many ending cases ): [ ';' ',' ')' , ']' ]
		char *aux=evalExpression_Recursive(s,m_line,scope_info,&ast_node_to_be_evaluated,vt);

		if(aux!=NULL){
			// order ast nodes ...

		}

		return aux;

	}

	//---------------------------------------------------------------------------------------------------------------
	// PARSE OBJECT FUNCTIONS


	//---------------------------------------------------------------------------------------------------------------
	// PARSE KEYWORDS



	char * CEval::evalKeywordDelete(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;
		string symbol_value;
		KEYWORD_TYPE key_w;
		PASTNode   value;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::DELETE_KEYWORD){

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),m_line);

				if((end_p = getSymbol(aux_p, m_line,symbol_value))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected symbol");
					 return NULL;
				 }


				 aux_p=IGNORE_BLANKS(aux_p,m_line);

				 if(*aux_p != ';'){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ;");
					 return NULL;
				 }

				return aux_p;
			}
		}
		return NULL;
	}

	char * CEval::isClassMember(const char *s,int & m_line,short & idxScopeClass, bool & error){

		char *aux_p = (char *)s;
		char *end_var;
		string class_name;
		idxScopeClass=-1;
		CScriptClass *sc=NULL;

		error = true;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check whwther the function is anonymous or not.
		if((end_var=getSymbol(aux_p,m_line,class_name))==NULL){
			return NULL;
		}

		aux_p=end_var;
		aux_p=IGNORE_BLANKS(aux_p,m_line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

			if((sc=CScriptClass::getScriptClassByName(class_name)) != NULL){

				idxScopeClass=sc->scope_info.idxScope;

				return aux_p+2;

			}else{
				error=true;
			}

		}
		return NULL;
	}


	char * CEval::evalKeywordClass(const char *s,int & m_line, CScope *scope_info, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;

		CScope *class_scope_info=NULL;
		//CScriptClass *class_info=NULL;
		int class_line;
		string class_name;
		string base_class_name="";
		CScriptClass *sc;
		//CScriptFunctionObject * class_object=NULL;

		KEYWORD_TYPE key_w;
		PASTNode function_collection_node=NULL,vars_collection_node=NULL,child_node=NULL, base_class_node = NULL;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::CLASS_KEYWORD){

				if(scope_info->getIdxParent()!=ZS_UNDEFINED_IDX){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"class keyword is not allowed");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),m_line);

				// check for symbol's name
				if((end_p = getSymbol(aux_p, m_line,class_name))==NULL){

					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected name class");
					return NULL;
				}
				// try to register class...
				class_line = m_line;


				aux_p=IGNORE_BLANKS(end_p,m_line);

				if(*aux_p == ':' ){

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					if((end_p = getSymbol(aux_p, m_line,base_class_name))==NULL){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected class name");
						return NULL;
					}

					aux_p=IGNORE_BLANKS(end_p, m_line);
				}

				// register class


				if((sc=CScriptClass::registerClass(class_name,base_class_name,NULL))==NULL){
					return NULL;
				}


				// create new scope...
				class_scope_info = CScope::newScope();
				class_scope_info->idxScriptClass=sc->idxClass;
				sc->scope_info.idxScope=class_scope_info->idxScope;



				zs_print_debug_cr("registered class \"%s\" line %i ",class_name.c_str(), class_line);

				if(*aux_p == '{' ){

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					// TODO: Register class and baseof

					// register info class ...
					// check for named functions or vars...
					while(*aux_p != '}' && *aux_p != 0){

						tInfoFunctionCompile *local_ifc=NULL;

						// 1st. check whether eval a keyword...
						key_w = isKeyword(aux_p);
						if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
							switch(key_w){
							default:
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected \"var\" or \"function\" keyword");
								return NULL;
								break;
							case KEYWORD_TYPE::FUNCTION_KEYWORD:

								if((aux_p = evalKeywordFunction(aux_p, m_line,class_scope_info,&local_ifc)) == NULL){
									return NULL;
								}
								break;
							case KEYWORD_TYPE::VAR_KEYWORD:
								if((aux_p = evalKeywordVar(aux_p, m_line,class_scope_info,NULL,error)) == NULL){
									return NULL;
								}
								break;
							}
						}else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected \"var\" or \"function\" keyword");
							return NULL;
						}
						aux_p=IGNORE_BLANKS(aux_p,m_line);
					}

					if(*aux_p != '}'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					if(*aux_p != ';'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,class_line ,"class \"%s\" not end with ;",class_name.c_str());
						return NULL;
					}

					return aux_p+1;

				}else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '{'");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalExpressionArgs(char c1,char c2,const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler *> 		*	asm_op){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		PASTNode   node_arg_expression=NULL;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		if(*aux_p == c1){
			aux_p++;
			aux_p=IGNORE_BLANKS(aux_p,m_line);


			if(*aux_p != c2 ){
				if(*aux_p == ',' ){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Unexpected %c",c2);
					return NULL;
				}

				do{
					if((aux_p = evalExpression(aux_p,m_line,scope_info,asm_op))==NULL){
						return NULL;
					}

					if(*aux_p == ',' ){
						aux_p++;
						aux_p=IGNORE_BLANKS(aux_p,m_line);
					}else{
						if(*aux_p != c2 ){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected %c",c2);
							return NULL;
						}
					}

				}while(*aux_p != c2 && *aux_p != 0);
			}

			return aux_p+1;
		}
		return NULL;
	}

	//
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	//  KEYWORDS
	//

	char * CEval::evalKeywordFunction(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile ** ifc){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *symbol_value,*end_var;
		KEYWORD_TYPE key_w;

		PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
		string conditional_str;
		bool error=false;
		short idxScopeClass;


		tScopeVar * irv=NULL;
		string str_name,arg_value;
		string class_member,class_name, function_name="";


		int idxScope=ZS_UNDEFINED_IDX;
		CScope *body_scope=NULL;



		idxScope=scope_info->idxScope;


		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::FUNCTION_KEYWORD){

				// advance keyword...
				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);


				bool named_function = *aux_p!='(';

				if(named_function){ // is named function..

					if((end_var=isClassMember(aux_p,m_line, idxScopeClass, error))!=NULL){ // check if particular case extension attribute class

						idxScope = idxScopeClass;
						symbol_value = (char *)class_member.c_str();
						function_name = symbol_value;

					}
					else{
						if(error){
							return NULL;
						}
						else{ // get normal name...

							// check whwther the function is anonymous with a previous arithmetic operation ....

							if((end_var=getSymbol(aux_p,m_line,function_name)) == NULL){
								return NULL;
							}
						}
					}
					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,m_line);
				}
				else{ //function node
					// TODO manage function object
					printf("TODO manage function object\n");
				}


				// eval function args...
				if(*aux_p == '('){ // push arguments...

					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,m_line);
					vector<string> arg;


					// grab words separated by ,
					while(*aux_p != 0 && *aux_p != ')'){


						aux_p=IGNORE_BLANKS(aux_p,m_line);

						if(arg.size()>0){
							if(*aux_p != ','){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ',' ");
								return NULL;
							}
							aux_p=IGNORE_BLANKS(aux_p+1,m_line);
						}


						if(*aux_p == ')' || *aux_p == ','){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected arg");
							return NULL;
						}

						int m_start_arg=m_line;
						if((end_var=getSymbol(aux_p,m_line,arg_value))==NULL){
							return NULL;
						}


						// check if repeats...
						for(unsigned k = 0; k < arg.size(); k++){
							if(arg[k] == arg_value){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Repeated argument '%s' argument ",arg_value.c_str());
								return NULL;
							}
						}

						// check whether parameter name's matches with some global variable...
						if((irv=body_scope->getInfoRegisteredSymbol(symbol_value,-1,false)) != NULL){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Ambiguous symbol argument \"%s\" name with var defined at %i", symbol_value, -1);
							return NULL;
						}
							// ok register symbol into the object function ...
						arg.push_back(symbol_value);

						aux_p=end_var;
						aux_p=IGNORE_BLANKS(aux_p,m_line);

						if(*aux_p != ')'){


						}

					}


					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,m_line);

					if(*aux_p != '{'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '{'");
						return NULL;
					}

					// ok let's go to body..
					if((aux_p = evalBlock(
							aux_p,
							m_line,
							SCOPE_NODE(idxScope),
							*ifc,
							error
						)) != NULL){

						if(!error){

							{

								// link scope / ast
								body_scope->idxAstNode=body_node->idxAstNode;
								body_node->idxScope =body_scope->idxScope;

								// register function symbol...
								int n_params=0;

								if(args_node != NULL){
									n_params=args_node->children.size();
								}

								if(named_function){ // register named function...
									if((irv=SCOPE_NODE(idxScope)->getInfoRegisteredSymbol(function_name,n_params,false)) != NULL){

										if(irv->idxAstNode!=ZS_UNDEFINED_IDX){
											writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Function name \"%s\" is already defined with same args at %s:%i", function_name.c_str(),GET_AST_FILENAME_LINE(irv->idxAstNode));
										}else{
											writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Function name \"%s\" is no allowed it has conflict with name of already registered function in C/C++", function_name.c_str());
										}

										return NULL;
									}

									if((irv=SCOPE_NODE(idxScope)->registerSymbol(function_name,NULL,n_params))==NULL){
										return NULL;
									}


								}else{ // register anonymouse function at global scope...
									irv=SCOPE_NODE(IDX_GLOBAL_SCOPE)->registerAnonymouseFunction(NULL);
								}



								SCOPE_NODE(idxScope)->popScope();
							}

							return aux_p;
						}
					}

				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line," Expected '('");
				}
			}else{
				writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected operator or function operator");
			}
		}
		return NULL;
	}

	char *  CEval::evalKeywordReturn(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		string s_aux;

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...
				PASTNode child_node=NULL;
				aux_p += strlen(defined_keyword[key_w].str);


				aux_p=IGNORE_BLANKS(aux_p,m_line);


				if((aux_p = evalExpression(aux_p, m_line, scope_info,&ifc->asm_op))!= NULL){


					if(*aux_p!=';'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ';'");
						return NULL;
					}
					aux_p++;
					return aux_p;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordWhile(const char *s,int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		CScope *_currentScope=NULL;

		PASTNode conditional_expression=NULL, while_node=NULL;
		string conditional_str;
		error = false;


		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::WHILE_KEYWORD){



				aux_p += strlen(defined_keyword[key_w].str);

				// evaluate conditional line ...
				aux_p=IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p == '('){

					if((end_expr = evalExpression(aux_p+1,m_line,_currentScope,&ifc->asm_op)) != NULL){

						if(*end_expr != ')'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')'");
							return NULL;
						}
						if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
							return NULL;
						}


						aux_p=IGNORE_BLANKS(end_expr+1,m_line);
						if(*aux_p != '{'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected while-block open block ('{') ");
							return NULL;
						}
						if((aux_p=evalBlock(aux_p
								,m_line
								,_currentScope
								,ifc
								,error
								))!= NULL){
							if(!error){


								scope_info->popScope();

								return aux_p;
							}
						}
					}else{
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')' while ");
						return NULL;
					}

				}else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' while ");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordDoWhile(const char *s,int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		CScope *_currentScope=NULL;

		PASTNode conditional_expression=NULL, body_node=NULL, while_node=NULL;
		string conditional_str;
		error = false;

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::DO_WHILE_KEYWORD){


				aux_p += strlen(defined_keyword[key_w].str);

				//1st evaluate body ..
				aux_p=IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p != '{'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected open block ('{') in do-while expression");
					return NULL;
				}
				if((aux_p=evalBlock(aux_p
						,m_line
						,_currentScope
						,ifc
						,error

						))!= NULL){
					if(!error){

						// Finally evaluate conditional line ...
						aux_p=IGNORE_BLANKS(aux_p,m_line);

						// check for keyword ...
						key_w = isKeyword(aux_p);

						if(key_w!=WHILE_KEYWORD){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected while keyword");
							return NULL;
						}

						aux_p += strlen(defined_keyword[key_w].str);

						aux_p=IGNORE_BLANKS(aux_p,m_line);

						if(*aux_p == '('){

							if((end_expr = evalExpression(aux_p+1,m_line,_currentScope,&ifc->asm_op)) != NULL){

								if(*end_expr != ')'){
									writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')'");
									return NULL;
								}
								if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
									return NULL;
								}


							}else{
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')' do-while expression");
								return NULL;
							}

						}else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' do-while expression");
							return NULL;
						}

						scope_info->popScope();
						return end_expr+1;
					}
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordIf(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		int dl=-1;
		KEYWORD_TYPE key_w;
		PASTNode conditional=NULL, if_node=NULL, else_node=NULL,block=NULL, group_conditional_nodes = NULL;
		string conditional_str;
		error = false;
		int conditional_line;



		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::IF_KEYWORD){

				do{

					aux_p += strlen(defined_keyword[key_w].str);
					aux_p=IGNORE_BLANKS(aux_p,m_line);

					if(*aux_p != '('){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' if");
						return NULL;
					}

					conditional_line=m_line;

					if((end_expr = evalExpression(aux_p+1,m_line,scope_info,&ifc->asm_op)) == NULL){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')' if ");
						return NULL;
					}

					if(*end_expr != ')'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')'");
						return NULL;
					}

					if(IGNORE_BLANKS(aux_p+1,dl)==end_expr){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"no conditional expression");
						return NULL;
					}

					if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					conditional_str=start_symbol;

					aux_p=IGNORE_BLANKS(end_expr+1,m_line);
					if(*aux_p != '{'){
						if(!printErrorUnexpectedKeywordOrPunctuator(aux_p, m_line)){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected if-block open block ('{')");
						}
						return NULL;
					}


					if((aux_p=evalBlock(aux_p
							,m_line
							,scope_info
							,ifc
							,error
							))== NULL){
						return NULL;
					}

					if(error){
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p,m_line);

					bool else_key = false;
					if((key_w = isKeyword(aux_p)) != KEYWORD_TYPE::UNKNOWN_KEYWORD){
						else_key = (key_w == KEYWORD_TYPE::ELSE_KEYWORD);
					}

					if(else_key){
						aux_p += strlen(defined_keyword[key_w].str);

						if(*aux_p != '{'){
							aux_p++;
						}

						aux_p=IGNORE_BLANKS(aux_p,m_line);

						bool if_key = false;
						if((key_w = isKeyword(aux_p)) != KEYWORD_TYPE::UNKNOWN_KEYWORD){
							if_key = (key_w == KEYWORD_TYPE::IF_KEYWORD);
						}

						if(!if_key){

							if(*aux_p != '{'){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected else-block open block ('{')");
								return NULL;
							}

							if((aux_p=evalBlock(aux_p
									,m_line
									,scope_info
									,ifc
									,error
									))!= NULL){
									if(!error){
										return aux_p;
									}
									else{
										return NULL;
									}
							}else{
								return NULL;
							}
						} // else keep up parsing if nodes case ...
					}else{
						return aux_p;
					}

				}while(true); // loop
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordFor(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		error=false;
		PASTNode block_for = NULL,pre_node=NULL,cond_node=NULL,post_node=NULL, pre_node_expression=NULL, cond_node_expression=NULL,post_node_expression=NULL;
		string eval_for;

		//CScope *_localScope =  scope_info != NULL?scope_info->symbol_info.ast->scope_info_ptr:NULL; // gets scope...
		CScope *_currentScope=NULL;



		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::FOR_KEYWORD){


				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p == '('){ // ready ...

					// save scope pointer ...
					_currentScope =scope_info->pushScope(); // push current scope


					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					if(*aux_p != ';'){ // there's some var init...
						// init node ...
						KEYWORD_TYPE key_w = isKeyword(aux_p);

						if(key_w == VAR_KEYWORD){
							if((aux_p = evalKeywordVar(aux_p,m_line, _currentScope,ifc,error))==NULL){
								return NULL;
							}

							aux_p = aux_p - 1; // redirect aux_p to ';'
						}
						else{

							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected 'var' keyword");
							return NULL;
						}

					}

					aux_p=IGNORE_BLANKS(aux_p,m_line);

					key_w = isKeyword(aux_p);
					if(key_w == KEYWORD_TYPE::IN_KEYWORD){

						PASTNode node_for_in_right_op_expression=NULL;

						aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[KEYWORD_TYPE::IN_KEYWORD].str),m_line);


						if((aux_p = evalExpression((const char *)aux_p,m_line,_currentScope,&ifc->asm_op)) == NULL){
							return NULL;
						}
					}

					else{ // expects conditional and post (i.e for(;;) )


						if(*aux_p != ';'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ';'");
							return NULL;

						}

						aux_p=IGNORE_BLANKS(aux_p+1,m_line);

						if(*aux_p != ';'){ // conditional...
							char * end_p=IGNORE_BLANKS(aux_p+1,m_line);

							if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

								if((aux_p = evalExpression((const char *)aux_p,m_line,_currentScope,&ifc->asm_op)) == NULL){
									return NULL;
								}

							}
						}

						aux_p=IGNORE_BLANKS(aux_p,m_line);

						if(*aux_p != ';'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ';'");
							return NULL;

						}

						aux_p=IGNORE_BLANKS(aux_p+1,m_line);


						if(*aux_p != ')' ){ // finally do post op...

							if(*aux_p == ',' ){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Unexpected ) ");
								return NULL;
							}

							do{
								if((aux_p = evalExpression(aux_p,m_line,_currentScope,&ifc->asm_op))==NULL){
									return NULL;
								}


								if(*aux_p == ',' ){
									aux_p=IGNORE_BLANKS(aux_p+1,m_line);
								}else{
									if(*aux_p != ')' ){
										writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')'");
										return NULL;
									}
								}

							}while(*aux_p != ')' && *aux_p != 0);
						}
					}

					if(*aux_p != ')'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')'");
						return NULL;

					}

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);
					if(*aux_p != '{'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '{' for-block");
						return NULL;
					}

					// eval block ...
					if((aux_p=evalBlock(aux_p
							,m_line
							,_currentScope
							,ifc
							,error
						))!= NULL){ // true: We treat declared variables into for as another scope.
						if(!error){

							scope_info->popScope(); // push current scope
							return aux_p;
						}

					}
				}else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' for");
					return NULL;
				}
			}
		}
		return NULL;
	}


	char * CEval::evalKeywordSwitch(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_symbol,*start_symbol;
		PASTNode switch_node=NULL,
					 group_cases=NULL,
					 case_value_node=NULL,
					 default_switch_node=NULL;

		CScope *scope_case=NULL;
		PASTNode body_switch=NULL;

		PUNCTUATOR_TYPE ip;
		char *value_to_eval;
		string val;
		KEYWORD_TYPE key_w,key_w2;
		CScope *currentScope=scope_info;

		error=false;
		int n_cases;



		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::SWITCH_KEYWORD){

				currentScope=scope_info->pushScope();

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p == '('){
						aux_p=IGNORE_BLANKS(aux_p+1,m_line);

						// evaluate switch vale expression ...
						PASTNode condition_expression_to_evaluate = NULL;
						//static char * evalExpression_Recursive(const char *s, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated=NULL,GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL);
						if((aux_p = CEval::evalExpression(
								aux_p,
								m_line,
								scope_info,
								&ifc->asm_op
								))==NULL)
						{
							return NULL;
						}


						if(*aux_p != ')'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ')' switch");
							error = true;
							return NULL;
						}

						aux_p=IGNORE_BLANKS(aux_p+1,m_line);

						if(*aux_p == '{'){

							aux_p++;

							if((aux_p=eval_Recursive(aux_p, m_line, currentScope,ifc, error))==NULL){
								return NULL;
							}

							aux_p=IGNORE_BLANKS(aux_p,m_line);

							if(*aux_p != '}'){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '}' switch");
								return NULL;
							}


							scope_info->popScope();


							return aux_p+1;
						}
						else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '{' switch");
							return NULL;
						}
				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' switch ");
					return NULL;
				}
			}
		}
		return NULL;
	}


	char * CEval::evalKeywordVar(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass

		char *aux_p = (char *)s;

		KEYWORD_TYPE key_w;
		char *start_var,*end_var;
		string class_member;
		PASTNode class_node;
		PASTNode var_node;
		PASTNode vars_collection_node=NULL;

		int idxScope=ZS_UNDEFINED_IDX;
		string s_aux,variable_name;
		char *symbol_value;
		bool end=false;
		bool allow_for_in=true;
		short idxScopeClass=-1;

		bool parent_scope_is_class=false;
		int m_startLine=0;

		if(scope_info != NULL){// && class_scope){ // if class scope let's see if is within function member..
			if(scope_info->getIdxBaseScope() != 0) { // if base scope != 0 is a class
				parent_scope_is_class = scope_info->getIdxBaseScope() == scope_info->getCurrentScopePointer()->idxScope;
			}
		}

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);


				while(*aux_p != ';' && *aux_p != 0 && !end){ // JE: added multivar feature.

					bool is_class_member=parent_scope_is_class;

					aux_p=IGNORE_BLANKS(aux_p,m_line);
					start_var=aux_p;
					m_startLine = m_line;
					vars_collection_node=NULL;

					//parent_eval_to_insert_var=NULL;
					if(scope_info != NULL){ // main as default
						idxScope=scope_info->getCurrentScopePointer()->idxScope;
					}

					if((end_var=isClassMember(aux_p,m_line,idxScopeClass, error))!=NULL){ // check if particular case extension attribute class
						idxScope = idxScopeClass; // override scope info
						symbol_value = (char *)class_member.c_str();
						variable_name = symbol_value;

						is_class_member=true;


					}
					else{ // causal variable
						if(error){
							return NULL;
						}
						else{ // get normal name...

							m_line = m_startLine;

							// check whwther the function is anonymous with a previous arithmetic operation ....
							if((end_var=getSymbol(aux_p,m_line,variable_name))==NULL){
									return NULL;
							}
						}
					}

					KEYWORD_TYPE keyw = isKeyword(variable_name.c_str());

					if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Cannot use reserved word (%s) as var.",defined_keyword[keyw].str);
						return NULL;
					}

					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,m_line);
					//}
					bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;
					if(is_class_member && *aux_p == '='){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Variable member is not assignable on its declaration. Should be initialized within constructor.");
						return NULL;
					}

					if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
						allow_for_in=false;
						//zs_print_debug_cr("registered symbol \"%s\" line %i ",variable_name, m_line);
						var_node = NULL;


						if(*aux_p == '='){ // only for variables (not class members)

							PASTNode children_node=NULL;


							// try to evaluate expression...
							aux_p=IGNORE_BLANKS(aux_p,m_line);

							if((aux_p = evalExpression(start_var,m_startLine,scope_info,&ifc->asm_op)) == NULL){
								return NULL;
							}

							if(var_node != NULL){

								if(children_node==NULL){
									THROW_RUNTIME_ERROR("internal:children node == NULL");
									return NULL;
								}
								var_node->children.push_back(children_node->idxAstNode);
							}

							m_line = m_startLine;
						}

						 // define as many vars is declared within ','

						if(!SCOPE_NODE(idxScope)->registerSymbol(var_node->symbol_value,var_node)){
								return NULL;
						}

						zs_print_debug_cr("registered symbol \"%s\" line %i ",(var_node)->symbol_value.c_str(), (var_node)->line_value);

					}
					else{

						KEYWORD_TYPE keyw = isKeyword(variable_name.c_str());
						if(keyw == KEYWORD_TYPE::IN_KEYWORD){ // in keyword was detected (return to evalr)...
							if(!allow_for_in){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"'in' keyword should be used with an uninitialized variable (example: for ( var e in v) {...} )", *aux_p);
								return NULL;
							}
							end=true;
						}
						else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"unexpected '%c'", *aux_p);
							return NULL;
						}
					}

					// ignores ';' or ','
					if(*aux_p == ',')
						aux_p++;
				}

				if(*aux_p == ';'){
					aux_p++;
				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ';'");
					return NULL;
				}

				return aux_p;
			}
		}
		return NULL;
	}

	char * CEval::evalBlock(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;

		//CScope *_localScope =  scope_info != NULL ? scope_info->symbol_info.ast->scope_info_ptr: NULL;
		CScope *currentScope=  NULL;
		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		if(*aux_p == '{'){
			aux_p++;

			currentScope =scope_info->getCurrentScopePointer();
			currentScope = scope_info->pushScope(); // special case... ast is created later ...


			if((aux_p = eval_Recursive(aux_p, m_line, currentScope,ifc,error)) != NULL){
				if(error){
					return NULL;
				}

				if(*aux_p != '}'){
					error = true;
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected } ");
					return NULL;
				}


				scope_info->popScope();

				return aux_p+1;
			}
		}
		return NULL;
	}

	bool CEval::printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line){
		PUNCTUATOR_TYPE ip=isPunctuator(current_string_ptr);
		KEYWORD_TYPE kw=isKeyword(current_string_ptr);

		if(kw!=KEYWORD_TYPE::UNKNOWN_KEYWORD){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Unexpected %s",defined_keyword[kw].str);
			return true;
		}
		else if(ip!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Unexpected %s",defined_operator_punctuator[ip].str);
			return true;
		}
		return false;
	}

	char *CEval::evalBreak(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"

		char *aux_p=(char *)s;

		fprintf(stderr,"find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n");

		{ // ok break is valid in current scope...

			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"break\" allowed within loop or case-switch statements");
			error = true;
			return NULL;
		}


		if(*aux_p != ';'){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected ';'");
			error = true;
			return NULL;
		}
	}

	char *CEval::evalContinue(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
		char *aux_p=(char*)s;
		{ // ok break is valid in current scope...

			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"continue\" allowed within loop or case-switch statements");
			error = true;
			return NULL;
		}


		if(*aux_p != ';'){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected ';'");
			error = true;
			return NULL;
		}
	}

	char *CEval::evalDefaultCase(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		char *aux_p=(char *)s;
		string value_to_eval;

		aux_p=IGNORE_BLANKS(aux_p, m_line);

		KEYWORD_TYPE keyw = isKeyword(aux_p);

		if(keyw == KEYWORD_TYPE::CASE_KEYWORD){ // a keyword was detected...


			aux_p=IGNORE_BLANKS(aux_p,m_line);

			// get the symbol...
			char *start_symbol=aux_p;
			PUNCTUATOR_TYPE ip;

			if((ip = isPunctuator(aux_p)) != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
				if(ip == PUNCTUATOR_TYPE::ADD_PUNCTUATOR ||ip == PUNCTUATOR_TYPE::SUB_PUNCTUATOR){
					aux_p+=strlen(defined_operator_punctuator[ip].str);
				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"unexpected token %s",defined_operator_punctuator[ip].str);
					error = true;
					return NULL;
				}
			}


			aux_p=getSymbol(aux_p, m_line,value_to_eval);

			if(aux_p==NULL){ return NULL;}

		}

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		if(*aux_p != ':'){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected  ':' ");
			return NULL;
		}



		return aux_p+1;
	}


	char *CEval::evalKeyword(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p= (char *)s;
		tInfoFunctionCompile *ifc_new = NULL;

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD,keyw2nd=KEYWORD_TYPE::UNKNOWN_KEYWORD;

		aux_p=IGNORE_BLANKS(aux_p, m_line);

		// check if condition...
		keyw = isKeyword(aux_p);

		if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...

			switch(keyw){
			case CASE_KEYWORD:
			case DEFAULT_KEYWORD:
				return evalDefaultCase(s,m_line,scope_info,ifc,error);

			case FUNCTION_KEYWORD:
				if((aux_p = evalKeywordFunction(s,m_line,scope_info,&ifc_new)) != NULL){
					return aux_p;
				}
				error = true;
				return NULL;
			case CLASS_KEYWORD:
				if((aux_p = CEval::evalKeywordClass(s,m_line,scope_info,error)) != NULL){
					return aux_p;
				}
				error = true;
				return NULL;

			default:

				if(defined_keyword[keyw].eval_fun != NULL){

					return  defined_keyword[keyw].eval_fun(s,m_line,scope_info,ifc,error);
				}
				writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Not implemented");
				error = true;
				return NULL;
			}

			// something wrong was happen..
		}

		return NULL;
	}

	char * CEval::eval_Recursive(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD;
		bool custom_quit = false;
		char *aux = (char *)s;
		char *end_expr=0;
		PASTNode children=NULL;

		bool processed_directive=false;


		aux=IGNORE_BLANKS(aux, m_line);

		while(*aux != 0 && !custom_quit){

			processed_directive=false;
			children = NULL;
			// ignore all ;
			while(*aux==';' && *aux != 0){
				aux =IGNORE_BLANKS(aux+1, m_line);
			}

			if(*aux==0){ // custom case exit..
				return aux;
			}

			if(*aux == '}'){ // trivial cases...
				return aux;
			}else{

				// try directive ...
				DIRECTIVE_TYPE directive = isDirective(aux);
				char *start_var,* end_var,*symbol_name;
				if(directive != DIRECTIVE_TYPE::UNKNOWN_DIRECTIVE){
					switch(directive){
					case INCLUDE_DIRECTIVE:
						aux += strlen(defined_directive[directive].str);
						aux = IGNORE_BLANKS(aux,m_line);
						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected starting \" directive");
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						aux++;
						start_var=aux;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected end \" directive");
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						end_var=aux;

						if((symbol_name=CZetScriptUtils::copyStringFromInterval(start_var,end_var)) == NULL){
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						zs_print_debug_cr("include file: %s",symbol_name);

						{

							// save current file info...
							string current_file_str=CEval::current_parsing_filename;
							int current_file_idx=CEval::current_idx_parsing_filename;
							string file_to_eval=symbol_name;

							if(CZetScript::getInstance()->isFilenameAlreadyParsed(file_to_eval.c_str())){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"%s\" already evald",file_to_eval.c_str());
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							try{
								CZetScript::getInstance()->eval_file(file_to_eval.c_str());
							}catch(script_error & error){
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							//restore current file info...
							CEval::current_parsing_filename=current_file_str.c_str();
							CEval::current_idx_parsing_filename=current_file_idx;
						}

						aux++;// advance ..
						break;
					default:
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"directive \"%s\" not supported",defined_directive[directive].str);
						break;
					}

					processed_directive = true;
					end_expr=aux;
				}
			}

			// 0st special case member class extension ...
			if(children==NULL && !processed_directive){ // not processed yet ...
				// 1st. check whether eval a keyword...
				if((end_expr = evalKeyword(aux, m_line, scope_info,ifc, error)) == NULL){

					// If was unsuccessful then try to eval expression.
					if(error){

						THROW_SCRIPT_ERROR();
						return NULL;
					}
					// 2nd. check whether eval a block
					if((end_expr = evalBlock(aux
							,m_line
							, scope_info
							,ifc
							, error
							))==NULL){

						// If was unsuccessful then try to eval expression.
						if(error){
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						// 2nd. try expression
						int starting_expression=m_line;

						if((end_expr = evalExpression(aux,m_line, scope_info,&ifc->asm_op)) == NULL){ // something wrong was happen.
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr == ')'){ // unexpected close parenthesis.
							error = true;
							writeErrorMsg(CURRENT_PARSING_FILENAME,starting_expression,"missing '('");
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr != ';'){
							error = true;
							writeErrorMsg(CURRENT_PARSING_FILENAME,starting_expression,"Expected ';'");
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						end_expr++;
					}
				}

				// new expression ready to be evaluated...


			}

			aux=end_expr;
			aux=IGNORE_BLANKS(aux, m_line);
		}
		return aux;
	}
	//------------------------------------------------------------------------------------------------------------
	//
	// PUBLIC
	//
	void CEval::init(){


		// init operator punctuators...
		defined_operator_punctuator[UNKNOWN_PUNCTUATOR]={UNKNOWN_PUNCTUATOR, "none",NULL};

		defined_operator_punctuator[ADD_PUNCTUATOR]={ADD_PUNCTUATOR, "+",isPlusPunctuator};
		defined_operator_punctuator[SUB_PUNCTUATOR]={SUB_PUNCTUATOR, "-",isMinusPunctuator};
		defined_operator_punctuator[MUL_PUNCTUATOR]={MUL_PUNCTUATOR, "*",isMulPunctuator};
		defined_operator_punctuator[DIV_PUNCTUATOR]={DIV_PUNCTUATOR, "/",isDivPunctuator};
		defined_operator_punctuator[MOD_PUNCTUATOR]={MOD_PUNCTUATOR, "%",isModPunctuator};
		defined_operator_punctuator[FIELD_PUNCTUATOR]={FIELD_PUNCTUATOR, ".",isFieldPunctuator};
		defined_operator_punctuator[TERNARY_IF_PUNCTUATOR]={TERNARY_IF_PUNCTUATOR, "?",isInlineIfPunctuator};
		defined_operator_punctuator[TERNARY_ELSE_PUNCTUATOR]={TERNARY_ELSE_PUNCTUATOR, ":",isInlineElsePunctuator};
		defined_operator_punctuator[ASSIGN_PUNCTUATOR]={ASSIGN_PUNCTUATOR, "=",isAssignPunctuator};
		defined_operator_punctuator[ADD_ASSIGN_PUNCTUATOR]={ADD_ASSIGN_PUNCTUATOR, "+=",isAddAssignPunctuator};
		defined_operator_punctuator[SUB_ASSIGN_PUNCTUATOR]={SUB_ASSIGN_PUNCTUATOR, "-=",isSubAssignPunctuator};
		defined_operator_punctuator[MUL_ASSIGN_PUNCTUATOR]={MUL_ASSIGN_PUNCTUATOR, "*=",isMulAssignPunctuator};
		defined_operator_punctuator[DIV_ASSIGN_PUNCTUATOR]={DIV_ASSIGN_PUNCTUATOR, "/=",isDivAssignPunctuator};
		defined_operator_punctuator[MOD_ASSIGN_PUNCTUATOR]={MOD_ASSIGN_PUNCTUATOR, "%=",isModAssignPunctuator};
		defined_operator_punctuator[BINARY_XOR_PUNCTUATOR]={BINARY_XOR_PUNCTUATOR, "^",isBinaryXorPunctuator};
		defined_operator_punctuator[BINARY_AND_PUNCTUATOR]={BINARY_AND_PUNCTUATOR, "&",isBinaryAndPunctuator};
		defined_operator_punctuator[BINARY_OR_PUNCTUATOR]={BINARY_OR_PUNCTUATOR, "|",isBinaryOrPunctuator};
		defined_operator_punctuator[SHIFT_LEFT_PUNCTUATOR]={SHIFT_LEFT_PUNCTUATOR, "<<",isShiftLeftPunctuator};
		defined_operator_punctuator[SHIFT_RIGHT_PUNCTUATOR]={SHIFT_RIGHT_PUNCTUATOR, ">>",isShiftRightPunctuator};
		defined_operator_punctuator[LOGIC_AND_PUNCTUATOR]={LOGIC_AND_PUNCTUATOR, "&&",isLogicAndPunctuator};
		defined_operator_punctuator[LOGIC_OR_PUNCTUATOR]={LOGIC_OR_PUNCTUATOR, "||",isLogicOrPunctuator};
		defined_operator_punctuator[LOGIC_EQUAL_PUNCTUATOR]={LOGIC_EQUAL_PUNCTUATOR, "==",isLogicEqualPunctuator};
		defined_operator_punctuator[LOGIC_NOT_EQUAL_PUNCTUATOR]={LOGIC_NOT_EQUAL_PUNCTUATOR, "!=",isLogicNotEqualPunctuator};
		defined_operator_punctuator[LOGIC_GT_PUNCTUATOR]={LOGIC_GT_PUNCTUATOR, ">",isLogicGreatherThanPunctuator};
		defined_operator_punctuator[LOGIC_LT_PUNCTUATOR]={LOGIC_LT_PUNCTUATOR, "<",isLogicLessThanPunctuator};
		defined_operator_punctuator[LOGIC_GTE_PUNCTUATOR]={LOGIC_GTE_PUNCTUATOR, ">=",isLogicGreatherEqualThanPunctuator};
		defined_operator_punctuator[LOGIC_LTE_PUNCTUATOR]={LOGIC_LTE_PUNCTUATOR, "<=",isLessEqualThanPunctuator};
		defined_operator_punctuator[INSTANCEOF_PUNCTUATOR]={INSTANCEOF_PUNCTUATOR, "instanceof",isInstanceOfPunctuator};

		defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR]={LOGIC_NOT_PUNCTUATOR, "!",isNotPunctuator};
		defined_operator_punctuator[PRE_INC_PUNCTUATOR]={PRE_INC_PUNCTUATOR, "++",isIncPunctuator};
		defined_operator_punctuator[PRE_DEC_PUNCTUATOR]={PRE_DEC_PUNCTUATOR, "--",isDecPunctuator};
		defined_operator_punctuator[POST_INC_PUNCTUATOR]={POST_INC_PUNCTUATOR, "++",isIncPunctuator};
		defined_operator_punctuator[POST_DEC_PUNCTUATOR]={POST_DEC_PUNCTUATOR, "--",isDecPunctuator};

		// special punctuators...
		defined_operator_punctuator[COMA_PUNCTUATOR]={COMA_PUNCTUATOR, ",",NULL};
		defined_operator_punctuator[SEMICOLON_PUNCTUATOR]={SEMICOLON_PUNCTUATOR, ";",NULL};
		defined_operator_punctuator[OPEN_PARENTHESIS_PUNCTUATOR]={OPEN_PARENTHESIS_PUNCTUATOR, "(",NULL};
		defined_operator_punctuator[CLOSE_PARENTHESIS_PUNCTUATOR]={CLOSE_PARENTHESIS_PUNCTUATOR, ")",NULL};
		defined_operator_punctuator[OPEN_BRAKET_PUNCTUATOR]={OPEN_BRAKET_PUNCTUATOR, "{",NULL};
		defined_operator_punctuator[CLOSE_BRAKET_PUNCTUATOR]={CLOSE_BRAKET_PUNCTUATOR, "}",NULL};
		defined_operator_punctuator[OPEN_SQUARE_BRAKET_PUNCTUATOR]={OPEN_SQUARE_BRAKET_PUNCTUATOR, "[",NULL};
		defined_operator_punctuator[CLOSE_SQUARE_BRAKET_PUNCTUATOR]={CLOSE_SQUARE_BRAKET_PUNCTUATOR, "]",NULL};

		// init special punctuators...
		// init keywords...
		defined_keyword[KEYWORD_TYPE::UNKNOWN_KEYWORD] = {UNKNOWN_KEYWORD, "none",NULL};
		defined_keyword[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var",evalKeywordVar};
		defined_keyword[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",NULL};
		defined_keyword[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else",NULL};
		defined_keyword[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for",NULL};
		defined_keyword[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while",NULL};
		defined_keyword[KEYWORD_TYPE::DO_WHILE_KEYWORD] = {DO_WHILE_KEYWORD,"do",NULL}; // while is expected in the end ...

		defined_keyword[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch",NULL};
		defined_keyword[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case",NULL};
		defined_keyword[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break",NULL};
		defined_keyword[KEYWORD_TYPE::CONTINUE_KEYWORD] = {CONTINUE_KEYWORD,"continue",NULL};
		defined_keyword[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default",NULL};
		defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",NULL};
		defined_keyword[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return",NULL};
		defined_keyword[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this", NULL};
		defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",NULL};
		defined_keyword[KEYWORD_TYPE::NEW_KEYWORD] = {NEW_KEYWORD,"new", NULL};
		defined_keyword[KEYWORD_TYPE::DELETE_KEYWORD] = {DELETE_KEYWORD,"delete",NULL};
		defined_keyword[KEYWORD_TYPE::IN_KEYWORD] = {IN_KEYWORD,"in",NULL};

		// DIRECTIVES
		defined_directive[UNKNOWN_DIRECTIVE]={UNKNOWN_DIRECTIVE, NULL};
		defined_directive[INCLUDE_DIRECTIVE]={INCLUDE_DIRECTIVE, "import"};



			// create main ast management
	}



}
