#include "zg_script.h"

#define MAX_EXPRESSION_LENGHT 2096


#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __VERBOSE_MESSAGE__
#endif

#ifdef  __VERBOSE_MESSAGE__

#define print_ast_cr print_info_cr
#else
#define print_ast_cr(s,...)
#endif





bool CAst::parsePlusPunctuator(const char *s){
	char *aux=(char *)s;

	if(*aux=='+'){

		if(*(aux+1) == '+') {// inc op
			return false;
		}

		return true;
	}

	return false;
}

bool CAst::parseMinusPunctuator(const char *s){
	char *aux=(char *)s;


	if(*aux=='-'){

		if(*(aux+1) == '-') {// dec op
			return false;
		}

		return true;
	}

	return false;

}

bool CAst::parseMulPunctuator(const char *s){
	char *aux=(char *)s;

	if(*aux=='*'){
		return true;
	}

	return false;

}

bool CAst::parseDivPunctuator(const char *s){
	char *aux=(char *)s;

	if(*aux=='/'){
		return true;
	}
	return false;
}

bool CAst::parseModPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='%'){
		return true;
	}

	return false;
}

bool CAst::parseAssignPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='='){
		return true;
	}

	return false;
}

bool CAst::parseBinaryXorPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='^'){
		return true;
	}
	return false;
}

bool CAst::parseBinaryAndPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='&'){
		if(*(aux+1) == '&') {// logical op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseBinaryOrPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='|'){
		if(*(aux+1) == '|') {// logical op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseShiftLeftPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='<'){
		if(*(aux+1) == '<') {// shift left op
			return true;
		}
	}
	return false;
}

bool CAst::parseShiftRightPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='>'){
		if(*(aux+1) == '>') {// shift right op
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicAndPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='&'){
		if(*(aux+1) == '&') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicOrPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='|'){
		if(*(aux+1) == '|') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicEqualPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='='){
		if(*(aux+1) == '=') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicNotEqualPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='!'){
		if(*(aux+1) == '=') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicGreatherThanPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='>'){
		if(*(aux+1) == '=') {// get op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseLogicLessThanPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='<'){
		if(*(aux+1) == '=') {// let op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseLogicGreatherEqualThanPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='>'){
		if(*(aux+1) == '=') {// get op ok!
			return true;
		}
	}
	return false;

}

bool CAst::parseLessEqualThanPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='<'){
		if(*(aux+1) == '=') {// let op ok!
			return false;
		}
		return true;
	}
	return false;

}

bool CAst::parseNotPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='!'){
		if(*(aux+1) == '=') {// logical op
			return false;
		}
		return true;
	}
	return false;

}

bool CAst::parseNotEqualPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='!'){
		if(*(aux+1) == '=') {// logical op
			return true;
		}
	}
	return false;

}

bool CAst::parseIncPunctuator(const char *s){
	char *aux=( char *)s;


	if(*aux=='+'){
		if(*(aux+1) == '+') {// logical op
			return true;
		}
	}
	return false;
}

bool CAst::parseDecPunctuator(const char *s){
	char *aux=( char *)s;

	if(*aux=='-'){
		if(*(aux+1) == '-') {// logical op
			return true;
		}
	}
	return false;
}



#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

tInfoPunctuator  * CAst::parsePunctuatorGroup0(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			PLUS_PUNCTUATOR,
			MINUS_PUNCTUATOR,
			BINARY_XOR_PUNCTUATOR,
			BINARY_AND_PUNCTUATOR,
			BINARY_OR_PUNCTUATOR,
			SHIFT_LEFT_PUNCTUATOR,
			SHIFT_RIGHT_PUNCTUATOR,
			ASSIGN_PUNCTUATOR
	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return NULL;
}

