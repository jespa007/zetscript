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
	char *aux=(*s);

	if(*aux=='+'){

		if(*(aux+1) == '+') {// inc op
			return false;
		}

		return true;
	}

	return false;
}

bool CAst::parseMinusPunctuator(const char *s){
	char *aux=(*s);

	char *aux=(*s);

	if(*aux=='-'){

		if(*(aux+1) == '-') {// dec op
			return false;
		}

		return true;
	}

	return false;

}

bool CAst::parseMulPunctuator(const char *s){
	char *aux=(*s);

	if(*aux=='*'){
		return true;
	}

	return false;

}

bool CAst::parseDivPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='/'){
		return true;
	}
	return false;
}

bool CAst::parseModPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='%'){
		return true;
	}

	return false;
}

bool CAst::parseAssignPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='='){
		return true;
	}

	return false;
}

bool CAst::parseBinaryXorPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='^'){
		return true;
	}
	return false;
}

bool CAst::parseBinaryAndPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='&'){
		if(*(aux+1) == '&') {// logical op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseBinaryOrPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='|'){
		if(*(aux+1) == '|') {// logical op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseShiftLeftPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='<'){
		if(*(aux+1) == '<') {// shift left op
			return true;
		}
	}
	return false;
}

bool CAst::parseShiftRightPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='>'){
		if(*(aux+1) == '>') {// shift right op
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicAndPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='&'){
		if(*(aux+1) == '&') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicOrPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='|'){
		if(*(aux+1) == '|') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicEqualPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='='){
		if(*(aux+1) == '=') {// logical op ok
			return true;
		}
	}
	return false;
}

bool CAst::parseLogicGreatherThanPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='>'){
		if(*(aux+1) == '=') {// get op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseLogicLessThanPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='<'){
		if(*(aux+1) == '=') {// let op
			return false;
		}
		return true;
	}
	return false;
}

bool CAst::parseLogicGreatherEqualThanPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='>'){
		if(*(aux+1) == '=') {// get op ok!
			return true;
		}
	}
	return false;

}

bool CAst::parseLessEqualThanPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='<'){
		if(*(aux+1) == '=') {// let op ok!
			return false;
		}
		return true;
	}
	return false;

}

bool CAst::parseNotPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='!'){
		if(*(aux+1) == '=') {// logical op
			return false;
		}
		return true;
	}
	return false;

}

bool CAst::parseNotEqualPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='!'){
		if(*(aux+1) == '=') {// logical op
			return true;
		}
	}
	return false;

}

bool CAst::parseIncPunctuator(const char *s){
	char *aux=(*s);

	if(*aux=='+'){
		if(*(aux+1) == '+') {// logical op
			return true;
		}
	}
	return false;
}

bool CAst::parseDecPunctuator(const char *s){
	char *aux=(*s);
	if(*aux=='-'){
		if(*(aux+1) == '-') {// logical op
			return true;
		}
	}
	return false;
}



#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

