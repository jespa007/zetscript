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
	if(*s=='+')
		return (*(s+1) != '+');
	return false;
}

bool CAst::parseMinusPunctuator(const char *s){
	if(*s=='-')
		return (*(s+1) != '-');
	return false;
}

bool CAst::parseMulPunctuator(const char *s){
	return *s == '*';
}

bool CAst::parseDivPunctuator(const char *s){
	return *s == '/';
}

bool CAst::parseModPunctuator(const char *s){
	return *s == '%';
}

bool CAst::parseFieldPunctuator(const char *s){
	return *s == '.';
}

bool CAst::parseInlineIfPunctuator(const char *s){
	return *s == '?';
}

bool CAst::parseInlineElsePunctuator(const char *s){
	return *s == ':';
}

bool CAst::parseAssignPunctuator(const char *s){
	if(*s=='=')
		return (*(s+1) != '=');
	return false;
}

bool CAst::parseBinaryXorPunctuator(const char *s){
	return *s == '^';
}

bool CAst::parseBinaryAndPunctuator(const char *s){
	if(*s=='&')
		return (*(s+1) != '&');
	return false;
}

bool CAst::parseBinaryOrPunctuator(const char *s){
	if(*s=='|')
		return (*(s+1) != '|');
	return false;
}

bool CAst::parseShiftLeftPunctuator(const char *s){
	if(*s=='<')
		return (*(s+1) == '<');
	return false;
}

bool CAst::parseShiftRightPunctuator(const char *s){
	if(*s=='>')
		return (*(s+1) == '>');
	return false;
}

bool CAst::parseLogicAndPunctuator(const char *s){
	if(*s=='&')
		return (*(s+1) == '&');
	return false;
}

bool CAst::parseLogicOrPunctuator(const char *s){
	if(*s=='|')
		return (*(s+1) == '|');
	return false;
}

bool CAst::parseLogicEqualPunctuator(const char *s){
	if(*s=='=')
		return (*(s+1) == '=');
	return false;
}

bool CAst::parseLogicNotEqualPunctuator(const char *s){
	if(*s=='!')
		return (*(s+1) == '=');
	return false;
}

bool CAst::parseLogicGreatherThanPunctuator(const char *s){
	return *s == '>';
}

bool CAst::parseLogicLessThanPunctuator(const char *s){
	return *s == '<';
}

bool CAst::parseLogicGreatherEqualThanPunctuator(const char *s){
	if(*s=='>')
		return (*(s+1) == '=');
	return false;
}

bool CAst::parseLessEqualThanPunctuator(const char *s){
	if(*s=='<')
		return (*(s+1) == '=');
	return false;

}

bool CAst::parseNotPunctuator(const char *s){
	if(*s=='!')
		return (*(s+1) != '=');
	return false;
}


bool CAst::parseIncPunctuator(const char *s){
	if(*s=='+')
		return (*(s+1) == '+');
	return false;
}

bool CAst::parseDecPunctuator(const char *s){
	if(*s=='-')
		return (*(s+1) == '-');
	return false;
}


#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

tInfoPunctuator  * CAst::parsePunctuatorGroup0(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			ADD_PUNCTUATOR,
			SUB_PUNCTUATOR,
			BINARY_XOR_PUNCTUATOR,
			BINARY_AND_PUNCTUATOR,
			BINARY_OR_PUNCTUATOR,
			SHIFT_LEFT_PUNCTUATOR,
			SHIFT_RIGHT_PUNCTUATOR,
			ASSIGN_PUNCTUATOR,
			INLINE_IF_PUNCTUATOR,
			INLINE_ELSE_PUNCTUATOR

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
			MOD_PUNCTUATOR,
			FIELD_PUNCTUATOR
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
			LOGIC_LTE_PUNCTUATOR,
			LOGIC_GTE_PUNCTUATOR,
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
char * CAst::getEndWord(const char *s, int m_line){

	char *aux=(char *)s;
	 tInfoPunctuator * sp;
	 tInfoKeyword *key_w;

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

		if((key_w = isKeyword(s))!= NULL){
			if(key_w->id != KEYWORD_TYPE::THIS_KEYWORD){ // unexpected token ?
				print_error_cr("Unexpected keyword \"%s\" at line %i",key_w->str, m_line);
				return NULL;
			}
		}

		while((*aux)!=0 && !(
				(*aux)==' ' ||
				(*aux)=='\t' ||
				(*aux)=='\n' ||
				(*aux)=='\r'
						) &&
				(isOperatorPunctuator(aux)==NULL)

		) {
			// check for special punctuator ( the field '.' separator is processed within the word )
			if((sp = isSpecialPunctuator(aux))!=NULL){
				if(sp->id != FIELD_PUNCTUATOR)
					return aux;
			}
			aux++;
		}
	}
	return aux;
}

char *CAst::getSymbolName(const char *s,int & m_line){

	char *aux_p=(char *)s;
	tInfoPunctuator *end_punctuator=isPunctuator(s);


	if(end_punctuator != NULL){
		print_error_cr("Unexpected '%s' at line",end_punctuator->str,m_line);
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
		print_error_cr(" line %i: Symbol name cannot begin with number %c",m_line, *aux_p);
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

	defined_operator_punctuator[ADD_PUNCTUATOR]={ADD_PUNCTUATOR, "+",parsePlusPunctuator};
	defined_operator_punctuator[SUB_PUNCTUATOR]={SUB_PUNCTUATOR, "-",parseMinusPunctuator};
	defined_operator_punctuator[MUL_PUNCTUATOR]={MUL_PUNCTUATOR, "*",parseMulPunctuator};
	defined_operator_punctuator[DIV_PUNCTUATOR]={DIV_PUNCTUATOR, "/",parseDivPunctuator};
	defined_operator_punctuator[MOD_PUNCTUATOR]={MOD_PUNCTUATOR, "%",parseModPunctuator};

	defined_operator_punctuator[FIELD_PUNCTUATOR]={FIELD_PUNCTUATOR, ".",parseFieldPunctuator};
	defined_operator_punctuator[INLINE_IF_PUNCTUATOR]={INLINE_IF_PUNCTUATOR, "?",parseInlineIfPunctuator};
	defined_operator_punctuator[INLINE_ELSE_PUNCTUATOR]={INLINE_ELSE_PUNCTUATOR, ":",parseInlineElsePunctuator};

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
	defined_operator_punctuator[LOGIC_GTE_PUNCTUATOR]={LOGIC_GTE_PUNCTUATOR, ">=",parseLogicGreatherEqualThanPunctuator};
	defined_operator_punctuator[LOGIC_LTE_PUNCTUATOR]={LOGIC_LTE_PUNCTUATOR, "<=",parseLessEqualThanPunctuator};
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
	defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",parseClass};
	defined_keyword[KEYWORD_TYPE::NEW_KEYWORD] = {NEW_KEYWORD,"new", NULL};
	defined_keyword[KEYWORD_TYPE::DELETE_KEYWORD] = {DELETE_KEYWORD,"delete",NULL};

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
	if(parsePlusPunctuator(s)) 	return &defined_operator_punctuator[ADD_PUNCTUATOR];
	if(parseMinusPunctuator(s)) return &defined_operator_punctuator[SUB_PUNCTUATOR];
	if(parseNotPunctuator(s))   return &defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR];

	return 0;
}