tInfoPunctuator  * CAst::parsePunctuatorGroup1(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_AND_PUNCTUATOR,
			MUL_PUNCTUATOR,
			DIV_PUNCTUATOR,
			MOD_PUNCTUATOR
	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){ print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tInfoPunctuator  * CAst::parsePunctuatorGroup2(const char *s){


	PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_EQUAL_PUNCTUATOR,
			LOGIC_NOT_EQUAL_PUNCTUATOR,
			LOGIC_LET_PUNCTUATOR,
			LOGIC_GET_PUNCTUATOR,
			LOGIC_GT_PUNCTUATOR,
			LOGIC_LT_PUNCTUATOR
	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){ print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tInfoPunctuator  * CAst::parsePunctuatorGroup3(const char *s){


	PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_NOT_PUNCTUATOR,
	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){ print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tInfoPunctuator *  CAst::isOperatorPunctuator(const char *s){

	for(unsigned i = 0; i < MAX_OPERATOR_PUNCTUATORS; i++){
		if(defined_operator_punctuator[i].parse_fun != NULL){
			if(defined_operator_punctuator[i].parse_fun(s)){
				return &defined_operator_punctuator[i];
			}
		}
	}

	return 0;
}


tInfoPunctuator *  CAst::isSpecialPunctuator(const char *s){

	for(unsigned i = 0; i < MAX_SPECIAL_PUNCTUATORS; i++){

		if(*defined_special_punctuator[i].str == *s){
			return &defined_special_punctuator[i];
		}

	}

	return 0;
}


tInfoPunctuator *  CAst::isPunctuator(const char *s){

	tInfoPunctuator * ip = isOperatorPunctuator(s);

	if(ip!=NULL){
		return ip;
	}

	return isSpecialPunctuator(s);
}



// to string utils ...
char * CAst::getEndWord(const char *s){

	char *aux=(char *)s;

	if(*aux == '\"'){
		aux++;
		while((*aux)!=0 && !((*aux)=='\n') && !((*aux)=='\"')) {
			aux++;
		}

		if(*aux != '\"') {
			print_error_cr("Error \" not closed");
			return NULL;
		}
		aux++;

	}else{
		while((*aux)!=0 && !(
				(*aux)==' ' ||
				(*aux)=='\t' ||
				(*aux)=='\n' ||
				(*aux)=='\r'
						) &&
				(isOperatorPunctuator(aux)==NULL) &&
				isSpecialPunctuator(aux)==NULL
		) {
			aux++;
		}
	}
	return aux;
}

char *CAst::getSymbolName(const char *s,int & m_startLine){

	char *aux_p=(char *)s;
	tInfoPunctuator *end_punctuator=isPunctuator(s);


	if(end_punctuator != NULL){
		print_error_cr("Unexpected '%s' at line",end_punctuator->str,m_startLine);
		return NULL;
	}

	if(*aux_p!=0 && (
	   ('a' <= *aux_p && *aux_p <='z') ||
	   ('A' <= *aux_p && *aux_p <='Z'))
	){ // let's see it has right chars...

		aux_p++;
		while((*aux_p!=0) && (('a' <= *aux_p && *aux_p <='z') ||
			  ('0' <= *aux_p && *aux_p <='9') ||
			  (*aux_p=='_') ||
			  ('A' <= *aux_p && *aux_p <='Z'))){
			aux_p++;
		}
	}else{
		print_error_cr("Symbol name cannot start with char %c",*aux_p);
		return NULL;
	}

	return aux_p;
}

tInfoKeyword CAst::defined_keyword[MAX_KEYWORD];
tInfoPunctuator CAst::defined_operator_punctuator[MAX_OPERATOR_PUNCTUATORS];
tInfoPunctuator CAst::defined_special_punctuator[MAX_SPECIAL_PUNCTUATORS];


void CAst::createSingletons(){

	// init operator punctuators...
	defined_operator_punctuator[UNKNOWN_PUNCTUATOR]={UNKNOWN_PUNCTUATOR, "none",NULL};

	defined_operator_punctuator[PLUS_PUNCTUATOR]={PLUS_PUNCTUATOR, "+",parsePlusPunctuator};
	defined_operator_punctuator[MINUS_PUNCTUATOR]={MINUS_PUNCTUATOR, "-",parseMinusPunctuator};
	defined_operator_punctuator[MUL_PUNCTUATOR]={MUL_PUNCTUATOR, "*",parseMulPunctuator};
	defined_operator_punctuator[DIV_PUNCTUATOR]={DIV_PUNCTUATOR, "/",parseDivPunctuator};
	defined_operator_punctuator[MOD_PUNCTUATOR]={MOD_PUNCTUATOR, "%",parseModPunctuator};

	defined_operator_punctuator[ASSIGN_PUNCTUATOR]={ASSIGN_PUNCTUATOR, "=",parseAssignPunctuator};

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
	defined_operator_punctuator[LOGIC_GET_PUNCTUATOR]={LOGIC_GET_PUNCTUATOR, ">=",parseLogicGreatherEqualThanPunctuator};
	defined_operator_punctuator[LOGIC_LET_PUNCTUATOR]={LOGIC_LET_PUNCTUATOR, "<=",parseLessEqualThanPunctuator};
	defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR]={LOGIC_NOT_PUNCTUATOR, "!",parseNotPunctuator};

	defined_operator_punctuator[PRE_INC_PUNCTUATOR]={PRE_INC_PUNCTUATOR, "++",parseIncPunctuator};
	defined_operator_punctuator[PRE_DEC_PUNCTUATOR]={PRE_DEC_PUNCTUATOR, "--",parseDecPunctuator};

	defined_operator_punctuator[POST_INC_PUNCTUATOR]={POST_INC_PUNCTUATOR, "++",parseIncPunctuator};
	defined_operator_punctuator[POST_DEC_PUNCTUATOR]={POST_DEC_PUNCTUATOR, "--",parseDecPunctuator};


	// special punctuators...
	defined_special_punctuator[UNKNOWN_PUNCTUATOR]={UNKNOWN_PUNCTUATOR, "",NULL};
	defined_special_punctuator[COMA_PUNCTUATOR]={COMA_PUNCTUATOR, ",",NULL};
	defined_special_punctuator[SEMICOLON_PUNCTUATOR]={SEMICOLON_PUNCTUATOR, ";",NULL};
	defined_special_punctuator[OPEN_PARENTHESIS_PUNCTUATOR]={OPEN_PARENTHESIS_PUNCTUATOR, "(",NULL};
	defined_special_punctuator[CLOSE_PARENTHESIS_PUNCTUATOR]={CLOSE_PARENTHESIS_PUNCTUATOR, ")",NULL};
	defined_special_punctuator[OPEN_BRAKET_PUNCTUATOR]={OPEN_BRAKET_PUNCTUATOR, "{",NULL};
	defined_special_punctuator[CLOSE_BRAKET_PUNCTUATOR]={CLOSE_BRAKET_PUNCTUATOR, "}",NULL};
	defined_special_punctuator[OPEN_SQUARE_BRAKET_PUNCTUATOR]={OPEN_SQUARE_BRAKET_PUNCTUATOR, "[",NULL};
	defined_special_punctuator[CLOSE_SQUARE_BRAKET_PUNCTUATOR]={CLOSE_SQUARE_BRAKET_PUNCTUATOR, "]",NULL};
	defined_special_punctuator[FIELD_PUNCTUATOR]={FIELD_PUNCTUATOR, ".",NULL};


	// init special punctuators...


	// init keywords...
	defined_keyword[KEYWORD_TYPE::UNKNOWN_KEYWORD] = {UNKNOWN_KEYWORD, "none",NULL};
	defined_keyword[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var",parseVar};
	defined_keyword[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",parseIf},
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
	defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",parseClass};

}


void CAst::destroySingletons(){

}


tInfoKeyword * CAst::isKeyword(const char *c){

	int m_line=0;

	char *str=CStringUtils::IGNORE_BLANKS(c,m_line);

	for(int i = 0; i < MAX_KEYWORD; i++){
		int size = strlen(defined_keyword[i].str);
		char *aux = str+size;
		if((strncmp(str,defined_keyword[i].str,size)==0) && (
				*aux == 0  || // carry end
				*aux == ' '  || // space

				*aux == '\t'  || // tab
				isOperatorPunctuator(aux)!=NULL ||
				isSpecialPunctuator(aux)!=NULL ||
				*aux == '\n' || // carry return


			   (*aux == '/' && *(aux+1) == '*')) //start block comment
			   ){
			return &defined_keyword[i];
		}
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
tInfoPunctuator *CAst::checkPreOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return &defined_operator_punctuator[PRE_INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return &defined_operator_punctuator[PRE_DEC_PUNCTUATOR];
	if(parsePlusPunctuator(s)) 	return &defined_operator_punctuator[PLUS_PUNCTUATOR];
	if(parseMinusPunctuator(s)) return &defined_operator_punctuator[MINUS_PUNCTUATOR];
	if(parseNotPunctuator(s))   return &defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR];

	return 0;
}

tInfoPunctuator *CAst::checkPostOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return &defined_operator_punctuator[POST_INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return &defined_operator_punctuator[POST_DEC_PUNCTUATOR];

	return 0;
}

PASTNode CAst::preNode(tInfoPunctuator * operator_info,PASTNode affected_op){ // can be -,+,! etc...
	PASTNode op=new tASTNode;
	op->operator_info = operator_info;
	op->node_type =PUNCTUATOR_NODE;
	op->children.push_back(affected_op);
	return op;
}
//-----------------------------------------------------------------------------------------------------------
char * CAst::parseExpression_Recursive(const char *s, int m_line, CScriptFunction *sf, PASTNode *op, GROUP_TYPE type_group,PASTNode parent ){
	//int index=0;
	char *aux=(char *)s;
	char *s_effective_start=(char *)s;
	char *expr_start_op;
	char *end_expression=(char *)s ; // by default end expression isequal to
	PASTNode symbol_node=NULL; // can be a function or array.
	PASTNode args_node=NULL;
	string symbol_name;
	string operator_str="";
	tInfoPunctuator *pre_operator=NULL,
					*post_operator=NULL,
					*operator_group=NULL;
	bool theres_some_operator=false;

	*op=NULL; // always preallocate 2 nodes (left and right)


	int m_startLine=m_line;
	int m_define_symbol_line;

	aux=CStringUtils::IGNORE_BLANKS(aux, m_startLine);
	
	if(*aux==0){
		return NULL;
	}

	if(type_group>=MAX_GROUPS) {
		print_error_cr("reached max groups");
		return NULL;
	}

	print_ast_cr("new expression eval:\"%.30s ...\" group:%i at line %i",aux,type_group, m_startLine);
	
	// searching for operator!
	print_ast_cr("searching for operator type %i...",type_group);
	
	while(*aux!=0 && *aux!=';' && *aux!=','  && *aux!=')'  && *aux!=']' && (operator_group==0)){
	
		print_ast_cr("checkpoint1:%c\n",*aux);
		// 1. ignore spaces...
		aux=CStringUtils::IGNORE_BLANKS(aux, m_startLine);

		if((pre_operator=checkPreOperatorPunctuator(aux))!=NULL){
			aux+=strlen(pre_operator->str);
			aux=CStringUtils::IGNORE_BLANKS(aux, m_startLine);
		}

		if(*aux=='('){

			int start_line = m_startLine;

			if(pre_operator != NULL){
				if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
						pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
					print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_startLine);
					return NULL;
				}
			}

			aux = parseExpression_Recursive(aux+1, m_startLine, sf, op, type_group,parent);

			if(*aux != ')'){
				print_error_cr("Not closed parenthesis starting at line %i", start_line);
				return NULL;
			}

			end_expression = aux+1;

		}else{ // check for symbols (must have a symbol at least)

			tInfoKeyword *key_w;
			m_define_symbol_line = m_startLine;

			// usually we have to take care about special op symbols
			if((key_w =isKeyword(aux)) != NULL){

				if(key_w->id == KEYWORD_TYPE::THIS_KEYWORD){ // do a recursive way to get all fields until end ?

					end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);

					aux+=strlen(aux);
				}

				if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){ // function object ...

					if(pre_operator!=NULL){
						if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
								pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
							print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_startLine);
							return NULL;
						}
					}

					end_expression = parseFunction(aux,m_define_symbol_line,sf,&symbol_node);
					symbol_node->value_symbol="anonymous_function";
					if(end_expression == NULL){
						return NULL;
					}

					symbol_node->node_type = NODE_TYPE::KEYWORD_NODE;
					symbol_node->keyword_info = &defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD];

				}else{
					print_error_cr("Unexpected keyword %s at line %i",key_w->str,m_define_symbol_line);
					return NULL;
				}
			}else{

				if(*aux == '['){ // vector object ...
					if(pre_operator != NULL){
						if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
								pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
							print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_startLine);
							return NULL;
						}
					}

					end_expression = parseArgs(aux,m_define_symbol_line,sf,&symbol_node, '[', ']');
					symbol_node->value_symbol="anonymous_array";

					if(end_expression == NULL){
						return NULL;
					}

					symbol_node->node_type = NODE_TYPE::ARRAY_NODE;

				}
				else{

					end_expression = getEndWord(aux);

					 if(end_expression == NULL || end_expression == aux){
						 print_error_cr("Expected symbol at line %i",m_startLine);
						 return NULL;
					 }

					 symbol_name=CStringUtils::copyStringFromInterval(aux,end_expression);

					 // check for post opertator...
					 end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);
					 if((post_operator = checkPostOperatorPunctuator(end_expression)) != NULL){
						 end_expression+=strlen(post_operator->str);
					 }
				}
			}

			// if there's function or array access after symbol or object created ...
			end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);

			if(*end_expression == '('){ // parse args within '(' ')'...

				end_expression = parseArgs(end_expression,m_define_symbol_line,sf,&args_node, '(', ')');

				if(end_expression == NULL){
					return NULL;
				}

				args_node->node_type = NODE_TYPE::FUNCTION_ARGS_DECL_NODE;
				end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);
			}
			else if(*end_expression == '['){ // parse args within '(' ')'...

				end_expression = parseArgs(end_expression,m_define_symbol_line,sf,&args_node, '[', ']');

				if(end_expression == NULL){
					return NULL;
				}

				args_node->node_type = NODE_TYPE::ARRAY_INDEX_NODE;

				end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);

				// check for post opertator...
				if((post_operator = checkPostOperatorPunctuator(end_expression)) != NULL){
				 end_expression+=strlen(post_operator->str);
				}

				end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);
			}
		}

		aux=end_expression;

		print_ast_cr("checkpoint3:%c\n",*aux);

		aux=CStringUtils::IGNORE_BLANKS(aux, m_startLine);

		if(*aux!=0 && *aux!=';'  && *aux!=','  && *aux!=')'  && *aux!=']' ){ // is not end expression

			if((operator_group=isOperatorPunctuator(aux))!=0){

				theres_some_operator |= true;
				expr_start_op=aux;
				aux+=strlen(operator_group->str);

				switch(type_group){
				case GROUP_0:	operator_group = parsePunctuatorGroup0(expr_start_op);break;
				case GROUP_1:	operator_group = parsePunctuatorGroup1(expr_start_op);break;
				case GROUP_2:	operator_group = parsePunctuatorGroup2(expr_start_op);break;
				case GROUP_3:	operator_group = parsePunctuatorGroup3(expr_start_op);break;
				default: break;
				}
			}
		}
	}

	if(operator_group==NULL) {// there's no any operators \"type_group\"...
		if(!theres_some_operator){ // only we have a value (trivial)

			if(symbol_node != NULL){ // the symbol was found before...
				*op = symbol_node;
			}else{
				(*op)=new tASTNode;
				(*op)->node_type = SYMBOL_NODE;
				(*op)->value_symbol=symbol_name; // assign its value ...
			}

			print_ast_cr("trivial value \"%s\" at line %i",(*op)->value_symbol.c_str(), m_define_symbol_line);

			(*op)->parent=parent;
			(*op)->definedValueline=m_define_symbol_line;

			if(args_node != NULL){ // put args whether the symbol or object are function/array.
				(*op)->children.push_back(args_node);
			}

			if(pre_operator!= NULL || post_operator != NULL){ // create pre operator node ...

				if(pre_operator!=NULL)
					*op = preNode(pre_operator,*op);
				else
					*op = preNode(post_operator,*op);
			}
		}
		else{

			if(end_expression!= NULL){
			// there's a Punctuator, so let's perform generate its AST
				// reset prePunctuator...
				pre_operator=NULL;
				print_ast_cr("try to generate group1 expression: %.20s ...\n",s_effective_start);
				return parseExpression_Recursive(s,m_startLine,sf,op,(GROUP_TYPE)(((int)type_group)+1),parent);
			}
		}
	}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...

		*op=new tASTNode(2); // always preallocate 2 nodes (left and right)
		(*op)->parent=parent;
		char * expr_op_end = expr_start_op+strlen(operator_group->str);

		print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_group->str);
		char eval_left[MAX_EXPRESSION_LENGTH]={0};

		strncpy(eval_left,s_effective_start,expr_start_op-s_effective_start); // copy its left side...
		if(parseExpression_Recursive(eval_left,m_effective_start_line,sf,&(*op)->children[LEFT_NODE],type_group,*op)==NULL){
			return NULL;
		}

		if(pre_operator!=NULL){
			(*op)->children[LEFT_NODE]=preNode(pre_operator,(*op)->children[LEFT_NODE]);
		}

		if((aux=parseExpression_Recursive(expr_op_end,m_startLine,sf,&(*op)->children[RIGHT_NODE],type_group,(*op))) == NULL){
			return NULL;
		}

		// minus operators has special management because two negatives can be + but sums of negatives works
		if(operator_group->id == MINUS_PUNCTUATOR) {
			(*op)->children[RIGHT_NODE]=preNode(operator_group,(*op)->children[RIGHT_NODE]);

			// override the operator by +
			operator_group=&defined_operator_punctuator[PLUS_PUNCTUATOR];
		}

		(*op)->node_type = PUNCTUATOR_NODE;
		(*op)->operator_info = operator_group;
	}
	return aux;
}

