#include "zs_core.h"

#define MAX_EXPRESSION_LENGHT 2096


#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __VERBOSE_MESSAGE__
#endif

#ifdef  __VERBOSE_MESSAGE__

#define print_ast_cr print_info_cr
#else
#define print_ast_cr(s,...)
#endif



vector<CASTNode *> 			* CASTNode::vec_ast_node=NULL;



void CASTNode::setVectorASTNode(vector<CASTNode *> 	* set_vec_ast_node){
	vec_ast_node = set_vec_ast_node;
}

vector<CASTNode *> 		*		CASTNode::getVectorASTNodeNode(){
	return vec_ast_node;
}

CASTNode	*CASTNode::newASTNode(int preallocated_nodes){


	if(vec_ast_node->size() >= MAX_AST_NODES){
		print_error_cr("Max AST Nodes reached (%i)",MAX_AST_NODES);
		return NULL;
	}

	CASTNode	*ast_node = new CASTNode(preallocated_nodes);
	vec_ast_node->push_back(ast_node);
	ast_node->idxAstNode = vec_ast_node->size()-1;
	return ast_node;
}


/**
 * Get CASTNode Node by its idx, regarding current state.
 */
CASTNode 		* CASTNode::getAstNode(int idx){
	if(idx==ZS_UNDEFINED_IDX){
		return NULL;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return NULL;
	}
	 return vec_ast_node->at(idx);
}

int 	CASTNode::getScopeIdx(int idx){
	if(idx==ZS_UNDEFINED_IDX){
		return ZS_UNDEFINED_IDX;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return ZS_UNDEFINED_IDX;
	}

	return vec_ast_node->at(idx)->idxScope;
}

CScope * 	CASTNode::getScope(int idx){
	if(idx==ZS_UNDEFINED_IDX){
		return NULL;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return NULL;
	}

	return SCOPE_INFO_NODE(vec_ast_node->at(idx)->idxScope);
}

int 		CASTNode::getAstLine(int idx){
	if(idx==ZS_UNDEFINED_IDX){
		return -1;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return -1;
	}

	return vec_ast_node->at(idx)->line_value;
}

const char * CASTNode::getAstSymbolName(int idx){
	if(idx==ZS_UNDEFINED_IDX){
		return "undefined symbol";
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return "";
	}

	return vec_ast_node->at(idx)->symbol_value.c_str();
}