tInfoPunctuator *CAst::checkPostOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return &defined_operator_punctuator[POST_INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return &defined_operator_punctuator[POST_DEC_PUNCTUATOR];

	return 0;
}

PASTNode CAst::preNodePunctuator(tInfoPunctuator * operator_info,PASTNode affected_op){ // can be -,+,! etc...
	PASTNode op=new tASTNode;
	op->operator_info = operator_info;
	op->definedValueline = affected_op->definedValueline;
	op->node_type =PUNCTUATOR_NODE;
	op->children.push_back(affected_op);
	affected_op->parent = op;
	return op;
}



char * CAst::parseNonTrivialSymbol(const char *str, int & m_line, CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated, PASTNode parent){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux = (char *)str;
	char *end_expression;
	int m_startLine = m_line;
	tInfoKeyword *key_w  = NULL;
	vector<PASTNode> vector_args_node;


	string symbol_value="";

	if(ast_node_to_be_evaluated != NULL){
		*ast_node_to_be_evaluated = NULL;
	}

	aux = CStringUtils::IGNORE_BLANKS(aux, m_startLine);

	key_w = isKeyword(aux);

	if(key_w != NULL){ // can be new,delete or function...

		aux = CStringUtils::IGNORE_BLANKS(aux+strlen(key_w->str), m_startLine);

		switch(key_w->id){
		case KEYWORD_TYPE::FUNCTION_KEYWORD:
			if((aux=parseFunction(str,m_startLine,sf,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL)) == NULL){
				return NULL;
			}

			break;
		case KEYWORD_TYPE::NEW_KEYWORD:
			if((aux = parseNew(str,m_startLine,sf,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL)) == NULL){
				return NULL;
			}
			break;
		case KEYWORD_TYPE::DELETE_KEYWORD:
			if((aux=parseDelete(str,m_startLine,sf,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL)) == NULL){
				return NULL;
			}
			break;
		case KEYWORD_TYPE::THIS_KEYWORD:
			return parseExpression_Recursive(str,  m_line, sf, ast_node_to_be_evaluated,GROUP_TYPE::GROUP_0,parent);
			break;
		default:
			print_error_cr("\"%s\" cannot be evaluated in expression",key_w->str);
			break;

		}


	}else if(*aux == '[') { // is an array object ...

		if((aux = parseArgs('[', ']',aux,m_startLine,sf,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL))== NULL){
			return NULL;
		}

		aux = CStringUtils::IGNORE_BLANKS(aux, m_startLine);

		if(ast_node_to_be_evaluated != NULL){
			(*ast_node_to_be_evaluated)->node_type = ARRAY_OBJECT_NODE;
		}


	}else {
		char *word_str=NULL;
		// try to get function/array object ...
		end_expression = getEndWord(aux, m_startLine);
		bool function_or_array = false;


		 if(!(end_expression == NULL || end_expression == aux)){ // word ok.

			 symbol_value = CStringUtils::copyStringFromInterval(aux,end_expression);
			 word_str = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);
			 function_or_array = (*word_str == '(' || *word_str == '[');
		 }

		 if(function_or_array) {// try expression...

			 aux = word_str; // we will evaluate later ...

			 if(ast_node_to_be_evaluated != NULL) {// save node
				 *ast_node_to_be_evaluated = new tASTNode();
				 (*ast_node_to_be_evaluated)->value_symbol = symbol_value; // is array or function ...
				 (*ast_node_to_be_evaluated)->node_type  = ARRAY_OBJECT_NODE;
			 }

			 if(*word_str == '('){
				 if(ast_node_to_be_evaluated != NULL){
					 (*ast_node_to_be_evaluated)->node_type  = FUNCTION_OBJECT_NODE;
				 }
			 }

		 }
		 else{

			 // try function or array access evaluate expression...
			return parseExpression_Recursive(aux,  m_line, sf, ast_node_to_be_evaluated,GROUP_TYPE::GROUP_0,parent);

		}
	}

	if(ast_node_to_be_evaluated != NULL){
		aux = CStringUtils::IGNORE_BLANKS(aux, m_startLine);

		if(*aux == '(' ){

			vector_args_node.push_back(NULL);

			if((*ast_node_to_be_evaluated)->node_type != FUNCTION_OBJECT_NODE){
				print_error_cr("Expected function object before '(' at line %i",m_startLine);
				return NULL;
			}

			if((aux=parseArgs('(', ')',aux,m_startLine,sf,&vector_args_node[0])) == NULL){
				return NULL;
			}

			vector_args_node[0]->node_type = NODE_TYPE::FUNCTION_OR_CLASS_ARGS_CALL_NODE;


		}else if (*aux == '['){

			int i = 0;
			bool end = false;
			PASTNode args_node=NULL;


			if((*ast_node_to_be_evaluated)->node_type != ARRAY_OBJECT_NODE){
				print_error_cr("Expected array object before '[' at line %i",m_startLine);
				return NULL;
			}

			do{
				args_node=NULL;
				char *aux_ptr;


				if((aux_ptr=parseArgs('[', ']',aux,m_startLine,sf,&args_node)) != NULL){
					vector_args_node.push_back(args_node);
					args_node->node_type = NODE_TYPE::FUNCTION_OR_CLASS_ARGS_CALL_NODE;
					aux =  CStringUtils::IGNORE_BLANKS(aux_ptr,m_startLine);
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

		// the deepth node is the object and the parent will the access node (function or array) ...
		if(vector_args_node.size() > 0){
			PASTNode obj =  *ast_node_to_be_evaluated;
			PASTNode calling_object = new tASTNode();
			tInfoPunctuator *ip=NULL;

			calling_object->node_type = CALLING_OBJECT_NODE;

			calling_object->children.push_back(obj); // the object itself...

			for(unsigned i=0; i < vector_args_node.size(); i++){
				calling_object->children.push_back(vector_args_node[i]); // the args ...
			}

			calling_object->parent=parent;

			// finally save ast node...

			*ast_node_to_be_evaluated = calling_object;

			if((ip=checkPostOperatorPunctuator(aux)) != NULL){

				if(obj->node_type != ARRAY_OBJECT_NODE){
					print_error_cr("line:%i, function doesn't allow post operators ",m_startLine);
					return NULL;
				}

				aux+=strlen(ip->str);

				if( parent == NULL){

					*ast_node_to_be_evaluated = preNodePunctuator(ip,*ast_node_to_be_evaluated);
				}
			}
		}

	}

	if(ast_node_to_be_evaluated != NULL) { // save line ...
		m_line = m_startLine;
	}

	return aux;

}


bool CAst::printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line){
	tInfoPunctuator *ip=CAst::isPunctuator(current_string_ptr);
	tInfoKeyword *kw=CAst::isKeyword(current_string_ptr);

	if(kw){
							print_error_cr("Unexpected %s at line %i",kw->str,m_line);
							return true;
						}
						else if(ip){
							print_error_cr("Unexpected %s at line %i",ip->str,m_line);
							return true;
						}
	return false;
}

char *CAst::getSymbolValue(
		const char *current_string_ptr,
		int & m_line,
		CScriptFunction *sf,

		string & symbol_name,
		int & m_definedSymbolLine,
		tInfoPunctuator *pre_operator,
		tInfoPunctuator **post_operator,
		bool & is_symbol_trivial
		){

	char *aux = (char *)current_string_ptr;
	char *end_expression = aux;
	is_symbol_trivial = false;
	m_definedSymbolLine = m_line;


	//----------------------------------------------------------
	// GETTING TRIVIAL SYMBOLS
	if(*aux=='('){ // packed symbol...

		if(pre_operator != NULL){
			if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
					pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
				print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_line);
				return NULL;
			}
		}

		// update '('
		aux=aux+1;


		end_expression = parseExpression_Recursive(aux, m_line, sf);//, ast_node_to_be_evaluated, type_group,parent);

		if(*end_expression != ')'){
			print_error_cr("Not closed parenthesis starting at line %i", m_definedSymbolLine);
			return NULL;
		}

		 if(end_expression == NULL || end_expression == aux){
			 print_error_cr("Expected expression at line %i",m_line);
			 return NULL;
		 }

		 symbol_name=CStringUtils::copyStringFromInterval(aux,end_expression);

		end_expression=end_expression+1;

		//end_expression = aux+1;

	}else{ // check for symbols (must have a symbol at least)

		tInfoKeyword *key_w;
		bool is_function=false;
		bool is_new=false;
		bool is_delete=false;



		// usually we have to take care about special op symbols

		if((key_w =isKeyword(aux)) != NULL){
			is_function = key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD;
			is_new=key_w->id == KEYWORD_TYPE::NEW_KEYWORD;
			is_delete=key_w->id == KEYWORD_TYPE::DELETE_KEYWORD;
		}

		if(is_function){ // function object ...

			if(pre_operator!=NULL){
				if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
						pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
					print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_line);
					return NULL;
				}
			}

			// parse function but do not create ast node (we will create in trivial case value
			end_expression = parseFunction(aux,m_line,sf);

			//key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD
			//symbol_node->value_symbol="anonymous_function";
			if(end_expression == NULL){
				return NULL;
			}

			symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);

			//symbol_node->node_type = NODE_TYPE::KEYWORD_NODE;
			//symbol_node->keyword_info = &defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD];
		}else{

			if(*aux == '['){ // vector object ...
				if(pre_operator != NULL){
					if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
							pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
						print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_line);
						return NULL;
					}
				}

				// parse function but do not create ast node (we will create in trivial case value
				end_expression = parseArgs('[', ']',aux,m_line,sf);
				//symbol_node->value_symbol="anonymous_array";

				if(end_expression == NULL){
					return NULL;
				}

				symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);

				//symbol_node->node_type = NODE_TYPE::ARRAY_NODE;

			}
			else{
				char *start_expression = aux;
				is_symbol_trivial = true;

				// treat as symbol...
				if(is_new || is_delete) {
					aux = CStringUtils::IGNORE_BLANKS(aux + strlen(key_w->str), m_line);
					is_symbol_trivial = false;
				}

				end_expression = getEndWord(aux, m_line);

				 if(end_expression == NULL || end_expression == aux){

					 // check if punctuator or keyword..
					if(!printErrorUnexpectedKeywordOrPunctuator(aux, m_line)){
						print_error_cr("Expected symbol at line %i",m_line);
					}
					 return NULL;
				 }

				 symbol_name=CStringUtils::copyStringFromInterval(start_expression,end_expression);

				 // check for post opertator...
				 end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);
				 if((*post_operator = checkPostOperatorPunctuator(end_expression)) != NULL){
					 end_expression+=strlen((*post_operator)->str);
				 }

				 aux = start_expression;
			}
		}

		// if there's function or array access after symbol or object created ...
		end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);

		if(*end_expression == '('){ // parse args within '(' ')'...
			is_symbol_trivial = false;

			end_expression = parseArgs('(', ')',end_expression,m_line,sf);

			if(end_expression == NULL){
				return NULL;
			}

			symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);

			//(*args_node)->node_type = NODE_TYPE::FUNCTION_OR_CLASS_ARGS_CALL_NODE;
			end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);
		}
		else if(*end_expression == '['){ // parse args within '(' ')'...
			is_symbol_trivial = false;
			char *aux_ptr;
			int i=0;
			bool end=false;

			do{
				aux_ptr = end_expression;
				end_expression = parseArgs('[', ']',end_expression,m_line,sf);
				if(end_expression == NULL){
					if(i==0){
						return NULL;
					}else{
						end = true;
					}
				}

				i++;
			}while(!end);

			end_expression = aux_ptr;

			symbol_name = CStringUtils::copyStringFromInterval(aux, end_expression);

			//(*args_node)->node_type = NODE_TYPE::ARRAY_INDEX_NODE;

			end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);


			// check for post opertator...
			if(((*post_operator) = checkPostOperatorPunctuator(end_expression)) != NULL){
			 end_expression+=strlen((*post_operator)->str);
			}

			end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);
		}
	}
	// GETTING TRIVIAL SYMBOLS
	//----------------------------------------------------------
	return end_expression;
}