char * CAst::parseExpression(const char *s, int m_line, CScriptFunction *sf , PASTNode * ast_node_to_be_evaluated ){

	// PRE: s is current string to parse. This function tries to parse an expression like i+1; and generates binary ast.
	// If this functions finds ';' then the function will generate ast.

	// last character is in charge of who is calling parseExpression because there's many ending cases ): [ ';' ',' ')' , ']' ]
	return parseExpression_Recursive(s,m_line,sf,ast_node_to_be_evaluated);
}

//---------------------------------------------------------------------------------------------------------------
// PARSE KEYWORDS
char * CAst::parseClass(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_p;
	int m_startLine = m_line;
	tInfoKeyword *key_w;
	PASTNode function_collection_node=NULL,vars_collection_node=NULL,child_node=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::CLASS_KEYWORD){

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p+strlen(key_w->str),m_startLine);

		// check for symbol's name
		end_p = getEndWord(aux_p);
		if(end_p == NULL || end_p == aux_p){
			print_error_cr("Expected name class at line %i ",m_startLine);
			return NULL;
		}


		aux_p=CStringUtils::IGNORE_BLANKS(end_p,m_startLine);

		if(*aux_p == '{'){

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

			// it seem's we have a good built class...
			*ast_node_to_be_evaluated = new tASTNode;
			(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
			(*ast_node_to_be_evaluated)->keyword_info = key_w;

			// create var & functions collection...
			(*ast_node_to_be_evaluated)->children.push_back(vars_collection_node = new tASTNode);
			(*ast_node_to_be_evaluated)->children.push_back(function_collection_node = new tASTNode);

			// check for named functions or vars...

			while(*aux_p != '}' && *aux_p != 0){

				// 1st. check whether parse a keyword...
				key_w = isKeyword(aux_p);
				if(key_w != NULL){
					switch(key_w->id){
					default:
						print_error_cr("Expected \"var\" or \"function\" keyword at line %i");
						break;
					case KEYWORD_TYPE::FUNCTION_KEYWORD:
						if((aux_p = parseFunction(aux_p, m_startLine,sf, &child_node)) != NULL){
							if(child_node->value_symbol != ""){
								function_collection_node->children.push_back(child_node);
							}
							else {
								print_error_cr("Expected symbol after function at line %i",m_startLine);
								return NULL;
							}
						} else{
							return NULL;
						}
						break;
					case KEYWORD_TYPE::VAR_KEYWORD:
						if((aux_p = parseVar(aux_p, m_startLine,sf, &child_node)) != NULL){

							vars_collection_node->children.push_back(child_node);

						} else{
							return NULL;
						}
						break;

					}
				}else{
					print_error_cr("Expected \"var\" or \"function\" keyword at line %i");
				}


				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);
			}


		}else{
			print_error_cr("Expected '{' at line %i ",m_startLine);
			return NULL;
		}



	}
	return NULL;
}

