#include "zs_core.h"


#define MAX_EXPRESSION_LENGHT 2096


#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __VERBOSE_MESSAGE__
#endif

#ifdef  __VERBOSE_MESSAGE__

#define print_ast_cr zs_print_info_cr
#else
#define print_ast_cr(s,...)
#endif

namespace zetscript{


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
				while (*aux_p!=0 && *aux_p !='\"' && *aux_p != '\n') {aux_p++;}

				if(*aux_p != '\"'){
					zs_print_error_cr("string is not closed at line %i",m_line);
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

				//while(*aux_p!=0 && *aux_p!='*') {



				//}
				//if(*aux_p == '\n') {aux_p++;m_line++;}; // make compatible windows format...
				//if(*aux_p == '\r') aux_p++;
				if(*aux_p == '*' && *(aux_p+1) == '/') {
					end=true;
					//aux_p+=2;
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



	tKeywordInfo CASTNode::defined_keyword[MAX_KEYWORD];
	tPunctuatorInfo CASTNode::defined_operator_punctuator[MAX_PUNCTUATORS];
	int CASTNode::DUMMY_LINE=0;


	vector<CASTNode *> 			* CASTNode::vec_ast_node=NULL;


	void CASTNode::setVectorASTNode(vector<CASTNode *> 	* set_vec_ast_node){
		vec_ast_node = set_vec_ast_node;
	}

	vector<CASTNode *> 		*		CASTNode::getVectorASTNode(){
		return vec_ast_node;
	}

	CASTNode	*CASTNode::newASTNode(){


		if(vec_ast_node->size() >= MAX_AST_NODES){
			zs_print_error_cr("Max AST Nodes reached (%i)",MAX_AST_NODES);
			return NULL;
		}

		CASTNode	*ast_node = new CASTNode();
		vec_ast_node->push_back(ast_node);
		ast_node->idxAstNode = (short)(vec_ast_node->size()-1);
		return ast_node;
	}

	/**
	 * Get CASTNode Node by its idx, regarding current state.
	 */
	CASTNode 		* CASTNode::getAstNode(short idx){
		if(idx==ZS_UNDEFINED_IDX){
			return NULL;
		}

		if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
			zs_print_error_cr("Ast node out of bound");
			return NULL;
		}
		 return vec_ast_node->at(idx);
	}

	int 	CASTNode::getScopeIdx(short idx){
		if(idx==ZS_UNDEFINED_IDX){
			return ZS_UNDEFINED_IDX;
		}

		if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
			zs_print_error_cr("Ast node out of bound");
			return ZS_UNDEFINED_IDX;
		}

		return vec_ast_node->at(idx)->idxScope;
	}

	CScope * 	CASTNode::getScope(short idx){
		if(idx==ZS_UNDEFINED_IDX){
			return NULL;
		}

		if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
			zs_print_error_cr("Ast node out of bound");
			return NULL;
		}