bool CAst::isMarkEndExpression(char c){
	return (c==0 || c==';' || c==',' ||  c==')'  || c==']' || c==':');
}

//-----------------------------------------------------------------------------------------------------------
char * CAst::parseExpression_Recursive(const char *s, int & m_line, CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated, GROUP_TYPE type_group,PASTNode parent ){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux=(char *)s;
	char *s_effective_start=(char *)s;
	char *expr_start_op;
	int start_line = m_line; // set another start line because left node or reparse to try another group was already parsed before.
	int m_lineOperator=-2;

	char *end_expression=(char *)s ; // by default end expression isequal to
	//PASTNode symbol_node=NULL; // can be a function or array.

	bool is_symbol_trivial_value=false;
	string symbol_value;
	string operator_str="";
	tInfoPunctuator *pre_operator=NULL,
					*post_operator=NULL,
					*operator_group=NULL;
	bool theres_some_operator=false;
	int m_definedSymbolLine;
	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

	if(isMarkEndExpression(*aux)){ // returning because is trivial!
		return aux;
	}

	if(type_group>=MAX_GROUPS) {
		print_error_cr("reached max groups");
		return NULL;
	}

	if(strncmp(aux,"10))",4)==0){
		int jjjj=0;
		jjjj++;
	}


	print_ast_cr("new expression eval:\"%.30s ...\" group:%i at line %i",aux,type_group, m_line);

	// searching for operator!
	print_ast_cr("searching for operator type %i...",type_group);

	while(!isMarkEndExpression(*aux) && (operator_group==0)){

		print_ast_cr("checkpoint1:%c\n",*aux);
		// 1. ignore spaces...
		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

		if((pre_operator=checkPreOperatorPunctuator(aux))!=NULL){
			aux+=strlen(pre_operator->str);
			aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
		}

		// try get symbol string
		aux=getSymbolValue(aux, m_line, sf,symbol_value, m_definedSymbolLine,pre_operator,&post_operator,is_symbol_trivial_value);

		print_ast_cr("checkpoint3:%c\n",*aux);

		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

		if(!isMarkEndExpression(*aux)){ // is not end expression


			if((operator_group=isOperatorPunctuator(aux))!=0){

				theres_some_operator |= true;
				expr_start_op=aux;
				m_lineOperator = m_line;
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

			/*if(symbol_node != NULL){ // the symbol was found before...
				*ast_node_to_be_evaluated = symbol_node;
			}else{*/

			if(ast_node_to_be_evaluated != NULL){


				if(is_symbol_trivial_value){


					(*ast_node_to_be_evaluated)=new tASTNode;
					(*ast_node_to_be_evaluated)->node_type = SYMBOL_NODE;
					(*ast_node_to_be_evaluated)->value_symbol=symbol_value; // assign its value ...
				}else{
					if(parseNonTrivialSymbol(symbol_value.c_str(),m_definedSymbolLine,sf, ast_node_to_be_evaluated, parent) == NULL){
						return NULL;
					}

				}


				print_ast_cr("---------------------");
				print_ast_cr("%s value \"%s\" at line %i",(is_symbol_trivial_value?"trivial":"NOT trivial"),symbol_value.c_str(), m_definedSymbolLine);
				print_ast_cr("---------------------");

				// put values by default ...
				(*ast_node_to_be_evaluated)->parent=parent;
				(*ast_node_to_be_evaluated)->definedValueline=m_definedSymbolLine;


				if(pre_operator!= NULL || post_operator != NULL){ // create pre operator node ...

					if(pre_operator!=NULL)
						*ast_node_to_be_evaluated = preNodePunctuator(pre_operator,*ast_node_to_be_evaluated);
					else
						*ast_node_to_be_evaluated = preNodePunctuator(post_operator,*ast_node_to_be_evaluated);
				}

			}
		}
		else{

			if(end_expression!= NULL){
			// there's a Punctuator, so let's perform generate its AST
				// reset prePunctuator...
				pre_operator=NULL;
				print_ast_cr("try to generate group1 expression: %.20s ...\n",s_effective_start);
				return parseExpression_Recursive(
						s,
						start_line, // start line because was reparsed before (i.e group -1)
						sf,
						ast_node_to_be_evaluated,
						(GROUP_TYPE)(((int)type_group)+1),parent);
			}
		}
	}else{ // we found the operator respect of GROUPX so let's put the AST to the left the resulting expression...

		if(ast_node_to_be_evaluated != NULL){
			*ast_node_to_be_evaluated=new tASTNode(2); // always preallocate 2 nodes (left and right)
			(*ast_node_to_be_evaluated)->parent=parent;
			(*ast_node_to_be_evaluated)->node_type = EXPRESSION_NODE;

		}
		char * expr_op_end = expr_start_op+strlen(operator_group->str);

		print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_group->str);
		char eval_left[MAX_EXPRESSION_LENGTH]={0};

		strncpy(eval_left,s_effective_start,expr_start_op-s_effective_start); // copy its left side...
		if(parseExpression_Recursive(eval_left,
									start_line, // start line because was reparsed before...
									 sf,
									 ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[LEFT_NODE]: NULL,
									 type_group,
									 ast_node_to_be_evaluated != NULL? *ast_node_to_be_evaluated : NULL)==NULL){
			return NULL;
		}

		if(	ast_node_to_be_evaluated != NULL ){
			if(pre_operator!=NULL ){
				(*ast_node_to_be_evaluated)->children[LEFT_NODE]=preNodePunctuator(pre_operator,(*ast_node_to_be_evaluated)->children[LEFT_NODE]);
			}
		}

		if((aux=parseExpression_Recursive(
								expr_op_end,
								m_line,
								sf,
								ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[RIGHT_NODE] : NULL,
								type_group,
								ast_node_to_be_evaluated != NULL ? (*ast_node_to_be_evaluated) : NULL)) == NULL){
			return NULL;
		}


		// check for inline-else
		if(operator_group->id == INLINE_IF_PUNCTUATOR){
			// 0-node is conditional node
			// 1-node is if-node...
			// 2-node is else-node

			if(*aux == ':'){

				PASTNode else_node;
				if((aux=parseExpression_Recursive(
										aux+1,
										m_line,
										sf,
										ast_node_to_be_evaluated != NULL ? &else_node : NULL,
										type_group,
										ast_node_to_be_evaluated != NULL ? (*ast_node_to_be_evaluated) : NULL)) == NULL){
					return NULL;
				}
				else{
					if(ast_node_to_be_evaluated != NULL){ // inline-else
						//else_node->operator_info = defined_punctuator[INLINE_ELSE_PUNCTUATOR];
						(*ast_node_to_be_evaluated)->children.push_back(else_node);
					}
				}
			}else{
				print_error_cr("inline-if has not ':' at line %i",m_line);
			}
		}



		if(ast_node_to_be_evaluated != NULL){
			// minus operators has special management because two negatives can be + but sums of negatives works
			if(operator_group->id == SUB_PUNCTUATOR) {
				(*ast_node_to_be_evaluated)->children[RIGHT_NODE]=preNodePunctuator(operator_group,(*ast_node_to_be_evaluated)->children[RIGHT_NODE]);

				// override the operator by +
				operator_group=&defined_operator_punctuator[ADD_PUNCTUATOR];
			}

			(*ast_node_to_be_evaluated)->node_type = PUNCTUATOR_NODE;
			(*ast_node_to_be_evaluated)->operator_info = operator_group;
			(*ast_node_to_be_evaluated)->definedValueline = m_lineOperator;
		}
	}
	return aux;
}