char * CAst::parseArgs(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated, char c1,char c2){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	int m_startLine = m_line;

	PASTNode   node_arg_expression=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	if(*aux_p == c1){
		aux_p++;

		(*ast_node_to_be_evaluated) = new tASTNode;

		do{
			if((aux_p = parseExpression(aux_p,m_startLine,sf,&node_arg_expression))==NULL){
				return NULL;
			}

			if(*aux_p == ',' ){
				aux_p++;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
			}else{
				if(*aux_p != c2 ){
					print_error_cr("Expected %c",c2);
					return NULL;
				}
			}

		}while(*aux_p != c2 && *aux_p != 0);

		return aux_p+1;
	}
	return NULL;
}

char * CAst::parseFunction(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *value_symbol,*end_var;
	tInfoKeyword *key_w;
	int m_startLine = m_line;
	PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
	string conditional_str;
	bool error=false;
	CScope::tInfoRegisteredVar * irv=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){

		// advance keyword...
		aux_p += strlen(key_w->str);
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_info = key_w;

		(*ast_node_to_be_evaluated)->children.push_back(args_node = new tASTNode);
		(*ast_node_to_be_evaluated)->children.push_back(body_node = new tASTNode);

		args_node->node_type = NODE_TYPE::FUNCTION_ARGS_DECL_NODE;
		body_node->node_type = BODY_NODE;


		// start line ...
		(*ast_node_to_be_evaluated)->definedValueline = m_startLine;

		if(*aux_p!='('){ // is named function..

			// check whwther the function is anonymous or not.
			end_var=getSymbolName(aux_p,m_startLine);

			if(end_var != NULL){

				if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;

				// check whether parameter name's matches with some global variable...
				if((irv=sf->getScope()->getCurrentScope()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
					print_error_cr("Function name \"%s\" defined at line %i is ambiguos with symbol defined at %i", value_symbol, m_startLine,irv->m_line);
					return NULL;
				}

				(*ast_node_to_be_evaluated)->value_symbol = value_symbol;

				// define value symbol...
				sf->getScope()->registerSymbol((*ast_node_to_be_evaluated)->value_symbol,m_startLine);
			}else{
				return NULL;
			}

			aux_p=end_var;
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		}
		// parse function args...
		if(*aux_p == '('){ // push arguments...

			aux_p++;

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

			// grab words separated by ,
			while(*aux_p != 0 && *aux_p != ')'){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

				if(*aux_p == ')' || *aux_p == ','){
					print_error_cr("Expected arg at line %i ",m_startLine);
					return NULL;
				}

				end_var=getSymbolName(aux_p,m_startLine);

				if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;

				// check if repeats...
				for(unsigned k = 0; k < args_node->children.size(); k++){
					if(args_node->children[k]->value_symbol == value_symbol){
						print_error_cr("Repeated argument '%s' argument at line %i ",value_symbol,m_startLine);
						return NULL;
					}
				}

				// check whether parameter name's matches with some global variable...
				if((irv=sf->getScope()->getCurrentScope()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
					print_error_cr("Ambiguos symbol argument \"%s\" at line %i name with var defined at %i", value_symbol, m_startLine,irv->m_line);
					return NULL;

				}
				aux_p=end_var;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

				if(*aux_p != ')'){

					if(*aux_p != ','){
						print_error_cr("Expected ',' at line ",m_startLine);
						return NULL;
					}

					aux_p++;
				}
				// PUSH NEW ARG...
				arg_node = new tASTNode;
				arg_node->value_symbol=value_symbol;
				args_node->children.push_back(arg_node);
			}

			if(*aux_p != ')'){
				print_error_cr("Expected ')' at line ",m_startLine);
				return NULL;
			}

			aux_p++;
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

			if(*aux_p != '{'){
				print_error_cr("Expected '{' at line ",m_startLine);
				return NULL;
			}

			// ok let's go to body..
			if((aux_p = parseBlock(aux_p,m_startLine,sf,error,&body_node)) != NULL){

				if(!error){
					m_line = m_startLine;
					return aux_p;
				}
			}
		}
		else{
			print_error_cr("Unclosed function defined at line %i",m_startLine);
		}
	}
	return NULL;
}

char *  CAst::parseReturn(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	tInfoKeyword *key_w;
	PASTNode expression_node=NULL; // in case there's = expression.

	string s_aux;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...

		aux_p += strlen(key_w->str);

		if((aux_p = parseExpression(aux_p, m_startLine, sf, &expression_node))!= NULL){

			if(*aux_p!=';'){
				print_error_cr("Expected ';' at line %i", m_startLine);
				return NULL;
			}
			aux_p++;

			*ast_node_to_be_evaluated = new tASTNode;
			(*ast_node_to_be_evaluated)->node_type = NODE_TYPE::KEYWORD_NODE;
			(*ast_node_to_be_evaluated)->keyword_info = key_w;

			(*ast_node_to_be_evaluated)->children.push_back(expression_node);

			m_line = m_startLine;

			return aux_p;
		}
	}
	return NULL;
}