		return SCOPE_INFO_NODE(vec_ast_node->at(idx)->idxScope);
	}

	int 		CASTNode::getAstLine(short idx){
		if(idx==ZS_UNDEFINED_IDX){
			return -1;
		}

		if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
			zs_print_error_cr("Ast node out of bound");
			return -1;
		}
		return vec_ast_node->at(idx)->line_value;
	}

	const char * CASTNode::getAstSymbolName(short idx){
		if(idx==ZS_UNDEFINED_IDX){
			return "undefined symbol";
		}

		if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
			zs_print_error_cr("Ast node out of bound");
			return "";
		}
		return vec_ast_node->at(idx)->symbol_value.c_str();
	}

	const char 		* CASTNode::getAstSymbolNameConstChar(short idx){
		if(idx==ZS_UNDEFINED_IDX){
			return "undefined symbol";
		}

		if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
			zs_print_error_cr("Ast node out of bound");
			return NULL;
		}

		return vec_ast_node->at(idx)->symbol_value.c_str();
	}


	bool CASTNode::parsePlusPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) != '+');
		return false;
	}

	bool CASTNode::parseMinusPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) != '-');
		return false;
	}

	bool CASTNode::parseMulPunctuator(const char *s){
		return *s == '*';
	}

	bool CASTNode::parseDivPunctuator(const char *s){
		return *s == '/';
	}

	bool CASTNode::parseModPunctuator(const char *s){
		return *s == '%';
	}

	bool CASTNode::parseFieldPunctuator(const char *s){
		return *s == '.';
	}

	bool CASTNode::parseInlineIfPunctuator(const char *s){
		return *s == '?';
	}

	bool CASTNode::parseInlineElsePunctuator(const char *s){
		return *s == ':';
	}

	bool CASTNode::parseAssignPunctuator(const char *s){
		if(*s=='=')
			return (*(s+1) != '=');
		return false;
	}

	bool CASTNode::parseAddAssignPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseSubAssignPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseMulAssignPunctuator(const char *s){
		if(*s=='*')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseDivAssignPunctuator(const char *s){
		if(*s=='/')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseModAssignPunctuator(const char *s){
		if(*s=='%')
			return (*(s+1) == '=');
		return false;
	}


	bool CASTNode::parseBinaryXorPunctuator(const char *s){
		return *s == '^';
	}

	bool CASTNode::parseBinaryAndPunctuator(const char *s){
		if(*s=='&')
			return (*(s+1) != '&');
		return false;
	}

	bool CASTNode::parseBinaryOrPunctuator(const char *s){
		if(*s=='|')
			return (*(s+1) != '|');
		return false;
	}

	bool CASTNode::parseShiftLeftPunctuator(const char *s){
		if(*s=='<')
			return (*(s+1) == '<');
		return false;
	}

	bool CASTNode::parseShiftRightPunctuator(const char *s){
		if(*s=='>')
			return (*(s+1) == '>');
		return false;
	}

	bool CASTNode::parseLogicAndPunctuator(const char *s){
		if(*s=='&')
			return (*(s+1) == '&');
		return false;
	}

	bool CASTNode::parseLogicOrPunctuator(const char *s){
		if(*s=='|')
			return (*(s+1) == '|');
		return false;
	}

	bool CASTNode::parseLogicEqualPunctuator(const char *s){
		if(*s=='=')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseLogicNotEqualPunctuator(const char *s){
		if(*s=='!')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseLogicGreatherThanPunctuator(const char *s){
		if( *s == '>')
			return (*(s+1) != '>');
		return false;
	}

	bool CASTNode::parseLogicLessThanPunctuator(const char *s){
		if(*s == '<')
			return (*(s+1) != '<');

		return false;
	}

	bool CASTNode::parseLogicGreatherEqualThanPunctuator(const char *s){
		if(*s=='>')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseLessEqualThanPunctuator(const char *s){
		if(*s=='<')
			return (*(s+1) == '=');
		return false;
	}

	bool CASTNode::parseNotPunctuator(const char *s){
		if(*s=='!')
			return (*(s+1) != '=');
		return false;
	}


	bool CASTNode::parseIncPunctuator(const char *s){
		if(*s=='+')
			return (*(s+1) == '+');
		return false;
	}

	bool CASTNode::parseDecPunctuator(const char *s){
		if(*s=='-')
			return (*(s+1) == '-');
		return false;
	}

	#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup0(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				ASSIGN_PUNCTUATOR,
				ADD_ASSIGN_PUNCTUATOR,
				SUB_ASSIGN_PUNCTUATOR,
				MUL_ASSIGN_PUNCTUATOR,
				DIV_ASSIGN_PUNCTUATOR,
				MOD_ASSIGN_PUNCTUATOR
		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup1(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				TERNARY_IF_PUNCTUATOR,
				TERNARY_ELSE_PUNCTUATOR
		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup2(const char *s){


		PUNCTUATOR_TYPE index_to_evaluate[]={
				LOGIC_EQUAL_PUNCTUATOR,
				LOGIC_LTE_PUNCTUATOR,
				LOGIC_GTE_PUNCTUATOR,
				LOGIC_GT_PUNCTUATOR,
				LOGIC_LT_PUNCTUATOR
		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup3(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				ADD_PUNCTUATOR,
				SUB_PUNCTUATOR,
				BINARY_XOR_PUNCTUATOR,
				BINARY_AND_PUNCTUATOR,
				BINARY_OR_PUNCTUATOR,
				SHIFT_LEFT_PUNCTUATOR,
				SHIFT_RIGHT_PUNCTUATOR
		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup4(const char *s){


		PUNCTUATOR_TYPE index_to_evaluate[]={
				LOGIC_NOT_EQUAL_PUNCTUATOR
		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE CASTNode::parsePunctuatorGroup5(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				LOGIC_AND_PUNCTUATOR,
				LOGIC_OR_PUNCTUATOR,
				MUL_PUNCTUATOR,
				DIV_PUNCTUATOR,
				MOD_PUNCTUATOR
		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup6(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				LOGIC_NOT_PUNCTUATOR

		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE  CASTNode::parsePunctuatorGroup7(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				FIELD_PUNCTUATOR

		};

		for(unsigned char  i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}
			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE   CASTNode::isOperatorPunctuator(const char *s){

		for(unsigned char  i = 0; i < MAX_OPERATOR_PUNCTUATORS; i++){
			if(defined_operator_punctuator[i].parse_fun != NULL){
				if(defined_operator_punctuator[i].parse_fun(s)){
					return defined_operator_punctuator[i].id;
				}
			}
		}
		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE   CASTNode::isSpecialPunctuator(const char *s){

		for(unsigned char i = START_SPECIAL_PUNCTUATORS; i < MAX_SPECIAL_PUNCTUATORS; i++){

			if(*defined_operator_punctuator[i].str == *s){
				return defined_operator_punctuator[i].id;
			}
		}

		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE   CASTNode::parseArithmeticPunctuator(const char *s){

		PUNCTUATOR_TYPE index_to_evaluate[]={
				SHIFT_LEFT_PUNCTUATOR, // <<
				SHIFT_RIGHT_PUNCTUATOR, // >>

				LOGIC_AND_PUNCTUATOR, // &&
				LOGIC_OR_PUNCTUATOR, // ||
				LOGIC_EQUAL_PUNCTUATOR, // =
				LOGIC_NOT_EQUAL_PUNCTUATOR, // !=
				LOGIC_GTE_PUNCTUATOR, // >=
				LOGIC_LTE_PUNCTUATOR, // <=

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

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){
				zs_print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);
				return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
			}

			if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
				return defined_operator_punctuator[index_to_evaluate[i]].id;
			}
		}
		return PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;;
	}

	PUNCTUATOR_TYPE   CASTNode::isPunctuator(const char *s){

		PUNCTUATOR_TYPE ip = isOperatorPunctuator(s);

		if(ip!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			return ip;
		}

		return isSpecialPunctuator(s);
	}

	// to string utils ...
	char * CASTNode::getEndWord(const char *s, int m_line){

		char *start_str=(char *)s;
		char *aux=(char *)s;
		PUNCTUATOR_TYPE sp;
		KEYWORD_TYPE key_w;
		 bool is_possible_number=false;
		 int i=0;
		 bool start_digit = false;


		if(*aux == '\"'){
			aux++;
			while((*aux)!=0 && !((*aux)=='\n') && !((*aux)=='\"')) {
				aux++;
			}

			if(*aux != '\"') {
				zs_print_error_cr("Error \" not closed");
				return NULL;
			}
			aux++;

		}else{

			if((key_w = isKeyword(s))!= KEYWORD_TYPE::UNKNOWN_KEYWORD){
				if( key_w != KEYWORD_TYPE::THIS_KEYWORD){
				 //&& key_w->id != KEYWORD_TYPE::SUPER_KEYWORD ){ // unexpected token ?
					zs_print_error_cr("Unexpected keyword \"%s\" at line %i. Forgot \";\" ?",defined_keyword[key_w].str, m_line);
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

					is_possible_number = CStringUtils::isDigit(*aux);
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
				string num = CStringUtils::copyStringFromInterval(start_str,aux);
				if(!CStringUtils::isNumber(num)){
					zs_print_error_cr("line %i: Invalid number format",m_line);
					return NULL;
				}
			}
		}
		return aux;
	}

	char *CASTNode::getSymbolName(const char *s,int & m_line){

		char *aux_p=(char *)s;
		PUNCTUATOR_TYPE end_punctuator=isPunctuator(s);


		if(end_punctuator != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			zs_print_error_cr("Unexpected '%s' at line %i",defined_operator_punctuator[end_punctuator].str,m_line);
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
			zs_print_error_cr(" line %i: Symbol name cannot begin with %c",m_line, *aux_p);
			return NULL;
		}

		return aux_p;
	}

	KEYWORD_TYPE CASTNode::isKeyword(const char *c){
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
	//------------------------------------------------------------------------------------------------------------
	PUNCTUATOR_TYPE CASTNode::checkPreOperatorPunctuator(const char *s){

		if(parseIncPunctuator(s)) 	return PRE_INC_PUNCTUATOR;
		if(parseDecPunctuator(s))	return PRE_DEC_PUNCTUATOR;
		if(parsePlusPunctuator(s)) 	return ADD_PUNCTUATOR;
		if(parseMinusPunctuator(s)) return SUB_PUNCTUATOR;
		if(parseNotPunctuator(s))   return LOGIC_NOT_PUNCTUATOR;

		return UNKNOWN_PUNCTUATOR;
	}

	PUNCTUATOR_TYPE CASTNode::checkPostOperatorPunctuator(const char *s){

		PUNCTUATOR_TYPE op=UNKNOWN_PUNCTUATOR;

		if(parseIncPunctuator(s)){
			op=POST_INC_PUNCTUATOR;
		}

		if(parseDecPunctuator(s)){
			op = POST_DEC_PUNCTUATOR;
		}

		if(op != UNKNOWN_PUNCTUATOR){ // let's check some situations whether is not allowed having post operator
			PUNCTUATOR_TYPE pt=UNKNOWN_PUNCTUATOR;
			int line=0;
			char *aux=IGNORE_BLANKS(s+2,line); // advance to next char...

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

	char *CASTNode::functionArrayAccess_Recursive(const char *str, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, PASTNode parent){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux = (char *)str;

		aux = IGNORE_BLANKS(aux, m_line);

		if((*aux == '(' || *aux == '[')){

			 if(ast_node_to_be_evaluated != NULL) {// save node
				 if((*ast_node_to_be_evaluated = CASTNode::newASTNode())==NULL) return NULL;
				 //(*ast_node_to_be_evaluated)->symbol_value = symbol_value; // is array or function ...
				 (*ast_node_to_be_evaluated)->line_value = m_line;
				 (*ast_node_to_be_evaluated)->node_type  = ARRAY_REF_NODE;
				 (*ast_node_to_be_evaluated)->idxScope =scope_info->idxScope;
			 }

			 if(*aux == '('){
				 if(ast_node_to_be_evaluated != NULL){
					 (*ast_node_to_be_evaluated)->node_type  = FUNCTION_REF_NODE;
					 (*ast_node_to_be_evaluated)->line_value = m_line;
					 (*ast_node_to_be_evaluated)->idxScope =scope_info->idxScope;
				 }
			 }

			PASTNode args_obj=NULL;

			if(*aux == '('){ // function access

				if( ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated)->node_type != FUNCTION_REF_NODE && (*ast_node_to_be_evaluated)->node_type != FUNCTION_OBJECT_NODE){
						zs_print_error_cr("Expected function object before '(' at line %i",m_line);
						return NULL;
					}
				}

				if((aux=parseArgs('(', ')',aux,m_line,scope_info,ast_node_to_be_evaluated != NULL? &args_obj : NULL)) == NULL){
					return NULL;
				}

				if(ast_node_to_be_evaluated != NULL){
					args_obj->node_type = NODE_TYPE::ARGS_PASS_NODE;
					//vector_args_node.push_back(args_node);//[0]->node_type = NODE_TYPE::ARGS_PASS_NODE;
				}
			}else if (*aux == '['){ // array acces multi dimensional like this : array[i][j][z] ...

				int i = 0;
				bool end = false;

				if( ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated)->node_type != ARRAY_REF_NODE){
						zs_print_error_cr("Expected array object before '[' at line %i",m_line);
						return NULL;
					}
				}

				if( ast_node_to_be_evaluated != NULL){
					if((args_obj = CASTNode::newASTNode())==NULL){return NULL;}
					args_obj->node_type = ARRAY_ACCESS_NODE;
				}

				do{
					PASTNode args_node=NULL;
					char *aux_ptr;
					int ini_line_access=m_line;

					if((aux_ptr=parseArgs('[', ']',aux,m_line,scope_info,ast_node_to_be_evaluated != NULL ? &args_node: NULL)) != NULL){
						if( ast_node_to_be_evaluated != NULL){
							if(args_node->children.size() != 1){
								zs_print_error_cr("Invalid array access %i",m_line);
								return NULL;
							}
							args_obj->children.push_back(args_node->idxAstNode);
							args_node->node_type = NODE_TYPE::ARRAY_INDEX_NODE;
							args_node->line_value =ini_line_access;
						}
						aux =  IGNORE_BLANKS(aux_ptr,m_line);
					}else{
						if(i == 0){
							return NULL;
						}else{
							end=true;
						}
					}
					i++;

				}while(!end);
			}

			if(ast_node_to_be_evaluated != NULL){
				// the deepth node is the object and the parent will the access node (function or array) ...
				if(args_obj != NULL){ // there's arguments to be pushed ...
					PASTNode obj =  *ast_node_to_be_evaluated;
					PASTNode calling_object;
					if((calling_object= CASTNode::newASTNode()) == NULL) return NULL;
					//tPunctuatorInfo *ip=NULL;

					calling_object->node_type = CALLING_OBJECT_NODE;

					obj->idxAstParent = calling_object->idxAstNode;
					obj->symbol_value = "--";
					args_obj->idxAstParent = calling_object->idxAstNode;

					calling_object->children.push_back(obj->idxAstNode); // the object itself...
					calling_object->children.push_back(args_obj->idxAstNode); // the args itself...
					calling_object->idxAstParent=-1;
					if(parent!=NULL){
						calling_object->idxAstParent=parent->idxAstNode;
					}

					// finally save ast node...

					*ast_node_to_be_evaluated = calling_object;
				}
			}

			PASTNode another_access=NULL;
			aux = functionArrayAccess_Recursive(aux,m_line,scope_info,ast_node_to_be_evaluated!=NULL?&another_access:NULL,ast_node_to_be_evaluated!=NULL?*ast_node_to_be_evaluated:NULL);

			if(another_access != NULL){
				(*ast_node_to_be_evaluated)->children.push_back(another_access->idxAstNode);
			}
		}
		return aux;
	}

	char *CASTNode::functionArrayAccess(const char *str, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, PASTNode parent){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		return functionArrayAccess_Recursive(str,m_line,scope_info,ast_node_to_be_evaluated,parent);
	}

	char * CASTNode::deduceExpression(const char *str, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, PASTNode parent){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux = (char *)str;
		char *end_expression;
		int m_startLine = m_line;
		KEYWORD_TYPE key_w  = KEYWORD_TYPE::UNKNOWN_KEYWORD;
		vector<PASTNode> vector_args_node;
		bool try_array_or_function_access = false;
		bool should_be_access=false;

		string symbol_value="";

		if(ast_node_to_be_evaluated != NULL){
			*ast_node_to_be_evaluated = NULL;
		}

		aux = IGNORE_BLANKS(aux, m_startLine);

		key_w = isKeyword(aux);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // can be new,delete or function...

			aux = IGNORE_BLANKS(aux+strlen(defined_keyword[key_w].str), m_startLine);

			switch(key_w){
			// sould be function object ...
			case KEYWORD_TYPE::FUNCTION_KEYWORD:
				// function objects are stored in MainClass or global scope.
				if((aux=parseFunction(str,m_startLine,scope_info,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL)) == NULL){
					return NULL;
				}
				try_array_or_function_access = true;
				break;
			case KEYWORD_TYPE::NEW_KEYWORD:
				if((aux = parseNew(str,m_startLine,scope_info,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL)) == NULL){
					return NULL;
				}
				break;
			case KEYWORD_TYPE::THIS_KEYWORD:
				return parseExpression_Recursive(str,  m_line, scope_info, ast_node_to_be_evaluated,GROUP_TYPE::GROUP_0,parent);
				break;
			default:
				zs_print_error_cr("Invalid using of \"%s\" at line %i",defined_keyword[key_w].str, m_startLine);
				break;
			}
		}else if(*aux == '[') { // is an array object ...

			if((aux = parseArgs('[', ']',aux,m_startLine,scope_info,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL))== NULL){
				return NULL;
			}

			aux = IGNORE_BLANKS(aux, m_startLine);
			try_array_or_function_access = true;

			if(ast_node_to_be_evaluated != NULL){
				(*ast_node_to_be_evaluated)->node_type = ARRAY_OBJECT_NODE;

			}
		}
		else{ // symbol or expression...
			char *word_str=NULL;
			// try to get function/array object ...
			end_expression = getEndWord(aux, m_startLine);
			//bool function_or_array = false;


			 if(!(end_expression == NULL || end_expression == aux)){ // is valid word...

				 symbol_value = CStringUtils::copyStringFromInterval(aux,end_expression);
				 word_str = IGNORE_BLANKS(end_expression, m_startLine);

				 try_array_or_function_access = true;

				 should_be_access = (*word_str == '(' || *word_str == '[');

				 if(!should_be_access){
					 return parseExpression_Recursive(aux,  m_line, scope_info, ast_node_to_be_evaluated,GROUP_TYPE::GROUP_0,parent);
				 }
				 else{
					 aux = word_str;
				 }
			 }else{ // try parse expression...
				 //zs_print_error_cr("Cannot parse ");
				 return parseExpression_Recursive(aux,  m_line, scope_info, ast_node_to_be_evaluated,GROUP_TYPE::GROUP_0,parent);
			 }
		}

		if(try_array_or_function_access){// try array/function access
			if((aux = functionArrayAccess(aux, m_line,scope_info,ast_node_to_be_evaluated,parent)) == NULL){
				 zs_print_error_cr("Cannot parse 2");
				return NULL;
			}

			if(ast_node_to_be_evaluated != NULL){
				if(*ast_node_to_be_evaluated == NULL && should_be_access){
					zs_print_error_cr("Cannot parse 3");
					return NULL;
				}

				if((*ast_node_to_be_evaluated)->children.size() > 0){
					vec_ast_node->at((*ast_node_to_be_evaluated)->children[0])->symbol_value = symbol_value;
				}
			}
		}
		return aux;
	}

	bool CASTNode::printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line){
		PUNCTUATOR_TYPE ip=CASTNode::isPunctuator(current_string_ptr);
		KEYWORD_TYPE kw=CASTNode::isKeyword(current_string_ptr);

		if(kw!=KEYWORD_TYPE::UNKNOWN_KEYWORD){
			zs_print_error_cr("Unexpected %s at line %i",defined_keyword[kw].str,m_line);
			return true;
		}
		else if(ip!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
			zs_print_error_cr("Unexpected %s at line %i",defined_operator_punctuator[ip].str,m_line);
			return true;
		}
		return false;
	}

	char *CASTNode::getSymbolValue(
			const char *current_string_ptr,
			int & m_line,
			CScope *scope_info,

			string & symbol_name,
			int & m_definedSymbolLine,
			PUNCTUATOR_TYPE pre_operator,
			PUNCTUATOR_TYPE & post_operator,
			bool & is_symbol_trivial
			){

		char *aux = (char *)current_string_ptr;
		char *end_expression = aux;
		is_symbol_trivial = false;
		m_definedSymbolLine = m_line;

		if(symbol_name == "super"){
			zs_print_error_cr("Invalid using \"super\" keyword at line %i",m_line);
			return NULL;
		}

		//----------------------------------------------------------
		// GETTING TRIVIAL SYMBOLS
		if(*aux=='('){ // packed symbol...

			if(pre_operator != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
				if(pre_operator == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
				   pre_operator == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
					zs_print_error_cr("Unexpected '%s' before ( at line %i",defined_operator_punctuator[pre_operator].str,m_line);
					return NULL;
				}
			}

			// update '('
			aux=aux+1;

			// only parse no evaluate (don't save ast node)
			end_expression = parseExpression_Recursive(aux, m_line, scope_info);//, ast_node_to_be_evaluated, type_group,parent);

			if(*end_expression != ')'){
				zs_print_error_cr("Not closed parenthesis starting at line %i", m_definedSymbolLine);
				return NULL;
			}

			 if(end_expression == NULL || end_expression == aux){
				 zs_print_error_cr("Expected expression at line %i",m_line);
				 return NULL;
			 }

			 symbol_name=CStringUtils::copyStringFromInterval(aux,end_expression);
			end_expression=end_expression+1;
			//end_expression = aux+1;
		}else{ // check for symbols (must have a symbol at least)

			KEYWORD_TYPE key_w;
			bool is_function_or_operator=false;
			bool is_new=false;
			bool is_delete=false;
			// usually we have to take care about special op symbols

			if((key_w =isKeyword(aux)) != KEYWORD_TYPE::UNKNOWN_KEYWORD){
				is_function_or_operator = key_w == KEYWORD_TYPE::FUNCTION_KEYWORD;
				is_new=key_w == KEYWORD_TYPE::NEW_KEYWORD;
				is_delete=key_w == KEYWORD_TYPE::DELETE_KEYWORD;
			}

			if(is_function_or_operator){ // function object ...

				if(pre_operator!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
					if(pre_operator == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
						pre_operator == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
						zs_print_error_cr("Unexpected '%s' before ( at line %i",defined_operator_punctuator[pre_operator].str,m_line);
						return NULL;
					}
				}
				// parse function but do not create ast node (we will create in trivial case value
				end_expression = parseFunction(aux,m_line,scope_info);

				//symbol_node->symbol_value="anonymous_function";
				if(end_expression == NULL){
					return NULL;
				}
				symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);
			}
			else // array object ...
			{
				if(*aux == '['){ // vector object ...
					// parse function but do not create ast node (we will create in trivial case value
					end_expression = parseArgs('[', ']',aux,m_line,scope_info);
					//symbol_node->symbol_value="anonymous_array";

					if(end_expression == NULL){
						return NULL;
					}
					symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);
				}
				else{
					char *start_expression = aux;
					is_symbol_trivial = true;

					// treat as symbol...
					if(is_new || is_delete) {
						aux = IGNORE_BLANKS(aux + strlen(defined_keyword[key_w].str), m_line);
						is_symbol_trivial = false;
					}

					end_expression = getEndWord(aux, m_line);

					 if(end_expression == NULL || end_expression == aux){

						 // check if punctuator or keyword..

						 if(end_expression != NULL){
							 if(!printErrorUnexpectedKeywordOrPunctuator(aux, m_line)){
								 zs_print_error_cr("Expected symbol at line %i",m_line);
							 }
						 }
						 return NULL;
					 }

					 symbol_name=CStringUtils::copyStringFromInterval(start_expression,end_expression);

					 // check for post opertator...
					 end_expression = IGNORE_BLANKS(end_expression, m_line);
					 if((post_operator = checkPostOperatorPunctuator(end_expression)) != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
						 end_expression+=strlen(defined_operator_punctuator[post_operator].str);
					 }

					 aux = start_expression;
				}
			}

			// if there's function or array access after symbol or object created ...
			end_expression = IGNORE_BLANKS(end_expression, m_line);

			if(*end_expression == '[' || *end_expression == '('){ // function or array access --> process its ast but not save ...
				is_symbol_trivial = false;
				end_expression = functionArrayAccess(end_expression,m_line,scope_info);

				if(end_expression == NULL){
					return NULL;
				}

				end_expression = IGNORE_BLANKS(end_expression, m_line);
				// check for post opertator...
				if(((post_operator) = checkPostOperatorPunctuator(end_expression)) != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
				 end_expression+=strlen(defined_operator_punctuator[post_operator].str);
				 end_expression = IGNORE_BLANKS(end_expression, m_line);
				}

				symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);
			}
		}
		// GETTING TRIVIAL SYMBOLS
		//----------------------------------------------------------
		return end_expression;
	}

	bool CASTNode::isMarkEndExpression(char c){
		return (c==0 || c==';' || c==',' ||  c==')'  || c==']' || c=='}');//|| c==':');
	}
	//-----------------------------------------------------------------------------------------------------------
	char * CASTNode::parseExpression_Recursive(const char *s, int & m_line,CScope *scope_info, PASTNode *ast_node_to_be_evaluated, GROUP_TYPE type_group,PASTNode parent ){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux=(char *)s;
		char *s_effective_start=(char *)s;
		char *expr_start_op=NULL;
		int start_line = m_line; // set another start line because left node or reparse to try another group was already parsed before.
		int m_lineOperator=-2;
		char *end_expression=(char *)s ; // by default end expression isequal to
		bool is_packed_expression=false;
		//PASTNode symbol_node=NULL; // can be a function or array.

		bool is_symbol_trivial_value=false;
		string symbol_value;
		string operator_str="";
		PUNCTUATOR_TYPE pre_operator=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR,
						post_operator=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR,
						operator_group=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
		bool theres_some_operator=false;
		int m_definedSymbolLine;
		bool special_pre_post_cond = false; // in case of particular pre/post...
		bool is_packed_node = false;
		//bool is_preoperator = true;


		aux=IGNORE_BLANKS(aux, m_line);

		if(isMarkEndExpression(*aux)){ // returning because is trivial!
			return aux;
		}

		if(type_group>=MAX_GROUPS) {
			zs_print_error_cr("Cannot find ast tree operator");
			return NULL;
		}

		print_ast_cr("new expression eval:\"%.30s ...\" group:%i at line %i",aux,type_group, m_line);

		// searching for operator!
		if(*aux == '{'){ //json expression...
			print_ast_cr("detected json expression");
			return parseStruct(aux,m_line,scope_info,ast_node_to_be_evaluated);
		}

		print_ast_cr("searching for operator type %i...",type_group);

		while(!isMarkEndExpression(*aux) && (operator_group==0)){
			special_pre_post_cond = false;
			print_ast_cr("checkpoint1:%c\n",*aux);
			// 1. ignore spaces...
			aux=IGNORE_BLANKS(aux, m_line);

			if((pre_operator=checkPreOperatorPunctuator(aux))!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
				aux+=strlen(defined_operator_punctuator[pre_operator].str);
				aux=IGNORE_BLANKS(aux, m_line);
			}

			if(*aux=='('){ // packed node let's said that is a packed node...
				if(ast_node_to_be_evaluated != NULL){
					is_packed_node = true;
				}
			}

			// try get symbol string
			if((aux=getSymbolValue(aux, m_line, scope_info,symbol_value, m_definedSymbolLine,pre_operator,post_operator,is_symbol_trivial_value)) == NULL){
				return NULL;
			}

			print_ast_cr("checkpoint3:%c\n",*aux);

			aux=IGNORE_BLANKS(aux, m_line);

			if(!isMarkEndExpression(*aux)){ // is not end expression


				if((operator_group=isOperatorPunctuator(aux))!=0){



					// particular cases... since the getSymbol alrady checks the pre operator it cannot be possible to get a pre inc or pre dec...
					if(operator_group ==  PRE_INC_PUNCTUATOR){ // ++ really is a + PUNCTUATOR...
						operator_group = ADD_PUNCTUATOR;
						special_pre_post_cond=true;
					}
					else if(operator_group ==  PRE_DEC_PUNCTUATOR){ // -- really is a + PUNCTUATOR...
						operator_group = SUB_PUNCTUATOR;
						special_pre_post_cond=true;
					}

					theres_some_operator |= true;
					expr_start_op=aux;
					m_lineOperator = m_line;

					//if(advance_ptr) // advance ...
					aux+=strlen(defined_operator_punctuator[operator_group].str);

					if(!special_pre_post_cond){ // not check because special case pre/post op...

						switch(type_group){
						case GROUP_0:	operator_group = parsePunctuatorGroup0(expr_start_op);break;
						case GROUP_1:	operator_group = parsePunctuatorGroup1(expr_start_op);break;
						case GROUP_2:	operator_group = parsePunctuatorGroup2(expr_start_op);break;
						case GROUP_3:	operator_group = parsePunctuatorGroup3(expr_start_op);break;
						case GROUP_4:	operator_group = parsePunctuatorGroup4(expr_start_op);break;
						case GROUP_5:	operator_group = parsePunctuatorGroup5(expr_start_op);break;
						case GROUP_6:	operator_group = parsePunctuatorGroup6(expr_start_op);break;
						case GROUP_7:	operator_group = parsePunctuatorGroup7(expr_start_op);break;
						default: break;
						}
					}
				}else{
					zs_print_error_cr("line %i:expected operator or punctuator after \"%s\"",m_line,symbol_value.c_str());
					return NULL;
				}
			}
		}

		if(operator_group==PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR) {// there's no any operators \"type_group\"...
			if(!theres_some_operator){ // only we have a value (trivial)

				if(ast_node_to_be_evaluated != NULL){

					if(is_symbol_trivial_value){

						if(((*ast_node_to_be_evaluated)=CASTNode::newASTNode()) == NULL) return NULL;
						(*ast_node_to_be_evaluated)->node_type = SYMBOL_NODE;
						(*ast_node_to_be_evaluated)->symbol_value=symbol_value; // assign its value ...
						(*ast_node_to_be_evaluated)->idxScope = scope_info->idxScope;

					}else{
						if(deduceExpression(symbol_value.c_str(),m_definedSymbolLine,scope_info, ast_node_to_be_evaluated, parent) == NULL){
							return NULL;
						}

						if(is_packed_node){ // packed node let's said that is a packed node...
							if(ast_node_to_be_evaluated != NULL){
								(*ast_node_to_be_evaluated)->is_packed_node = true;
							}
						}
					}

					print_ast_cr("---------------------");
					print_ast_cr("%s value \"%s\" at line %i",(is_symbol_trivial_value?"trivial":"NOT trivial"),symbol_value.c_str(), m_definedSymbolLine);
					print_ast_cr("---------------------");

					// put values by default ...
					(*ast_node_to_be_evaluated)->idxAstParent=ZS_UNDEFINED_IDX;
					if(parent!=NULL){
						(*ast_node_to_be_evaluated)->idxAstParent=parent->idxAstNode;
					}
					(*ast_node_to_be_evaluated)->line_value=m_definedSymbolLine;


					if(pre_operator!= PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR || post_operator != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){ // create pre operator node ...

						if(post_operator!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR)
							(*ast_node_to_be_evaluated)->pre_post_operator_info = post_operator; // preNodePunctuator(post_operator,*ast_node_to_be_evaluated);

						if(pre_operator!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
							if(post_operator!=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
								if(     (pre_operator == PRE_INC_PUNCTUATOR  || pre_operator  == PRE_DEC_PUNCTUATOR) &&
										(post_operator== POST_INC_PUNCTUATOR || post_operator == POST_DEC_PUNCTUATOR)){
									zs_print_error_cr("object \"%s\" has left \"%s\" and right \"%s\" is ambiguous",(*ast_node_to_be_evaluated)->symbol_value.c_str(),defined_operator_punctuator[pre_operator].str, defined_operator_punctuator[post_operator].str);
									return NULL;
								}

							}

							(*ast_node_to_be_evaluated)->pre_post_operator_info = pre_operator; //preNodePunctuator(pre_operator,*ast_node_to_be_evaluated);
						}
					}
				}
			}
			else{

				if(end_expression!= NULL){
				// there's a Punctuator, so let's perform generate its AST
					// reset prePunctuator...
					pre_operator=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
					print_ast_cr("try to generate group1 expression: %.20s ...\n",s_effective_start);
					return parseExpression_Recursive(
							s,
							start_line, // start line because was reparsed before (i.e group -1)
							scope_info,
							ast_node_to_be_evaluated,
							(GROUP_TYPE)(((int)type_group)+1),parent);
				}
			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...

			PASTNode left_node = NULL;
			PASTNode right_node = NULL;

			if(ast_node_to_be_evaluated != NULL){
				if((*ast_node_to_be_evaluated=CASTNode::newASTNode())==NULL) return NULL; // always preallocate 2 nodes (left and right)
				(*ast_node_to_be_evaluated)->idxAstParent = ZS_UNDEFINED_IDX;
				if(parent!=NULL){
					(*ast_node_to_be_evaluated)->idxAstParent=parent->idxAstNode;
				}
				(*ast_node_to_be_evaluated)->node_type = EXPRESSION_NODE;
			}
			char * expr_op_end = expr_start_op;



			expr_op_end+=strlen(defined_operator_punctuator[operator_group].str);

			//if(special_pre_post_cond){
			//expr_op_end++;
			//}
			//if(!(operator_group==ADD_PUNCTUATOR && *expr_start_op=='-')) // special case (preserve the sign as preneg)...
		//		expr_op_end+=strlen(defined_operator_punctuator[operator_group].str);

			print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",CASTNode::defined_operator_punctuator[operator_group].str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};

			// LEFT BRANCH
			strncpy(eval_left,s_effective_start,expr_start_op-s_effective_start); // copy its left side...
			if(parseExpression_Recursive(eval_left,
										start_line, // start line because was reparsed before...
										 scope_info,
										 ast_node_to_be_evaluated != NULL ? &left_node: NULL,
										 type_group,
										 ast_node_to_be_evaluated != NULL? *ast_node_to_be_evaluated : NULL)==NULL){
				return NULL;
			}

			if(ast_node_to_be_evaluated != NULL){
				if(left_node != NULL){
					(*ast_node_to_be_evaluated)->children.push_back(left_node->idxAstNode);
				}
			}

			if(	ast_node_to_be_evaluated != NULL ){
				if(left_node->pre_post_operator_info == PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){ // may subnode found post operator...
					left_node->pre_post_operator_info = pre_operator;
				}
			}

			// RIGHT BRANCH
			if((aux=parseExpression_Recursive(
									expr_op_end,
									m_line,
									scope_info,
									ast_node_to_be_evaluated != NULL ? &right_node : NULL,
									type_group,
									ast_node_to_be_evaluated != NULL ? (*ast_node_to_be_evaluated) : NULL)) == NULL){
				return NULL;
			}

			if(ast_node_to_be_evaluated != NULL){
				if(right_node != NULL){
					(*ast_node_to_be_evaluated)->children.push_back(right_node->idxAstNode);
				}
			}

			if(ast_node_to_be_evaluated != NULL){
				// minus operators has special management because two negatives can be + but sums of negatives works
				if(operator_group == SUB_PUNCTUATOR) { // supose a-b
					// 1. change - by +
					operator_group=ADD_PUNCTUATOR;
					CASTNode *rn =AST_NODE((*ast_node_to_be_evaluated)->children[RIGHT_NODE]);

					// 2. create neg node.
					PASTNode ast_neg_node=NULL;
					if((ast_neg_node = CASTNode::newASTNode())==NULL) return NULL;
					ast_neg_node->node_type = NODE_TYPE::PUNCTUATOR_NODE;
					ast_neg_node->operator_info = SUB_PUNCTUATOR;

					// 3. insert node between rigth node and ast_node

					if(rn->node_type == NODE_TYPE::SYMBOL_NODE || rn->is_packed_node){ // end symbol node... let's take the right one...
						ast_neg_node->idxAstParent = (*ast_node_to_be_evaluated)->idxAstNode;
						ast_neg_node->children.push_back((*ast_node_to_be_evaluated)->children[RIGHT_NODE]);
						(*ast_node_to_be_evaluated)->children[RIGHT_NODE]=ast_neg_node->idxAstNode;
					}else{ // let's take the right-left node ...
						CASTNode *rn_ln = AST_NODE(rn->children[LEFT_NODE]);
						ast_neg_node->idxAstParent = rn->idxAstNode;
						ast_neg_node->children.push_back(rn_ln->idxAstNode);
						rn_ln->idxAstParent = ast_neg_node->idxAstNode;
						rn->children[LEFT_NODE]=ast_neg_node->idxAstNode;
						//(*ast_node_to_be_evaluated)->children[LEFT_NODE]=ast_neg_node->idxAstNode;
					}
				}

				(*ast_node_to_be_evaluated)->node_type = PUNCTUATOR_NODE;
				(*ast_node_to_be_evaluated)->operator_info = operator_group;

				(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
				if(scope_info != NULL){
					(*ast_node_to_be_evaluated)->idxScope = scope_info->idxScope;
				}
				(*ast_node_to_be_evaluated)->line_value = m_lineOperator;
			}
		}
		return aux;
	}

	char * CASTNode::parseExpression(const char *s, int & m_line, CScope *scope_info, PASTNode * ast_node_to_be_evaluated ){

		// PRE: s is current string to parse. This function tries to parse an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.
		if(*s==0) {
			zs_print_error_cr("End string");
			return NULL;
		}

		// last character is in charge of who is calling parseExpression because there's many ending cases ): [ ';' ',' ')' , ']' ]
		char *aux_p = parseExpression_Recursive(s,m_line,scope_info,ast_node_to_be_evaluated);
		//char *aux = parseExpression_Recursive(s, m_line, scope_info, ast_node_to_be_evaluated, NULL);

		if(aux_p != NULL && ast_node_to_be_evaluated != NULL && *ast_node_to_be_evaluated!=NULL){ // can save the node and tells that is an starting of expression node...

			PASTNode ast_node= NULL;
			if((ast_node=CASTNode::newASTNode())==NULL) return NULL;
			ast_node->node_type = EXPRESSION_NODE;
			ast_node->children.push_back((*ast_node_to_be_evaluated)->idxAstNode);
			(*ast_node_to_be_evaluated)->idxAstParent = ast_node->idxAstNode; // save parent ..
			*ast_node_to_be_evaluated=ast_node;
		}
		return aux_p;
	}

	//---------------------------------------------------------------------------------------------------------------
	// PARSE OBJECT FUNCTIONS

	char * CASTNode::parseStruct_Recursive(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		string symbol_value;
		char *end_p;
		PASTNode attr_node = NULL;
		int m_lineSymbol;

		if(*aux_p == '{'){ // go for final ...

			if(ast_node_to_be_evaluated!=NULL){
				if(((*ast_node_to_be_evaluated)=CASTNode::newASTNode())==NULL) return NULL;
				(*ast_node_to_be_evaluated)->line_value = m_line;
				(*ast_node_to_be_evaluated)->node_type = STRUCT_NODE;
				(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
				if(scope_info != NULL){
					(*ast_node_to_be_evaluated)->idxScope = scope_info->idxScope;
				}
			}

			while (*aux_p != '}' && *aux_p != 0){

				m_lineSymbol = m_line;
				aux_p=IGNORE_BLANKS(aux_p+1,m_line);

				// expect word...
				end_p = getEndWord(aux_p, m_line);

				 if(end_p == NULL || end_p == aux_p){
					 zs_print_error_cr("Expected symbol at line %i after ','",m_lineSymbol);
					 return NULL;
				 }


				 symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_p);
				 aux_p=IGNORE_BLANKS(end_p,m_line);

				 if(*aux_p != ':'){ // expected : ...
					 zs_print_error_cr("Expected ':' at line %i",m_line);
					 return NULL;
				 }

				 aux_p++;

				 // go to variable...
				 if((aux_p=parseExpression(aux_p,m_line,scope_info,ast_node_to_be_evaluated!=NULL?&attr_node:NULL)) == NULL){  //IGNORE_BLANKS(aux_p+1,m_line);
					 return NULL;
				 }

				 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...
				 if(ast_node_to_be_evaluated!=NULL){
					 attr_node->symbol_value = symbol_value;
					 attr_node->line_value = m_lineSymbol;
					(*ast_node_to_be_evaluated)->children.push_back(attr_node->idxAstNode);
					attr_node->idxAstParent =(*ast_node_to_be_evaluated)->idxAstNode;
				 }

				 aux_p=IGNORE_BLANKS(aux_p,m_line);

				 if(*aux_p != ',' && *aux_p != '}' ){
					 zs_print_error_cr("expected '}' or ','");
					 return NULL;
				 }
			}
		}
		else{
			zs_print_error_cr("Expected '{'");
		}
		return aux_p;
	}

	char * CASTNode::parseStruct(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *end=parseStruct_Recursive(s,m_line,  scope_info, ast_node_to_be_evaluated);

		if(end == NULL){
			return NULL;
		}

		if(*end != '}'){
			zs_print_error_cr("Expected '{'");
			return NULL;
		}

		end=IGNORE_BLANKS(end+1,m_line);

		return (end);
	}
	//---------------------------------------------------------------------------------------------------------------
	// PARSE KEYWORDS

	char * CASTNode::parseNew(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
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
					 zs_print_error_cr("Expected symbol at line %i",m_line);
					 return NULL;
				 }
				 symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_p);
				 aux_p=IGNORE_BLANKS(end_p,m_line);

				 if(*aux_p != '('){
					 zs_print_error_cr("Expected '(' after \'%s\'",defined_keyword[key_w].str);
					 return NULL;
				 }

				 aux_p = parseArgs('(', ')',aux_p,m_line,scope_info,&args_node);
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

	char * CASTNode::parseDelete(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;
		string symbol_value;
		KEYWORD_TYPE key_w;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::DELETE_KEYWORD){

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),m_line);

				end_p = getEndWord(aux_p, m_line);

				 if(end_p == NULL || end_p == aux_p){
					 zs_print_error_cr("Expected symbol at line %i",m_line);
					 return NULL;
				 }

				 symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_p);

				if(((*ast_node_to_be_evaluated) = CASTNode::newASTNode()) == NULL) return NULL;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
				(*ast_node_to_be_evaluated)->symbol_value = symbol_value;

				return aux_p;
			}
		}
		return NULL;
	}

	char * CASTNode::parseClass(const char *s,int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
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
					zs_print_error_cr("line %i:class keyword is not allowed",m_line);
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),m_line);

				// check for symbol's name
				end_p = getEndWord(aux_p, m_line);
				if(end_p == NULL || end_p == aux_p){
					zs_print_error_cr("Expected name class at line %i ",m_line);
					return NULL;
				}
				// try to register class...
				class_line = m_line;
				class_name = CStringUtils::copyStringFromInterval(aux_p, end_p);

				zs_print_debug_cr("registered class \"%s\" line %i ",class_name.c_str(), class_line);

				aux_p=IGNORE_BLANKS(end_p,m_line);

				if(*aux_p == ':' ){
					string ext_name;
					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					end_p = getEndWord(aux_p, m_line);

					if(end_p == NULL || end_p == aux_p){
						zs_print_error_cr("Expected class name at line %i ",m_line);
						return NULL;
					}

					ext_name=CStringUtils::copyStringFromInterval(aux_p, end_p);

					/*if(!CScriptClass::isClassRegistered(ext_name)){
						zs_print_error_cr("extended class \"%s\" not exist");
						return NULL;
					}*/

					if(ast_node_to_be_evaluated != NULL){
						if((base_class_node = CASTNode::newASTNode()) == NULL) return NULL;
						base_class_node->symbol_value = CStringUtils::copyStringFromInterval(aux_p, end_p);
						base_class_node->node_type = BASE_CLASS_NODE;
					}

					aux_p=IGNORE_BLANKS(end_p, m_line);
				}
				if(*aux_p == '{' ){

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					// it seem's we have a good built class...
					//if(ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated = CASTNode::newASTNode())== NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
					(*ast_node_to_be_evaluated)->symbol_value = class_name;

					CScope *scp = CScope::newScope(ZS_UNDEFINED_IDX);
					(*ast_node_to_be_evaluated)->idxScope =scp->idxScope;
					class_scope_info =scp;

					// create var & functions collection...
					if((vars_collection_node = CASTNode::newASTNode())==NULL) return NULL;
					if((function_collection_node = CASTNode::newASTNode())==NULL) return NULL;
					(*ast_node_to_be_evaluated)->children.push_back(vars_collection_node->idxAstNode);
					(*ast_node_to_be_evaluated)->children.push_back(function_collection_node->idxAstNode);

					if(base_class_node != NULL) {
						(*ast_node_to_be_evaluated)->children.push_back(	base_class_node ->idxAstNode);
					}
					// register info class ...
					// check for named functions or vars...
					while(*aux_p != '}' && *aux_p != 0){

						// 1st. check whether parse a keyword...
						key_w = isKeyword(aux_p);
						if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
							switch(key_w){
							default:
								zs_print_error_cr("Expected \"var\" or \"function\" keyword at line %i", m_line);
								return NULL;
								break;
							case KEYWORD_TYPE::FUNCTION_KEYWORD:
								if((aux_p = parseFunction(aux_p, m_line,class_scope_info, &child_node)) != NULL){
									if(child_node->symbol_value != ""){
										if(ast_node_to_be_evaluated != NULL){
											function_collection_node->children.push_back(child_node->idxAstNode);
										}
									}
									else {
										zs_print_error_cr("Expected symbol after function at line %i",m_line);
										return NULL;
									}
								} else{
									return NULL;
								}
								break;
							case KEYWORD_TYPE::VAR_KEYWORD:
								if((aux_p = parseMemberVar(aux_p, m_line,class_scope_info, &child_node)) != NULL){

									if(ast_node_to_be_evaluated != NULL){
										vars_collection_node->children.push_back(child_node->idxAstNode);
									}

								} else{
									return NULL;
								}
								break;
							}
						}else{
							zs_print_error_cr("Expected \"var\" or \"function\" keyword at line %i",m_line);
							return NULL;
						}
						aux_p=IGNORE_BLANKS(aux_p,m_line);
					}

					if(*aux_p != '}'){
						zs_print_error_cr("class \"%s\" declared line %i is not closed ",class_name.c_str(),class_line);
					}

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					if(*aux_p != ';'){
						zs_print_error_cr("class \"%s\" declared line %i not end with ;",class_name.c_str(),class_line);
					}

					return aux_p+1;

				}else{
					zs_print_error_cr("Expected '{' at line %i ",m_line);
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CASTNode::parseArgs(char c1,char c2,const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		PASTNode   node_arg_expression=NULL;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		if(*aux_p == c1){
			aux_p++;
			aux_p=IGNORE_BLANKS(aux_p,m_line);

			if(ast_node_to_be_evaluated!=NULL){
				if(((*ast_node_to_be_evaluated) = CASTNode::newASTNode()) == NULL) return NULL;
			}

			if(*aux_p != c2 ){

				do{
					if((aux_p = parseExpression(aux_p,m_line,scope_info,(ast_node_to_be_evaluated != NULL ? &node_arg_expression : NULL)))==NULL){
						return NULL;
					}

					// push arg into node...
					if(ast_node_to_be_evaluated!=NULL){
						(*ast_node_to_be_evaluated)->children.push_back(node_arg_expression->idxAstNode);
					}

					if(*aux_p == ',' ){
						aux_p++;
						aux_p=IGNORE_BLANKS(aux_p,m_line);
					}else{
						if(*aux_p != c2 ){
							zs_print_error_cr("Expected %c at line %i",c2,m_line);
							return NULL;
						}
					}

				}while(*aux_p != c2 && *aux_p != 0);
			}

			return aux_p+1;
		}
		return NULL;
	}

	PASTNode  CASTNode::findAst(const string & _symbol_name_to_find, NODE_TYPE _node_type, KEYWORD_TYPE _keyword_type){
		return findAstRecursive(_symbol_name_to_find, _node_type, _keyword_type, MAIN_AST_NODE);
	}

	PASTNode  CASTNode::findAstRecursive(const string & _symbol_name_to_find, NODE_TYPE _node_type,KEYWORD_TYPE _keyword_type, PASTNode _node){
		if(_node->symbol_value == _symbol_name_to_find && _node->node_type == _node_type){
			if(_node_type==NODE_TYPE::KEYWORD_NODE){
				if(_keyword_type== _node->keyword_info){
					return _node;
				}
			}
			else{
				return _node;
			}
		}

		for(unsigned i = 0; i < _node->children.size(); i++){
			PASTNode ch;
			if((ch=findAstRecursive(_symbol_name_to_find,_node_type,_keyword_type,AST_NODE(_node->children[i])))!=NULL){
				return ch;
			}
		}
		return NULL;
	}

	char * CASTNode::parseFunction(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *symbol_value,*end_var;
		KEYWORD_TYPE key_w;

		PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
		string conditional_str;
		bool error=false;
		CScope *_currentScope=NULL;
		tScopeVar * irv=NULL;
		string str_name;
		string class_member,class_name, function_name="";
		PASTNode class_node=NULL;
		int idxScope=ZS_UNDEFINED_IDX;

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

				if(ast_node_to_be_evaluated!=NULL){ // we save node...

					if((*ast_node_to_be_evaluated = CASTNode::newASTNode())==NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;

					if((args_node = CASTNode::newASTNode()) == NULL) return NULL;
					(*ast_node_to_be_evaluated)->children.push_back(args_node->idxAstNode);

					args_node->node_type = NODE_TYPE::ARGS_DECL_NODE;

					// start line ...
					(*ast_node_to_be_evaluated)->line_value = m_line;
				}

				bool named_function = *aux_p!='(';

				if(named_function){ // is named function..

					if((end_var=isClassMember(aux_p,m_line,class_name,class_member,class_node, error,key_w))!=NULL){ // check if particular case extension attribute class
						idxScope = class_node->idxScope; // override scope info
						symbol_value = (char *)class_member.c_str();
					}
					else{
						if(error){
							return NULL;
						}
						else{ // get normal name...

							// check whwther the function is anonymous with a previous arithmetic operation ....
							end_var=getSymbolName(aux_p,m_line);

							if(end_var != NULL){

								if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
									return NULL;

								function_name = symbol_value;

								// check whether parameter name's matches with some global variable...
								/*if(scope_info != NULL){
									if((irv=scope_info->getCurrentScopePointer()->getInfoRegisteredSymbol(symbol_value,false)) != NULL){

										if(irv->idxAstNode!=ZS_UNDEFINED_IDX){
											zs_print_error_cr("Function name \"%s\" defined at line %i is already defined at %i", symbol_value, m_line,AST_LINE_VALUE(irv->idxAstNode));
										}else{
											zs_print_error_cr("Function name \"%s\" at line %i is no allowed it has conflict with name of already registered function in C/C++", symbol_value, m_line);
										}

										return NULL;
									}
								}*/
							}else{
								return NULL;
							}
						}
					}
					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,m_line);
				}
				else{ //function node
					if(ast_node_to_be_evaluated!=NULL){ // save as function object...
						(*ast_node_to_be_evaluated)->node_type = FUNCTION_OBJECT_NODE;
						//(*ast_node_to_be_evaluated)->scope_info_ptr = scope_info;
					}
				}

				if(ast_node_to_be_evaluated!=NULL){
					PASTNode ast_node = NULL;
					// create object function ...
					if(named_function){

						/*if((irv=scope_info->registerSymbol(symbol_value,(*ast_node_to_be_evaluated)))==NULL){
							return NULL;
						}

						if((*ast_node_to_be_evaluated) != NULL){
							(*ast_node_to_be_evaluated)->symbol_value=symbol_value;
						}*/

						ast_node=*ast_node_to_be_evaluated;
					}
					else{
						irv=scope_info->registerAnonymouseFunction((*ast_node_to_be_evaluated));
						ast_node=AST_NODE(irv->idxAstNode);
						ast_node->symbol_value = irv->name;

					}

					// define value symbol...
					/*if(irv == NULL){
						return NULL;
					}*/
					ast_node->idxScope = idxScope;
				}
				// parse function args...
				if(*aux_p == '('){ // push arguments...

					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,m_line);

					if(ast_node_to_be_evaluated != NULL){

						_currentScope=scope_info->pushScope();
					}

					// grab words separated by ,
					while(*aux_p != 0 && *aux_p != ')'){

						aux_p=IGNORE_BLANKS(aux_p,m_line);

						if(*aux_p == ')' || *aux_p == ','){
							zs_print_error_cr("Expected arg at line %i ",m_line);
							return NULL;
						}

						end_var=getSymbolName(aux_p,m_line);

						if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
							return NULL;

						// check if repeats...
						if(ast_node_to_be_evaluated != NULL){
							for(unsigned k = 0; k < args_node->children.size(); k++){
								if(AST_NODE(args_node->children[k])->symbol_value == symbol_value){
									zs_print_error_cr("Repeated argument '%s' argument at line %i ",symbol_value,m_line);
									return NULL;
								}
							}

							// check whether parameter name's matches with some global variable...
							if((irv=_currentScope->getInfoRegisteredSymbol(symbol_value,-1,false)) != NULL){
								zs_print_error_cr("Ambiguous symbol argument \"%s\" at line %i name with var defined at %i", symbol_value, m_line, AST_LINE_VALUE(irv->idxAstNode));
								return NULL;
							}

							// ok register arg symbol...

							if((_currentScope->registerSymbol(symbol_value,args_node)) == NULL){
								return NULL;
							}
							// ok register symbol into the object function ...
							//object_function.m_arg.push_back(symbol_value);
						}
						aux_p=end_var;
						aux_p=IGNORE_BLANKS(aux_p,m_line);

						if(*aux_p != ')'){

							if(*aux_p != ','){
								zs_print_error_cr("Expected ',' at line ",m_line);
								return NULL;
							}
							aux_p++;
						}
						// PUSH NEW ARG...
						if(ast_node_to_be_evaluated != NULL){
							if((arg_node = CASTNode::newASTNode()) == NULL) return NULL;
							arg_node->symbol_value=symbol_value;
							args_node->children.push_back(arg_node->idxAstNode);
						}
					}

					if(*aux_p != ')'){
						zs_print_error_cr("Expected ')' at line ",m_line);
						return NULL;
					}

					// register function symbol...


					if(ast_node_to_be_evaluated !=NULL){

						int n_params=0;

						if(args_node != NULL){
							n_params=args_node->children.size();
						}

						if(function_name!=""){
							if((irv=scope_info->getCurrentScopePointer()->getInfoRegisteredSymbol(function_name,n_params,false)) != NULL){

								if(irv->idxAstNode!=ZS_UNDEFINED_IDX){
									zs_print_error_cr("Line %i: Function name \"%s\" is already defined with same number of args at line %i", m_line, function_name.c_str(),AST_LINE_VALUE(irv->idxAstNode));
								}else{
									zs_print_error_cr("Function name \"%s\" at line %i is no allowed it has conflict with name of already registered function in C/C++", function_name.c_str(), m_line);
								}

								return NULL;
							}

							if((irv=scope_info->registerSymbol(function_name,(*ast_node_to_be_evaluated),n_params))==NULL){
								return NULL;
							}

							if((*ast_node_to_be_evaluated) != NULL){
								(*ast_node_to_be_evaluated)->symbol_value=function_name;
							}

						}

					}

					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,m_line);

					if(*aux_p != '{'){
						zs_print_error_cr("Expected '{' at line ",m_line);
						return NULL;
					}

					// ok let's go to body..
					if((aux_p = parseBlock(
							aux_p,
							m_line,
							ast_node_to_be_evaluated != NULL ? scope_info:NULL ,
							error,
							ast_node_to_be_evaluated != NULL ? &body_node : NULL

						)) != NULL){

						if(!error){

							if(ast_node_to_be_evaluated != NULL){
								(*ast_node_to_be_evaluated)->children.push_back(body_node->idxAstNode);
								scope_info->popScope();
							}

							return aux_p;
						}
					}
				}
				else{
					zs_print_error_cr("Unclosed function defined at line %i",m_line);
				}
			}else{
				zs_print_error_cr("Expected operator or function operator at line %i",m_line);
			}
		}
		return NULL;
	}

	char *  CASTNode::parseReturn(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
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

				if(ast_node_to_be_evaluated != NULL){ // save
					if((*ast_node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL; // reserve for expression return
					(*ast_node_to_be_evaluated)->node_type = NODE_TYPE::KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
					(*ast_node_to_be_evaluated)->line_value = m_line;
				}

				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if((aux_p = parseExpression(aux_p, m_line, scope_info, ast_node_to_be_evaluated != NULL ? &child_node : NULL))!= NULL){

					if(ast_node_to_be_evaluated != NULL){
						if(child_node == NULL){
							zs_print_error_cr("parse_return: child_node null");
							return NULL;
						}

						(*ast_node_to_be_evaluated)->children.push_back(child_node->idxAstNode);
					}

					if(*aux_p!=';'){
						zs_print_error_cr("Expected ';' at line %i", m_line);
						return NULL;
					}
					aux_p++;
					return aux_p;
				}
			}
		}
		return NULL;
	}

	char * CASTNode::parseWhile(const char *s,int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;

		PASTNode conditional_expression=NULL, while_node=NULL;
		string conditional_str;
		bool error = false;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::WHILE_KEYWORD){

				if(ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
				}

				aux_p += strlen(defined_keyword[key_w].str);

				// evaluate conditional line ...
				aux_p=IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p == '('){

					if((end_expr = parseExpression(aux_p+1,m_line,scope_info,&conditional_expression)) != NULL){

						if(*end_expr != ')'){
							zs_print_error_cr("Expected ')' at line %i",m_line);
							return NULL;
						}
						if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
							return NULL;
						}

						if( ast_node_to_be_evaluated != NULL){
							PASTNode aux;
							if((aux= CASTNode::newASTNode()) == NULL) return NULL;
							aux->node_type = CONDITIONAL_NODE;
							aux->children.push_back(conditional_expression->idxAstNode);
							(*ast_node_to_be_evaluated)->children.push_back(aux->idxAstNode);
						}
						aux_p=IGNORE_BLANKS(end_expr+1,m_line);
						if(*aux_p != '{'){
							zs_print_error_cr("Expected while-block open block ('{') %i",m_line);
							return NULL;
						}
						if((aux_p=parseBlock(aux_p,m_line,scope_info,error,ast_node_to_be_evaluated != NULL ? &while_node : NULL))!= NULL){
							if(!error){
								if(ast_node_to_be_evaluated != NULL){
									(*ast_node_to_be_evaluated)->children.push_back(while_node->idxAstNode);
								}


								return aux_p;
							}
						}
					}else{
						zs_print_error_cr("Expected ')' while %i",m_line);
						return NULL;
					}

				}else{
					zs_print_error_cr("Expected '(' while %i",m_line);
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CASTNode::parseIf(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		PASTNode conditional=NULL, if_node=NULL, else_node=NULL;
		string conditional_str;
		bool error = false;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::IF_KEYWORD){

				if(ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
				}
				aux_p += strlen(defined_keyword[key_w].str);

				// evaluate conditional line ...
				aux_p=IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p == '('){
					if((end_expr = parseExpression(aux_p+1,m_line,scope_info,ast_node_to_be_evaluated != NULL? &conditional: NULL)) != NULL){

						if(*end_expr != ')'){
							zs_print_error_cr("Expected ')' at line %i",m_line);
							return NULL;
						}

						if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
							return NULL;
						}

						conditional_str=start_symbol;

						if(ast_node_to_be_evaluated!=NULL){
							PASTNode aux;
							if((aux = CASTNode::newASTNode()) == NULL) return NULL;
							aux->children.push_back(conditional->idxAstNode);
							aux->node_type = CONDITIONAL_NODE;
							(*ast_node_to_be_evaluated)->children.push_back(aux->idxAstNode);
						}

						aux_p=IGNORE_BLANKS(end_expr+1,m_line);
						if(*aux_p != '{'){
							if(!printErrorUnexpectedKeywordOrPunctuator(aux_p, m_line)){
								zs_print_error_cr("Expected if-block open block ('{') %i",m_line);
							}
							return NULL;
						}

						if((aux_p=parseBlock(aux_p,m_line,scope_info,error,ast_node_to_be_evaluated != NULL ? &if_node : NULL))!= NULL){
							if(!error){
								if(ast_node_to_be_evaluated != NULL){
									(*ast_node_to_be_evaluated)->children.push_back(if_node->idxAstNode);
								}

								aux_p=IGNORE_BLANKS(aux_p,m_line);

								bool else_key = false;
								if((key_w = isKeyword(aux_p)) != KEYWORD_TYPE::UNKNOWN_KEYWORD){
									else_key = (key_w == KEYWORD_TYPE::ELSE_KEYWORD);
								}

								if(else_key){
									aux_p += strlen(defined_keyword[key_w].str);

									aux_p=IGNORE_BLANKS(aux_p,m_line);
									if(*aux_p != '{'){
										zs_print_error_cr("Expected else-block open block ('{') %i",m_line);
										return NULL;
									}

									if((aux_p=parseBlock(aux_p,m_line,scope_info,error,ast_node_to_be_evaluated != NULL ? &else_node : NULL))!= NULL){
											if(!error){

												if( ast_node_to_be_evaluated != NULL){
													(*ast_node_to_be_evaluated)->children.push_back(else_node->idxAstNode);
												}

												return aux_p;
											}
											else{

											}
									}
								}else{
									return aux_p;
								}
							}
						}
					}else{
						zs_print_error_cr("Expected ')' if %i",m_line);
						return NULL;
					}

				}else{
					zs_print_error_cr("Expected '(' if %i",m_line);
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CASTNode::parseFor(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		bool error=false;
		PASTNode block_for = NULL,node_for_expression=NULL;
		string eval_for;
		//CScope *_localScope =  scope_info != NULL?scope_info->symbol_info.ast->scope_info_ptr:NULL; // gets scope...
		CScope *_currentScope=NULL;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::FOR_KEYWORD){

				if(ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL;; // ini, conditional, post
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;

					// reserve 3 nodes for init/eval/post_op
					(*ast_node_to_be_evaluated)->children.push_back(ZS_UNDEFINED_IDX);
					(*ast_node_to_be_evaluated)->children.push_back(ZS_UNDEFINED_IDX);
					(*ast_node_to_be_evaluated)->children.push_back(ZS_UNDEFINED_IDX);

				}

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p == '('){ // ready ...

					// save scope pointer ...
					if(ast_node_to_be_evaluated != NULL){
						_currentScope =scope_info->pushScope(); // push current scope
						(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
						if(_currentScope!=NULL){
							(*ast_node_to_be_evaluated)->idxScope=_currentScope->idxScope;
						}
					}

					aux_p=IGNORE_BLANKS(aux_p+1,m_line);

					struct{
						char next_char;
						NODE_TYPE node_type;
					}info_for[3]={
							{';',PRE_FOR_NODE},
							{';',CONDITIONAL_NODE},
							{')',POST_FOR_NODE}
					};

					for(int i = 0; i < 3; i++){
						bool parse_var = false;

						if(i==0){
							KEYWORD_TYPE key_w = isKeyword(aux_p);
							if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
								if(key_w == VAR_KEYWORD){
									if((aux_p = parseVar(aux_p,m_line, _currentScope, ast_node_to_be_evaluated != NULL ? &node_for_expression: NULL))==NULL){
										return NULL;
									}
									else{
										parse_var = true;
										aux_p = aux_p - 1; // particular case to get end op...
									}
								}
							}
						}

						if(!parse_var){
							if((aux_p = parseExpression((const char *)aux_p,m_line,_currentScope, ast_node_to_be_evaluated != NULL ? &node_for_expression: NULL)) == NULL){
								return NULL;
							}
						}

						if(*aux_p == info_for[i].next_char){
							if(ast_node_to_be_evaluated != NULL){ // add node as for node type and its expression below.
								PASTNode aux;
								if((aux= CASTNode::newASTNode()) == NULL) return NULL;
								aux->node_type = info_for[i].node_type;
								aux->children.push_back(node_for_expression->idxAstNode);
								(*ast_node_to_be_evaluated)->children[i]=aux->idxAstNode;
							}
						}
						else{
							zs_print_error_cr("Expected '%c' in for statment %i",info_for[i].next_char, m_line);
							return NULL;
						}
						node_for_expression=NULL;
						aux_p=IGNORE_BLANKS(aux_p+1,m_line);
					}

					aux_p=IGNORE_BLANKS(aux_p,m_line);
					if(*aux_p != '{'){
						zs_print_error_cr("Expected '{' for-block at line %i",m_line);
						return NULL;
					}

					// parse block ...
					if((aux_p=parseBlock(aux_p,m_line,_currentScope,error,ast_node_to_be_evaluated != NULL ? &block_for : NULL,true))!= NULL){ // true: We treat declared variables into for as another scope.
						if(!error){

							if(ast_node_to_be_evaluated != NULL) {
								(*ast_node_to_be_evaluated)->children.push_back(block_for->idxAstNode);
								scope_info->popScope(); // push current scope
							}
							return aux_p;
						}
						else{

						}
					}
				}else{
					zs_print_error_cr("Expected '(' for %i",m_line);
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CASTNode::parseSwitch(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_symbol,*start_symbol;
		PASTNode switch_node=NULL,
					 group_cases=NULL,
					 case_value_node=NULL,
					 default_switch_node=NULL;

		PASTNode case_body_node=NULL;

		CScope *m_currentScope;
		PUNCTUATOR_TYPE ip;
		char *value_to_eval;
		string val;
		KEYWORD_TYPE key_w,key_w2;

		bool error=false;
		int n_cases;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::SWITCH_KEYWORD){

				if( ast_node_to_be_evaluated != NULL){
					if((*ast_node_to_be_evaluated = CASTNode::newASTNode())==NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
				}

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p == '('){
					//if((end_expr = ADVANCE_TO_CHAR(aux_p,')',m_line)) != NULL){
						aux_p=IGNORE_BLANKS(aux_p+1,m_line);

						// evaluate switch vale expression ...
						PASTNode base_expression_to_evaluate = NULL;
						//static char * parseExpression_Recursive(const char *s, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated=NULL,GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL);
						if((aux_p = CASTNode::parseExpression(
								aux_p,
								m_line,
								scope_info,
								ast_node_to_be_evaluated==NULL?NULL:&base_expression_to_evaluate))==NULL)
						{// getEndWord(aux_p, m_line);
							return NULL;
						}

						if(ast_node_to_be_evaluated != NULL){
							base_expression_to_evaluate->idxAstParent = (*ast_node_to_be_evaluated)->idxAstNode;
							(*ast_node_to_be_evaluated)->children.push_back(base_expression_to_evaluate->idxAstNode);
						}

						if(*aux_p != ')'){
							zs_print_error_cr("Expected ')' switch %i",m_line);
							error = true;
							return NULL;
						}

						aux_p=IGNORE_BLANKS(aux_p+1,m_line);

						if(*aux_p == '{'){

							// ok try to get cases and default nodes ...
							aux_p++;
							aux_p = IGNORE_BLANKS(aux_p,m_line);


							// foreach case evaluate its body,
							while(*aux_p!='}' && *aux_p!=0){
									n_cases = 0;
									bool theres_a_default= false;
									// init node ..
									if(ast_node_to_be_evaluated!= NULL){
										if((switch_node = CASTNode::newASTNode()) == NULL) return NULL;
										if((group_cases = CASTNode::newASTNode()) == NULL) return NULL;

										(*ast_node_to_be_evaluated)->children.push_back(switch_node->idxAstNode);
										switch_node->children.push_back(group_cases->idxAstNode); // multiple conditions that performs the same body => switch_node->children[0]
										switch_node->children.push_back(ZS_UNDEFINED_IDX); // the body => switch_node->children[1]
										group_cases->node_type = GROUP_CASES_NODE;
									}

									bool end = false;

									while((key_w = isKeyword(aux_p)) != KEYWORD_TYPE::UNKNOWN_KEYWORD && !end){ // acumulative cases /defaults...

										switch(key_w){
										case DEFAULT_KEYWORD:
										case CASE_KEYWORD:

											val = "default";
											aux_p += strlen(defined_keyword[key_w].str);
											aux_p=IGNORE_BLANKS(aux_p,m_line);

											if(key_w == CASE_KEYWORD){
												// get the symbol...
												start_symbol=aux_p;

												if((ip = isPunctuator(aux_p)) != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
													if(ip == PUNCTUATOR_TYPE::ADD_PUNCTUATOR ||ip == PUNCTUATOR_TYPE::SUB_PUNCTUATOR){
														aux_p+=strlen(defined_operator_punctuator[ip].str);
													}
													else{
														zs_print_error_cr("unexpected token %s at line",defined_operator_punctuator[ip].str,m_line);
														error = true;
														return NULL;
													}
												}
												end_symbol = getEndWord(aux_p, m_line);
												aux_p=end_symbol;

												value_to_eval = CStringUtils::copyStringFromInterval(start_symbol, end_symbol);

												if(value_to_eval==NULL){ return NULL;}

												val = value_to_eval;

												aux_p=IGNORE_BLANKS(aux_p,m_line);
											}else{
												theres_a_default = true;
											}

											if(*aux_p == ':'){

												if(ast_node_to_be_evaluated != NULL){
													// check if repeated (starts from 1 because the first is the switch expression)...
													for(unsigned j=1; j <(*ast_node_to_be_evaluated)->children.size(); j++ ){ // switch nodes

														PASTNode child_access_j_0 = AST_NODE(AST_NODE((*ast_node_to_be_evaluated)->children[j])->children[0]);

														for(unsigned h=1; h <child_access_j_0->children.size(); h++ ){ // groups nodes
															if(AST_NODE(child_access_j_0->children[h])->symbol_value == val){
																zs_print_error_cr("Symbol %s repeteaded in switch at line %i",val.c_str(),m_line);
																return NULL;
															}
														}
													}

													if((case_value_node = CASTNode::newASTNode())==NULL) return NULL;
													case_value_node->node_type = KEYWORD_NODE;
													case_value_node->keyword_info = key_w;
													case_value_node->symbol_value = val;
													case_value_node->line_value=m_line;
													group_cases->children.push_back(case_value_node->idxAstNode);

													if(key_w==DEFAULT_KEYWORD){ // save switch node ...
														default_switch_node=switch_node;
													}
												}
												aux_p++;
												n_cases++;
											}
											else{
												zs_print_error_cr("Expected  : case at %i",m_line);
												return NULL;
											}
											break;

											aux_p += strlen(defined_keyword[key_w].str);
											aux_p=IGNORE_BLANKS(aux_p,m_line);
											if(*aux_p == ':'){
												n_cases++;
												aux_p++;
											}
											else{
												zs_print_error_cr("Expected  : default at %i",m_line);
												return NULL;
											}
											break;
										case BREAK_KEYWORD:
											end=true;
											break;
										default:
											zs_print_error_cr("Expected case or default in switch %i",m_line);
											return NULL;
											break;
										}
										aux_p=IGNORE_BLANKS(aux_p,m_line);
									}

									if(n_cases == 0){
										zs_print_error_cr("Expected case or default in switch %i",m_line);
										return NULL;
									}

									// print warning ignored cases in case there's a default in there...
									if(ast_node_to_be_evaluated!= NULL){
										if(group_cases->children.size() > 1 && theres_a_default){



											for(vector<short>::iterator it = group_cases->children.begin(); it != group_cases->children.end(); ){ //erase cases and print warning !
												if(AST_NODE(*it)->keyword_info == CASE_KEYWORD){
													zs_print_warning_cr("Ignored case %s defined at line %i because is joined by default keyword",AST_NODE(*it)->symbol_value.c_str(),AST_NODE(*it)->line_value);
													group_cases->children.erase(it);

												}else{
													it++;
												}
											}
										}
									}

									// save scope pointer ...
									if(ast_node_to_be_evaluated != NULL){
										m_currentScope = scope_info->pushScope();
										switch_node->idxScope =m_currentScope->idxScope;
									}

									// eval block...
									if((aux_p=generateAST_Recursive(aux_p, m_line, scope_info, error, ast_node_to_be_evaluated != NULL ? &case_body_node : NULL,true))==NULL){
										return NULL;
									}


									if(error){
										return NULL;
									}

									// assign the body ast...
									switch_node->children[1]=case_body_node->idxAstNode;


									key_w2 = isKeyword(aux_p);
									if(key_w2 == BREAK_KEYWORD){
										aux_p += strlen(defined_keyword[key_w2].str);
										IGNORE_BLANKS(aux_p,m_line);

										if(*aux_p == ';'){ // the new scope ...
											if(ast_node_to_be_evaluated != NULL){

												if(switch_node->children[1] != ZS_UNDEFINED_IDX){
													AST_NODE(switch_node->children[1])->node_type = BODY_NODE;
												}
											}
										}
										else{
											zs_print_error_cr("Expected break %i",m_line);
											return NULL;
										}

									}else{
										zs_print_error_cr("Expected break %i",m_line);
										return NULL;
									}

									if(scope_info != NULL){
										scope_info->popScope();
									}

									aux_p =IGNORE_BLANKS(aux_p+1,m_line);
								}

							if(*aux_p == '}'){

								// check default node to put to the end of the list ...
									if(default_switch_node!= NULL){
										bool end=false;
										// start iterator from begin + 1 because the first is the switch value expression itself
										for(vector<short>::iterator it = ((*ast_node_to_be_evaluated)->children.begin()+1); it != (*ast_node_to_be_evaluated)->children.end() && !end; ){ //erase cases and print warning !

											if(AST_NODE(AST_NODE(AST_NODE(*it)->children[0])->children[0])->keyword_info == DEFAULT_KEYWORD){ // group cases-case node access
												(*ast_node_to_be_evaluated)->children.erase(it);
												end=true;
											}else{
												it++;
											}
										}

										(*ast_node_to_be_evaluated)->children.push_back(default_switch_node->idxAstNode);
									}
								return aux_p + 1;
							}
							else{
								zs_print_error_cr("Expected '}' switch %i",m_line);
							}
						}
						else{
							zs_print_error_cr("Expected '{' switch %i",m_line);
						}
				}
				else{
					zs_print_error_cr("Expected '(' switch %i",m_line);
				}
			}
		}
		return NULL;
	}

	char * CASTNode::parseMemberVar(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

		char *aux_p = (char *)s;
		CScope *_currentScope = NULL;
		KEYWORD_TYPE key_w;
		char *start_var,*end_var, *symbol_name;
		string class_name, class_member;
		PASTNode class_node;
		bool error=false;
		string s_aux;
		bool extension_prop=false;

		aux_p=IGNORE_BLANKS(aux_p,m_line);
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				//
				if(ast_node_to_be_evaluated != NULL){
					_currentScope=scope_info->getCurrentScopePointer(); // gets current evaluating scope...
					if(((*ast_node_to_be_evaluated) = CASTNode::newASTNode()) == NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
				}

				if((end_var=isClassMember(aux_p,m_line,class_name,class_member,class_node, error,key_w))!=NULL){ // particular case extension attribute class
					if(ast_node_to_be_evaluated!=NULL){ // define as many vars is declared within ','

						PASTNode var_new=NULL;
						if((var_new=CASTNode::newASTNode()) == NULL) return NULL;
						// save symbol in the node ...
						var_new->symbol_value = class_member;
						var_new->idxScope = ZS_UNDEFINED_IDX;
						if(_currentScope != NULL){
							var_new->idxScope =_currentScope->idxScope;
						}
						var_new->line_value = m_line;

						if(!_currentScope->registerSymbol(var_new->symbol_value,var_new)){
							return NULL;
						}

						(*ast_node_to_be_evaluated)->children.push_back(var_new->idxAstNode);
					}

					aux_p=IGNORE_BLANKS(end_var,m_line);

					if(*aux_p == ';'){
						aux_p++;
					}
					else{
						zs_print_error_cr("line %i; expected ';' after extension class attribute", m_line,*aux_p);
						return NULL;
					}

					zs_print_debug_cr("registered symbol \"%s::%s\" line %i ",class_name.c_str(),class_member.c_str(), m_line);
					return aux_p;
				}
				else{
					if(error){
						return NULL;
					}
				}

				while(*aux_p != ';' && *aux_p != 0){ // JE: added multivar feature.
					start_var=aux_p;
					//m_startLine = m_line;

					if((end_var=getSymbolName(aux_p,m_line))==NULL){
						return NULL;
					}

					if((symbol_name=CStringUtils::copyStringFromInterval(start_var,end_var)) == NULL){
						return NULL;
					}

					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,m_line);

					if((*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
						zs_print_debug_cr("registered symbol \"%s\" line %i ",symbol_name, m_line);

						if(ast_node_to_be_evaluated!=NULL){ // define as many vars is declared within ','

							PASTNode var_new=NULL;
							if((var_new=CASTNode::newASTNode()) == NULL) return NULL;
							// save symbol in the node ...
							var_new->symbol_value = symbol_name;
							var_new->idxScope = ZS_UNDEFINED_IDX;
							if(_currentScope != NULL){
								var_new->idxScope = _currentScope->idxScope;
							}
							var_new->line_value = m_line;
							if(!_currentScope->registerSymbol(var_new->symbol_value,var_new)){
								return NULL;
							}
							(*ast_node_to_be_evaluated)->children.push_back(var_new->idxAstNode);
						}
					}
					else{
						zs_print_error_cr("line %i: unexpected '%c'", m_line,*aux_p);
						return NULL;
					}

					// ignores ';' or ','
					if(*aux_p == ',')
						aux_p++;
				}

				if(*aux_p == ';'){
					aux_p++;
				}
				else{
					zs_print_error_cr("Expected ';' at line %i", m_line);
					return NULL;
				}

				return aux_p;
			}
		}
		return NULL;
	}

	char * CASTNode::parseVar(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		CScope *_currentScope = NULL;
		PASTNode var_node;
		KEYWORD_TYPE key_w;
		char *start_var,*end_var, *symbol_name;
		int m_startLine=0;
		string s_aux;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...
				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,m_line);

				if(ast_node_to_be_evaluated != NULL){
					_currentScope=scope_info->getCurrentScopePointer(); // gets current evaluating scope...
					if(((*ast_node_to_be_evaluated) = CASTNode::newASTNode()) == NULL) return NULL;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;
					(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
					if(_currentScope != NULL){
						(*ast_node_to_be_evaluated)->idxScope = _currentScope->idxScope;
					}
				}
				while(*aux_p != ';' && *aux_p != 0 ){ // JE: added multivar feature.
					start_var=aux_p;
					m_startLine = m_line;

					if((end_var=getSymbolName(aux_p,m_line))==NULL){
						return NULL;
					}
					if((symbol_name=CStringUtils::copyStringFromInterval(start_var,end_var)) == NULL){
						return NULL;
					}

					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,m_line);
					if((*aux_p == ';' || *aux_p == '=' || *aux_p == ',' )){ // JE: added multivar feature (',)).
						zs_print_debug_cr("registered symbol \"%s\" line %i ",symbol_name, m_line);
						var_node = NULL;
						if(ast_node_to_be_evaluated!=NULL){

							if((var_node = CASTNode::newASTNode()) == NULL) return NULL;
							// save symbol in the node ...
							(var_node)->symbol_value = symbol_name;
							(var_node)->idxScope = ZS_UNDEFINED_IDX;

							if(_currentScope != NULL){
								(var_node)->idxScope = _currentScope->idxScope;
							}

							(var_node)->line_value = m_line;

							(*ast_node_to_be_evaluated)->children.push_back(var_node->idxAstNode);

							zs_print_debug_cr("scope %i",_currentScope->idxScope);


							if(!_currentScope->registerSymbol(symbol_name,var_node)){
								return NULL;
							}
						}
						if(*aux_p == '='){
							PASTNode children_node=NULL;

							// try to evaluate expression...
							aux_p=IGNORE_BLANKS(aux_p,m_line);

							if((aux_p = parseExpression(start_var,m_startLine,scope_info,var_node != NULL ? &children_node : NULL)) == NULL){
								return NULL;
							}

							if(var_node != NULL){

								if(children_node==NULL){
									zs_print_error_cr("children node == NULL");
									return NULL;
								}

								var_node->children.push_back(children_node->idxAstNode);
							}

							m_line = m_startLine;

							if(!(*aux_p == ';' || *aux_p == '=' || *aux_p == ',' )){
								zs_print_error_cr("line %i: expected ',',';' or '=' but it was '%c'", m_line,*aux_p);
								return NULL;
							}
						}
					}
					else{
						zs_print_error_cr("line %i: expected ',',';' or '=' but it was '%c'", m_line,*aux_p);
						return NULL;
					}
					// ignores ';' or ','
					if(*aux_p == ',')
						aux_p++;
				}

				if(*aux_p == ';'){
					aux_p++;
				}
				else{
					zs_print_error_cr("Expected ';' at line %i", m_line);
					return NULL;
				}

				return aux_p;
			}
		}
		return NULL;
	}

	char * CASTNode::parseBlock(const char *s,int & m_line,  CScope *scope_info, bool & error,PASTNode *ast_node_to_be_evaluated, bool push_scope){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;

		//CScope *_localScope =  scope_info != NULL ? scope_info->symbol_info.ast->scope_info_ptr: NULL;
		CScope *currentScope=  NULL;
		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check for keyword ...
		if(*aux_p == '{'){
			aux_p++;

			if(scope_info != NULL){
				currentScope =scope_info->getCurrentScopePointer();
				if(push_scope){
					currentScope = scope_info->pushScope();
				}
			}

			if((aux_p = generateAST_Recursive(aux_p, m_line,scope_info,error,ast_node_to_be_evaluated)) != NULL){
				if(error){
					return NULL;
				}

				if(ast_node_to_be_evaluated != NULL){
					(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
					if(currentScope != NULL){
						(*ast_node_to_be_evaluated)->idxScope = currentScope->idxScope;
					}
				}

				if(*aux_p != '}'){
					error = true;
					zs_print_error_cr("Expected } ");
					return NULL;
				}

				if(scope_info != NULL && push_scope){
					scope_info->popScope();
				}

				if(ast_node_to_be_evaluated != NULL){
					(*ast_node_to_be_evaluated)->node_type = BODY_NODE;
				}
				return aux_p+1;
			}
		}
		return NULL;
	}

	char * CASTNode::isClassMember(const char *s,int & m_line, string & _class_name, string & var_name, PASTNode & _class_node, bool & error, KEYWORD_TYPE kwi){

		char *aux_p = (char *)s;
		char *end_var;
		char *symbol_value;

		error = true;

		aux_p=IGNORE_BLANKS(aux_p,m_line);

		// check whwther the function is anonymous or not.
		end_var=getSymbolName(aux_p,m_line);

		if(end_var != NULL){

			if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
				return NULL;
		}else{
			return NULL;
		}
		aux_p=end_var;
		aux_p=IGNORE_BLANKS(aux_p,m_line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...
			_class_name = symbol_value;
			_class_node = findAst(_class_name,NODE_TYPE::KEYWORD_NODE, KEYWORD_TYPE::CLASS_KEYWORD);

			if(_class_node == NULL){
				zs_print_error_cr("Class \"%s\" at line %i is no defined", symbol_value, m_line);
				return NULL;
			}

			aux_p=IGNORE_BLANKS(aux_p+2,m_line); // ignore ::

			end_var=getSymbolName(aux_p,m_line);



			if(end_var != NULL){

				if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
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

	char *CASTNode::parseKeyWord(const char *s, int & m_line, CScope *scope_info, bool & error, PASTNode *ast_node_to_be_evaluated){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p= (char *)s;


		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD,keyw2nd=KEYWORD_TYPE::UNKNOWN_KEYWORD;

		aux_p=IGNORE_BLANKS(aux_p, m_line);

		// check if condition...
		keyw = isKeyword(aux_p);

		if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...

			aux_p+=strlen(defined_keyword[keyw].str);
			aux_p=IGNORE_BLANKS(aux_p, m_line);

			// check if non named function...
			if(keyw == KEYWORD_TYPE::FUNCTION_KEYWORD){
				if( *aux_p == '('){
					// Is no named. No named function is an object and should be processed within parseExpression ...
					return NULL;
				}
			}

			// check if class and is not main class (scope )...
			if(keyw == KEYWORD_TYPE::CLASS_KEYWORD){

				if((aux_p = parseClass(s,m_line,scope_info,ast_node_to_be_evaluated)) != NULL){
					return aux_p;
				}

				error = true;
				return NULL;
			}

			// check if another kwyword is defined ...
			if((keyw2nd = isKeyword(aux_p))!= KEYWORD_TYPE::UNKNOWN_KEYWORD){

				if(
						keyw2nd != KEYWORD_TYPE::FUNCTION_KEYWORD   // list of exceptional keywords...
				  ){

					zs_print_error_cr("unexpected keyword \"%s\" at line %i",defined_keyword[keyw2nd].str, m_line);
					error = true;
					return NULL;
				}
			}

			if(defined_keyword[keyw].parse_fun != NULL){

				if((aux_p = defined_keyword[keyw].parse_fun(s,m_line,scope_info, ast_node_to_be_evaluated)) != NULL){
					return aux_p;
				}
				error = true;
			}else{
				//zs_print_error_cr("%s has no parse function implemented yet!",defined_keyword[keyw->id].str);
			}
			// something wrong was happen..
		}

		return NULL;
	}

	char * CASTNode::generateAST_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error, PASTNode *node_to_be_evaluated, bool allow_breaks, bool is_main_node){

		// PRE: **node must be created and is i/o ast pointer variable where to write changes.
		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD;
		char *aux = (char *)s;
		char *end_expr=0;
		PASTNode children,_class_node;

		if(node_to_be_evaluated!= NULL){
			if(!is_main_node){
				if((*node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL;
				(*node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
				if(scope_info != NULL){ // by default put global scope.
					(*node_to_be_evaluated)->idxScope = scope_info->idxScope;
				}
			}
		}
		aux=IGNORE_BLANKS(aux, m_line);

		while(*aux != 0 ){

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
				keyw = isKeyword(aux);
				if(keyw!= KEYWORD_TYPE::UNKNOWN_KEYWORD){

					if(keyw == KEYWORD_TYPE::BREAK_KEYWORD){
						if(allow_breaks){

							return aux;
						}
						else{
							zs_print_error_cr("unexpected \"break\" at line %i",m_line);
							error=true;
							return NULL;
						}
					}else if(keyw == KEYWORD_TYPE::FUNCTION_KEYWORD||keyw == KEYWORD_TYPE::VAR_KEYWORD){

						int startLine = m_line;
						string _class_name, _member_name;
						bool error=false;
						if(isClassMember(aux+strlen(defined_keyword[keyw].str),startLine, _class_name, _member_name, _class_node,error, keyw) != NULL){


							if(keyw == KEYWORD_TYPE::VAR_KEYWORD){
								if((end_expr=parseMemberVar(aux,m_line,SCOPE_INFO_NODE(_class_node->idxScope),&children))==NULL){
									return NULL;
								}
								// push into var collection ...
								AST_NODE(_class_node->children[0])->children.push_back(children->idxAstNode);

							}else{
								startLine = m_line;
								end_expr=parseFunction(aux,m_line,SCOPE_INFO_NODE(_class_node->idxScope),&children);
								if(end_expr==NULL){
									return NULL;
								}
								// push into function collection...
								AST_NODE(_class_node->children[1])->children.push_back(children->idxAstNode);
							}
						}else{
							if(error){
								return NULL;
							}

						}
					}
				}
			}
			// 0st special case member class extension ...
			if(children==NULL){ // not processed yet ...
				// 1st. check whether parse a keyword...
				if((end_expr = parseKeyWord(aux, m_line, scope_info, error, node_to_be_evaluated != NULL ? &children : NULL)) == NULL){

					// If was unsuccessful then try to parse expression.
					if(error){
						return NULL;
					}
					// 2nd. check whether parse a block
					if((end_expr = parseBlock(aux,m_line, scope_info, error,node_to_be_evaluated != NULL ? &children:NULL))==NULL){

						// If was unsuccessful then try to parse expression.
						if(error){
							return NULL;
						}
						// 2nd. try expression
						int starting_expression=m_line;

						if((end_expr = parseExpression(aux,m_line, scope_info,node_to_be_evaluated != NULL ? &children:NULL)) == NULL){ // something wrong was happen.
							return NULL;
						}

						if(*end_expr != ';'){
							error = true;
							zs_print_error_cr("Expected ';' at expression starting at line %i",starting_expression);
							return NULL;
						}
						end_expr++;
					}
				}

				// new expression ready to be evaluated...
				if(node_to_be_evaluated != NULL && children != NULL){
					(*node_to_be_evaluated)->children.push_back(children->idxAstNode);
				}
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
	void CASTNode::init(){

			// init operator punctuators...
			defined_operator_punctuator[UNKNOWN_PUNCTUATOR]={UNKNOWN_PUNCTUATOR, "none",NULL};

			defined_operator_punctuator[ADD_PUNCTUATOR]={ADD_PUNCTUATOR, "+",parsePlusPunctuator};
			defined_operator_punctuator[SUB_PUNCTUATOR]={SUB_PUNCTUATOR, "-",parseMinusPunctuator};
			defined_operator_punctuator[MUL_PUNCTUATOR]={MUL_PUNCTUATOR, "*",parseMulPunctuator};
			defined_operator_punctuator[DIV_PUNCTUATOR]={DIV_PUNCTUATOR, "/",parseDivPunctuator};
			defined_operator_punctuator[MOD_PUNCTUATOR]={MOD_PUNCTUATOR, "%",parseModPunctuator};
			defined_operator_punctuator[FIELD_PUNCTUATOR]={FIELD_PUNCTUATOR, ".",parseFieldPunctuator};
			defined_operator_punctuator[TERNARY_IF_PUNCTUATOR]={TERNARY_IF_PUNCTUATOR, "?",parseInlineIfPunctuator};
			defined_operator_punctuator[TERNARY_ELSE_PUNCTUATOR]={TERNARY_ELSE_PUNCTUATOR, ":",parseInlineElsePunctuator};
			defined_operator_punctuator[ASSIGN_PUNCTUATOR]={ASSIGN_PUNCTUATOR, "=",parseAssignPunctuator};
			defined_operator_punctuator[ADD_ASSIGN_PUNCTUATOR]={ADD_ASSIGN_PUNCTUATOR, "+=",parseAddAssignPunctuator};
			defined_operator_punctuator[SUB_ASSIGN_PUNCTUATOR]={SUB_ASSIGN_PUNCTUATOR, "-=",parseSubAssignPunctuator};
			defined_operator_punctuator[MUL_ASSIGN_PUNCTUATOR]={MUL_ASSIGN_PUNCTUATOR, "*=",parseMulAssignPunctuator};
			defined_operator_punctuator[DIV_ASSIGN_PUNCTUATOR]={DIV_ASSIGN_PUNCTUATOR, "/=",parseDivAssignPunctuator};
			defined_operator_punctuator[MOD_ASSIGN_PUNCTUATOR]={MOD_ASSIGN_PUNCTUATOR, "%=",parseModAssignPunctuator};
			defined_operator_punctuator[BINARY_XOR_PUNCTUATOR]={BINARY_XOR_PUNCTUATOR, "^",parseBinaryXorPunctuator};
			defined_operator_punctuator[BINARY_AND_PUNCTUATOR]={BINARY_AND_PUNCTUATOR, "&",parseBinaryAndPunctuator};
			defined_operator_punctuator[BINARY_OR_PUNCTUATOR]={BINARY_OR_PUNCTUATOR, "|",parseBinaryOrPunctuator};
			defined_operator_punctuator[SHIFT_LEFT_PUNCTUATOR]={SHIFT_LEFT_PUNCTUATOR, "<<",parseShiftLeftPunctuator};
			defined_operator_punctuator[SHIFT_RIGHT_PUNCTUATOR]={SHIFT_RIGHT_PUNCTUATOR, ">>",parseShiftRightPunctuator};
			defined_operator_punctuator[LOGIC_AND_PUNCTUATOR]={LOGIC_AND_PUNCTUATOR, "&&",parseLogicAndPunctuator};
			defined_operator_punctuator[LOGIC_OR_PUNCTUATOR]={LOGIC_OR_PUNCTUATOR, "||",parseLogicOrPunctuator};
			defined_operator_punctuator[LOGIC_EQUAL_PUNCTUATOR]={LOGIC_EQUAL_PUNCTUATOR, "==",parseLogicEqualPunctuator};
			defined_operator_punctuator[LOGIC_NOT_EQUAL_PUNCTUATOR]={LOGIC_NOT_EQUAL_PUNCTUATOR, "!=",parseLogicNotEqualPunctuator};
			defined_operator_punctuator[LOGIC_GT_PUNCTUATOR]={LOGIC_GT_PUNCTUATOR, ">",parseLogicGreatherThanPunctuator};
			defined_operator_punctuator[LOGIC_LT_PUNCTUATOR]={LOGIC_LT_PUNCTUATOR, "<",parseLogicLessThanPunctuator};
			defined_operator_punctuator[LOGIC_GTE_PUNCTUATOR]={LOGIC_GTE_PUNCTUATOR, ">=",parseLogicGreatherEqualThanPunctuator};
			defined_operator_punctuator[LOGIC_LTE_PUNCTUATOR]={LOGIC_LTE_PUNCTUATOR, "<=",parseLessEqualThanPunctuator};
			defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR]={LOGIC_NOT_PUNCTUATOR, "!",parseNotPunctuator};
			defined_operator_punctuator[PRE_INC_PUNCTUATOR]={PRE_INC_PUNCTUATOR, "++",parseIncPunctuator};
			defined_operator_punctuator[PRE_DEC_PUNCTUATOR]={PRE_DEC_PUNCTUATOR, "--",parseDecPunctuator};
			defined_operator_punctuator[POST_INC_PUNCTUATOR]={POST_INC_PUNCTUATOR, "++",parseIncPunctuator};
			defined_operator_punctuator[POST_DEC_PUNCTUATOR]={POST_DEC_PUNCTUATOR, "--",parseDecPunctuator};

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
			defined_keyword[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var",parseVar};
			defined_keyword[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",parseIf};
			defined_keyword[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else",NULL};
			defined_keyword[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for",parseFor};
			defined_keyword[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while",parseWhile};

			defined_keyword[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch",parseSwitch};
			defined_keyword[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case",NULL};
			defined_keyword[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break",NULL};
			defined_keyword[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default",NULL};
			defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",parseFunction};
			defined_keyword[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return",parseReturn};
			defined_keyword[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this", NULL};
		//	defined_keyword[KEYWORD_TYPE::SUPER_KEYWORD] = {SUPER_KEYWORD,"super", NULL};
			defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",NULL};
			defined_keyword[KEYWORD_TYPE::NEW_KEYWORD] = {NEW_KEYWORD,"new", NULL};
			defined_keyword[KEYWORD_TYPE::DELETE_KEYWORD] = {DELETE_KEYWORD,"delete",NULL};
			// create main ast management

			if(vec_ast_node->size()==0){
				// create main ast/scope...
				CASTNode *ast=newASTNode();
				CScope *scp = CScope::newScope(ZS_UNDEFINED_IDX);
				ast->idxScope = scp->idxScope;
				ast->node_type = NODE_TYPE::BODY_NODE;

			}
			else{
				zs_print_warning("ASTNode already initialized!");
			}
	}

	CASTNode::CASTNode(){
		node_type = UNKNOWN_NODE;
		keyword_info = KEYWORD_TYPE::UNKNOWN_KEYWORD;
		pre_post_operator_info = PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
		line_value=ZS_UNDEFINED_IDX;
		operator_info=PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
		symbol_value="";
		idxAstParent=ZS_UNDEFINED_IDX;
		//aux_value=NULL;

		idxAstNode = ZS_UNDEFINED_IDX;
		idxScope = ZS_UNDEFINED_IDX;

		is_packed_node = false;

	}


	CASTNode::~CASTNode(){
		//destroyChildren();
	}

}