char * CAst::parseExpression(const char *s, int & m_line, CScriptFunction *sf , PASTNode * ast_node_to_be_evaluated ){

	// PRE: s is current string to parse. This function tries to parse an expression like i+1; and generates binary ast.
	// If this functions finds ';' then the function will generate ast.

	// last character is in charge of who is calling parseExpression because there's many ending cases ): [ ';' ',' ')' , ']' ]
	//char *aux = parseExpression_Recursive(s,m_line,sf,ast_node_to_be_evaluated);
	char *aux = parseNonTrivialSymbol(s, m_line, sf, ast_node_to_be_evaluated, NULL);

	if(aux != NULL && ast_node_to_be_evaluated != NULL){ // can save the node and tells that is an starting of expression node...

		if((*ast_node_to_be_evaluated)->node_type == NODE_TYPE::PUNCTUATOR_NODE){
			PASTNode ast_node=new tASTNode;
			ast_node->node_type = EXPRESSION_NODE;
			ast_node->children.push_back(*ast_node_to_be_evaluated);
			*ast_node_to_be_evaluated=ast_node;
		}

	}

	return aux;
}

//---------------------------------------------------------------------------------------------------------------
// PARSE KEYWORDS

char * CAst::parseNew(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_p;
	string symbol_value;

	tInfoKeyword *key_w;
	PASTNode args_node=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::NEW_KEYWORD){
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p+strlen(key_w->str),m_line);
			// try get symbol ...

			end_p = getEndWord(aux_p, m_line);

			 if(end_p == NULL || end_p == aux_p){
				 print_error_cr("Expected symbol at line %i",m_line);
				 return NULL;
			 }

			 symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_p);

			 aux_p=CStringUtils::IGNORE_BLANKS(end_p,m_line);

			 if(*aux_p != '('){
				 print_error_cr("Expected '(' after \'%s\'",key_w->str);
				 return NULL;
			 }

			 aux_p = parseArgs('(', ')',aux_p,m_line,sf,&args_node);
			 if(aux_p == NULL){
				 return NULL;
			 }

			args_node->node_type = FUNCTION_OR_CLASS_ARGS_CALL_NODE;



			 // it seems everything is allright... let's create the node...

			(*ast_node_to_be_evaluated) = new tASTNode();
			(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
			(*ast_node_to_be_evaluated)->keyword_info = key_w;
			(*ast_node_to_be_evaluated)->value_symbol = symbol_value;
			(*ast_node_to_be_evaluated)->children.push_back(args_node);

			return aux_p;
		}
	}

	return NULL;

}


