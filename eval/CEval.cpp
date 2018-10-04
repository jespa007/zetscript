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

	tKeywordInfo CEval::defined_keyword[MAX_KEYWORD];
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

	char *ADVANCE_TO_CHAR(char *str,char c, int & m_line) {
		char *aux_p = str;
		// make compatible windows format (\r)...
		while(*aux_p!=0 && (*aux_p !=(c) )) {
			if(*aux_p == '\"') { // go to end..
				aux_p++;
				while (*aux_p!=0 && !(*aux_p =='\"' && *(aux_p-1) !='\\') && *aux_p != '\n') {aux_p++;}

				if(*aux_p != '\"'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line, "string is not closed at line %i",m_line);
					return NULL;
				}
			}
			if(*aux_p == '\n') {m_line++;}; // make compatible windows format...
			aux_p++;
		}
		return aux_p;
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
				//aux_p = ADVANCE_TO_CHAR(aux_p,'\n', m_line);

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

	char *IGNORE_BLANKS_REVERSE(const char *str_begin,const char *str_end, int &m_line) {
		char *aux_p = (char *)str_begin;
		bool end = false;
		while(!end){
			end = true;
			while(aux_p!=str_end && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p--;

			// make compatible windows format...
			if(*aux_p == '\r')
				aux_p--;

			if(*aux_p == '\n') {
				m_line=m_line+1;
				end=false;
				aux_p--;
			}
		}
		return aux_p;
	}

	char *ADVANCE_TO_ONE_OF_COLLECTION_CHAR(char *str,char *end_char_standard_value, int &m_line) {
		char *aux_p = str;
		char *chk_char;
		while(*aux_p!=0){
			chk_char = end_char_standard_value;

			// comment blocks also is returned (these lines must be ignored)
			if(IS_START_COMMENT(aux_p)) {
				aux_p = ADVANCE_TO_END_COMMENT(aux_p, m_line);
				if(IS_END_COMMENT(aux_p))
					aux_p+=2;
			}

			if(IS_SINGLE_COMMENT(aux_p)) {
				aux_p = ADVANCE_TO_CHAR(aux_p,'\n', m_line);
			}

			while(*chk_char != 0){
				if(*chk_char == *aux_p)
					return aux_p;
				chk_char++;
			}
			aux_p++;
		}
		return aux_p;
	}


	bool CEval::evalExpressionPlusPunctuator(const char *s){
		if(*s=='+')
			return ((*(s+1) != '+') && (*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionMinusPunctuator(const char *s){
		if(*s=='-')
			return ((*(s+1) != '-') && (*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionMulPunctuator(const char *s){
		if(*s == '*')
			return ((*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionDivPunctuator(const char *s){
		if(*s == '/')
			return ((*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionModPunctuator(const char *s){
		if(*s == '%')
			return ((*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionFieldPunctuator(const char *s){
		return *s == '.';
	}

	bool CEval::evalExpressionInlineIfPunctuator(const char *s){
		return *s == '?';
	}

	bool CEval::evalExpressionInlineElsePunctuator(const char *s){
		return *s == ':';
	}

	bool CEval::evalExpressionAssignPunctuator(const char *s){
		if(*s=='=')
			return (*(s+1) != '=');
		return false;
	}

	bool CEval::evalExpressionAddAssignPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionSubAssignPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionMulAssignPunctuator(const char *s){
		if(*s=='*')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionDivAssignPunctuator(const char *s){
		if(*s=='/')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionModAssignPunctuator(const char *s){
		if(*s=='%')
			return (*(s+1) == '=');
		return false;
	}


	bool CEval::evalExpressionBinaryXorPunctuator(const char *s){
		if(*s == '^')
			return ((*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionBinaryAndPunctuator(const char *s){
		if(*s=='&')
			return ((*(s+1) != '&')  && (*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionBinaryOrPunctuator(const char *s){
		if(*s=='|')
			return ((*(s+1) != '|') &&  (*(s+1) != '='));
		return false;
	}

	bool CEval::evalExpressionShiftLeftPunctuator(const char *s){
		if(*s=='<')
			return (*(s+1) == '<');
		return false;
	}

	bool CEval::evalExpressionShiftRightPunctuator(const char *s){
		if(*s=='>')
			return (*(s+1) == '>');
		return false;
	}

	bool CEval::evalExpressionLogicAndPunctuator(const char *s){
		if(*s=='&')
			return (*(s+1) == '&');
		return false;
	}

	bool CEval::evalExpressionLogicOrPunctuator(const char *s){
		if(*s=='|')
			return (*(s+1) == '|');
		return false;
	}

	bool CEval::evalExpressionLogicEqualPunctuator(const char *s){
		if(*s=='=')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionInstanceOfPunctuator(const char *s){
		return strncmp("instanceof",s,10) == 0;
	}

	bool CEval::evalExpressionLogicNotEqualPunctuator(const char *s){
		if(*s=='!')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionLogicGreatherThanPunctuator(const char *s){
		if( *s == '>')
			return (*(s+1) != '>');
		return false;
	}

	bool CEval::evalExpressionLogicLessThanPunctuator(const char *s){
		if(*s == '<')
			return (*(s+1) != '<');

		return false;
	}

	bool CEval::evalExpressionLogicGreatherEqualThanPunctuator(const char *s){
		if(*s=='>')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionLessEqualThanPunctuator(const char *s){
		if(*s=='<')
			return (*(s+1) == '=');
		return false;
	}

	bool CEval::evalExpressionNotPunctuator(const char *s){
		if(*s=='!')
			return (*(s+1) != '=');
		return false;
	}


	bool CEval::evalExpressionIncPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) == '+');
		return false;
	}

	bool CEval::evalExpressionDecPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) == '-');
		return false;
	}


	PUNCTUATOR_TYPE   CEval::isOperatorPunctuator(const char *s){

		for(unsigned char  i = 0; i < MAX_OPERATOR_PUNCTUATORS; i++){
			if(defined_operator_punctuator[i].eval_fun != NULL){
				if(defined_operator_punctuator[i].eval_fun(s)){
					return defined_operator_punctuator[i].id;
				}
			}
		}
		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE   CEval::isSpecialPunctuator(const char *s){

		for(unsigned char i = START_SPECIAL_PUNCTUATORS; i < MAX_SPECIAL_PUNCTUATORS; i++){

			if(*defined_operator_punctuator[i].str == *s){
				return defined_operator_punctuator[i].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE   CEval::evalExpressionArithmeticPunctuator(const char *s){

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

	PUNCTUATOR_TYPE   CEval::isPunctuator(const char *s){

		PUNCTUATOR_TYPE ip = isOperatorPunctuator(s);

		if(ip!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			return ip;
		}

		return isSpecialPunctuator(s);
	}

	// to string utils ...
	char * CEval::getEndWord(const char *s, int m_line){

		char *start_str=(char *)s;
		char *aux=(char *)s;
		PUNCTUATOR_TYPE sp;
		KEYWORD_TYPE key_w;
		 bool is_possible_number=false;
		 int i=0;
		 bool start_digit = false;

		if(*aux == '\"'){
			aux++;
			while((*aux)!=0 && !((*aux)=='\n') && !((*aux)=='\"' && *(aux-1) !='\\')) {
				aux++;
			}

			if(*aux != '\"') {
				writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Error \" not closed");
				return NULL;
			}
			aux++;

		}else{

			if((key_w = isKeyword(s))!= KEYWORD_TYPE::UNKNOWN_KEYWORD){
				if( key_w != KEYWORD_TYPE::THIS_KEYWORD){
				 //&& key_w->id != KEYWORD_TYPE::SUPER_KEYWORD ){ // unexpected token ?
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Unexpected keyword \"%s\". Forgot \";\" ?",defined_keyword[key_w].str);
					return NULL;
				}
			}

			while((*aux)!=0 && !(
					(*aux)==' ' ||
					(*aux)=='\t' ||
					(*aux)=='\n' ||
					(*aux)=='\r'
							) &&
					(isSpecialPunctuator(aux)==PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR)

			) {
				// check for special punctuator ( the field '.' separator is processed within the word )
				if(i==0 && !start_digit){ // possible digit ...

					is_possible_number = CZetScriptUtils::isDigit(*aux);
					start_digit = true;
				}

				if((sp = isOperatorPunctuator(aux))!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
					if(sp == FIELD_PUNCTUATOR  || ((*aux=='-' ||  *aux=='+') && ((i>0 && (*(aux-1)=='e'))))){
						if(!is_possible_number){
							return aux;
						}
					}
					else{
						return aux;
					}
				}

				aux++;
				i++;
			}

			if(is_possible_number){
				string num = CZetScriptUtils::copyStringFromInterval(start_str,aux);

				if(!CZetScriptUtils::isBinary(num)){

					if(!CZetScriptUtils::isNumber(num)){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"%s is not a valid number",num.c_str());
						return NULL;
					}
				}
			}
		}
		return aux;
	}


	char *CEval::getSymbolName(const char *s,int & m_line){

		char *aux_p=(char *)s;
		PUNCTUATOR_TYPE end_punctuator=isPunctuator(s);


		if(end_punctuator != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Unexpected '%s'",defined_operator_punctuator[end_punctuator].str);
			return NULL;
		}

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
			return NULL;
		}

		return aux_p;
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
	PUNCTUATOR_TYPE CEval::checkPreOperatorPunctuator(const char *s){

		if(evalExpressionIncPunctuator(s)) 	return PRE_INC_PUNCTUATOR;
		if(evalExpressionDecPunctuator(s))	return PRE_DEC_PUNCTUATOR;
		if(evalExpressionPlusPunctuator(s)) 	return ADD_PUNCTUATOR;
		if(evalExpressionMinusPunctuator(s)) return SUB_PUNCTUATOR;
		if(evalExpressionNotPunctuator(s))   return LOGIC_NOT_PUNCTUATOR;

		return UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE CEval::checkPostOperatorPunctuator(const char *s){

		PUNCTUATOR_TYPE op=UNKNOWN_PUNCTUATOR;

		if(evalExpressionIncPunctuator(s)){
			op=POST_INC_PUNCTUATOR;
		}

		if(evalExpressionDecPunctuator(s)){
			op = POST_DEC_PUNCTUATOR;
		}

		if(op != UNKNOWN_PUNCTUATOR){ // let's check some situations whether is not allowed having post operator
			PUNCTUATOR_TYPE pt=UNKNOWN_PUNCTUATOR;
			int line=0;

			char *aux=(char *)(s+strlen(defined_operator_punctuator[op].str));

			aux=IGNORE_BLANKS(aux,line); // advance to next char...

			if(*aux == 0){
				return op;
			}

			// if is an operator ... ok!
			pt=isPunctuator(aux);

			if(pt != UNKNOWN_PUNCTUATOR){
				if(pt < MAX_OPERATOR_PUNCTUATORS){ // ok...
					return op;
				}

				if(   pt == COMA_PUNCTUATOR  // ok
				  ||  pt == SEMICOLON_PUNCTUATOR
				  ||  pt == CLOSE_PARENTHESIS_PUNCTUATOR
				  ||  pt == CLOSE_SQUARE_BRAKET_PUNCTUATOR
				  ){
					return op;
				}
			}
		}
		return UNKNOWN_PUNCTUATOR;
	}

	char * CEval::evalExpressionFunctionObject(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

		return NULL;
	}


	bool CEval::printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line){
		PUNCTUATOR_TYPE ip=CEval::isPunctuator(current_string_ptr);
		KEYWORD_TYPE kw=CEval::isKeyword(current_string_ptr);

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

	char * CEval::evalExpressionStructObject(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			string symbol_value;
			char *end_p;
			PASTNode attr_node = NULL;
			int m_lineSymbol;

			if(*aux_p == '{'){ // go for final ...


				if(((*ast_node_to_be_evaluated)=CASTNode::newASTNode())==NULL) return NULL;
				(*ast_node_to_be_evaluated)->line_value = m_line;
				(*ast_node_to_be_evaluated)->node_type = STRUCT_NODE;
				(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
				if(scope_info != NULL){
					(*ast_node_to_be_evaluated)->idxScope = scope_info->idxScope;
				}


				// this solve problem void structs...
				aux_p=IGNORE_BLANKS(aux_p+1,m_line);

				while (*aux_p != '}' && *aux_p != 0){

					m_lineSymbol = m_line;
					//aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					// expect word...
					end_p = getEndWord(aux_p, m_line);

					 if(end_p == NULL || end_p == aux_p){
						 writeErrorMsg(CURRENT_PARSING_FILENAME,m_lineSymbol ,"Expected symbol after ','");
						 return NULL;
					 }


					 symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_p);
					 aux_p=IGNORE_BLANKS(end_p,m_line);

					 if(*aux_p != ':'){ // expected : ...
						 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected ':'");
						 return NULL;
					 }

					 aux_p++;

					 // go to variable...
					 if((aux_p=evalExpression_Recursive(aux_p,m_line,scope_info,&attr_node,vt)) == NULL){  //IGNORE_BLANKS(aux_p+1,m_line);
						 return NULL;
					 }

					 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...

					 attr_node->symbol_value = symbol_value;
					 attr_node->line_value = m_lineSymbol;
					(*ast_node_to_be_evaluated)->children.push_back(attr_node->idxAstNode);
					attr_node->idxAstParent =(*ast_node_to_be_evaluated)->idxAstNode;


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


	char * CEval::evalExpressionVectorObject(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated, vector<CASTNode *> & vt){


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


			aux_p=evalExpression(aux_p,m_line,scope_info);

			v_elements++;
		}

		return aux_p;


	}

	char * CEval::evalExpressionNew(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;
		string symbol_value;

		KEYWORD_TYPE key_w;
		PASTNode args_node=NULL;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::NEW_KEYWORD){
				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),m_line);
				// try get symbol ...

				end_p = getEndWord(aux_p, m_line);

				 if(end_p == NULL || end_p == aux_p){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected symbol");
					 return NULL;
				 }
				 symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_p);
				 aux_p=IGNORE_BLANKS(end_p,m_line);

				 if(*aux_p != '('){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected '(' after \'%s\'",defined_keyword[key_w].str);
					 return NULL;
				 }

				 aux_p = evalExpressionArgs('(', ')',aux_p,m_line,scope_info,&args_node);
				 if(aux_p == NULL){
					 return NULL;
				 }

				args_node->node_type = ARGS_PASS_NODE;
				 // it seems everything is allright... let's create the node...

				if(((*ast_node_to_be_evaluated) = CASTNode::newASTNode())==NULL) return NULL;
				(*ast_node_to_be_evaluated)->node_type = NEW_OBJECT_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = KEYWORD_TYPE::UNKNOWN_KEYWORD;
				(*ast_node_to_be_evaluated)->symbol_value = symbol_value;
				(*ast_node_to_be_evaluated)->children.push_back(args_node->idxAstNode);

				return aux_p;
			}
		}
		return NULL;
	}


	bool CEval::isMarkEndExpression(char c){
		return (c==0 || c==';' || c==',' ||  c==')'  || c==']' || c=='}');//|| c==':');
	}
	//-----------------------------------------------------------------------------------------------------------


	char * CEval::evalExpression(const char *s, int & m_line, CScope *scope_info ){


		vector<CASTNode *> vt;
		PASTNode ast_node_to_be_evaluated=NULL;

		// PRE: s is current string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.
		if(*s==0) {
			writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"End string");
			return NULL;
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



	char * CEval::evalKeywordDelete(const char *s,int & m_line,  CScope *scope_info){
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

				end_p = getEndWord(aux_p, m_line);

				 if(end_p == NULL || end_p == aux_p){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected symbol");
					 return NULL;
				 }

				 symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_p);


				// ends with ;

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

	char * CEval::evalKeywordClass(const char *s,int & m_line, CScope *scope_info){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;

		CScope *class_scope_info=NULL;
		//CScriptClass *class_info=NULL;
		int class_line;
		string class_name;
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
				end_p = getEndWord(aux_p, m_line);
				if(end_p == NULL || end_p == aux_p){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected name class");
					return NULL;
				}
				// try to register class...
				class_line = m_line;
				class_name = CZetScriptUtils::copyStringFromInterval(aux_p, end_p);

				zs_print_debug_cr("registered class \"%s\" line %i ",class_name.c_str(), class_line);

				aux_p=IGNORE_BLANKS(end_p,m_line);

				if(*aux_p == ':' ){
					string ext_name;
					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					end_p = getEndWord(aux_p, m_line);

					if(end_p == NULL || end_p == aux_p){
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected class name");
						return NULL;
					}

					ext_name=CZetScriptUtils::copyStringFromInterval(aux_p, end_p);



					aux_p=IGNORE_BLANKS(end_p, m_line);
				}
				if(*aux_p == '{' ){

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					// TODO: Register class and baseof

					// register info class ...
					// check for named functions or vars...
					while(*aux_p != '}' && *aux_p != 0){

						// 1st. check whether eval a keyword...
						key_w = isKeyword(aux_p);
						if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
							switch(key_w){
							default:
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected \"var\" or \"function\" keyword");
								return NULL;
								break;
							case KEYWORD_TYPE::FUNCTION_KEYWORD:
								if((aux_p = evalKeywordFunction(aux_p, m_line,class_scope_info)) == NULL){
									return NULL;
								}
								break;
							case KEYWORD_TYPE::VAR_KEYWORD:
								if((aux_p = evalKeywordVar(aux_p, m_line,class_scope_info)) == NULL){
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

	char * CEval::evalExpressionArgs(char c1,char c2,const char *s,int & m_line,  CScope *scope_info){
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
					if((aux_p = evalExpression(aux_p,m_line,scope_info))==NULL){
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

	char * CEval::evalKeywordFunction(const char *s,int & m_line,  CScope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *symbol_value,*end_var;
		KEYWORD_TYPE key_w;

		PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
		string conditional_str;
		bool error=false;
		short idxScopeClass;


		tScopeVar * irv=NULL;
		string str_name;
		string class_member,class_name, function_name="";


		int idxScope=ZS_UNDEFINED_IDX;
		CScope *body_scope=NULL;


		if(scope_info != NULL){
			idxScope=scope_info->idxScope;
		}

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::FUNCTION_KEYWORD){

				// advance keyword...
				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);


				bool named_function = *aux_p!='(';

				if(named_function){ // is named function..

					if((end_var=isClassMember(aux_p,m_line, idxScopeClass,class_name, error,key_w))!=NULL){ // check if particular case extension attribute class
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
							end_var=getSymbolName(aux_p,m_line);

							if(end_var != NULL){

								if((symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_var))==NULL)
									return NULL;

								function_name = symbol_value;

								// check whether parameter name's matches with some global variable...
							}else{
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
						end_var=getSymbolName(aux_p,m_line);

						if((symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_var))==NULL)
							return NULL;

						// check if repeats...
						for(unsigned k = 0; k < arg.size(); k++){
							if(arg[k] == symbol_value){
								writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Repeated argument '%s' argument ",symbol_value);
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

	char *  CEval::evalKeywordReturn(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		string s_aux;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...
				PASTNode child_node=NULL;
				aux_p += strlen(defined_keyword[key_w].str);


				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if((aux_p = evalExpression(aux_p, m_line, scope_info))!= NULL){

					if(ast_node_to_be_evaluated != NULL){ // return value;
						if(child_node != NULL){
							(*ast_node_to_be_evaluated)->children.push_back(child_node->idxAstNode);
						}
						else{ // return;

						}
					}

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

	char * CEval::evalWhile(const char *s,int & m_line, CScope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		CScope *_currentScope=NULL;

		PASTNode conditional_expression=NULL, while_node=NULL;
		string conditional_str;
		bool error = false;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::WHILE_KEYWORD){



				aux_p += strlen(defined_keyword[key_w].str);

				// evaluate conditional line ...
				aux_p=IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p == '('){

					if((end_expr = evalExpression(aux_p+1,m_line,_currentScope)) != NULL){

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

	char * CEval::evalDoWhile(const char *s,int & m_line, CScope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		CScope *_currentScope=NULL;

		PASTNode conditional_expression=NULL, body_node=NULL, while_node=NULL;
		string conditional_str;
		bool error = false;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

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

							if((end_expr = evalExpression(aux_p+1,m_line,_currentScope)) != NULL){

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

	char * CEval::evalKeywordIf(const char *s,int & m_line,  CScope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		int dl=-1;
		KEYWORD_TYPE key_w;
		PASTNode conditional=NULL, if_node=NULL, else_node=NULL,block=NULL, group_conditional_nodes = NULL;
		string conditional_str;
		bool error = false;
		int conditional_line;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

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

					if((end_expr = evalExpression(aux_p+1,m_line,scope_info)) == NULL){
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

	char * CEval::evalKeywordFor(const char *s,int & m_line,  CScope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		bool error=false;
		PASTNode block_for = NULL,pre_node=NULL,cond_node=NULL,post_node=NULL, pre_node_expression=NULL, cond_node_expression=NULL,post_node_expression=NULL;
		string eval_for;

		//CScope *_localScope =  scope_info != NULL?scope_info->symbol_info.ast->scope_info_ptr:NULL; // gets scope...
		CScope *_currentScope=NULL;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

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
							if((aux_p = evalKeywordVar(aux_p,m_line, _currentScope))==NULL){
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


						if((aux_p = evalExpression((const char *)aux_p,m_line,_currentScope)) == NULL){
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

								if((aux_p = evalExpression((const char *)aux_p,m_line,_currentScope)) == NULL){
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
								if((aux_p = evalExpression(aux_p,m_line,_currentScope))==NULL){
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


	char * CEval::evalKeWordSwitch(const char *s,int & m_line,  CScope *scope_info){

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

		bool error=false;
		int n_cases;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::SWITCH_KEYWORD){

				currentScope=scope_info->pushScope();

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p == '('){
					//if((end_expr = ADVANCE_TO_CHAR(aux_p,')',m_line)) != NULL){
						aux_p=IGNORE_BLANKS(aux_p+1,m_line);

						// evaluate switch vale expression ...
						PASTNode condition_expression_to_evaluate = NULL;
						//static char * evalExpression_Recursive(const char *s, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated=NULL,GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL);
						if((aux_p = CEval::evalExpression(
								aux_p,
								m_line,
								scope_info
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

							if((aux_p=eval_Recursive(aux_p, m_line, currentScope, error))==NULL){
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

	char * CEval::evalVar(const char *s,int & m_line,  CScope *scope_info){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

		char *aux_p = (char *)s;

		KEYWORD_TYPE key_w;
		char *start_var,*end_var;
		string class_name, class_member;
		PASTNode class_node;
		PASTNode var_node;
		PASTNode vars_collection_node=NULL;
		bool error=false;
		int idxScope=ZS_UNDEFINED_IDX;
		string s_aux,variable_name;
		char *symbol_value;
		bool end=false;
		bool allow_for_in=true;

		bool parent_scope_is_class=false;
		int m_startLine=0;

		if(scope_info != NULL){// && class_scope){ // if class scope let's see if is within function member..
			if(scope_info->getIdxBaseScope() != 0) { // if base scope != 0 is a class
				parent_scope_is_class = scope_info->getIdxBaseScope() == scope_info->getCurrentScopePointer()->idxScope;
			}
		}

		aux_p=IGNORE_BLANKS(aux_p,m_line);
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				//


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

					if((end_var=isClassMember(aux_p,m_line,class_name,class_member, error,key_w))!=NULL){ // check if particular case extension attribute class
						idxScope = class_node->idxScope; // override scope info
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
							end_var=getSymbolName(aux_p,m_line);

							if(end_var != NULL){

								if((symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_var))==NULL)
									return NULL;

								variable_name = symbol_value;

								// check whether parameter name's matches with some global variable...
							}else{
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

							if((aux_p = evalExpression(start_var,m_startLine,scope_info)) == NULL){
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

	char * CEval::evalBlock(const char *s,int & m_line,  CScope *scope_info, bool & error){
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


			if((aux_p = eval_Recursive(aux_p, m_line, currentScope,error)) != NULL){
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

	char * CEval::isClassMember(const char *s,int & m_line, short & idxScopeClass, string & _class_name, string & var_name, bool & error, KEYWORD_TYPE kwi){

		char *aux_p = (char *)s;
		char *end_var;
		char *symbol_value;
		idxScopeClass=-1;

		error = true;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check whwther the function is anonymous or not.
		end_var=getSymbolName(aux_p,m_line);

		if(end_var != NULL){

			if((symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_var))==NULL)
				return NULL;
		}else{
			return NULL;
		}
		aux_p=end_var;
		aux_p=IGNORE_BLANKS(aux_p,m_line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...
			_class_name = symbol_value;

			// TODO: find symbol from CScriptClass instead of AST
			fprintf(stderr,"find symbol from CScriptClass instead of AST\n");
			return NULL;


			aux_p=IGNORE_BLANKS(aux_p+2,m_line); // ignore ::

			end_var=getSymbolName(aux_p,m_line);

			if(end_var != NULL){

				if((symbol_value = CZetScriptUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;
			}else{
				return NULL;
			}

			var_name = symbol_value;
			aux_p=IGNORE_BLANKS(end_var,m_line);
			error = false;
			return aux_p;
		}else {
			error = false;
			return NULL;
		}
		return NULL;
	}


	char *CEval::evalKeyword(const char *s, int & m_line, CScope *scope_info, bool & error, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p= (char *)s;

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD,keyw2nd=KEYWORD_TYPE::UNKNOWN_KEYWORD;

		aux_p=IGNORE_BLANKS(aux_p, m_line);

		// check if condition...
		keyw = isKeyword(aux_p);

		if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...

			aux_p+=strlen(defined_keyword[keyw].str);
			aux_p=IGNORE_BLANKS(aux_p, m_line);
			char *value_to_eval;

			if(keyw == KEYWORD_TYPE::CASE_KEYWORD ||keyw == KEYWORD_TYPE::DEFAULT_KEYWORD){
				PASTNode ast=AST_NODE(scope_info->idxAstNode);
				if(ast->keyword_info != KEYWORD_TYPE::SWITCH_KEYWORD){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"case or default\" are allowed only within switch statements");
					error = true;
					return NULL;
				}

				if(keyw == KEYWORD_TYPE::CASE_KEYWORD){

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
					char *end_symbol = getEndWord(aux_p, m_line);
					aux_p=end_symbol;

					value_to_eval = CZetScriptUtils::copyStringFromInterval(start_symbol, end_symbol);

					if(value_to_eval==NULL){ return NULL;}

				}

				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p != ':'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"Expected  ':' ");
					return NULL;
				}



				return aux_p+1;

			}else if(keyw == KEYWORD_TYPE::BREAK_KEYWORD){


				if(ast_node_to_be_evaluated!=NULL){


					// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
					fprintf(stderr,"find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n");

					{ // ok break is valid in current scope...

						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"break\" allowed within loop or case-switch statements");
						error = true;
						return NULL;
					}
				}

				if(*aux_p != ';'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected ';'");
					error = true;
					return NULL;
				}

				return aux_p+1;
			}else if(keyw == KEYWORD_TYPE::CONTINUE_KEYWORD){

				if(ast_node_to_be_evaluated!=NULL){

					// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
					fprintf(stderr,"find findConditionForContinue if current stackBreakForWhileSwitch.size() > 0\n");

					{ // ok break is valid in current scope...
						writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"\"continue\" allowed within for or foreach loop");
						error = true;
						return NULL;
					}
				}

				if(*aux_p != ';'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"expected ';'");
					error = true;
					return NULL;
				}


				return aux_p+1;

			}

			// check if non named function...
			if(keyw == KEYWORD_TYPE::FUNCTION_KEYWORD){
				if( *aux_p == '('){
					// Is no named. No named function is an object and should be processed within evalExpression ...
					return NULL;
				}
			}

			// check if class and is not main class (scope )...
			if(keyw == KEYWORD_TYPE::CLASS_KEYWORD){

				if((aux_p = evalKeywordClass(s,m_line,scope_info)) != NULL){
					return aux_p;
				}

				error = true;
				return NULL;
			}

			// check if another kwyword is defined ...
			if((keyw2nd = isKeyword(aux_p))!= KEYWORD_TYPE::UNKNOWN_KEYWORD){

				if(
						   (keyw2nd != KEYWORD_TYPE::FUNCTION_KEYWORD)   // list of exceptional keywords...
						&& (keyw2nd != KEYWORD_TYPE::THIS_KEYWORD)   // list of exceptional keywords...
						&& (keyw2nd != KEYWORD_TYPE::NEW_KEYWORD)   // list of exceptional keywords...
				  ){

					writeErrorMsg(CURRENT_PARSING_FILENAME,m_line,"unexpected keyword \"%s\"",defined_keyword[keyw2nd].str);
					error = true;
					return NULL;
				}
			}

			if(defined_keyword[keyw].eval_fun != NULL){

				if((aux_p = defined_keyword[keyw].eval_fun(s,m_line,scope_info, ast_node_to_be_evaluated)) != NULL){
					return aux_p;
				}
				error = true;
			}
			// something wrong was happen..
		}

		return NULL;
	}

	char * CEval::eval_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error){

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
				if((end_expr = evalKeyword(aux, m_line, scope_info, error)) == NULL){

					// If was unsuccessful then try to eval expression.
					if(error){
						manageOnErrorParse(node_to_be_evaluated);
						THROW_SCRIPT_ERROR();
						return NULL;
					}
					// 2nd. check whether eval a block
					if((end_expr = evalBlock(aux
							,m_line
							, scope_info
							, error
							))==NULL){

						// If was unsuccessful then try to eval expression.
						if(error){
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						// 2nd. try expression
						int starting_expression=m_line;

						if((end_expr = evalExpression(aux,m_line, scope_info)) == NULL){ // something wrong was happen.
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr == ')'){ // unexpected close parenthesis.
							error = true;
							writeErrorMsg(CURRENT_PARSING_FILENAME,starting_expression,"missing '('");
							manageOnErrorParse(node_to_be_evaluated);
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr != ';'){
							error = true;
							writeErrorMsg(CURRENT_PARSING_FILENAME,starting_expression,"Expected ';'");
							manageOnErrorParse(node_to_be_evaluated);
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

		defined_operator_punctuator[ADD_PUNCTUATOR]={ADD_PUNCTUATOR, "+",evalExpressionPlusPunctuator};
		defined_operator_punctuator[SUB_PUNCTUATOR]={SUB_PUNCTUATOR, "-",evalExpressionMinusPunctuator};
		defined_operator_punctuator[MUL_PUNCTUATOR]={MUL_PUNCTUATOR, "*",evalExpressionMulPunctuator};
		defined_operator_punctuator[DIV_PUNCTUATOR]={DIV_PUNCTUATOR, "/",evalExpressionDivPunctuator};
		defined_operator_punctuator[MOD_PUNCTUATOR]={MOD_PUNCTUATOR, "%",evalExpressionModPunctuator};
		defined_operator_punctuator[FIELD_PUNCTUATOR]={FIELD_PUNCTUATOR, ".",evalExpressionFieldPunctuator};
		defined_operator_punctuator[TERNARY_IF_PUNCTUATOR]={TERNARY_IF_PUNCTUATOR, "?",evalExpressionInlineIfPunctuator};
		defined_operator_punctuator[TERNARY_ELSE_PUNCTUATOR]={TERNARY_ELSE_PUNCTUATOR, ":",evalExpressionInlineElsePunctuator};
		defined_operator_punctuator[ASSIGN_PUNCTUATOR]={ASSIGN_PUNCTUATOR, "=",evalExpressionAssignPunctuator};
		defined_operator_punctuator[ADD_ASSIGN_PUNCTUATOR]={ADD_ASSIGN_PUNCTUATOR, "+=",evalExpressionAddAssignPunctuator};
		defined_operator_punctuator[SUB_ASSIGN_PUNCTUATOR]={SUB_ASSIGN_PUNCTUATOR, "-=",evalExpressionSubAssignPunctuator};
		defined_operator_punctuator[MUL_ASSIGN_PUNCTUATOR]={MUL_ASSIGN_PUNCTUATOR, "*=",evalExpressionMulAssignPunctuator};
		defined_operator_punctuator[DIV_ASSIGN_PUNCTUATOR]={DIV_ASSIGN_PUNCTUATOR, "/=",evalExpressionDivAssignPunctuator};
		defined_operator_punctuator[MOD_ASSIGN_PUNCTUATOR]={MOD_ASSIGN_PUNCTUATOR, "%=",evalExpressionModAssignPunctuator};
		defined_operator_punctuator[BINARY_XOR_PUNCTUATOR]={BINARY_XOR_PUNCTUATOR, "^",evalExpressionBinaryXorPunctuator};
		defined_operator_punctuator[BINARY_AND_PUNCTUATOR]={BINARY_AND_PUNCTUATOR, "&",evalExpressionBinaryAndPunctuator};
		defined_operator_punctuator[BINARY_OR_PUNCTUATOR]={BINARY_OR_PUNCTUATOR, "|",evalExpressionBinaryOrPunctuator};
		defined_operator_punctuator[SHIFT_LEFT_PUNCTUATOR]={SHIFT_LEFT_PUNCTUATOR, "<<",evalExpressionShiftLeftPunctuator};
		defined_operator_punctuator[SHIFT_RIGHT_PUNCTUATOR]={SHIFT_RIGHT_PUNCTUATOR, ">>",evalExpressionShiftRightPunctuator};
		defined_operator_punctuator[LOGIC_AND_PUNCTUATOR]={LOGIC_AND_PUNCTUATOR, "&&",evalExpressionLogicAndPunctuator};
		defined_operator_punctuator[LOGIC_OR_PUNCTUATOR]={LOGIC_OR_PUNCTUATOR, "||",evalExpressionLogicOrPunctuator};
		defined_operator_punctuator[LOGIC_EQUAL_PUNCTUATOR]={LOGIC_EQUAL_PUNCTUATOR, "==",evalExpressionLogicEqualPunctuator};
		defined_operator_punctuator[LOGIC_NOT_EQUAL_PUNCTUATOR]={LOGIC_NOT_EQUAL_PUNCTUATOR, "!=",evalExpressionLogicNotEqualPunctuator};
		defined_operator_punctuator[LOGIC_GT_PUNCTUATOR]={LOGIC_GT_PUNCTUATOR, ">",evalExpressionLogicGreatherThanPunctuator};
		defined_operator_punctuator[LOGIC_LT_PUNCTUATOR]={LOGIC_LT_PUNCTUATOR, "<",evalExpressionLogicLessThanPunctuator};
		defined_operator_punctuator[LOGIC_GTE_PUNCTUATOR]={LOGIC_GTE_PUNCTUATOR, ">=",evalExpressionLogicGreatherEqualThanPunctuator};
		defined_operator_punctuator[LOGIC_LTE_PUNCTUATOR]={LOGIC_LTE_PUNCTUATOR, "<=",evalExpressionLessEqualThanPunctuator};
		defined_operator_punctuator[INSTANCEOF_PUNCTUATOR]={INSTANCEOF_PUNCTUATOR, "instanceof",evalExpressionInstanceOfPunctuator};

		defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR]={LOGIC_NOT_PUNCTUATOR, "!",evalExpressionNotPunctuator};
		defined_operator_punctuator[PRE_INC_PUNCTUATOR]={PRE_INC_PUNCTUATOR, "++",evalExpressionIncPunctuator};
		defined_operator_punctuator[PRE_DEC_PUNCTUATOR]={PRE_DEC_PUNCTUATOR, "--",evalExpressionDecPunctuator};
		defined_operator_punctuator[POST_INC_PUNCTUATOR]={POST_INC_PUNCTUATOR, "++",evalExpressionIncPunctuator};
		defined_operator_punctuator[POST_DEC_PUNCTUATOR]={POST_DEC_PUNCTUATOR, "--",evalExpressionDecPunctuator};

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
		defined_keyword[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",evalKeywordIf};
		defined_keyword[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else",NULL};
		defined_keyword[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for",evalKeywordFor};
		//defined_keyword[KEYWORD_TYPE::FOREACH_KEYWORD] = {FOREACH_KEYWORD,"foreach",evalForeach};
		defined_keyword[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while",evalKeywordWhile};
		defined_keyword[KEYWORD_TYPE::DO_WHILE_KEYWORD] = {DO_WHILE_KEYWORD,"do",evalKeywordDoWhile}; // while is expected in the end ...

		defined_keyword[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch",evalKeywordSwitch};
		defined_keyword[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case",NULL};
		defined_keyword[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break",NULL};
		defined_keyword[KEYWORD_TYPE::CONTINUE_KEYWORD] = {CONTINUE_KEYWORD,"continue",NULL};
		defined_keyword[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default",NULL};
		defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",evalKeywordFunction};
		defined_keyword[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return",evalKeywordReturn};
		defined_keyword[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this", NULL};
	//	defined_keyword[KEYWORD_TYPE::SUPER_KEYWORD] = {SUPER_KEYWORD,"super", NULL};
		defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",NULL};
		defined_keyword[KEYWORD_TYPE::NEW_KEYWORD] = {NEW_KEYWORD,"new", NULL};
		defined_keyword[KEYWORD_TYPE::DELETE_KEYWORD] = {DELETE_KEYWORD,"delete",evalKeywordDelete};
		defined_keyword[KEYWORD_TYPE::IN_KEYWORD] = {IN_KEYWORD,"in",NULL};

		// DIRECTIVES
		defined_directive[UNKNOWN_DIRECTIVE]={UNKNOWN_DIRECTIVE, NULL};
		defined_directive[INCLUDE_DIRECTIVE]={INCLUDE_DIRECTIVE, "import"};



			// create main ast management
	}



}