char * CAst::parseWhile(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tInfoKeyword *key_w;
	int m_startLine = m_line;
	PASTNode conditional=NULL, while_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::WHILE_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_info = key_w;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,')',m_startLine)) != NULL){

				if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
					return NULL;
				}

				conditional_str=start_symbol;

				if(parseExpression((const char *)conditional_str.c_str(),m_startLine,sf,&conditional) == NULL){
					return NULL;
				}

				conditional->node_type = CONDITIONAL_NODE;
				(*ast_node_to_be_evaluated)->children.push_back(conditional);

				aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_startLine);
				if(*aux_p != '{'){
					print_error_cr("Expected while-block open block ('{') %i",m_startLine);
					return NULL;
				}

				if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&while_node))!= NULL){
					if(!error){
						while_node->node_type = WHILE_NODE;
						(*ast_node_to_be_evaluated)->children.push_back(while_node);
						m_line = m_startLine;
						return aux_p;
					}
				}

			}else{
				print_error_cr("Expected ')' while %i",m_startLine);
				return NULL;
			}

		}else{
			print_error_cr("Expected '(' while %i",m_startLine);
			return NULL;
		}
	}
	return NULL;
}

char * CAst::parseIf(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tInfoKeyword *key_w;
	int m_startLine = m_line;
	PASTNode conditional=NULL, if_node=NULL, else_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::IF_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_info = key_w;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,')',m_startLine)) != NULL){

				if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
					return NULL;
				}

				conditional_str=start_symbol;

				if(parseExpression((const char *)conditional_str.c_str(),m_startLine,sf,&conditional) == NULL){
					return NULL;
				}

				conditional->node_type = CONDITIONAL_NODE;
				(*ast_node_to_be_evaluated)->children.push_back(conditional);

				aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_startLine);
				if(*aux_p != '{'){
					print_error_cr("Expected if-block open block ('{') %i",m_startLine);
					return NULL;
				}

				if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&if_node))!= NULL){
					if(!error){
						if_node->node_type = IF_NODE;
						(*ast_node_to_be_evaluated)->children.push_back(if_node);

						aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

						bool else_key = false;
						if((key_w = isKeyword(aux_p)) != NULL){
							else_key = (key_w->id == KEYWORD_TYPE::ELSE_KEYWORD);
						}

						if(else_key){
							aux_p += strlen(key_w->str);

							aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
							if(*aux_p != '{'){
								print_error_cr("Expected else-block open block ('{') %i",m_startLine);
								return NULL;
							}

							if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&else_node))!= NULL){
									if(!error){

										else_node->node_type = ELSE_NODE;
										(*ast_node_to_be_evaluated)->children.push_back(else_node);
										m_startLine = m_line;
										return aux_p;
									}
									else{

									}
							}
						}else{
							m_line = m_startLine;
							return aux_p;
						}
					}
				}
			}else{
				print_error_cr("Expected ')' if %i",m_startLine);
				return NULL;
			}

		}else{
			print_error_cr("Expected '(' if %i",m_startLine);
			return NULL;
		}
	}
	return NULL;
}