tInfoPunctuator  * CAst::parsePunctuatorGroup0(const char *s){

	OPERATOR_PUNCTUATOR_TYPE index_to_evaluate[]={
			PLUS_PUNCTUATOR,
			MINUS_PUNCTUATOR,
			BINARY_XOR_PUNCTUATOR,
			BINARY_AND_PUNCTUATOR,
			BINARY_OR_PUNCTUATOR,
			SHIFT_LEFT_PUNCTUATOR,
			SHIFT_RIGHT_PUNCTUATOR,
			ASSIGN_PUNCTUATOR
	};

	for(int i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return NULL;
}

tInfoPunctuator  * CAst::parsePunctuatorGroup1(const char *s){

	OPERATOR_PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_AND_PUNCTUATOR,
			MUL_PUNCTUATOR,
			DIV_PUNCTUATOR,
			MOD_PUNCTUATOR
	};

	for(int i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tInfoPunctuator  * CAst::parsePunctuatorGroup2(const char *s){


	OPERATOR_PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_EQUAL_PUNCTUATOR,
			LOGIC_NOT_EQUAL_PUNCTUATOR,
			LOGIC_LET_PUNCTUATOR,
			LOGIC_GET_PUNCTUATOR,
			LOGIC_GT_PUNCTUATOR,
			LOGIC_LT_PUNCTUATOR
	};

	for(int i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tInfoPunctuator  * CAst::parsePunctuatorGroup3(const char *s){


	OPERATOR_PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_NOT_PUNCTUATOR,
	};

	for(int i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tInfoPunctuator *  CAst::isOperatorPunctuator(const char *s){

	for(int i = 0; i < ARRAY_LENGTH(defined_operator_punctuator); i++){
		if(defined_operator_punctuator[i].parse_fun(s)){
			return &defined_operator_punctuator[i];
		}
	}

	return 0;
}


tInfoPunctuator *  CAst::isSpecialPunctuator(const char *s){

	for(int i = 0; i < ARRAY_LENGTH(defined_special_punctuator); i++){
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

				isSpecialPunctuator(aux)==NULL ||

				(*aux)=='\r'
						) &&
				(isOperatorPunctuator(aux)==NULL)
		) {
			aux++;
		}
	}
	return aux;
}

char *CAst::getSymbolName(const char *s,int & m_startLine){

	char *aux_p=(char *)s;
	tInfoPunctuator *end_punctuator=isPunctuator(s);
	char *Punctuator;

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
	defined_operator_punctuator[UNKNOWN_OPERATOR_PUNCTUATOR]={UNKNOWN_OPERATOR_PUNCTUATOR, "none",NULL};
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

	defined_operator_punctuator[INC_PUNCTUATOR]={INC_PUNCTUATOR, "++",parseIncPunctuator};
	defined_operator_punctuator[DEC_PUNCTUATOR]={DEC_PUNCTUATOR, "--",parseDecPunctuator};

	// special punctuators...
	defined_special_punctuator[UNKNOWN_SPECIAL_PUNCTUATOR]={UNKNOWN_SPECIAL_PUNCTUATOR, "",NULL};
	defined_special_punctuator[COMA_PUNCTUATOR]={COMA_PUNCTUATOR, ",",NULL};
	defined_special_punctuator[SEMICOLON_PUNCTUATOR]={SEMICOLON_PUNCTUATOR, ";",NULL};
	defined_special_punctuator[OPEN_PARENTHESIS_PUNCTUATOR]={OPEN_PARENTHESIS_PUNCTUATOR, "(",NULL};
	defined_special_punctuator[CLOSE_PARENTHESIS_PUNCTUATOR]={CLOSE_PARENTHESIS_PUNCTUATOR, ")",NULL};
	defined_special_punctuator[OPEN_BRAKET_PUNCTUATOR]={OPEN_BRAKET_PUNCTUATOR, "{",NULL};
	defined_special_punctuator[CLOSE_BRAKET_PUNCTUATOR]={CLOSE_BRAKET_PUNCTUATOR, "}",NULL};
	defined_special_punctuator[OPEN_SQUARE_BRAKET_PUNCTUATOR]={OPEN_SQUARE_BRAKET_PUNCTUATOR, "[",NULL};
	defined_special_punctuator[CLOSE_SQUARE_BRAKET_PUNCTUATOR]={CLOSE_SQUARE_BRAKET_PUNCTUATOR, "]",NULL};
	defined_special_punctuator[FIELD_PUNCTUATOR]={FIELD_PUNCTUATOR, "]",NULL};


	// init special punctuators...


	// init keywords...
	defined_keyword[KEYWORD_TYPE::NONE_KEYWORD] = {UNKNOWN_KEYWORD, "none",NULL};
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
	defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",NULL};
	defined_keyword[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this",NULL};

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
string GET_STR_WITHOUT_SPACES(const string & s){
	string f="";

	for(unsigned i =0; i < s.size(); i++){
		if(s[i]!=' ')
			f+=s[i];
	}

	return f;
}

//------------------------------------------------------------------------------------------------------------
/*inline char * GET_CLOSED_PARENTHESIS(const char *s){
	//bool end = false;
	char *aux = (char *)s;
	
	if(*aux != '('){
		print_error_cr("Error first character must be a parenthesis!" );
		return NULL;
	}
	int num_levels=1; // because first is a parenthesis;
	do{
		aux++;
		if(*aux==')'){
			num_levels--;
		}else if(*aux=='('){
			num_levels++;
		}
	}while((*aux) != 0 && num_levels>0);
		
	return aux;
}*/


tInfoPunctuator *CAst::checkPreOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return defined_operator_punctuator[INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return defined_operator_punctuator[DEC_PUNCTUATOR];
	if(parsePlusPunctuator(s)) 	return defined_operator_punctuator[PLUS_PUNCTUATOR];
	if(parseMinusPunctuator(s)) return defined_operator_punctuator[MINUS_PUNCTUATOR];
	if(parseNotPunctuator(s))   return defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR];

	return 0;
}

tInfoPunctuator *CAst::checkPostOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return defined_operator_punctuator[INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return defined_operator_punctuator[DEC_PUNCTUATOR];

	return 0;
}

/*char *postoperator_Punctuator( char *c, int & m_line){
	char *aux=CStringUtils::IGNORE_BLANKS(c,m_line);

	// detection ++ operator.
	if(*aux=='+' && *(aux+1)=='+'){
		aux+=2;
		return aux;
	}

	// detection -- operator.
	if(*aux=='-' && *(aux+1)=='-'){
		aux+=2;
		return aux;
	}

	return 0;
}*/
/*
PASTNode CAst::preOperator(OPERATOR_PUNCTUATOR_TYPE operator_id,PASTNode affected_op){ // can be -,+,! etc...
	PASTNode op=new tASTNode(2);
	op->operator_id = operator_id;
	op->children[LEFT_NODE]=affected_op;
	return op;
}

PASTNode CAst::postOperator(string Punctuator,PASTNode affected_op){ // can be -,+,! etc...
	string Punctuator1 = "post_"+Punctuator;

	return preOperator(Punctuator1,affected_op);
}*/
//------------------------------------------------------------------------------------------------------------
char * CAst::parseExpression_Recursive(const char *s, int m_line, bool & error, CScriptFunction *sf, PASTNode *op, GROUP_TYPE type_group,PASTNode parent ){
	//int index=0;
	char *aux=(char *)s;
	char *s_effective_start=(char *)s;
	char *start_expression=NULL,*end_expression=(char *)s ; // by default end expression isequal to
	string pre_Punctuator="";
	string post_Punctuator="";
	PASTNode symbol_node=NULL; // can be a function or array.
	PASTNode args_node=NULL;
	string symbol_name;
	bool eval_preoperator=true;
	bool eval_postoperator=true;
	string operator_str="";
	tInfoPunctuator *pre_operator,*post_operator;

	*op=NULL; // always preallocate 2 nodes (left and right)


	int m_effective_start_line=m_line;
	int m_define_symbol_line;


	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
	
	if(*aux==0){
		return NULL;
	}

	if(type_group>=MAX_GROUPS) {
		print_error_cr("max groups");
		error = true;
		return NULL;
	}

	print_ast_cr("new expression eval:\"%.30s ...\" group:%i at line %i",aux,type_group, m_line);
	
	//if(type_group==GROUP_0) 
	 // search for operator +/-...
		start_expression=aux;
		bool theres_some_Punctuator=false;
		char * find_operator_group = 0;
		char *expr_op=0;
		
		// searching for operator!
		print_ast_cr("searching for operator type %i...",type_group);

	
		while(*aux!=0 && *aux!=';' && *aux!=','  && *aux!=')'  && *aux!=']' && (find_operator_group==0)){
	
			print_ast_cr("checkpoint1:%c\n",*aux);
			// 1. ignore spaces...
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			
			/*if(isOperatorPunctuator(aux)!=0){

				if((end_expression=preoperatorPunctuator(aux, m_line))!=0){
						if(eval_preoperator || (!eval_preoperator&&(eval_postoperator))){ // first Punctuator! Save the operator to apply on the result expression.
							char defined_keyword[10]={0};

							if(eval_preoperator){ // It can't do preoperator and postoperator at the same time.
								eval_postoperator=false;
							}

							strncpy(defined_keyword,aux,end_expression-aux);

							print_ast_cr("preoperator %s!",defined_keyword);
							pre_Punctuator = defined_keyword;
							aux=end_expression; // ignore first Punctuator...
							s_effective_start=end_expression;
							m_effective_start_line = m_line;
							start_expression=aux;
						}else{ // just ignore it
							aux=end_expression; // ignore first Punctuator...
						}
				}
				else{
					print_error_cr("unexpected Punctuator %c at line %i",*aux,m_line);
					error = true;
					return NULL;
				}
			}*/
			
			//print_ast_cr("checkpoint2:%c",*aux);
			
			//start_value=aux;
		/*	if(*aux=='('){ // exp within ()
				print_ast_cr("try find parenthesis close");
				end_expression = GET_CLOSED_PARENTHESIS(aux);
				
				if(end_expression==0){
					error = true;
					print_error_cr("Parenthesis not closed at line %i",m_line);
					return NULL;
				}
				
				print_ast_cr("checkpoint4:%c",*end_expression);
				
				if(*end_expression==0){
						print_error_cr("')' not found");
						error = true;
						return NULL;
				}else{ // advance )
					end_expression++;
				}				
			}
			else{*/ // value

			if((pre_operator=checkPreOperatorPunctuator(aux))!=NULL){
				aux+=strlen(pre_operator->str);
				aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			}


			if(*aux=='('){
				aux = parseExpression_Recursive(s, m_line, error, sf, op, type_group,parent);

				if(*aux != ')'){
					print_error_cr("Not closed parenthesis at line %i", m_line);
					return NULL;
				}

			}else{

				tInfoKeyword *key_w;
				m_define_symbol_line = m_line;

				// usually we have to take care about special op symbols
				if((key_w =isKeyword(aux)) != NULL){

					if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){ // function object ...

						end_expression = parseFunction(aux,m_define_symbol_line,sf,&symbol_node);
						if(end_expression == NULL){
							return NULL;
						}

						symbol_node->node_type = NODE_TYPE::KEYWORD_NODE;
						symbol_node->keyword_type = KEYWORD_TYPE::FUNCTION_KEYWORD;

					}else{
						print_error_cr("Unexpected keyword %s at line %i",key_w->str,m_define_symbol_line);
						return NULL;
					}

				}else{

					if(*aux == '['){ // vector object ...
						end_expression = parseArgs(aux,m_define_symbol_line,sf,&symbol_node, '[', ']');

						if(end_expression == NULL){
							error = true;
							return NULL;
						}

						symbol_node->node_type = NODE_TYPE::ARRAY_NODE;

					}
					else{
						char *gg;
						end_expression = getEndWord(aux);

						 gg=CStringUtils::copyStringFromInterval(aux,end_expression);

						 if(gg != NULL){
						 	 symbol_name = gg;
						 }
						 else return NULL;


						 end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);
						 if((post_operator = checkPreOperatorPunctuator(end_expression)) != NULL){
							 end_expression+=strlen(post_operator->str);
						 }


					}
				}

				// if there's function or array access after symbol or object created ...
				end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);

				if(*end_expression == '('){ // parse args within '(' ')'...

					end_expression = parseArgs(end_expression,m_define_symbol_line,sf,&args_node, '(', ')');

					if(end_expression == NULL){
						error = true;
						return NULL;
					}

					args_node->node_type = NODE_TYPE::FUNCTION_ARGS_DECL_NODE;

					end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);

				}
				else if(*end_expression == '['){ // parse args within '(' ')'...


					end_expression = parseArgs(end_expression,m_define_symbol_line,sf,&args_node, '[', ']');

					if(end_expression == NULL){
						error = true;
						return NULL;
					}

					args_node->node_type = NODE_TYPE::ARRAY_INDEX_NODE;

					end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);

				}

			}
			
			aux=end_expression;
			
			print_ast_cr("checkpoint3:%c\n",*aux);
			
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			
			// eval post operators...
			/*if(*aux != 0  && *aux!=';' && *aux!=','  && *aux!=')'  && *aux!=']' && eval_postoperator){
				char *aux2;
				if((aux2=postoperator_Punctuator(aux, m_line))!=0){
					char defined_keyword[10]={0};
					strncpy(defined_keyword,aux,aux2-aux);
					print_ast_cr("postperator %s!",defined_keyword);
					post_Punctuator = defined_keyword;

					aux=aux2;
				}
				aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
			}*/

			if(*aux!=0 && *aux!=';'  && *aux!=','  && *aux!=')'  && *aux!=']' ){ // is not end expression

				expr_op=aux;
				tInfoPunctuator *adv_op;
				if((adv_op=isOperatorPunctuator(expr_op))!=0){
					theres_some_Punctuator|=true;
					
					switch(type_group){
					case  GROUP_0:	find_operator_group = parsePunctuatorGroup0(expr_op);break;
					case GROUP_1:	find_operator_group = parsePunctuatorGroup1(expr_op);break;
					case GROUP_2:	find_operator_group = parsePunctuatorGroup2(expr_op);break;
					case GROUP_3:	find_operator_group = parsePunctuatorGroup3(expr_op);break;
					default: break;
					}

					if(find_operator_group != NULL){
						char *gg=CStringUtils::copyStringFromInterval();
						if(gg==NULL){
							return NULL;
						}
						operator_str
					}

					aux=adv_op; // advance operator...
				}
			}

			/*if(!eval_preoperator){
				eval_postoperator=false;
			}

			eval_preoperator = false; // set it false due is not first.*/
		}
		
		if(find_operator_group==0) {// there's no any operators \"type_group\"...
			if(!theres_some_Punctuator){ // only we have a value (trivial)

				  if( *start_expression!='('){
					  char subexpr[MAX_EXPRESSION_LENGTH]={0}; // I hope this is enough...
					  strncpy(subexpr,start_expression,end_expression-start_expression); // copy sub expression

					print_ast_cr("trivial value %s at line %i",subexpr, m_define_symbol_line);

					if(symbol_node != NULL){
						*op = symbol_node;
					}else{
						(*op)=new tASTNode;
						(*op)->value=symbol_name; // assign its value ...

					}


					(*op)->parent=parent;
					(*op)->definedValueline=m_define_symbol_line;

					if(args_node != NULL){ // put args whether the symbol or object are function/array.
						(*op)->children.push_back(args_node);
					}

					if(pre_Punctuator!=""){ // generate a prenode operator..
						 if(*subexpr == 0){
							 print_error_cr("expected symbol before %s at line %i",pre_Punctuator.c_str(),m_line);
							 error = true;
							 return NULL;
						 }

						 (*op)=preOperator(pre_Punctuator,*op);
					}
					else if(post_Punctuator!=""){ // generate a post node operator..

						 if(*subexpr == 0){
							 print_error_cr("expected symbol after %s at line %i",pre_Punctuator.c_str(),m_line);
							 error = true;
							 return NULL;
						 }


						 (*op)=postOperator(post_Punctuator,*op);
					}


				  }else{ // parenthesis!

					print_ast_cr("START:%c",*start_expression);
					string subexpr = CStringUtils::copyStringFromInterval(start_expression+1,end_expression-1);
					print_ast_cr("expr:%s\n",subexpr.c_str());

					PASTNode p_gr=NULL;

					parseExpression_Recursive(subexpr.c_str(),m_line,error,sf,&p_gr, GROUP_0,(*op));
					if(error){
						return NULL;
					}


					if(pre_Punctuator!=""){
						p_gr = preOperator(pre_Punctuator,p_gr);
					}

					// set node ...
					*op = p_gr;


				}
			}
			else{

				if(end_expression!= NULL){
				// there's a Punctuator, so let's perform generate its AST
					// reset prePunctuator...
					pre_Punctuator="";
					print_ast_cr("try to generate group1 expression: %s\n",s_effective_start);
					return parseExpression_Recursive(s,m_effective_start_line,error,sf,op,(GROUP_TYPE)(((int)type_group)+1),parent);
				}

			}
		}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...

			*op=new tASTNode(2); // always preallocate 2 nodes (left and right)
			(*op)->parent=parent;

			char operator_str[10]={0};
			strncpy(operator_str,expr_op,expr_op_end-expr_op);

			print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_str);
			char eval_left[MAX_EXPRESSION_LENGTH]={0};

			strncpy(eval_left,s_effective_start,expr_op-s_effective_start); // copy its left side...
			parseExpression_Recursive(eval_left,m_effective_start_line,error,sf,&(*op)->children[LEFT_NODE],type_group,*op);

			if(error){
				return NULL;
			}

			if(pre_Punctuator!=""){
				(*op)->children[LEFT_NODE]=preOperator(pre_Punctuator,(*op)->children[LEFT_NODE]);
			}



			aux = parseExpression_Recursive(expr_op_end,m_line,error,sf,&(*op)->children[RIGHT_NODE],type_group,(*op));
			if(error){
				return NULL;
			}

			if(!strcmp(operator_str,"-")) {
				(*op)->children[RIGHT_NODE]=preOperator("-",(*op)->children[RIGHT_NODE]);
				strcpy(operator_str,"+");
			}
			
			if((*op)->children[RIGHT_NODE] == NULL && (*op)->children[LEFT_NODE] == NULL){ // some wrong was happened
				return NULL;
			}
			
			(*op)->Punctuator = operator_str;

		}
		

	return aux;
}