char * CAst::parseDelete(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_p;
	string symbol_value;
	tInfoKeyword *key_w;


	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::DELETE_KEYWORD){

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p+strlen(key_w->str),m_line);

			end_p = getEndWord(aux_p, m_line);

			 if(end_p == NULL || end_p == aux_p){
				 print_error_cr("Expected symbol at line %i",m_line);
				 return NULL;
			 }

			 symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_p);

			(*ast_node_to_be_evaluated) = new tASTNode();
			(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
			(*ast_node_to_be_evaluated)->keyword_info = key_w;
			(*ast_node_to_be_evaluated)->value_symbol = symbol_value;

			return aux_p;

		}
	}


	return NULL;
}

char * CAst::parseClass(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_p;

	int class_line;
	string class_name;
	CScriptFunction *class_object=NULL;
	tInfoKeyword *key_w;
	PASTNode function_collection_node=NULL,vars_collection_node=NULL,child_node=NULL, base_class_node = NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::CLASS_KEYWORD){

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p+strlen(key_w->str),m_line);

			// check for symbol's name
			end_p = getEndWord(aux_p, m_line);
			if(end_p == NULL || end_p == aux_p){
				print_error_cr("Expected name class at line %i ",m_line);
				return NULL;
			}


			// try to register class...
			class_line = m_line;
			class_name = CStringUtils::copyStringFromInterval(aux_p, end_p);

			if((class_object = sf->registerClass(class_name, class_line)) == NULL){
				return NULL;
			}

			print_info_cr("registered class \"%s\" line %i ",class_name.c_str(), class_line);

			sf = class_object; // override the new function ...


			aux_p=CStringUtils::IGNORE_BLANKS(end_p,m_line);


			if(*aux_p == ':' ){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

				end_p = getEndWord(aux_p, m_line);

				if(end_p == NULL || end_p == aux_p){
					print_error_cr("Expected name class at line %i ",m_line);
					return NULL;
				}

				if(ast_node_to_be_evaluated != NULL){
					base_class_node = new tASTNode;
					base_class_node->value_symbol = CStringUtils::copyStringFromInterval(aux_p, end_p);
					base_class_node->node_type = BASE_CLASS_NODE;
				}

				aux_p=CStringUtils::IGNORE_BLANKS(end_p, m_line);
			}



			if(*aux_p == '{' ){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

				// it seem's we have a good built class...
				if(ast_node_to_be_evaluated != NULL){
					*ast_node_to_be_evaluated = new tASTNode;
					(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
					(*ast_node_to_be_evaluated)->keyword_info = key_w;

					// create var & functions collection...
					(*ast_node_to_be_evaluated)->children.push_back(vars_collection_node = new tASTNode);
					(*ast_node_to_be_evaluated)->children.push_back(function_collection_node = new tASTNode);

					if(base_class_node != NULL) {
						(*ast_node_to_be_evaluated)->children.push_back(	base_class_node );
					}
				}

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
							if((aux_p = parseFunction(aux_p, m_line,sf, &child_node)) != NULL){
								if(child_node->value_symbol != ""){
									if(ast_node_to_be_evaluated != NULL){
										function_collection_node->children.push_back(child_node);
									}
								}
								else {
									print_error_cr("Expected symbol after function at line %i",m_line);
									return NULL;
								}
							} else{
								return NULL;
							}
							break;
						case KEYWORD_TYPE::VAR_KEYWORD:
							if((aux_p = parseVar(aux_p, m_line,sf, &child_node)) != NULL){

								if(ast_node_to_be_evaluated != NULL){
									vars_collection_node->children.push_back(child_node);
								}

							} else{
								return NULL;
							}
							break;

						}
					}else{
						print_error_cr("Expected \"var\" or \"function\" keyword at line %i");
					}


					aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);


				}

				if(*aux_p != '}'){
					print_error_cr("class \"%s\" declared line %i is not closed ",class_name.c_str(),class_line);
				}


				return aux_p+1;

			}else{
				print_error_cr("Expected '{' at line %i ",m_line);
				return NULL;
			}
		}
	}
	return NULL;
}

char * CAst::parseArgs(char c1,char c2,const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;


	PASTNode   node_arg_expression=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	if(*aux_p == c1){
		aux_p++;
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

		if(ast_node_to_be_evaluated!=NULL){
			(*ast_node_to_be_evaluated) = new tASTNode;
		}

		if(*aux_p != c2 ){

			do{
				if((aux_p = parseExpression(aux_p,m_line,sf,(ast_node_to_be_evaluated != NULL ? &node_arg_expression : NULL)))==NULL){
					return NULL;
				}

				// push arg into node...
				if(ast_node_to_be_evaluated!=NULL){
					(*ast_node_to_be_evaluated)->children.push_back(node_arg_expression);
				}

				if(*aux_p == ',' ){
					aux_p++;
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
				}else{
					if(*aux_p != c2 ){
						print_error_cr("Expected %c",c2);
						return NULL;
					}
				}

			}while(*aux_p != c2 && *aux_p != 0);
		}

		return aux_p+1;
	}

	return NULL;
}