char * CAst::parseFor(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *aux_p2;
	tInfoKeyword *key_w;
	char *end_expr,*start_symbol;
	int m_startLine = m_line;
	bool error=false;
	PASTNode block_for = NULL;
	string eval_for;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::FOR_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_info = key_w;

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){ // ready ...

			struct{
				char next_char;
				NODE_TYPE node_type;
			}info_for[3]={
					{';',PRE_FOR_NODE},
					{';',CONDITIONAL_NODE},
					{')',POST_FOR_NODE}
			};

			for(int i = 0; i < 3; i++){

				PASTNode for_node;
				if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,info_for[i].next_char,m_startLine)) != NULL){

					if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					eval_for = start_symbol;

					aux_p2=CStringUtils::IGNORE_BLANKS(eval_for.c_str(),m_startLine);

					if(*aux_p2 != 0){ // check is not void


						if(parseExpression((const char *)aux_p2,m_startLine,sf,&for_node) == NULL){
							return NULL;
						}

						for_node->node_type = info_for[i].node_type;
						(*ast_node_to_be_evaluated)->children.push_back(for_node);

					}
				}else{
					print_error_cr("Expected '%c' line %i",info_for[i].next_char,m_startLine);
				}

				aux_p = end_expr+1;
			}

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
			if(*aux_p != '{'){
				print_error_cr("Expected '{' for-block at line %i",m_startLine);
				return NULL;
			}

			// parse block ...
			if((aux_p=parseBlock(aux_p,m_startLine,sf,error,&block_for))!= NULL){
				if(!error){

					block_for->node_type = FOR_NODE;
					(*ast_node_to_be_evaluated)->children.push_back(block_for);
					m_line = m_startLine;
					return aux_p;
				}
				else{

				}
			}
		}else{
			print_error_cr("Expected '(' for %i",m_startLine);
			return NULL;
		}
	}

	return NULL;
}