char * CAst::parseExpression(const char *s, int m_line, bool & error, CScriptFunction *sf , PASTNode * ast_node_to_be_evaluated ){

	// PRE: s is current string to parse. This function tries to parse an expression like i+1; and generates binary ast.
	// If this functions finds ';' then the function will generate ast.

	// last character is in charge of who is calling parseExpression because there's many ending cases ): [ ';' ',' ')' , ']' ]
	return parseExpression_Recursive(s,m_line,error,sf,ast_node_to_be_evaluated);

}

//---------------------------------------------------------------------------------------------------------------
// PARSE KEYWORDS


char * CAst::parseArgs(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated, char c1,char c2){
	char *aux_p = (char *)s;
	int m_startLine = m_line;
	bool error = false;
	PASTNode   node_arg_expression;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...

	if(*aux_p == c1){
		aux_p++;

		(*ast_node_to_be_evaluated) = new tASTNode;


		do{
			aux_p = parseExpression(aux_p,m_startLine,error,sf,&node_arg_expression);
			if(aux_p == NULL || error == true){
				return NULL;
			}
			else{
				if(*aux_p == ',' ){
					aux_p++;
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
				}else{
					if(*aux_p != c2 ){
						print_error_cr("Expected %c",c2);
						return NULL;
					}
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
	bool error = false;
	CScope::tInfoRegisteredVar * irv=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	// check for keyword ...
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){
		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = FUNCTION_KEYWORD;


		(*ast_node_to_be_evaluated)->children.push_back(args_node = new tASTNode);
		(*ast_node_to_be_evaluated)->children.push_back(body_node = new tASTNode);



		args_node->node_type = NODE_TYPE::FUNCTION_ARGS_DECL_NODE;
		body_node->node_type = BODY_NODE;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		(*ast_node_to_be_evaluated)->definedValueline = m_startLine;

		if(*aux_p!='('){ // is named function..

			// check whwther the function is anonymous or not.
			end_var=GET_SYMBOL_NAME(aux_p,m_startLine);

			if(end_var != NULL){


				if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;


				// check whether parameter name's matches with some global variable...
				if((irv=sf->getScope()->getCurrentScope()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
					print_error_cr("Function name \"%s\" defined at line %i is ambiguos with symbol defined at %i", value_symbol, m_startLine,irv->m_line);
					return NULL;
				}

				(*ast_node_to_be_evaluated)->value = value_symbol;

				// define value symbol...
				sf->getScope()->registerSymbol((*ast_node_to_be_evaluated)->value,m_startLine);
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

				end_var=GET_SYMBOL_NAME(aux_p,m_startLine);

				if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
					return NULL;

				// check if repeats...
				for(unsigned k = 0; k < args_node->children.size(); k++){
					if(args_node->children[k]->value == value_symbol){
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
				arg_node->value=value_symbol;
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
	bool error=false;
	PASTNode expression_node=NULL; // in case there's = expression.

	string s_aux;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...

		aux_p += strlen(key_w->str);

		aux_p = parseExpression(aux_p, m_startLine, error, sf, &expression_node);

		if(aux_p != NULL){

			if(*aux_p!=';'){
				print_error_cr("Expected ';' at line %i", m_startLine);
				return NULL;
			}

			aux_p++;

			*ast_node_to_be_evaluated = new tASTNode;
			(*ast_node_to_be_evaluated)->node_type = NODE_TYPE::KEYWORD_NODE;
			(*ast_node_to_be_evaluated)->keyword_type = KEYWORD_TYPE::RETURN_KEYWORD;

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
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::WHILE_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = WHILE_KEYWORD;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,')',m_startLine)) != NULL){

				if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
					return NULL;
				}

				conditional_str=start_symbol;

				parseExpression((const char *)conditional_str.c_str(),m_startLine,error,sf,&conditional);

				if(error){
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
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::IF_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = IF_KEYWORD;

		aux_p += strlen(key_w->str);

		// evaluate conditional line ...

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);
		if(*aux_p == '('){
			if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p+1,')',m_startLine)) != NULL){

				if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
					return NULL;
				}

				conditional_str=start_symbol;

				parseExpression((const char *)conditional_str.c_str(),m_startLine,error,sf,&conditional);

				if(error){
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
						if((key_w = is_keyword(aux_p)) != NULL){
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
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::FOR_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = FOR_KEYWORD;

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
						parseExpression((const char *)aux_p2,m_startLine,error,sf,&for_node);

						if(error){
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
	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::SWITCH_KEYWORD){

		*ast_node_to_be_evaluated = new tASTNode;
		(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
		(*ast_node_to_be_evaluated)->keyword_type = SWITCH_KEYWORD;

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		if(*aux_p == '('){
			//if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p,')',m_startLine)) != NULL){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_startLine);

				end_expr = GET_END_WORD(aux_p);

				value_to_eval = CStringUtils::copyStringFromInterval(aux_p, end_expr);

				if(!value_to_eval) {return NULL;}

				//--- create node
				(*ast_node_to_be_evaluated)->value = value_to_eval;

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

							while((key_w = is_keyword(aux_p)) != NULL){ // acumulative cases /defaults...

								switch(key_w->id){
								case DEFAULT_KEYWORD:
								case CASE_KEYWORD:

									val = "default";
									aux_p += strlen(key_w->str);
									aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

									if(key_w->id == CASE_KEYWORD){


										// get the symbol...

										start_symbol=aux_p;
										end_symbol = GET_END_WORD(start_symbol);
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
												if((*ast_node_to_be_evaluated)->children[j]->children[h]->value == val){
													print_error_cr("Symbol %s repeteaded in switch at line %i",val.c_str(),m_startLine);
													return NULL;
												}
											}
										}

										case_value_node = new tASTNode;
										case_value_node->node_type = KEYWORD_NODE;
										case_value_node->keyword_type = CASE_KEYWORD;
										case_value_node->value = val;
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

							key_w2 = is_keyword(aux_p);
							if(key_w2->id == BREAK_KEYWORD){
								aux_p += strlen(key_w2->str);
								CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

								if(*aux_p == ';'){ // the new scope ...
									case_default_keyword_node->node_type = KEYWORD_NODE;
									//case_default_keyword_node->keyword_type = key_w->id; // think about
									case_default_keyword_node->value=value_to_eval;

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
	bool error=false;
	PASTNode expression_node=NULL; // in case there's = expression.

	string s_aux;
	int m_startLine = m_line;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

	key_w = is_keyword(aux_p);

	if(key_w->id == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

		aux_p += strlen(key_w->str);

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_startLine);

		start_var=aux_p;


		end_var=GET_SYMBOL_NAME(aux_p,m_startLine);

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

					end_var = parseExpression(aux_p+1,m_startLine,error,sf,&expression_node);

					if(error){
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
				(*ast_node_to_be_evaluated)->keyword_type = VAR_KEYWORD;


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
	keyw = is_keyword(aux_p);

	if(keyw != NULL){ // a keyword was detected...

		aux_p+=strlen(keyw->str);
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

		// check if another kwyword is put ...
		if((keyw2nd = is_keyword(aux_p))!= NULL){
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
			keyw = is_keyword(aux);
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
				if((end_expr = parseExpression(aux,m_line,error, sf, &child_node)) == NULL){ // something wrong was happen.
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