char * CAst::parseFunction(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *value_symbol,*end_var;
	tInfoKeyword *key_w;

	PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
	string conditional_str;
	bool error=false;
	CScope::tInfoRegisteredVar * irv=NULL;
	CScriptFunction *object_function;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD){

			// advance keyword...
			aux_p += strlen(key_w->str);
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if(ast_node_to_be_evaluated!=NULL){ // we save node...

				*ast_node_to_be_evaluated = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;

				(*ast_node_to_be_evaluated)->children.push_back(args_node = new tASTNode);



				args_node->node_type = NODE_TYPE::FUNCTION_ARGS_DECL_NODE;

				// start line ...
				(*ast_node_to_be_evaluated)->definedValueline = m_line;

			}

			if(*aux_p!='('){ // is named function..

				// check whwther the function is anonymous or not.
				end_var=getSymbolName(aux_p,m_line);

				if(end_var != NULL){

					if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
						return NULL;

					// check whether parameter name's matches with some global variable...
					if((irv=sf->getScope()->getCurrentScopePointer()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
						print_error_cr("Function name \"%s\" defined at line %i is ambiguos with symbol defined at %i", value_symbol, m_line,irv->m_line);
						return NULL;
					}

					if(ast_node_to_be_evaluated!=NULL){
						(*ast_node_to_be_evaluated)->value_symbol = value_symbol;
						// define value symbol...
						irv=sf->getScope()->registerSymbol((*ast_node_to_be_evaluated)->value_symbol,m_line);

						// create object function ...
						object_function =new CScriptFunction(sf);
						 irv->m_obj=object_function;
					}

				}else{
					return NULL;
				}

				aux_p=end_var;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
			}
			else{ //function node
				if(ast_node_to_be_evaluated!=NULL){ // save as function object...
					(*ast_node_to_be_evaluated)->node_type = FUNCTION_OBJECT_NODE;
				}
			}

			// parse function args...
			if(*aux_p == '('){ // push arguments...

				aux_p++;

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

				// grab words separated by ,
				while(*aux_p != 0 && *aux_p != ')'){

					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

					if(*aux_p == ')' || *aux_p == ','){
						print_error_cr("Expected arg at line %i ",m_line);
						return NULL;
					}

					end_var=getSymbolName(aux_p,m_line);

					if((value_symbol = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
						return NULL;

					// check if repeats...
					if(ast_node_to_be_evaluated != NULL){
						for(unsigned k = 0; k < args_node->children.size(); k++){
							if(args_node->children[k]->value_symbol == value_symbol){
								print_error_cr("Repeated argument '%s' argument at line %i ",value_symbol,m_line);
								return NULL;
							}
						}

						// check whether parameter name's matches with some global variable...
						if((irv=sf->getScope()->getCurrentScopePointer()->getInfoRegisteredSymbol(value_symbol,false)) != NULL){
							print_error_cr("Ambiguos symbol argument \"%s\" at line %i name with var defined at %i", value_symbol, m_line,irv->m_line);
							return NULL;

						}

						// ok register symbol into the object function ...
						irv=object_function->getScope()->registerSymbol(value_symbol,m_line);
					}
					aux_p=end_var;
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

					if(*aux_p != ')'){

						if(*aux_p != ','){
							print_error_cr("Expected ',' at line ",m_line);
							return NULL;
						}

						aux_p++;
					}
					// PUSH NEW ARG...
					if(ast_node_to_be_evaluated != NULL){
						arg_node = new tASTNode;
						arg_node->value_symbol=value_symbol;
						args_node->children.push_back(arg_node);
					}
				}

				if(*aux_p != ')'){
					print_error_cr("Expected ')' at line ",m_line);
					return NULL;
				}

				aux_p++;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

				if(*aux_p != '{'){
					print_error_cr("Expected '{' at line ",m_line);
					return NULL;
				}

				// ok let's go to body..
				if((aux_p = parseBlock(aux_p,m_line,sf,error,ast_node_to_be_evaluated != NULL ? &body_node : NULL)) != NULL){

					if(!error){

						if(ast_node_to_be_evaluated != NULL){
							(*ast_node_to_be_evaluated)->children.push_back(body_node);
							body_node->node_type = BODY_NODE;
						}

						return aux_p;
					}


				}
			}
			else{
				print_error_cr("Unclosed function defined at line %i",m_line);
			}
		}
	}
	return NULL;
}

char *  CAst::parseReturn(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	tInfoKeyword *key_w;
	string s_aux;


	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...

			aux_p += strlen(key_w->str);

			if(ast_node_to_be_evaluated != NULL){ // save
				*ast_node_to_be_evaluated = new tASTNode(1); // reserve for expression return
				(*ast_node_to_be_evaluated)->node_type = NODE_TYPE::KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			if((aux_p = parseExpression(aux_p, m_line, sf, ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[0] : NULL))!= NULL){

				if(*aux_p!=';'){
					print_error_cr("Expected ';' at line %i", m_line);
					return NULL;
				}
				aux_p++;

				return aux_p;
			}
		}
	}
	return NULL;
}

char * CAst::parseWhile(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tInfoKeyword *key_w;

	PASTNode conditional=NULL, while_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::WHILE_KEYWORD){

			if(ast_node_to_be_evaluated != NULL){
				*ast_node_to_be_evaluated = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p += strlen(key_w->str);

			// evaluate conditional line ...

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
			if(*aux_p == '('){



				if((end_expr = parseExpression(aux_p+1,m_line,sf,&conditional)) != NULL){

					if(*end_expr != ')'){
						print_error_cr("Expected ')' at line %i",m_line);
						return NULL;
					}


					if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					conditional_str=start_symbol;

					if(parseExpression((const char *)conditional_str.c_str(),m_line,sf,ast_node_to_be_evaluated != NULL ? &conditional : NULL) == NULL){
						return NULL;
					}

					if( ast_node_to_be_evaluated != NULL){
						PASTNode aux = new tASTNode;
						aux->node_type = CONDITIONAL_NODE;
						aux->children.push_back(conditional);
						(*ast_node_to_be_evaluated)->children.push_back(aux);
					}

					aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_line);
					if(*aux_p != '{'){
						print_error_cr("Expected while-block open block ('{') %i",m_line);
						return NULL;
					}

					if((aux_p=parseBlock(aux_p,m_line,sf,error,ast_node_to_be_evaluated != NULL ? &while_node : NULL))!= NULL){
						if(!error){
							if(ast_node_to_be_evaluated != NULL){
								(*ast_node_to_be_evaluated)->children.push_back(while_node);
							}


							return aux_p;
						}
					}

				}else{
					print_error_cr("Expected ')' while %i",m_line);
					return NULL;
				}

			}else{
				print_error_cr("Expected '(' while %i",m_line);
				return NULL;
			}
		}
	}
	return NULL;
}