char * CAst::parseSwitch(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_expr, *end_symbol,*start_symbol;
	PASTNode case_default_keyword_node,case_body_node,case_value_node;
	char *value_to_eval;
	string val;
	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	tInfoKeyword *key_w,*key_w2;
	int m_startLine = m_line;
	bool error=false;
	int n_cases;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::SWITCH_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_info = key_w;

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		if(*aux_p == '('){
			//if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p,')',m_startLine)) != NULL){
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

				end_expr = getEndWord(aux_p);

				value_to_eval = CStringUtils::copyStringFromInterval(aux_p, end_expr);

				if(!value_to_eval) {return NULL;}

				//--- create node
				(*ast_node_to_be_evaluated)->value_symbol = value_to_eval;

				//---
				aux_p=CStringUtils::IGNORE_BLANKS(end_expr,m_startLine);

				if(*aux_p != ')'){
					print_error_cr("Expected ')' switch %i",m_startLine);
					error = true;
					return NULL;
				}

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

				if(*aux_p == '{'){

					// ok try to get cases and default nodes ...
					aux_p++;
					aux_p = CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);


					while(*aux_p!='}' && *aux_p!=0){
							n_cases = 0;
							// init node ..
							case_default_keyword_node = new tASTNode;

							while((key_w = isKeyword(aux_p)) != NULL){ // acumulative cases /defaults...

								switch(key_w->id){
								case DEFAULT_KEYWORD:
								case CASE_KEYWORD:

									val = "default";
									aux_p += strlen(key_w->str);
									aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

									if(key_w->id == CASE_KEYWORD){


										// get the symbol...

										start_symbol=aux_p;
										end_symbol = getEndWord(start_symbol);
										aux_p=end_symbol;

										value_to_eval = CStringUtils::copyStringFromInterval(start_symbol, end_symbol);

										if(value_to_eval==NULL){ return NULL;}

										val = value_to_eval;

										aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
									}



									if(*aux_p == ':'){

										// check if repeated...
										for(unsigned j=0; j <(*ast_node_to_be_evaluated)->children.size(); j++ ){
											for(unsigned h=0; h <(*ast_node_to_be_evaluated)->children[j]->children.size(); h++ ){
												if((*ast_node_to_be_evaluated)->children[j]->children[h]->value_symbol == val){
													print_error_cr("Symbol %s repeteaded in switch at line %i",val.c_str(),m_startLine);
													return NULL;
												}
											}
										}

										case_value_node = new tASTNode;
										case_value_node->node_type = KEYWORD_NODE;
										case_value_node->keyword_info = &defined_keyword[CASE_KEYWORD];
										case_value_node->value_symbol = val;
										case_default_keyword_node->children.push_back(case_value_node);

										aux_p++;
										n_cases++;
									}
									else{
										print_error_cr("Expected  : case at %i",m_startLine);
										return NULL;
									}


									break;

									aux_p += strlen(key_w->str);
									aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
									if(*aux_p == ':'){
										n_cases++;
										aux_p++;
									}
									else{
										print_error_cr("Expected  : default at %i",m_startLine);
										return NULL;
									}

									break;
								default:
									print_error_cr("Expected case or default in switch %i",m_startLine);
									return NULL;
									break;
								}

								aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
							}

							if(n_cases == 0){
								print_error_cr("Expected case or default in switch %i",m_startLine);
								return NULL;
							}

							_localScope->pushScope();

							case_body_node = NULL;

							// eval block...
							if((aux_p=generateAST_Recursive(aux_p, m_startLine, sf, error, &case_body_node))==NULL){
								return NULL;
							}

							if(error){
								return NULL;
							}

							key_w2 = isKeyword(aux_p);
							if(key_w2->id == BREAK_KEYWORD){
								aux_p += strlen(key_w2->str);
								CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

								if(*aux_p == ';'){ // the new scope ...
									case_default_keyword_node->node_type = KEYWORD_NODE;
									//case_default_keyword_node->keyword_type = key_w->id; // think about
									case_default_keyword_node->value_symbol=value_to_eval;

									if(case_body_node != NULL){
										case_body_node->node_type = BODY_NODE;
									}
									// stack body ...
									case_default_keyword_node->children.push_back(case_body_node);

									// stack cases/default with body...
									(*ast_node_to_be_evaluated)->children.push_back(case_default_keyword_node);
								}
								else{
									print_error_cr("Expected break %i",m_startLine);
									return NULL;
								}

							}else{
								print_error_cr("Expected break %i",m_startLine);
								return NULL;
							}

							_localScope->popScope();

							aux_p =CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);
						}

					if(*aux_p == '}'){
						m_line = m_startLine;
						return aux_p + 1;
					}
					else{
						print_error_cr("Expected '}' switch %i",m_startLine);
					}
				}
				else{
					print_error_cr("Expected '{' switch %i",m_startLine);
				}
		}
		else{
			print_error_cr("Expected '(' switch %i",m_startLine);
		}
	}
	return NULL;
}