const char 		* CASTNode::getAstSymbolNameConstChar(int idx){
	if(idx==ZS_UNDEFINED_IDX){
		return "undefined symbol";
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
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
	return *s == '>';
}

bool CASTNode::parseLogicLessThanPunctuator(const char *s){
	return *s == '<';
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

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup0(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			ASSIGN_PUNCTUATOR,
			ADD_ASSIGN_PUNCTUATOR,
			SUB_ASSIGN_PUNCTUATOR,
			MUL_ASSIGN_PUNCTUATOR,
			DIV_ASSIGN_PUNCTUATOR,
			MOD_ASSIGN_PUNCTUATOR
	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return NULL;
}

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup1(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			TERNARY_IF_PUNCTUATOR,
			TERNARY_ELSE_PUNCTUATOR


	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return NULL;
}

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup2(const char *s){


	PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_EQUAL_PUNCTUATOR,
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

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup3(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			ADD_PUNCTUATOR,
			SUB_PUNCTUATOR,
			BINARY_XOR_PUNCTUATOR,
			BINARY_AND_PUNCTUATOR,
			BINARY_OR_PUNCTUATOR,
			SHIFT_LEFT_PUNCTUATOR,
			SHIFT_RIGHT_PUNCTUATOR


	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return NULL;
}

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup4(const char *s){


	PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_NOT_EQUAL_PUNCTUATOR
	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){ print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup5(const char *s){

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

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup6(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
			LOGIC_NOT_PUNCTUATOR

	};

	for(unsigned i = 0; i < ARRAY_LENGTH(index_to_evaluate); i++){
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){ print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}
		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return 0;
}

tPunctuatorInfo  * CASTNode::parsePunctuatorGroup7(const char *s){

	PUNCTUATOR_TYPE index_to_evaluate[]={
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

tPunctuatorInfo *  CASTNode::isOperatorPunctuator(const char *s){

	for(unsigned i = 0; i < MAX_OPERATOR_PUNCTUATORS; i++){
		if(defined_operator_punctuator[i].parse_fun != NULL){
			if(defined_operator_punctuator[i].parse_fun(s)){
				return &defined_operator_punctuator[i];
			}
		}
	}
	return 0;
}

tPunctuatorInfo *  CASTNode::isSpecialPunctuator(const char *s){

	for(unsigned i = 0; i < MAX_SPECIAL_PUNCTUATORS; i++){

		if(*defined_special_punctuator[i].str == *s){
			return &defined_special_punctuator[i];
		}
	}

	return 0;
}

tPunctuatorInfo  * CASTNode::parseArithmeticPunctuator(const char *s){

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

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun == NULL){print_error_cr("internal: %s not have parse function",defined_operator_punctuator[index_to_evaluate[i]].str);return NULL;}

		if(defined_operator_punctuator[index_to_evaluate[i]].parse_fun(s)){
			return &defined_operator_punctuator[index_to_evaluate[i]];
		}
	}

	return NULL;
}

tPunctuatorInfo *  CASTNode::isPunctuator(const char *s){

	tPunctuatorInfo * ip = isOperatorPunctuator(s);

	if(ip!=NULL){
		return ip;
	}

	return isSpecialPunctuator(s);
}

// to string utils ...
char * CASTNode::getEndWord(const char *s, int m_line){

	char *start_str=(char *)s;
	char *aux=(char *)s;
	 tPunctuatorInfo * sp;
	 tKeywordInfo *key_w;
	 bool is_possible_number=false;
	 int i=0;
	 bool start_digit = false;


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
			if( key_w->id != KEYWORD_TYPE::THIS_KEYWORD){
			 //&& key_w->id != KEYWORD_TYPE::SUPER_KEYWORD ){ // unexpected token ?
				print_error_cr("Unexpected keyword \"%s\" at line %i. Forgot \";\" ?",key_w->str, m_line);
				return NULL;
			}
		}

		while((*aux)!=0 && !(
				(*aux)==' ' ||
				(*aux)=='\t' ||
				(*aux)=='\n' ||
				(*aux)=='\r'
						) &&
				(isSpecialPunctuator(aux)==NULL)
				//(isOperatorPunctuator(aux)==NULL && !is_possible_number)

		) {
			// check for special punctuator ( the field '.' separator is processed within the word )
			if(i==0 && !start_digit){ // possible digit ...

				is_possible_number = CStringUtils::isDigit(*aux);
				start_digit = true;

			}


			if((sp = isOperatorPunctuator(aux))!=NULL){
				if(sp->id == FIELD_PUNCTUATOR  || ((*aux=='-' ||  *aux=='+') && ((i>0 && (*(aux-1)=='e'))))){
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
				print_error_cr("line %i: Invalid number format",m_line);
				return NULL;
			}
		}
	}
	return aux;
}

char *CASTNode::getSymbolName(const char *s,int & m_line){

	char *aux_p=(char *)s;
	tPunctuatorInfo *end_punctuator=isPunctuator(s);


	if(end_punctuator != NULL){
		print_error_cr("Unexpected '%s' at line %i",end_punctuator->str,m_line);
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

tKeywordInfo CASTNode::defined_keyword[MAX_KEYWORD];
tPunctuatorInfo CASTNode::defined_operator_punctuator[MAX_OPERATOR_PUNCTUATORS];
tPunctuatorInfo CASTNode::defined_special_punctuator[MAX_SPECIAL_PUNCTUATORS];


tKeywordInfo * CASTNode::isKeyword(const char *c){

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
tPunctuatorInfo *CASTNode::checkPreOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return &defined_operator_punctuator[PRE_INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return &defined_operator_punctuator[PRE_DEC_PUNCTUATOR];
	if(parsePlusPunctuator(s)) 	return &defined_operator_punctuator[ADD_PUNCTUATOR];
	if(parseMinusPunctuator(s)) return &defined_operator_punctuator[SUB_PUNCTUATOR];
	if(parseNotPunctuator(s))   return &defined_operator_punctuator[LOGIC_NOT_PUNCTUATOR];

	return 0;
}

tPunctuatorInfo *CASTNode::checkPostOperatorPunctuator(const char *s){

	if(parseIncPunctuator(s)) 	return &defined_operator_punctuator[POST_INC_PUNCTUATOR];
	if(parseDecPunctuator(s))	return &defined_operator_punctuator[POST_DEC_PUNCTUATOR];

	return 0;
}

char *CASTNode::functionArrayAccess_Recursive(const char *str, int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated, PASTNode parent){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux = (char *)str;

	aux = CStringUtils::IGNORE_BLANKS(aux, m_line);

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
					print_error_cr("Expected function object before '(' at line %i",m_line);
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
					print_error_cr("Expected array object before '[' at line %i",m_line);
					return NULL;
				}
			}

			if( ast_node_to_be_evaluated != NULL){
				if((args_obj = CASTNode::newASTNode())){return NULL;}
				args_obj->node_type = ARRAY_ACCESS_NODE;
			}

			do{
				PASTNode args_node=NULL;
				char *aux_ptr;
				int ini_line_access=m_line;

				if((aux_ptr=parseArgs('[', ']',aux,m_line,scope_info,ast_node_to_be_evaluated != NULL ? &args_node: NULL)) != NULL){
					if( ast_node_to_be_evaluated != NULL){
						if(args_node->children.size() != 1){
							print_error_cr("Invalid array access %i",m_line);
							return NULL;
						}
						args_obj->children.push_back(args_node);
						args_node->node_type = NODE_TYPE::ARRAY_INDEX_NODE;
						args_node->line_value =ini_line_access;
					}
					aux =  CStringUtils::IGNORE_BLANKS(aux_ptr,m_line);
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

				obj->parent = calling_object;
				obj->symbol_value = "--";
				args_obj->parent = calling_object;

				calling_object->children.push_back(obj); // the object itself...
				calling_object->children.push_back(args_obj); // the args itself...
				calling_object->parent=parent;

				// finally save ast node...

				*ast_node_to_be_evaluated = calling_object;
			}
		}

		PASTNode another_access=NULL;
		aux = functionArrayAccess_Recursive(aux,m_line,scope_info,ast_node_to_be_evaluated!=NULL?&another_access:NULL,ast_node_to_be_evaluated!=NULL?*ast_node_to_be_evaluated:NULL);

		if(another_access != NULL){
			(*ast_node_to_be_evaluated)->children.push_back(another_access);
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
	tKeywordInfo *key_w  = NULL;
	vector<PASTNode> vector_args_node;
	bool try_array_or_function_access = false;
	bool should_be_access=false;

	string symbol_value="";

	if(ast_node_to_be_evaluated != NULL){
		*ast_node_to_be_evaluated = NULL;
	}

	aux = CStringUtils::IGNORE_BLANKS(aux, m_startLine);

	key_w = isKeyword(aux);

	if(key_w != NULL){ // can be new,delete or function...


		aux = CStringUtils::IGNORE_BLANKS(aux+strlen(key_w->str), m_startLine);

		switch(key_w->id){
		// sould be function object ...
		case KEYWORD_TYPE::FUNCTION_KEYWORD:
		case KEYWORD_TYPE::OPERATOR_KEYWORD:
			// function objects are stored in MainClass or global scope.
			if((aux=parseFunctionOrOperator(str,m_startLine,scope_info,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL)) == NULL){
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
			print_error_cr("Invalid using of \"%s\" at line %i",key_w->str, m_startLine);
			break;
		}
	}else if(*aux == '[') { // is an array object ...

		if((aux = parseArgs('[', ']',aux,m_startLine,scope_info,ast_node_to_be_evaluated!=NULL?ast_node_to_be_evaluated:NULL))== NULL){
			return NULL;
		}

		aux = CStringUtils::IGNORE_BLANKS(aux, m_startLine);
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
			 word_str = CStringUtils::IGNORE_BLANKS(end_expression, m_startLine);

			 try_array_or_function_access = true;

			 should_be_access = (*word_str == '(' || *word_str == '[');

			 if(!should_be_access){
				 return parseExpression_Recursive(aux,  m_line, scope_info, ast_node_to_be_evaluated,GROUP_TYPE::GROUP_0,parent);
			 }
			 else{
				 aux = word_str;
			 }


		 }else{
			 print_error_cr("Cannot parse ");
			 return NULL;
		 }
	}

	if(try_array_or_function_access){// try array/function access


		if((aux = functionArrayAccess(aux, m_line,scope_info,ast_node_to_be_evaluated,parent)) == NULL){
			 print_error_cr("Cannot parse 2");
			return NULL;
		}

		if(ast_node_to_be_evaluated != NULL){
			if(*ast_node_to_be_evaluated == NULL && should_be_access){
				print_error_cr("Cannot parse 3");
				return NULL;
			}

			if((*ast_node_to_be_evaluated)->children.size() > 0){
				(*ast_node_to_be_evaluated)->children[0]->symbol_value = symbol_value;
			}
		}
	}
	return aux;
}

bool CASTNode::printErrorUnexpectedKeywordOrPunctuator(const char *current_string_ptr, int m_line){
	tPunctuatorInfo *ip=CASTNode::isPunctuator(current_string_ptr);
	tKeywordInfo *kw=CASTNode::isKeyword(current_string_ptr);

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

char *CASTNode::getSymbolValue(
		const char *current_string_ptr,
		int & m_line,
		CScope *scope_info,

		string & symbol_name,
		int & m_definedSymbolLine,
		tPunctuatorInfo *pre_operator,
		tPunctuatorInfo **post_operator,
		bool & is_symbol_trivial
		){

	char *aux = (char *)current_string_ptr;
	char *end_expression = aux;
	is_symbol_trivial = false;
	m_definedSymbolLine = m_line;

	if(symbol_name == "super"){
		print_error_cr("Invalid using \"super\" keyword at line %i",m_line);
		return NULL;
	}

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
		end_expression = parseExpression_Recursive(aux, m_line, scope_info);//, ast_node_to_be_evaluated, type_group,parent);

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

		tKeywordInfo *key_w;
		bool is_function_or_operator=false;
		bool is_new=false;
		bool is_delete=false;
		// usually we have to take care about special op symbols

		if((key_w =isKeyword(aux)) != NULL){
			is_function_or_operator = key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD || key_w->id == KEYWORD_TYPE::OPERATOR_KEYWORD;
			is_new=key_w->id == KEYWORD_TYPE::NEW_KEYWORD;
			is_delete=key_w->id == KEYWORD_TYPE::DELETE_KEYWORD;
		}

		if(is_function_or_operator){ // function object ...

			if(pre_operator!=NULL){
				if(pre_operator->id == PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR ||
						pre_operator->id == PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR){
					print_error_cr("Unexpected '%s' before ( at line %i",pre_operator->str,m_line);
					return NULL;
				}
			}

			// parse function but do not create ast node (we will create in trivial case value
			end_expression = parseFunctionOrOperator(aux,m_line,scope_info);

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
					aux = CStringUtils::IGNORE_BLANKS(aux + strlen(key_w->str), m_line);
					is_symbol_trivial = false;
				}

				end_expression = getEndWord(aux, m_line);

				 if(end_expression == NULL || end_expression == aux){

					 // check if punctuator or keyword..

					 if(end_expression != NULL){
						 if(!printErrorUnexpectedKeywordOrPunctuator(aux, m_line)){
							 print_error_cr("Expected symbol at line %i",m_line);
						 }
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

		if(*end_expression == '[' || *end_expression == '('){ // function or array access --> process its ast but not save ...
			is_symbol_trivial = false;
			end_expression = functionArrayAccess(end_expression,m_line,scope_info);

			if(end_expression == NULL){
				return NULL;
			}

			end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);
			// check for post opertator...
			if(((*post_operator) = checkPostOperatorPunctuator(end_expression)) != NULL){
			 end_expression+=strlen((*post_operator)->str);
			 end_expression = CStringUtils::IGNORE_BLANKS(end_expression, m_line);
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
	//PASTNode symbol_node=NULL; // can be a function or array.

	bool is_symbol_trivial_value=false;
	string symbol_value;
	string operator_str="";
	tPunctuatorInfo *pre_operator=NULL,
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


	print_ast_cr("new expression eval:\"%.30s ...\" group:%i at line %i",aux,type_group, m_line);

	// searching for operator!


	if(*aux == '{'){ //json expression...
		print_ast_cr("detected json expression");
		return parseStruct(aux,m_line,scope_info,ast_node_to_be_evaluated);
	}

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
		if((aux=getSymbolValue(aux, m_line, scope_info,symbol_value, m_definedSymbolLine,pre_operator,&post_operator,is_symbol_trivial_value)) == NULL){
			return NULL;
		}

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
				case GROUP_4:	operator_group = parsePunctuatorGroup4(expr_start_op);break;
				case GROUP_5:	operator_group = parsePunctuatorGroup5(expr_start_op);break;
				case GROUP_6:	operator_group = parsePunctuatorGroup6(expr_start_op);break;
				case GROUP_7:	operator_group = parsePunctuatorGroup7(expr_start_op);break;
				default: break;
				}
			}else{
				print_error_cr("line %i:expected operator or punctuator after \"%s\"",m_line,symbol_value.c_str());
				return NULL;
			}
		}
	}

	if(operator_group==NULL) {// there's no any operators \"type_group\"...
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

				}

				print_ast_cr("---------------------");
				print_ast_cr("%s value \"%s\" at line %i",(is_symbol_trivial_value?"trivial":"NOT trivial"),symbol_value.c_str(), m_definedSymbolLine);
				print_ast_cr("---------------------");

				// put values by default ...
				(*ast_node_to_be_evaluated)->parent=parent;
				(*ast_node_to_be_evaluated)->line_value=m_definedSymbolLine;


				if(pre_operator!= NULL || post_operator != NULL){ // create pre operator node ...

					if(post_operator!=NULL)
						(*ast_node_to_be_evaluated)->pre_post_operator_info = post_operator; // preNodePunctuator(post_operator,*ast_node_to_be_evaluated);

					if(pre_operator!=NULL){
						if(post_operator!=NULL){
							if(     (pre_operator->id == PRE_INC_PUNCTUATOR  || pre_operator->id  == PRE_DEC_PUNCTUATOR) &&
									(post_operator->id== POST_INC_PUNCTUATOR || post_operator->id == POST_DEC_PUNCTUATOR)){
								print_error_cr("object \"%s\" has left \"%s\" and right \"%s\" is ambiguous",(*ast_node_to_be_evaluated)->symbol_value.c_str(),pre_operator->str, post_operator->str);
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
				pre_operator=NULL;
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

		if(ast_node_to_be_evaluated != NULL){
			if((*ast_node_to_be_evaluated=CASTNode::newASTNode(2))==NULL) return NULL; // always preallocate 2 nodes (left and right)
			(*ast_node_to_be_evaluated)->parent=parent;
			(*ast_node_to_be_evaluated)->node_type = EXPRESSION_NODE;

		}
		char * expr_op_end = expr_start_op+strlen(operator_group->str);

		print_ast_cr("operator \"%s\" found we can evaluate left and right branches!!\n",operator_group->str);
		char eval_left[MAX_EXPRESSION_LENGTH]={0};

		strncpy(eval_left,s_effective_start,expr_start_op-s_effective_start); // copy its left side...
		if(parseExpression_Recursive(eval_left,
									start_line, // start line because was reparsed before...
									 scope_info,
									 ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[LEFT_NODE]: NULL,
									 type_group,
									 ast_node_to_be_evaluated != NULL? *ast_node_to_be_evaluated : NULL)==NULL){
			return NULL;
		}

		if(ast_node_to_be_evaluated != NULL){
			if((*ast_node_to_be_evaluated)->children[LEFT_NODE] == NULL){
				print_error_cr("line %i, Expected expression before \"%s\"",m_lineOperator,operator_group->str);
				return NULL;
			}
		}


		if(	ast_node_to_be_evaluated != NULL ){
			if((*ast_node_to_be_evaluated)->children[LEFT_NODE]->pre_post_operator_info == NULL){ // may subnode found post operator...
				(*ast_node_to_be_evaluated)->children[LEFT_NODE]->pre_post_operator_info = pre_operator;
			}
		}

		if((aux=parseExpression_Recursive(
								expr_op_end,
								m_line,
								scope_info,
								ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[RIGHT_NODE] : NULL,
								type_group,
								ast_node_to_be_evaluated != NULL ? (*ast_node_to_be_evaluated) : NULL)) == NULL){
			return NULL;
		}

		if(ast_node_to_be_evaluated != NULL){
			if((*ast_node_to_be_evaluated)->children[RIGHT_NODE] == NULL){
				print_error_cr("line %i, Expected expression after \"%s\"",m_lineOperator,operator_group->str);
				return NULL;
			}
		}

		if(ast_node_to_be_evaluated != NULL){
			// minus operators has special management because two negatives can be + but sums of negatives works
			if(operator_group->id == SUB_PUNCTUATOR) { // supose a-b

				// 1. change - by +
				operator_group=&defined_operator_punctuator[ADD_PUNCTUATOR];

				// 2. create neg node.
				PASTNode ast_neg_node=NULL;
				if((ast_neg_node = CASTNode::newASTNode())==NULL) return NULL;
				ast_neg_node->node_type = NODE_TYPE::PUNCTUATOR_NODE;
				ast_neg_node->operator_info = &defined_operator_punctuator[SUB_PUNCTUATOR];

				// 3. insert node between rigth node and ast_node
				ast_neg_node->parent = (*ast_node_to_be_evaluated);
				ast_neg_node->children.push_back((*ast_node_to_be_evaluated)->children[RIGHT_NODE]);
				(*ast_node_to_be_evaluated)->children[RIGHT_NODE]=ast_neg_node;


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
		print_error_cr("End string");
		return NULL;
	}

	// last character is in charge of who is calling parseExpression because there's many ending cases ): [ ';' ',' ')' , ']' ]
	char *aux_p = parseExpression_Recursive(s,m_line,scope_info,ast_node_to_be_evaluated);
	//char *aux = parseExpression_Recursive(s, m_line, scope_info, ast_node_to_be_evaluated, NULL);

	if(aux_p != NULL && ast_node_to_be_evaluated != NULL && *ast_node_to_be_evaluated!=NULL){ // can save the node and tells that is an starting of expression node...

		PASTNode ast_node= NULL;
		if((ast_node=CASTNode::newASTNode())==NULL) return NULL;
		ast_node->node_type = EXPRESSION_NODE;
		ast_node->children.push_back(*ast_node_to_be_evaluated);
		(*ast_node_to_be_evaluated)->parent = ast_node; // save parent ..
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
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);



			// expect word...
			end_p = getEndWord(aux_p, m_line);

			 if(end_p == NULL || end_p == aux_p){
				 print_error_cr("Expected symbol at line %i after ','",m_lineSymbol);
				 return NULL;
			 }


			 symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_p);
			 aux_p=CStringUtils::IGNORE_BLANKS(end_p,m_line);

			 if(*aux_p != ':'){ // expected : ...
				 print_error_cr("Expected ':' at line %i",m_line);
				 return NULL;
			 }

			 aux_p++;

			 // go to variable...
			 if((aux_p=parseExpression(aux_p,m_line,scope_info,ast_node_to_be_evaluated!=NULL?&attr_node:NULL)) == NULL){  //CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);
				 return NULL;
			 }

			 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...
			 if(ast_node_to_be_evaluated!=NULL){
				 attr_node->symbol_value = symbol_value;
				 attr_node->line_value = m_lineSymbol;
 			 	(*ast_node_to_be_evaluated)->children.push_back(attr_node);
 			 	attr_node->parent =(*ast_node_to_be_evaluated);
			 }

			 aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			 if(*aux_p != ',' && *aux_p != '}' ){
				 print_error_cr("expected '}' or ','");
				 return NULL;
			 }
		}
	}
	else{
		print_error_cr("Expected '{'");
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
		print_error_cr("Expected '{'");
		return NULL;
	}

	return (end+1);
}
//---------------------------------------------------------------------------------------------------------------
// PARSE KEYWORDS

char * CASTNode::parseNew(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_p;
	string symbol_value;

	tKeywordInfo *key_w;
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

			 aux_p = parseArgs('(', ')',aux_p,m_line,scope_info,&args_node);
			 if(aux_p == NULL){
				 return NULL;
			 }

			args_node->node_type = ARGS_PASS_NODE;
			 // it seems everything is allright... let's create the node...

			if(((*ast_node_to_be_evaluated) = CASTNode::newASTNode())==NULL) return NULL;
			(*ast_node_to_be_evaluated)->node_type = NEW_OBJECT_NODE;
			(*ast_node_to_be_evaluated)->keyword_info = NULL;
			(*ast_node_to_be_evaluated)->symbol_value = symbol_value;
			(*ast_node_to_be_evaluated)->children.push_back(args_node);

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
	tKeywordInfo *key_w;

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

	tKeywordInfo *key_w;
	PASTNode function_collection_node=NULL,vars_collection_node=NULL,child_node=NULL, base_class_node = NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::CLASS_KEYWORD){

			if(scope_info->getIdxParent()!=ZS_UNDEFINED_IDX){
				print_error_cr("line %i:class keyword is not allowed",m_line);
				return NULL;
			}

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

			print_debug_cr("registered class \"%s\" line %i ",class_name.c_str(), class_line);

			aux_p=CStringUtils::IGNORE_BLANKS(end_p,m_line);

			if(*aux_p == ':' ){
				string ext_name;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

				end_p = getEndWord(aux_p, m_line);

				if(end_p == NULL || end_p == aux_p){
					print_error_cr("Expected class name at line %i ",m_line);
					return NULL;
				}

				ext_name=CStringUtils::copyStringFromInterval(aux_p, end_p);

				if(CScriptClass::isClassRegistered(ext_name)){
					print_error_cr("extended class \"%s\" not exist");
					return NULL;
				}

				if(ast_node_to_be_evaluated != NULL){
					if((base_class_node = CASTNode::newASTNode()) == NULL) return NULL;
					base_class_node->symbol_value = CStringUtils::copyStringFromInterval(aux_p, end_p);
					base_class_node->node_type = BASE_CLASS_NODE;
				}

				aux_p=CStringUtils::IGNORE_BLANKS(end_p, m_line);
			}
			if(*aux_p == '{' ){

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

				// it seem's we have a good built class...
				//if(ast_node_to_be_evaluated != NULL){
				if((*ast_node_to_be_evaluated = CASTNode::newASTNode())== NULL) return NULL;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
				(*ast_node_to_be_evaluated)->symbol_value = class_name;

				CScope *scp = CScope::newScope(ZS_UNDEFINED_IDX);// new CScope(NULL);
				(*ast_node_to_be_evaluated)->idxScope =scp->idxScope;// new CScope(NULL); // scope function without base ...
				class_scope_info =scp; //(*ast_node_to_be_evaluated)->scope_info_ptr;

				// create var & functions collection...
				if((vars_collection_node = CASTNode::newASTNode())==NULL) return NULL;
				if((function_collection_node = CASTNode::newASTNode())==NULL) return NULL;
				(*ast_node_to_be_evaluated)->children.push_back(vars_collection_node);
				(*ast_node_to_be_evaluated)->children.push_back(function_collection_node);

				if(base_class_node != NULL) {
					(*ast_node_to_be_evaluated)->children.push_back(	base_class_node );
				}
				// register info class ...
				//scope_info = ; // override the new function ...
				// check for named functions or vars...

				while(*aux_p != '}' && *aux_p != 0){

					// 1st. check whether parse a keyword...
					key_w = isKeyword(aux_p);
					if(key_w != NULL){
						switch(key_w->id){
						default:
							print_error_cr("Expected \"var\" or \"function\" keyword at line %i", m_line);
							return NULL;
							break;
						case KEYWORD_TYPE::FUNCTION_KEYWORD:
						case KEYWORD_TYPE::OPERATOR_KEYWORD:
							if((aux_p = parseFunctionOrOperator(aux_p, m_line,class_scope_info, &child_node)) != NULL){
								if(child_node->symbol_value != ""){
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
							if((aux_p = parseMemberVar(aux_p, m_line,class_scope_info, &child_node)) != NULL){

								if(ast_node_to_be_evaluated != NULL){
									vars_collection_node->children.push_back(child_node);
								}

							} else{
								return NULL;
							}
							break;

						}
					}else{
						print_error_cr("Expected \"var\" or \"function\" keyword at line %i",m_line);
						return NULL;
					}
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
				}

				if(*aux_p != '}'){
					print_error_cr("class \"%s\" declared line %i is not closed ",class_name.c_str(),class_line);
				}

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);

				if(*aux_p != ';'){
					print_error_cr("class \"%s\" declared line %i not end with ;",class_name.c_str(),class_line);
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

char * CASTNode::parseArgs(char c1,char c2,const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	PASTNode   node_arg_expression=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	if(*aux_p == c1){
		aux_p++;
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

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

PASTNode  CASTNode::findAst(const string & _symbol_name_to_find, NODE_TYPE _node_type, KEYWORD_TYPE _keyword_type){
	return findAstRecursive(_symbol_name_to_find, _node_type, _keyword_type, MAIN_AST_NODE);
}

PASTNode  CASTNode::findAstRecursive(const string & _symbol_name_to_find, NODE_TYPE _node_type,KEYWORD_TYPE _keyword_type, PASTNode _node){
	if(_node->symbol_value == _symbol_name_to_find && _node->node_type == _node_type){
		if(_node_type==NODE_TYPE::KEYWORD_NODE){
			if(_keyword_type== _node->keyword_info->id){
				return _node;
			}
		}
		else{
			return _node;
		}
	}

	for(unsigned i = 0; i < _node->children.size(); i++){
		PASTNode ch;
		if((ch=findAstRecursive(_symbol_name_to_find,_node_type,_keyword_type,_node->children[i]))!=NULL){
			return ch;
		}
	}
	return NULL;
}

char * CASTNode::parseFunctionOrOperator(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *symbol_value,*end_var;
	tKeywordInfo *key_w;

	PASTNode args_node=NULL, body_node=NULL, arg_node=NULL;
	string conditional_str;
	bool error=false;
	CScope *_currentScope=NULL;
	tScopeVar * irv=NULL;
	string str_name;
	string class_member,class_name;
	PASTNode class_node=NULL;
	int idxScope=ZS_UNDEFINED_IDX;

	if(scope_info != NULL){
		idxScope=scope_info->idxScope;
	}

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::FUNCTION_KEYWORD || key_w->id == KEYWORD_TYPE::OPERATOR_KEYWORD){

			// advance keyword...
			aux_p += strlen(key_w->str);
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if(ast_node_to_be_evaluated!=NULL){ // we save node...

				*ast_node_to_be_evaluated = CASTNode::newASTNode();
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;

				if((args_node = CASTNode::newASTNode()) == NULL) return NULL;
				(*ast_node_to_be_evaluated)->children.push_back(args_node);

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

						// check whwther the function is anonymous or not.
						if(key_w->id == KEYWORD_TYPE::OPERATOR_KEYWORD){
							tPunctuatorInfo *ip = parseArithmeticPunctuator(aux_p);
							if(ip==NULL){
								print_error_cr("operator \"%.02s\" not valid at line %i",aux_p,m_line);
								return NULL;
							}
							end_var=aux_p + strlen(ip->str);//getSymbolName(aux_p,m_line);
						}
						else{
							end_var=getSymbolName(aux_p,m_line);
						}

						if(end_var != NULL){

							if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
								return NULL;

							// check whether parameter name's matches with some global variable...
							if(scope_info != NULL){
								if((irv=scope_info->getCurrentScopePointer()->getInfoRegisteredSymbol(symbol_value,false)) != NULL){

									if(irv->idxAstNode!=ZS_UNDEFINED_IDX){
										print_error_cr("Function name \"%s\" defined at line %i is already defined at %i", symbol_value, m_line,AST_LINE_VALUE(irv->idxAstNode));
									}else{
										print_error_cr("Function name \"%s\" at line %i is no allowed it has conflict with name of already registered function in C/C++", symbol_value, m_line);
									}

									return NULL;
								}
							}
						}else{
							return NULL;
						}
					}
				}
				aux_p=end_var;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
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

					if((irv=scope_info->registerSymbol(symbol_value,(*ast_node_to_be_evaluated)))==NULL){
						return NULL;
					}

					if((*ast_node_to_be_evaluated) != NULL){
						(*ast_node_to_be_evaluated)->symbol_value=symbol_value;
					}

					ast_node=*ast_node_to_be_evaluated;
				}
				else{
					irv=scope_info->registerAnonymouseFunction((*ast_node_to_be_evaluated));
					ast_node=AST_NODE(irv->idxAstNode);
					ast_node->symbol_value = irv->name;
				}

				// define value symbol...
				if(irv == NULL){
					return NULL;
				}
				ast_node->idxScope = idxScope;
			}
			// parse function args...
			if(*aux_p == '('){ // push arguments...

				aux_p++;
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

				if(ast_node_to_be_evaluated != NULL){

					_currentScope=scope_info->pushScope();
				}

				// grab words separated by ,
				while(*aux_p != 0 && *aux_p != ')'){

					aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

					if(*aux_p == ')' || *aux_p == ','){
						print_error_cr("Expected arg at line %i ",m_line);
						return NULL;
					}

					end_var=getSymbolName(aux_p,m_line);

					if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
						return NULL;

					// check if repeats...
					if(ast_node_to_be_evaluated != NULL){
						for(unsigned k = 0; k < args_node->children.size(); k++){
							if(args_node->children[k]->symbol_value == symbol_value){
								print_error_cr("Repeated argument '%s' argument at line %i ",symbol_value,m_line);
								return NULL;
							}
						}

						// check whether parameter name's matches with some global variable...
						if((irv=_currentScope->getInfoRegisteredSymbol(symbol_value,false)) != NULL){
							print_error_cr("Ambiguous symbol argument \"%s\" at line %i name with var defined at %i", symbol_value, m_line, AST_LINE_VALUE(irv->idxAstNode));
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
						if((arg_node = CASTNode::newASTNode()) == NULL) return NULL;
						arg_node->symbol_value=symbol_value;
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
				if((aux_p = parseBlock(
						aux_p,
						m_line,
						ast_node_to_be_evaluated != NULL ? scope_info:NULL ,
						error,
						ast_node_to_be_evaluated != NULL ? &body_node : NULL

					)) != NULL){

					if(!error){

						if(ast_node_to_be_evaluated != NULL){
							(*ast_node_to_be_evaluated)->children.push_back(body_node);
							scope_info->popScope();
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

char *  CASTNode::parseReturn(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	tKeywordInfo *key_w;
	string s_aux;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	key_w = isKeyword(aux_p);

	if(key_w != NULL){

		if(key_w->id == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...

			aux_p += strlen(key_w->str);

			if(ast_node_to_be_evaluated != NULL){ // save
				if((*ast_node_to_be_evaluated = CASTNode::newASTNode(1)) == NULL) return NULL; // reserve for expression return
				(*ast_node_to_be_evaluated)->node_type = NODE_TYPE::KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if((aux_p = parseExpression(aux_p, m_line, scope_info, ast_node_to_be_evaluated != NULL ? &(*ast_node_to_be_evaluated)->children[0] : NULL))!= NULL){

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

char * CASTNode::parseWhile(const char *s,int & m_line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tKeywordInfo *key_w;

	PASTNode conditional_expression=NULL, while_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::WHILE_KEYWORD){

			if(ast_node_to_be_evaluated != NULL){
				if((*ast_node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p += strlen(key_w->str);

			// evaluate conditional line ...
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
			if(*aux_p == '('){

				if((end_expr = parseExpression(aux_p+1,m_line,scope_info,&conditional_expression)) != NULL){

					if(*end_expr != ')'){
						print_error_cr("Expected ')' at line %i",m_line);
						return NULL;
					}
					if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					if( ast_node_to_be_evaluated != NULL){
						PASTNode aux;
						if((aux= CASTNode::newASTNode()) == NULL) return NULL;
						aux->node_type = CONDITIONAL_NODE;
						aux->children.push_back(conditional_expression);
						(*ast_node_to_be_evaluated)->children.push_back(aux);
					}
					aux_p=CStringUtils::IGNORE_BLANKS(end_expr+1,m_line);
					if(*aux_p != '{'){
						print_error_cr("Expected while-block open block ('{') %i",m_line);
						return NULL;
					}
					if((aux_p=parseBlock(aux_p,m_line,scope_info,error,ast_node_to_be_evaluated != NULL ? &while_node : NULL))!= NULL){
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

char * CASTNode::parseIf(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_expr,*start_symbol;
	tKeywordInfo *key_w;
	PASTNode conditional=NULL, if_node=NULL, else_node=NULL;
	string conditional_str;
	bool error = false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::IF_KEYWORD){

			if(ast_node_to_be_evaluated != NULL){
				if((*ast_node_to_be_evaluated = CASTNode::newASTNode()) == NULL) return NULL;
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}
			aux_p += strlen(key_w->str);

			// evaluate conditional line ...
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
			if(*aux_p == '('){
				if((end_expr = parseExpression(aux_p+1,m_line,scope_info,ast_node_to_be_evaluated != NULL? &conditional: NULL)) != NULL){

					if(*end_expr != ')'){
						print_error_cr("Expected ')' at line %i",m_line);
						return NULL;
					}


					if((start_symbol = CStringUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					conditional_str=start_symbol;

					if(ast_node_to_be_evaluated!=NULL){
						PASTNode aux;
						if((aux = CASTNode::newASTNode()) == NULL) return NULL;
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

					if((aux_p=parseBlock(aux_p,m_line,scope_info,error,ast_node_to_be_evaluated != NULL ? &if_node : NULL))!= NULL){
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

								if((aux_p=parseBlock(aux_p,m_line,scope_info,error,ast_node_to_be_evaluated != NULL ? &else_node : NULL))!= NULL){
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

char * CASTNode::parseFor(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	tKeywordInfo *key_w;
	bool error=false;
	PASTNode block_for = NULL,node_for_expression=NULL;
	string eval_for;
	//CScope *_localScope =  scope_info != NULL?scope_info->symbol_info.ast->scope_info_ptr:NULL; // gets scope...
	CScope *_currentScope=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::FOR_KEYWORD){

			if(ast_node_to_be_evaluated != NULL){
				if((*ast_node_to_be_evaluated = CASTNode::newASTNode(3)) == NULL) return NULL;; // ini, conditional, post
				(*ast_node_to_be_evaluated)->node_type = KEYWORD_NODE;
				(*ast_node_to_be_evaluated)->keyword_info = key_w;
			}

			aux_p += strlen(key_w->str);
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

			if(*aux_p == '('){ // ready ...

				// save scope pointer ...
				if(ast_node_to_be_evaluated != NULL){
					_currentScope =scope_info->pushScope(); // push current scope
					(*ast_node_to_be_evaluated)->idxScope = ZS_UNDEFINED_IDX;
					if(_currentScope!=NULL){
						(*ast_node_to_be_evaluated)->idxScope=_currentScope->idxScope;
					}
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
						tKeywordInfo *key_w = isKeyword(aux_p);
						if(key_w != NULL){
							if(key_w->id == VAR_KEYWORD){
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
							aux->children.push_back(node_for_expression);
							(*ast_node_to_be_evaluated)->children[i]=aux;
						}
					}
					else{
						print_error_cr("Expected '%c' in for statment %i",info_for[i].next_char, m_line);
						return NULL;
					}


					node_for_expression=NULL;
					aux_p=CStringUtils::IGNORE_BLANKS(aux_p+1,m_line);
				}

				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
				if(*aux_p != '{'){
					print_error_cr("Expected '{' for-block at line %i",m_line);
					return NULL;
				}

				// parse block ...
				if((aux_p=parseBlock(aux_p,m_line,_currentScope,error,ast_node_to_be_evaluated != NULL ? &block_for : NULL,true))!= NULL){ // true: We treat declared variables into for as another scope.
					if(!error){

						if(ast_node_to_be_evaluated != NULL) {
							(*ast_node_to_be_evaluated)->children.push_back(block_for);
							scope_info->popScope(); // push current scope
						}


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

char * CASTNode::parseSwitch(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	char *end_expr, *end_symbol,*start_symbol;
	PASTNode switch_node=NULL,
			 	 group_cases=NULL,
				 case_value_node=NULL,
				 default_switch_node=NULL;
	CScope *m_currentScope;
	tPunctuatorInfo * ip;
	char *value_to_eval;
	string val;
	//CScope *_localScope = scope_info!=NULL?scope_info->symbol_info.ast->scope_info_ptr:NULL; // gets current evaluating scope...
	tKeywordInfo *key_w,*key_w2;

	bool error=false;
	int n_cases;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check for keyword ...
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::SWITCH_KEYWORD){

			if( ast_node_to_be_evaluated != NULL){
				if((*ast_node_to_be_evaluated = CASTNode::newASTNode())==NULL) return NULL;
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
						(*ast_node_to_be_evaluated)->symbol_value = value_to_eval;
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
									if((switch_node = CASTNode::newASTNode()) == NULL) return NULL;
									if((group_cases = CASTNode::newASTNode()) == NULL) return NULL;

									(*ast_node_to_be_evaluated)->children.push_back(switch_node);
									switch_node->children.push_back(group_cases);
									switch_node->children.push_back(NULL);
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
														if((*ast_node_to_be_evaluated)->children[j]->children[0]->children[h]->symbol_value == val){
															print_error_cr("Symbol %s repeteaded in switch at line %i",val.c_str(),m_line);
															return NULL;
														}
													}
												}

												if((case_value_node = CASTNode::newASTNode())==NULL) return NULL;
												case_value_node->node_type = KEYWORD_NODE;
												case_value_node->keyword_info = key_w;
												case_value_node->symbol_value = val;
												case_value_node->line_value=m_line;
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
												print_warning_cr("Ignored case %s defined at line %i because is joined by default keyword",(*it)->symbol_value.c_str(),(*it)->line_value);
												delete *it;
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
								if((aux_p=generateAST_Recursive(aux_p, m_line, scope_info, error, ast_node_to_be_evaluated != NULL ? &switch_node->children[1] : NULL,true))==NULL){
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

								if(scope_info != NULL){
									scope_info->popScope();
								}

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

char * CASTNode::parseMemberVar(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.

	char *aux_p = (char *)s;
	CScope *_currentScope = NULL;
	tKeywordInfo *key_w;
	char *start_var,*end_var, *symbol_name;
	string class_name, class_member;
	PASTNode class_node;
	bool error=false;
	string s_aux;
	bool extension_prop=false;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

			aux_p += strlen(key_w->str);
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

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

					(*ast_node_to_be_evaluated)->children.push_back(var_new);
				}

				aux_p=CStringUtils::IGNORE_BLANKS(end_var,m_line);

				if(*aux_p == ';'){
					aux_p++;
				}
				else{
					print_error_cr("line %i; expected ';' after extension class attribute", m_line,*aux_p);
					return NULL;
				}

				print_debug_cr("registered symbol \"%s::%s\" line %i ",class_name.c_str(),class_member.c_str(), m_line);
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
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

				if((*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
					print_debug_cr("registered symbol \"%s\" line %i ",symbol_name, m_line);

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
						(*ast_node_to_be_evaluated)->children.push_back(var_new);
					}
				}
				else{
					print_error_cr("line %i: unexpected '%c'", m_line,*aux_p);
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

char * CASTNode::parseVar(const char *s,int & m_line,  CScope *scope_info, PASTNode *ast_node_to_be_evaluated){

	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;
	CScope *_currentScope = NULL;
	PASTNode var_node;
	tKeywordInfo *key_w;
	char *start_var,*end_var, *symbol_name;
	int m_startLine=0;
	string s_aux;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	key_w = isKeyword(aux_p);

	if(key_w != NULL){
		if(key_w->id == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...
			aux_p += strlen(key_w->str);
			aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

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
				aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);
				if((*aux_p == ';' || *aux_p == '=' || *aux_p == ',' )){ // JE: added multivar feature (',)).
					print_debug_cr("registered symbol \"%s\" line %i ",symbol_name, m_line);
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

						(*ast_node_to_be_evaluated)->children.push_back(var_node);

						print_debug_cr("scope %i",_currentScope->idxScope);


						if(!_currentScope->registerSymbol(symbol_name,var_node)){
							return NULL;
						}
					}
					if(*aux_p == '='){
						if(var_node != NULL){
							var_node->children.push_back(NULL);
						}
						// try to evaluate expression...
						aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

						if((aux_p = parseExpression(start_var,m_startLine,scope_info,var_node != NULL ? &var_node->children[var_node->children.size()-1] : NULL)) == NULL){
							return NULL;
						}
						m_line = m_startLine;

						if(!(*aux_p == ';' || *aux_p == '=' || *aux_p == ',' )){
							print_error_cr("line %i: expected ',',';' or '=' but it was '%c'", m_line,*aux_p);
							return NULL;
						}
					}
				}
				else{
					print_error_cr("line %i: expected ',',';' or '=' but it was '%c'", m_line,*aux_p);
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

char * CASTNode::parseBlock(const char *s,int & m_line,  CScope *scope_info, bool & error,PASTNode *ast_node_to_be_evaluated, bool push_scope){
	// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
	char *aux_p = (char *)s;

	//CScope *_localScope =  scope_info != NULL ? scope_info->symbol_info.ast->scope_info_ptr: NULL;
	CScope *currentScope=  NULL;
	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

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
				print_error_cr("Expected } ");
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

char * CASTNode::isClassMember(const char *s,int & m_line, string & _class_name, string & var_name, PASTNode & _class_node, bool & error, tKeywordInfo *kwi){

	char *aux_p = (char *)s;
	char *end_var;
	char *symbol_value;

	error = true;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	// check whwther the function is anonymous or not.
	end_var=getSymbolName(aux_p,m_line);

	if(end_var != NULL){

		if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
			return NULL;
	}else{
		return NULL;
	}
	aux_p=end_var;
	aux_p=CStringUtils::IGNORE_BLANKS(aux_p,m_line);

	if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...
		_class_name = symbol_value;
		_class_node = findAst(_class_name,NODE_TYPE::KEYWORD_NODE, KEYWORD_TYPE::CLASS_KEYWORD);

		if(_class_node == NULL){
			print_error_cr("Class \"%s\" at line %i is no defined", symbol_value, m_line);
			return NULL;
		}

		aux_p=CStringUtils::IGNORE_BLANKS(aux_p+2,m_line); // ignore ::

		if(kwi->id == KEYWORD_TYPE::OPERATOR_KEYWORD){
			tPunctuatorInfo *pi = parseArithmeticPunctuator(aux_p);
			if(pi==NULL){
				print_error_cr("operator \"%.02s\" not valid at line %i",aux_p,m_line);
				return NULL;
			}
			end_var=aux_p+strlen(pi->str);
		}
		else{
			end_var=getSymbolName(aux_p,m_line);
		}


		if(end_var != NULL){

			if((symbol_value = CStringUtils::copyStringFromInterval(aux_p,end_var))==NULL)
				return NULL;
		}else{
			return NULL;
		}

		var_name = symbol_value;
		aux_p=CStringUtils::IGNORE_BLANKS(end_var,m_line);
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


	tKeywordInfo *keyw=NULL,*keyw2nd=NULL;

	aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

	// check if condition...
	keyw = isKeyword(aux_p);

	if(keyw != NULL){ // a keyword was detected...

		aux_p+=strlen(keyw->str);
		aux_p=CStringUtils::IGNORE_BLANKS(aux_p, m_line);

		// check if non named function...
		if(keyw->id == KEYWORD_TYPE::FUNCTION_KEYWORD || keyw->id == KEYWORD_TYPE::OPERATOR_KEYWORD){
			if( *aux_p == '('){
				// Is no named. No named function is an object and should be processed within parseExpression ...
				return NULL;
			}
		}

		// check if class and is not main class (scope )...
		if(keyw->id == KEYWORD_TYPE::CLASS_KEYWORD){

			if((aux_p = parseClass(s,m_line,scope_info,ast_node_to_be_evaluated)) != NULL){
				return aux_p;
			}

			error = true;
			return NULL;
		}

		// check if another kwyword is defined ...
		if((keyw2nd = isKeyword(aux_p))!= NULL){

			if(
					keyw2nd->id != KEYWORD_TYPE::FUNCTION_KEYWORD && keyw2nd->id != KEYWORD_TYPE::OPERATOR_KEYWORD   // list of exceptional keywords...
			  ){

				print_error_cr("unexpected keyword \"%s\" at line %i",keyw2nd->str, m_line);
				error = true;
				return NULL;
			}
		}

		if(defined_keyword[keyw->id].parse_fun != NULL){

			if((aux_p = defined_keyword[keyw->id].parse_fun(s,m_line,scope_info, ast_node_to_be_evaluated)) != NULL){
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

char * CASTNode::generateAST_Recursive(const char *s, int & m_line, CScope *scope_info, bool & error, PASTNode *node_to_be_evaluated, bool allow_breaks, bool is_main_node){

	// PRE: **node must be created and is i/o ast pointer variable where to write changes.
	tKeywordInfo *keyw=NULL;
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
	aux=CStringUtils::IGNORE_BLANKS(aux, m_line);

	while(*aux != 0 ){

		children = NULL;
		// ignore all ;
		while(*aux==';' && *aux != 0){
			aux =CStringUtils::IGNORE_BLANKS(aux+1, m_line);
		}

		if(*aux==0){ // custom case exit..
			return aux;
		}

		if(*aux == '}'){ // trivial cases...
			return aux;
		}else{
			keyw = isKeyword(aux);
			if(keyw!= NULL){

				if(keyw->id == KEYWORD_TYPE::BREAK_KEYWORD){
					if(allow_breaks){

						return aux;
					}
					else{
						print_error_cr("unexpected \"break\" at line %i",m_line);
						error=true;
						return NULL;
					}
				}else if(keyw->id == KEYWORD_TYPE::FUNCTION_KEYWORD||keyw->id == KEYWORD_TYPE::VAR_KEYWORD){

					int startLine = m_line;
					string _class_name, _member_name;
					bool error=false;
					if(isClassMember(aux+strlen(keyw->str),startLine, _class_name, _member_name, _class_node,error, keyw) != NULL){


						if(keyw->id == KEYWORD_TYPE::VAR_KEYWORD){
							if((end_expr=parseMemberVar(aux,m_line,SCOPE_INFO_NODE(_class_node->idxScope),&children))==NULL){
								return NULL;
							}
							// push into var collection ...
							_class_node->children[0]->children.push_back(children);

						}else{
							startLine = m_line;
							end_expr=parseFunctionOrOperator(aux,m_line,SCOPE_INFO_NODE(_class_node->idxScope),&children);
							if(end_expr==NULL){
								return NULL;
							}
							// push into function collection...
							_class_node->children[1]->children.push_back(children);
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
						print_error_cr("Expected ';' at expression starting at line %i",starting_expression);
						return NULL;
					}
					end_expr++;
				}
			}

			// new expression ready to be evaluated...
			if(node_to_be_evaluated != NULL && children != NULL){
				(*node_to_be_evaluated)->children.push_back(children);
			}
		}
		aux=end_expr;
		aux=CStringUtils::IGNORE_BLANKS(aux, m_line);
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
		defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",parseFunctionOrOperator};
		defined_keyword[KEYWORD_TYPE::OPERATOR_KEYWORD] = {OPERATOR_KEYWORD,"operator",parseFunctionOrOperator};
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
			print_warning("ASTNode already initialized!");
		}
}

CASTNode::CASTNode(int preallocate_num_nodes){
	node_type = UNKNOWN_NODE;
	keyword_info = NULL;
	pre_post_operator_info = NULL;
	line_value=-1;
	operator_info=NULL;
	symbol_value="";
	parent=NULL;
	//aux_value=NULL;

	idxAstNode = ZS_UNDEFINED_IDX;
	idxScope = ZS_UNDEFINED_IDX;

	if(preallocate_num_nodes > 0){
		for(int i = 0; i < preallocate_num_nodes; i++){
			children.push_back(NULL);
		}
	}
}


CASTNode::~CASTNode(){
	//destroyChildren();
}