char * CAst::parseIf(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tInfoKeyword *key_w;

	PASTNode conditional=NULL, if_node=NULL, else_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::IF_KEYWORD){

			if(ast_node_to_be_evaluated != NULL){
				*ast_node_to_be_evaluated = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p += strlen(key_w->str);

			// evaluate conditional line ...

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
			if(*aux_p == '('){
				if((end_expr = parseExpression(aux_p+1,m_line,sf,ast_node_to_be_evaluated != NULL? &conditional: NULL)) != NULL){

					if(*end_expr != ')'){
						print_error_cr("Expected ')' at line %i",m_line);
						return NULL;
					}


					if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					conditional_str=start_symbol;

					if(ast_node_to_be_evaluated!=NULL){
						PASTNode aux = new tASTNode;
						aux->children.push_back(conditional);
						aux->node_type = CONDITIONAL_NODE;
						(*ast_node_to_be_evaluated)->children.push_back(aux);
					}

					aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_line);
					if(*aux_p != '{'){
						if(!printErrorUnexpectedKeywordOrPunctuator(aux_p, m_line)){
							print_error_cr("Expected if-block open block ('{') %i",m_line);
						}
						return NULL;
					}

					if((aux_p=parseBlock(aux_p,m_line,sf,error,ast_node_to_be_evaluated != NULL ? &if_node : NULL))!= NULL){
						if(!error){
							if(ast_node_to_be_evaluated != NULL){
								(*ast_node_to_be_evaluated)->children.push_back(if_node);
							}

							aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

							bool else_key = false;
							if((key_w = isKeyword(aux_p)) != NULL){
								else_key = (key_w->id == KEYWORD_TYPE::ELSE_KEYWORD);
							}

							if(else_key){
								aux_p += strlen(key_w->str);

								aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
								if(*aux_p != '{'){
									print_error_cr("Expected else-block open block ('{') %i",m_line);
									return NULL;
								}

								if((aux_p=parseBlock(aux_p,m_line,sf,error,ast_node_to_be_evaluated != NULL ? &else_node : NULL))!= NULL){
										if(!error){

											if( ast_node_to_be_evaluated != NULL){
												(*ast_node_to_be_evaluated)->children.push_back(else_node);
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
					print_error_cr("Expected ')' if %i",m_line);
					return NULL;
				}

			}else{
				print_error_cr("Expected '(' if %i",m_line);
				return NULL;
			}
		}
	}
	return NULL;
}

char * CAst::parseFor(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	tInfoKeyword *key_w;
	bool error=false;
	PASTNode block_for = NULL,node_for_expression=NULL;
	string eval_for;
	CScope *_localScope =  sf->getScope(); // gets scope...
	CScope *_currentScope;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::FOR_KEYWORD){

			if(ast_node_to_be_evaluated != NULL){
				*ast_node_to_be_evaluated = new tASTNode(3); // ini, conditional, post
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p += strlen(key_w->str);

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
			if(*aux_p == '('){ // ready ...

				// save scope pointer ...
				_currentScope =_localScope->pushScope(); // push current scope
				if(ast_node_to_be_evaluated != NULL){
					(*ast_node_to_be_evaluated)->scope_ptr =_currentScope;
				}

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);


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
						tInfoKeyword *key_w = isKeyword(aux_p);
						if(key_w != NULL){
							if(key_w->id == VAR_KEYWORD){
								if((aux_p = parseVar(aux_p,m_line, sf, ast_node_to_be_evaluated != NULL ? &node_for_expression: NULL))==NULL){
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
						if((aux_p = parseExpression((const char *)aux_p,m_line,sf, ast_node_to_be_evaluated != NULL ? &node_for_expression: NULL)) == NULL){
							return NULL;
						}
					}

					if(*aux_p == info_for[i].next_char){
						if(ast_node_to_be_evaluated != NULL){ // add node as for node type and its expression below.
							PASTNode aux = new tASTNode();
							aux->node_type = info_for[i].node_type;
							aux->children.push_back(node_for_expression);
							(*ast_node_to_be_evaluated)->children[i]=aux;
						}
					}
					else{
						print_error_cr("Expected '%c' in for statment %i",info_for[i].next_char, m_line);
						return NULL;
					}

					aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);
				}

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p != '{'){
					print_error_cr("Expected '{' for-block at line %i",m_line);
					return NULL;
				}

				// parse block ...
				if((aux_p=parseBlock(aux_p,m_line,sf,error,ast_node_to_be_evaluated != NULL ? &block_for : NULL))!= NULL){
					if(!error){

						if(ast_node_to_be_evaluated != NULL) {
							(*ast_node_to_be_evaluated)->children.push_back(block_for);
						}

						_localScope->popScope(); // push current scope

						return aux_p;
					}
					else{

					}
				}
			}else{
				print_error_cr("Expected '(' for %i",m_line);
				return NULL;
			}
		}
	}

	return NULL;
}

char * CAst::parseSwitch(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_expr, *end_symbol,*start_symbol;
	PASTNode switch_node=NULL,
			 	 group_cases=NULL,
				 case_value_node=NULL,
				 default_switch_node=NULL;
	CScope *m_currentScope;
	tInfoPunctuator * ip;


	char *value_to_eval;
	string val;
	CScope *_localScope =  sf->getScope(); // gets current evaluating scope...
	tInfoKeyword *key_w,*key_w2;

	bool error=false;
	int n_cases;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::SWITCH_KEYWORD){

			if( ast_node_to_be_evaluated != NULL){
				*ast_node_to_be_evaluated = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p += strlen(key_w->str);

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if(*aux_p == '('){
				//if((end_expr = CStringUtils::ADVANCE_TO_CHAR(aux_p,')',m_line)) != NULL){
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

					end_expr = getEndWord(aux_p, m_line);

					value_to_eval = CStringUtils::copyStringFromInterval(aux_p, end_expr);

					if(!value_to_eval) {return NULL;}

					//--- create node
					if(ast_node_to_be_evaluated!= NULL){
						(*ast_node_to_be_evaluated)->value_symbol = value_to_eval;
					}

					//---
					aux_p=CStringUtils::IGNORE_BLANKS(end_expr,m_line);

					if(*aux_p != ')'){
						print_error_cr("Expected ')' switch %i",m_line);
						error = true;
						return NULL;
					}

					aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

					if(*aux_p == '{'){

						// ok try to get cases and default nodes ...
						aux_p++;
						aux_p = CStringUtils::IGNORE_BLANKS(aux_p,m_line);


						while(*aux_p!='}' && *aux_p!=0){
								n_cases = 0;
								bool theres_a_default= false;
								// init node ..
								if(ast_node_to_be_evaluated!= NULL){
									(*ast_node_to_be_evaluated)->children.push_back(switch_node = new tASTNode);
									switch_node->children.push_back(group_cases = new tASTNode);
									switch_node->children.push_back(NULL);//case_body_node = new tASTNode);
									group_cases->node_type = GROUP_CASES_NODE;
								}

								bool end = false;

								while((key_w = isKeyword(aux_p)) != NULL && !end){ // acumulative cases /defaults...

									switch(key_w->id){
									case DEFAULT_KEYWORD:
									case CASE_KEYWORD:

										val = "default";
										aux_p += strlen(key_w->str);
										aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

										if(key_w->id == CASE_KEYWORD){


											// get the symbol...

											start_symbol=aux_p;

											if((ip = isPunctuator(aux_p)) != NULL){
												if(ip->id == PUNCTUATOR_TYPE::ADD_PUNCTUATOR ||ip->id == PUNCTUATOR_TYPE::SUB_PUNCTUATOR){
													aux_p+=strlen(ip->str);
												}
												else{
													print_error_cr("unexpected token %s at line",ip->str,m_line);
													error = true;
													return NULL;
												}
											}


											end_symbol = getEndWord(aux_p, m_line);
											aux_p=end_symbol;

											value_to_eval = CStringUtils::copyStringFromInterval(start_symbol, end_symbol);

											if(value_to_eval==NULL){ return NULL;}

											val = value_to_eval;

											aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
										}else{
											theres_a_default = true;
										}

										if(*aux_p == ':'){

											if(ast_node_to_be_evaluated != NULL){
												// check if repeated...
												for(unsigned j=0; j <(*ast_node_to_be_evaluated)->children.size(); j++ ){ // switch nodes
													for(unsigned h=0; h <(*ast_node_to_be_evaluated)->children[j]->children[0]->children.size(); h++ ){ // groups nodes
														if((*ast_node_to_be_evaluated)->children[j]->children[0]->children[h]->value_symbol == val){
															print_error_cr("Symbol %s repeteaded in switch at line %i",val.c_str(),m_line);
															return NULL;
														}
													}
												}



												case_value_node = new tASTNode;
												case_value_node->node_type = KEYWORD_NODE;
												case_value_node->keyword_info = key_w;
												case_value_node->value_symbol = val;
												case_value_node->definedValueline=m_line;
												group_cases->children.push_back(case_value_node);

												if(key_w->id==DEFAULT_KEYWORD){ // save switch node ...
													default_switch_node=switch_node;
												}


											}

											aux_p++;
											n_cases++;
										}
										else{
											print_error_cr("Expected  : case at %i",m_line);
											return NULL;
										}


										break;

										aux_p += strlen(key_w->str);
										aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
										if(*aux_p == ':'){
											n_cases++;
											aux_p++;
										}
										else{
											print_error_cr("Expected  : default at %i",m_line);
											return NULL;
										}

										break;
									case BREAK_KEYWORD:
										end=true;
										break;
									default:
										print_error_cr("Expected case or default in switch %i",m_line);
										return NULL;
										break;
									}

									aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
								}

								if(n_cases == 0){
									print_error_cr("Expected case or default in switch %i",m_line);
									return NULL;
								}

								// print warning ignored cases in case there's a default in there...
								if(ast_node_to_be_evaluated!= NULL){
									if(group_cases->children.size() > 1 && theres_a_default){

										for(vector<PASTNode>::iterator it = group_cases->children.begin(); it != group_cases->children.end(); ){ //erase cases and print warning !
											if((*it)->keyword_info->id == CASE_KEYWORD){
												print_warning_cr("Ignored case %s defined at line %i because is joined by default keyword",(*it)->value_symbol.c_str(),(*it)->definedValueline);
												delete *it;
												group_cases->children.erase(it);

											}else{
												it++;
											}
										}
									}
								}

								// save scope pointer ...
								m_currentScope = _localScope->pushScope();
								if(ast_node_to_be_evaluated != NULL){
									switch_node->scope_ptr =m_currentScope;
								}


								// eval block...
								if((aux_p=generateAST_Recursive(aux_p, m_line, sf, error, ast_node_to_be_evaluated != NULL ? &switch_node->children[1] : NULL))==NULL){
									return NULL;
								}

								if(error){
									return NULL;
								}

								key_w2 = isKeyword(aux_p);
								if(key_w2->id == BREAK_KEYWORD){
									aux_p += strlen(key_w2->str);
									CStringUtils::IGNORE_BLANKS(aux_p,m_line);

									if(*aux_p == ';'){ // the new scope ...
										if(ast_node_to_be_evaluated != NULL){

											if(switch_node->children[1] != NULL){
												switch_node->children[1]->node_type = BODY_NODE;
											}
										}
									}
									else{
										print_error_cr("Expected break %i",m_line);
										return NULL;
									}

								}else{
									print_error_cr("Expected break %i",m_line);
									return NULL;
								}

								_localScope->popScope();

								aux_p =CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);
							}

						if(*aux_p == '}'){

							// check default node to put to the end of the list ...
								if(default_switch_node!= NULL){
									bool end=false;
									for(vector<PASTNode>::iterator it = (*ast_node_to_be_evaluated)->children.begin(); it != (*ast_node_to_be_evaluated)->children.end() && !end; ){ //erase cases and print warning !

										if((*it)->children[0]->children[0]->keyword_info->id == DEFAULT_KEYWORD){ // group cases-case node access
											(*ast_node_to_be_evaluated)->children.erase(it);
											end=true;
										}else{
											it++;
										}
									}

									(*ast_node_to_be_evaluated)->children.push_back(default_switch_node);
								}




							return aux_p + 1;
						}
						else{
							print_error_cr("Expected '}' switch %i",m_line);
						}
					}
					else{
						print_error_cr("Expected '{' switch %i",m_line);
					}
			}
			else{
				print_error_cr("Expected '(' switch %i",m_line);
			}
		}
	}
	return NULL;
}