char * CAst::parseVar(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	CScope *_currentScope =  sf->getScope()->getCurrentScope(); // gets current evaluating scope...
	tInfoKeyword *key_w;
	char *start_var,*end_var, *symbol_name;
	PASTNode expression_node=NULL; // in case there's = expression.

	string s_aux;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	key_w = isKeyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		start_var=aux_p;

		end_var=getSymbolName(aux_p,m_startLine);

		if(end_var != NULL){

			if((symbol_name=CStringUtils::copyStringFromInterval(start_var,end_var)) == NULL){
				return NULL;
			}

			aux_p=end_var;

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if((*aux_p == ';' || *aux_p == '=')){

				print_info_cr("registered \"%s\" variable: ",symbol_name);

				if(!_currentScope->registerSymbol(symbol_name,m_startLine)){
					return NULL;
				}

				if(*aux_p == '='){

					// try to evaluate expression...
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

					if((end_var = parseExpression(aux_p+1,m_startLine,sf,&expression_node)) == NULL){
						return NULL;
					}else{

						if(*end_var == ';'){
							aux_p = end_var+1; // +1 ignores ;
						}else{
							print_error_cr("Expected ';' at line %i",m_startLine);
							return NULL;
						}
					}
				}else{ // ignores ';'
					aux_p++;
				}

				(*ast_node_to_be_evaluated) = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;

				if(expression_node != NULL){
					(*ast_node_to_be_evaluated)->children.push_back(expression_node);
				}

				// rewind line when the variable was declared because we pass the expression "variable=expression"
				m_line = m_startLine;

				return aux_p;
			}
			else{
				print_error_cr("variable cannot cannot contain char '%c' at line",*aux_p, m_line);
			}
		}
	}
	return NULL;
}

char * CAst::parseBlock(const char *s,int & m_line,  CScriptFunction *sf, bool & error,PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;

	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	//CScope *_ant, *_post;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...

	if(*aux_p == '{'){
		aux_p++;

		_localScope->pushScope();

		if((aux_p = generateAST_Recursive(aux_p, m_startLine,sf,error,ast_node_to_be_evaluated)) != NULL){
			if(error){
				return NULL;
			}

			if(*aux_p != '}'){
				error = true;
				print_error_cr("Expected } ");
				return NULL;
			}

			_localScope->popScope();

			(*ast_node_to_be_evaluated)->node_type = BODY_NODE;

			m_line = m_startLine;
			return aux_p+1;
		}
	}
	return NULL;
}

char *CAst::parseKeyWord(const char *s, int & m_start_line, CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p= (char *)s;
	int m_line = m_start_line;

	tInfoKeyword *keyw=NULL,*keyw2nd=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

	// check if condition...
	keyw = isKeyword(aux_p);

	if(keyw != NULL){ // a keyword was detected...

		aux_p+=strlen(keyw->str);
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

		// check if another kwyword is put ...
		if((keyw2nd = isKeyword(aux_p))!= NULL){
			print_error_cr("unexpected keyword \"%s\" at line %i",keyw2nd->str, m_line);
			error = true;
			return NULL;
		}

		if(defined_keyword[keyw->id].parse_fun != NULL){

			if((aux_p = defined_keyword[keyw->id].parse_fun(s,m_line,sf, ast_node_to_be_evaluated)) != NULL){
				return aux_p;
			}
			error = true;
		}else{
			print_error_cr("%s has no parse function implemented yet!",defined_keyword[keyw->id].str);
		}
    	// something wrong was happen..
	}

	return NULL;
}

char * CAst::generateAST_Recursive(const char *s, int m_line, CScriptFunction *sf, bool & error, PASTNode *node_to_be_evaluated){

	// PRE: **node must be created and is i/o ast pointer variable where to write changes.

	tInfoKeyword *keyw=NULL;
	char *aux = (char *)s;
	char *end_expr;
	*node_to_be_evaluated = new tASTNode;

	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

	while(*aux != 0 ){
		PASTNode child_node = NULL;

		if(*aux == '}'){ // trivial cases...
			return aux;
		}else{
			keyw = isKeyword(aux);
			if(keyw!= NULL){
				if(keyw->id == KEYWORD_TYPE::BREAK_KEYWORD){
					return aux;
				}
			}
		}

		// 1st. check whether parse a keyword...
		if((end_expr = parseKeyWord(aux, m_line, sf, error, &child_node)) == NULL){

			// If was unsuccessful then try to parse expression.
			if(error){
				return NULL;
			}

			// 2nd. check whether parse a block
			if((end_expr = parseBlock(aux,m_line, sf, error, &child_node))==NULL){

				// If was unsuccessful then try to parse expression.
				if(error){
					return NULL;
				}

				// 2nd. try expression
				if((end_expr = parseExpression(aux,m_line, sf, &child_node)) == NULL){ // something wrong was happen.
					return NULL;
				}

				if(*end_expr != ';'){
					error = true;
					print_error_cr("Expected ';' at line %i",m_line);
					return NULL;
				}
				end_expr++;
			}
		}

		(*node_to_be_evaluated)->children.push_back(child_node);
		aux=end_expr;
		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
	}
	return NULL;
}

PASTNode CAst::generateAST(const char *s, CScriptFunction *sf){

	int m_line = 1;
	bool error=false;
	PASTNode node = NULL;

	if(generateAST_Recursive(s, m_line,sf,error,&node) != NULL){
		node->node_type = MAIN_NODE;
		return node;
	}else{
		if(node != NULL){
			delete node;
		}
	}
	return NULL;
}