char * CAst::parseVar(const char *s,int & m_line,  CScriptFunction *sf, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	CScope *_currentScope =  sf->getScope()->getCurrentScopePointer(); // gets current evaluating scope...
	tInfoKeyword *key_w;
	char *start_var,*end_var, *symbol_name;
	int m_startLine=0;

	string s_aux;


	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

			aux_p += strlen(key_w->str);

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);


			//
			if(ast_node_to_be_evaluated != NULL){
				(*ast_node_to_be_evaluated) = new tASTNode;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
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

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

				if((*aux_p == ';' || *aux_p == '=' || *aux_p == ',' )){ // JE: added multivar feature (',)).

					print_info_cr("registered symbol \"%s\" line %i ",symbol_name, m_line);

					if(ast_node_to_be_evaluated!=NULL){
						if(!_currentScope->registerSymbol(symbol_name,m_line)){
							return NULL;
						}

						// save symbol in the node ...
						(*ast_node_to_be_evaluated)->value_symbol = symbol_name;
					}


					if(*aux_p == '='){

						if(ast_node_to_be_evaluated != NULL){
							(*ast_node_to_be_evaluated)->children.push_back(NULL);
						}

						// try to evaluate expression...
						aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

						if((aux_p = parseExpression(start_var,m_startLine,sf,ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[0] : NULL)) == NULL){
							return NULL;
						}

						m_line = m_startLine;
					}
				}
				else{
					print_error_cr("expected ',',';' or '='",*aux_p, m_line);
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
				print_error_cr("Expected ';' at line %i", m_line);
				return NULL;
			}

			return aux_p;
		}
	}
	return NULL;
}

char * CAst::parseBlock(const char *s,int & m_line,  CScriptFunction *sf, bool & error,PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;

	CScope *_localScope =  sf->getScope(); // gets scope...
	CScope *currentScope;
	//CScope *_ant, *_post;


	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...

	if(*aux_p == '{'){
		aux_p++;

		currentScope = _localScope->pushScope();


		if((aux_p = generateAST_Recursive(aux_p, m_line,sf,error,ast_node_to_be_evaluated)) != NULL){
			if(error){
				return NULL;
			}

			if(ast_node_to_be_evaluated != NULL){
				(*ast_node_to_be_evaluated)->scope_ptr =  currentScope;
			}

			if(*aux_p != '}'){
				error = true;
				print_error_cr("Expected } ");
				return NULL;
			}

			_localScope->popScope();

			if(ast_node_to_be_evaluated != NULL){
				(*ast_node_to_be_evaluated)->node_type = BODY_NODE;
			}
			return aux_p+1;
		}
	}
	return NULL;
}

char *CAst::parseKeyWord(const char *s, int & m_line, CScriptFunction *sf, bool & error, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p= (char *)s;


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
			//print_error_cr("%s has no parse function implemented yet!",defined_keyword[keyw->id].str);
		}
    	// something wrong was happen..
	}

	return NULL;
}

char * CAst::generateAST_Recursive(const char *s, int & m_line, CScriptFunction *sf, bool & error, PASTNode *node_to_be_evaluated){

	// PRE: **node must be created and is i/o ast pointer variable where to write changes.

	tInfoKeyword *keyw=NULL;
	char *aux = (char *)s;
	char *end_expr;

	if(node_to_be_evaluated!= NULL){
	*node_to_be_evaluated = new tASTNode;
	}

	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

	while(*aux != 0 ){

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

		if(node_to_be_evaluated != NULL){
			(*node_to_be_evaluated)->children.push_back(NULL);
		}

		// 1st. check whether parse a keyword...
		if((end_expr = parseKeyWord(aux, m_line, sf, error, node_to_be_evaluated != NULL ? &(*node_to_be_evaluated)->children[(*node_to_be_evaluated)->children.size()-1] : NULL)) == NULL){

			// If was unsuccessful then try to parse expression.
			if(error){
				return NULL;
			}

			// 2nd. check whether parse a block
			if((end_expr = parseBlock(aux,m_line, sf, error,node_to_be_evaluated != NULL ? &(*node_to_be_evaluated)->children[(*node_to_be_evaluated)->children.size()-1]:NULL))==NULL){

				// If was unsuccessful then try to parse expression.
				if(error){
					return NULL;
				}

				// 2nd. try expression
				if((end_expr = parseExpression(aux,m_line, sf,node_to_be_evaluated != NULL ? &(*node_to_be_evaluated)->children[(*node_to_be_evaluated)->children.size()-1]:NULL)) == NULL){ // something wrong was happen.
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


		aux=end_expr;
		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
	}
	return aux;
}

bool CAst::generateAST(const char *s, CScriptFunction *sf, PASTNode * ast_node_to_be_evaluated){

	int m_line = 1;
	bool error=false;

	if(generateAST_Recursive(s, m_line,sf,error,ast_node_to_be_evaluated) != NULL){

		if(*ast_node_to_be_evaluated != NULL){

			(*ast_node_to_be_evaluated)->node_type = BODY_NODE;
		}
		return true;
	}
	return false;
}
