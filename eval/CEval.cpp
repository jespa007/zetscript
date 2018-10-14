#include "CEvalParse.cpp"
#include "CEvalCompile.cpp"


/*

Constants (C)
-------------

C <= Integer,Number,Boolean,String


Pre operators (Pe)
-------------

Pe <= [ --, ++ ]

Post operators (Po)
-------------

Po <= [ --, ++ ]


Symbol (S)
-------------

S <=  [A-Za-Z ][A-Za-z0-9 ]*  --> token variable
S <=  [0-9]*.[0-9]* | [e|E][-|+][0-9]*  --> token integer or number
S <=  0x[0-9a-fA-F]* --> token hexadecimal
S <=  [0-1]*b --> token binary
S <= "[all characters]" --> token string


Punctuator (P)
-------------

Arithmetic

P <= [  /, *, %, -, + ]

Bit

P <= [  &, |, ^, <<, >> ]

Logical

P <= [  ==, >=, <=, >, <, !=, &&, || ]

Assignments

P <= [  =, +=, -=, /=, %=, *=, <<=, >>=, ^=, |=, &=]

Object

P <= [  . ]

Ternary

P <= [  ?,: ]


Anonymous Objects (O)
-------------

O <= [E1,E2,E3,...,En] 
O <= function(a1,a2,...,an){ B }
O <= {S1:E1,S2:E2,...,Sn:Em}

Expression (E)
-------------

E <= (E) // parentesis E
E <= E P E // punctuator
E <= !E // negate result of expression
E <= -E // Prerest (negate result of expression)
E <= +E // Presum (it does nothing)
E <= [Pe]S[Po] // variable with/out pre/post operator.
E <= (S|O)[E1][E2]...[En] // array access
E <= (S|O)(E1,E2,..,En) // function call
E(t)<= E; | E,


Keyword (K)
-------------

- var S=E;
- for(var S1=Ei1,S2=Ei2,...,Sn=Eim;Ec;Ep1,Ep2,Ep3,...,Epk){ B } //
- for(var S1 in S2) { B } //
- while(Ec) { B }
- do{ B } while(Ec);
- switch(E){ // switch should have constants on case
	default:
	case C0:
	    break;
	case C1:
	    break;
	    ..
	case CN:
	    break;
	}

-if(Ec){ B1 } else if (Ec2) { B2 } else { B3 }


Body (B)
----------

Starts with '{' and ends with '}'

B <- [E;|K]* // A set of expressions ended with ; or Keyword

*/

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
	CEval::tDirectiveInfo CEval::defined_directive[MAX_DIRECTIVES];
	CEval::tOperatorInfo CEval::defined_operator[MAX_OPERATOR_TYPES];
	CEval::tPreOperatorInfo CEval::defined_pre_operator[MAX_PRE_OPERATOR_TYPES];
	CEval::tPrePostIdentityOperatorInfo CEval::defined_pre_post_identity_operator[MAX_PRE_POST_IDENTITY_OPERATOR_TYPES];
	CEval::tSeparatorInfo CEval::defined_separator[MAX_SEPARATOR_TYPES];

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

	char *ADVANCE_TO_END_COMMENT(char *aux_p, int &line){

		if(IS_START_COMMENT(aux_p)){
			aux_p+=2; //advance first
			bool end = false;
			while(*aux_p != 0  && !end){//!IS_END_COMMENT(aux_p) && *aux_p != 0){

				if(*aux_p == '*' && *(aux_p+1) == '/') {
					end=true;
				} // not end comment ... advance ...
				else {
					if(*aux_p=='\n'){
						line++;
					}
					aux_p++;
				}
			}
		}
		return aux_p;
	}

	char *IGNORE_BLANKS(const char *str, int &line) {
		char *aux_p = (char *)str;
		bool end = false;
		while(!end){
			end = true;
			while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

			if(IS_SINGLE_COMMENT(aux_p)) // ignore line
				while(*aux_p!=0 && *aux_p!='\n') aux_p++;

			else if(IS_START_COMMENT(aux_p)){
				// ignore until get the end of the comment...
				aux_p = ADVANCE_TO_END_COMMENT(aux_p, line);

				if(IS_END_COMMENT(aux_p))
					aux_p+=2;

				end=false;
			}
			// make compatible windows format...
			if(*aux_p == '\r')
				aux_p++;

			if(*aux_p == '\n') {
				line=line+1;
				end=false;
				aux_p++;
			}
		}
		return aux_p;
	}

	bool isPlusOperator(const char *s){
		if(*s=='+')
			return ((*(s+1) != '+') && (*(s+1) != '='));
		return false;
	}

	bool isMinusOperator(const char *s){
		if(*s=='-')
			return ((*(s+1) != '-') && (*(s+1) != '='));
		return false;
	}

	bool isMulOperator(const char *s){
		if(*s == '*')
			return ((*(s+1) != '='));
		return false;
	}

	bool isDivOperator(const char *s){
		if(*s == '/')
			return ((*(s+1) != '='));
		return false;
	}

	bool isModOperator(const char *s){
		if(*s == '%')
			return ((*(s+1) != '='));
		return false;
	}

	bool isFieldOperator(const char *s){
		return *s == '.';
	}

	bool isInlineIfOperator(const char *s){
		return *s == '?';
	}

	bool isInlineElseOperator(const char *s){
		return *s == ':';
	}

	bool isAssignOperator(const char *s){
		if(*s=='=')
			return (*(s+1) != '=');
		return false;
	}

	bool isAddAssignOperator(const char *s){
		if(*s=='+')
			return (*(s+1) == '=');
		return false;
	}

	bool isSubAssignOperator(const char *s){
		if(*s=='-')
			return (*(s+1) == '=');
		return false;
	}

	bool isMulAssignOperator(const char *s){
		if(*s=='*')
			return (*(s+1) == '=');
		return false;
	}

	bool isDivAssignOperator(const char *s){
		if(*s=='/')
			return (*(s+1) == '=');
		return false;
	}

	bool isModAssignOperator(const char *s){
		if(*s=='%')
			return (*(s+1) == '=');
		return false;
	}


	bool isBinaryXorOperator(const char *s){
		if(*s == '^')
			return ((*(s+1) != '='));
		return false;
	}

	bool isBinaryAndOperator(const char *s){
		if(*s=='&')
			return ((*(s+1) != '&')  && (*(s+1) != '='));
		return false;
	}

	bool isBinaryOrOperator(const char *s){
		if(*s=='|')
			return ((*(s+1) != '|') &&  (*(s+1) != '='));
		return false;
	}

	bool isShiftLeftOperator(const char *s){
		if(*s=='<')
			return (*(s+1) == '<');
		return false;
	}

	bool isShiftRightOperator(const char *s){
		if(*s=='>')
			return (*(s+1) == '>');
		return false;
	}

	bool isLogicAndOperator(const char *s){
		if(*s=='&')
			return (*(s+1) == '&');
		return false;
	}

	bool isLogicOrOperator(const char *s){
		if(*s=='|')
			return (*(s+1) == '|');
		return false;
	}

	bool isLogicEqualOperator(const char *s){
		if(*s=='=')
			return (*(s+1) == '=');
		return false;
	}

	bool isInstanceOfOperator(const char *s){
		return strncmp("instanceof",s,10) == 0;
	}

	bool isLogicNotEqualOperator(const char *s){
		if(*s=='!')
			return (*(s+1) == '=');
		return false;
	}

	bool isLogicGreatherThanOperator(const char *s){
		if( *s == '>')
			return (*(s+1) != '>');
		return false;
	}

	bool isLogicLessThanOperator(const char *s){
		if(*s == '<')
			return (*(s+1) != '<');

		return false;
	}

	bool isLogicGreatherEqualThanOperator(const char *s){
		if(*s=='>')
			return (*(s+1) == '=');
		return false;
	}

	bool isLessEqualThanOperator(const char *s){
		if(*s=='<')
			return (*(s+1) == '=');
		return false;
	}

	bool isNotOperator(const char *s){
		if(*s=='!')
			return (*(s+1) != '=');
		return false;
	}


	bool isIncOperator(const char *s){
		if(*s=='+')
			return (*(s+1) == '+');
		return false;
	}

	bool isDecOperator(const char *s){
		if(*s=='-')
			return (*(s+1) == '-');
		return false;
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------


	void CEval::init(){


		// init operator punctuators...
		defined_operator[UNKNOWN_OPERATOR]={UNKNOWN_OPERATOR, "none",NULL};

		defined_operator[ADD_OPERATOR]={ADD_OPERATOR, "+",isPlusOperator};
		defined_operator[SUB_OPERATOR]={SUB_OPERATOR, "-",isMinusOperator};
		defined_operator[MUL_OPERATOR]={MUL_OPERATOR, "*",isMulOperator};
		defined_operator[DIV_OPERATOR]={DIV_OPERATOR, "/",isDivOperator};
		defined_operator[MOD_OPERATOR]={MOD_OPERATOR, "%",isModOperator};
		defined_operator[FIELD_OPERATOR]={FIELD_OPERATOR, ".",isFieldOperator};
		defined_operator[TERNARY_IF_OPERATOR]={TERNARY_IF_OPERATOR, "?",isInlineIfOperator};
		defined_operator[TERNARY_ELSE_OPERATOR]={TERNARY_ELSE_OPERATOR, ":",isInlineElseOperator};
		defined_operator[ASSIGN_OPERATOR]={ASSIGN_OPERATOR, "=",isAssignOperator};
		defined_operator[ADD_ASSIGN_OPERATOR]={ADD_ASSIGN_OPERATOR, "+=",isAddAssignOperator};
		defined_operator[SUB_ASSIGN_OPERATOR]={SUB_ASSIGN_OPERATOR, "-=",isSubAssignOperator};
		defined_operator[MUL_ASSIGN_OPERATOR]={MUL_ASSIGN_OPERATOR, "*=",isMulAssignOperator};
		defined_operator[DIV_ASSIGN_OPERATOR]={DIV_ASSIGN_OPERATOR, "/=",isDivAssignOperator};
		defined_operator[MOD_ASSIGN_OPERATOR]={MOD_ASSIGN_OPERATOR, "%=",isModAssignOperator};
		defined_operator[BINARY_XOR_OPERATOR]={BINARY_XOR_OPERATOR, "^",isBinaryXorOperator};
		defined_operator[BINARY_AND_OPERATOR]={BINARY_AND_OPERATOR, "&",isBinaryAndOperator};
		defined_operator[BINARY_OR_OPERATOR]={BINARY_OR_OPERATOR, "|",isBinaryOrOperator};
		defined_operator[SHIFT_LEFT_OPERATOR]={SHIFT_LEFT_OPERATOR, "<<",isShiftLeftOperator};
		defined_operator[SHIFT_RIGHT_OPERATOR]={SHIFT_RIGHT_OPERATOR, ">>",isShiftRightOperator};
		defined_operator[LOGIC_AND_OPERATOR]={LOGIC_AND_OPERATOR, "&&",isLogicAndOperator};
		defined_operator[LOGIC_OR_OPERATOR]={LOGIC_OR_OPERATOR, "||",isLogicOrOperator};
		defined_operator[LOGIC_EQUAL_OPERATOR]={LOGIC_EQUAL_OPERATOR, "==",isLogicEqualOperator};
		defined_operator[LOGIC_NOT_EQUAL_OPERATOR]={LOGIC_NOT_EQUAL_OPERATOR, "!=",isLogicNotEqualOperator};
		defined_operator[LOGIC_GT_OPERATOR]={LOGIC_GT_OPERATOR, ">",isLogicGreatherThanOperator};
		defined_operator[LOGIC_LT_OPERATOR]={LOGIC_LT_OPERATOR, "<",isLogicLessThanOperator};
		defined_operator[LOGIC_GTE_OPERATOR]={LOGIC_GTE_OPERATOR, ">=",isLogicGreatherEqualThanOperator};
		defined_operator[LOGIC_LTE_OPERATOR]={LOGIC_LTE_OPERATOR, "<=",isLessEqualThanOperator};
		defined_operator[INSTANCEOF_OPERATOR]={INSTANCEOF_OPERATOR, "instanceof",isInstanceOfOperator};


		defined_pre_operator[PRE_LOGIC_NOT_OPERATOR]={PRE_LOGIC_NOT_OPERATOR, "!",isNotOperator};
		defined_pre_operator[PRE_ADD_OPERATOR]={PRE_ADD_OPERATOR, "+",isPlusOperator};
		defined_pre_operator[PRE_SUB_OPERATOR]={PRE_SUB_OPERATOR, "-",isMinusOperator};


		defined_pre_post_identity_operator[PRE_POST_IDENTITY_INC_OPERATOR]={PRE_POST_IDENTITY_INC_OPERATOR, "++",isIncOperator};
		defined_pre_post_identity_operator[PRE_POST_IDENTITY_DEC_OPERATOR]={PRE_POST_IDENTITY_DEC_OPERATOR, "--",isDecOperator};


		// special punctuators...
		defined_separator[COMA_SEPARATOR]={COMA_SEPARATOR, ",",NULL};
		defined_separator[SEMICOLON_SEPARATOR]={SEMICOLON_SEPARATOR, ";",NULL};
		defined_separator[OPEN_PARENTHESIS_SEPARATOR]={OPEN_PARENTHESIS_SEPARATOR, "(",NULL};
		defined_separator[CLOSE_PARENTHESIS_SEPARATOR]={CLOSE_PARENTHESIS_SEPARATOR, ")",NULL};
		defined_separator[OPEN_BRAKET_SEPARATOR]={OPEN_BRAKET_SEPARATOR, "{",NULL};
		defined_separator[CLOSE_BRAKET_SEPARATOR]={CLOSE_BRAKET_SEPARATOR, "}",NULL};
		defined_separator[OPEN_SQUARE_BRAKET_SEPARATOR]={OPEN_SQUARE_BRAKET_SEPARATOR, "[",NULL};
		defined_separator[CLOSE_SQUARE_BRAKET_SEPARATOR]={CLOSE_SQUARE_BRAKET_SEPARATOR, "]",NULL};

		// init special punctuators...
		// init keywords...
		defined_keyword[KEYWORD_TYPE::UNKNOWN_KEYWORD] = {UNKNOWN_KEYWORD, "none",NULL};
		defined_keyword[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var",evalKeywordVar};
		defined_keyword[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",evalKeywordIf};
		defined_keyword[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else",NULL};
		defined_keyword[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for",evalKeywordFor};
		defined_keyword[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while",evalKeywordWhile};
		defined_keyword[KEYWORD_TYPE::DO_WHILE_KEYWORD] = {DO_WHILE_KEYWORD,"do",evalKeywordDoWhile}; // while is expected in the end ...

		defined_keyword[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch",evalKeywordSwitch};
		defined_keyword[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case",NULL};
		defined_keyword[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break",NULL};
		defined_keyword[KEYWORD_TYPE::CONTINUE_KEYWORD] = {CONTINUE_KEYWORD,"continue",NULL};
		defined_keyword[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default",NULL};
		defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",NULL};
		defined_keyword[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return",evalKeywordReturn};
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


	CEval::SEPARATOR_TYPE   CEval::evalSeparator(const char *s){

		for(unsigned char i = 1; i < MAX_SEPARATOR_TYPES; i++){

			if(*defined_separator[i].str == *s){
				return defined_separator[i].id;
			}
		}

		return SEPARATOR_TYPE::UNKNOWN_SEPARATOR;
	}

	bool CEval::isEndSeparator(char c){
		return (c==0 || c==';' || c==',' ||  c==')'  || c==']' || c=='}');//|| c==':');
	}



	CEval::OPERATOR_TYPE   CEval::evalOperator(const char *s){

		for(unsigned char i = 1; i < MAX_OPERATOR_TYPES; i++){

			if(*defined_operator[i].str == *s){
				return defined_operator[i].id;
			}
		}
		return OPERATOR_TYPE::UNKNOWN_OPERATOR;
	}

	CEval::PRE_OPERATOR_TYPE   	CEval::evalPreOperator(const char *s){
		for(unsigned char i = 1; i < MAX_PRE_OPERATOR_TYPES; i++){

			if(*defined_pre_operator[i].str == *s){
				return defined_pre_operator[i].id;
			}
		}
		return PRE_OPERATOR_TYPE::UNKNOWN_PRE_OPERATOR;
	}

	CEval::PRE_POST_OPERATOR_IDENTITY_TYPE   CEval::evalPrePostIdentityOperator(const char *s){
		for(unsigned char i = 1; i < MAX_PRE_POST_IDENTITY_OPERATOR_TYPES; i++){

			if(*defined_pre_post_identity_operator[i].str == *s){
				return defined_pre_post_identity_operator[i].id;
			}
		}
		return PRE_POST_OPERATOR_IDENTITY_TYPE::UNKNOWN_PRE_POST_IDENTITY_OPERATOR;
	}


	KEYWORD_TYPE CEval::isKeyword(const char *c){
		// PRE: The first char must be starting symbol.


		char *str=(char *)c;
		char *aux=str;

		for(int i = 0; i < MAX_KEYWORD; i++){
			int size = strlen(defined_keyword[i].str);
			char *aux = str+size;
			if((strncmp(str,defined_keyword[i].str,size)==0) && (
					*aux == 0  || // carry end
					*aux == ' '  || // space

					*aux == '\t'  || // tab
					evalOperator(aux)!=OPERATOR_TYPE::UNKNOWN_OPERATOR ||
					evalSeparator(aux)!=SEPARATOR_TYPE::UNKNOWN_SEPARATOR ||
					*aux == '\n' || // carry return

				   (*aux == '/' && *(aux+1) == '*')) //start block comment
				   ){
				return defined_keyword[i].id;
			}
		}
		return KEYWORD_TYPE::UNKNOWN_KEYWORD;
	}

	DIRECTIVE_TYPE CEval::isDirective(const char *c){
		// PRE: The first char must be starting symbol.
		char *str=(char *)c;

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


	bool CEval::evalLiteralNumber(const char *c, int & line, tTokenNode *token_node, bool & error){
		// PRE: a string given...
		char *aux_p = IGNORE_BLANKS(c,line);
		bool end=false;
		int current_part=0;
		string number_part[3];
		bool is01s=true;
		bool isHexa=(*aux_p == 'x' || *aux_p == 'X') || ((*aux_p == '0' && *(aux_p+1) == 'X') || (*aux_p == '0' && *(aux_p+1) == 'x'));

		if(!('0'<=*aux_p&&*aux_p<='9') || !isHexa || !(*aux_p == '.')){ // is no number directly
			return false;
		}

		// else is candidate to number ...
		do{

			if('0'<=*aux_p&&*aux_p<='9'){
				number_part[current_part]+=*aux_p;
			}
			else if(('a'<=*aux_p&& *aux_p<='f') || ('A'<=*aux_p && *aux_p<='F')){ // hexa ?

				if(isHexa){
						number_part[current_part]+=*aux_p;
				}
				else if(*aux_p == 'e' || *aux_p == 'E'){ // exponencial part ?

					if(current_part==0 || current_part==1){
						current_part=2;
						number_part[current_part]+=*aux_p;
						aux_p=IGNORE_BLANKS(aux_p+1,line);
						if(*(aux_p)=='+' || *(aux_p)=='-'){
							number_part[current_part]+=*aux_p;
							aux_p=IGNORE_BLANKS(aux_p+1,line);
						}
					}
					else{ // error
						 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Invalid number format");
						 error=true;
						 return false;
					}
				}
				else{ // error
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Invalid number format");
					 error=true;
					 return false;
				}

			}
			else if(*aux_p == '.'){ // fraccional part ?

				if(isHexa){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Invalid number format");
						 error=true;
						 return false;
				}

				if(current_part==0){
					current_part++;
				}
				else{ // error
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Invalid number format");
					 error=true;
					 return false;
				}
			}

			else if(*aux_p == 'b'){ // is end binary format?
				if(!is01s || (current_part != 0)){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Invalid number format");
					 error=true;
					 return false;
				}

				number_part[current_part]+=*aux_p;

				end=true;

			}
			else{
				 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Invalid number format");
				 error=true;
				 return false;
			}

			is01s&=(('0'==*aux_p) || ('1'==*aux_p));

			aux_p++;


			//if(evalSpecialPunctuator(aux)==OPERATOR_TYPE::FIELD_OPERATOR);
		}while(!end && !(isEndSymbolToken(aux_p)&&!(*aux_p=='.')));

		// check and eval token ?
		token_node->value = number_part[0]+number_part[1]+number_part[2];

		if(isEndSymbolToken(aux_p)){
			return true;
		}


		return false;

		// POST: detects integer/binary/fractional/hexa
	}



	bool CEval::isEndSymbolToken(char *s, char pre){
		return evalOperator(s)!=OPERATOR_TYPE::UNKNOWN_OPERATOR
			   || evalSeparator(s)!=SEPARATOR_TYPE::UNKNOWN_SEPARATOR
			   || *s==' '
			   || *s==0
			   || (*s=='\"' && pre!='\\');
	}

	// to string utils ...
	char * CEval::evalSymbol(const char *start_word, int line,tTokenNode * token_node){
		// PRE:

		char *start_str=(char *)start_word;
		char *aux=(char *)start_word;
		OPERATOR_TYPE sp;
		KEYWORD_TYPE key_w;
		 bool is_possible_number=false;
		 int i=0;
		 bool error;
		 bool start_digit = false;
		 token_node->token_type=TOKEN_TYPE::UNKNOWN_TOKEN;
		 KEYWORD_TYPE kw=isKeyword(aux);
		 int start_line = line;

		 // first solve embed objects...
		 if(*aux=='['){

		 }else if(*aux=='{'){

		 }else if(evalLiteralNumber(start_word,line,token_node ,error)){



		 }else { // try eval identifier, boolean identifier...

			 if(error){
				 return NULL;
			 }


			 token_node->value="";
			char pre=0;
			bool is_string = false;

			if(*aux=='\"'){
				is_string = true;
				aux++;
			}

			while(!isEndSymbolToken(aux,pre)){
				pre=*aux;
				token_node->value += (*aux++);
			}

			if(*aux==0){
				return NULL;
			}

			if(*aux=='\"'){
				 if(!is_string){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Unexpected '\"'");
					 return NULL;
				 }
				 aux++;
			}

			// compile constant ...


		}


		//value = CZetScriptUtils::copyStringFromInterval(start_word,aux);

		return aux;
	}

	char *  CEval::getIdentifierToken(const char *s, const string & symbol){

		char *aux_p = (char *)s;

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

				return aux_p;
		}


		return NULL;

	}

	bool CEval::isIdentifierNameExpressionOk(const string & symbol, int line){

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
			writeErrorMsg(CURRENT_PARSING_FILENAME,line," Symbol name cannot begin with %c", *aux_p);
			return false;
		}

		return true;
	}


	//------------------------------------------------------------------------------------------------------------


	char * CEval::evalExpressionFunctionObject(const char *s,int & line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op){

		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

		return NULL;
	}


	char * CEval::evalExpressionStructObject(const char *s,int & line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			string symbol_value;
			char *end_p;
			PASTNode attr_node = NULL;
			int lineSymbol;

			if(*aux_p == '{'){ // go for final ...

				// this solve problem void structs...
				aux_p=IGNORE_BLANKS(aux_p+1,line);

				while (*aux_p != '}' && *aux_p != 0){

					lineSymbol = line;

					if((aux_p=getIdentifierToken(aux_p,symbol_value))==NULL){
						 writeErrorMsg(CURRENT_PARSING_FILENAME,lineSymbol ,"Expected symbol");
						 return NULL;

					}

					 aux_p=IGNORE_BLANKS(aux_p,line);

					 if(*aux_p != ':'){ // expected : ...
						 writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ':'");
						 return NULL;
					 }

					 aux_p++;

					 // go to variable...
					 if((aux_p=evalExpression(aux_p,line,scope_info,NULL)) == NULL){  //IGNORE_BLANKS(aux_p+1,line);
						 return NULL;
					 }

					 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...


					 aux_p=IGNORE_BLANKS(aux_p,line);

					 if(*aux_p == ','){
						 aux_p=IGNORE_BLANKS(aux_p+1,line);
					 }
					 else if(*aux_p != '}' ){
						 writeErrorMsg(CURRENT_PARSING_FILENAME,line,"expected '}' or ','");
						 return NULL;
					 }
				}
			}
			else{
				writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '{'");
			}
			return aux_p;
	}


	char * CEval::evalExpressionVectorObject(const char *s,int & line,  CScope *scope_info,  vector<tInfoAsmOpCompiler> *	asm_op){

		char * aux_p=IGNORE_BLANKS(s,line);

		if(*aux_p != '['){
			writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '['");
			return NULL;
		}

		aux_p=IGNORE_BLANKS(aux_p+1,line);
		unsigned v_elements=0;

		while(*aux_p!=0 && *aux_p != ']'){

			aux_p=IGNORE_BLANKS(aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,",");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+1,line);
			}

			aux_p=evalExpression(aux_p,line,scope_info,NULL);

			v_elements++;
		}

		return aux_p;

	}

	char * CEval::evalExpressionNew(const char *s,int & line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;
		string symbol_value;

		KEYWORD_TYPE key_w;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::NEW_KEYWORD){
				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),line);
				// try get symbol ...

				if((aux_p=getIdentifierToken(aux_p,symbol_value))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
					 return NULL;
				}

				 aux_p=IGNORE_BLANKS(aux_p,line);

				 if(*aux_p != '('){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '(' after \'%s\'",defined_keyword[key_w].str);
					 return NULL;
				 }

				 aux_p = evalExpressionArgs('(', ')',aux_p,line,scope_info,asm_op);
				 if(aux_p == NULL){
					 return NULL;
				 }

				return aux_p;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------------------------------------------
	bool CEval::buildAstExpression(tTokenNode **node,vector<tTokenNode> * vExpressionTokens,int idx_start,int idx_end, bool & error){

		OPERATOR_TYPE 	op_split=OPERATOR_TYPE::UNKNOWN_OPERATOR;
		int 			idx_split=-1;
		tTokenNode      *split_node;

		// trivial case (no operator left)...
		if(idx_start>=idx_end){
			return false;
		}

		// search for the most priority operator...
		for(int i=idx_start; i < idx_end; i++){

			if( (vExpressionTokens->at(i).token_type == TOKEN_TYPE::OPERATOR_TOKEN)
				&& (op_split < vExpressionTokens->at(i).operator_type))
			{
				idx_split=i;
				op_split=vExpressionTokens->at(i).operator_type;
			}
		}

		if(idx_split == -1){
			writeErrorMsg(CURRENT_PARSING_FILENAME,-1,"Unexpected error");
			error =true;
			return false;
		}


		// operator that splits...
		*node=&vExpressionTokens->at(idx_split);



		return buildAstExpression(&split_node->left,vExpressionTokens,idx_start,idx_split-1, error) // left branches...
									&&
  		       buildAstExpression(&split_node->right,vExpressionTokens,idx_split+1,idx_end,error); // right branches...
	}

	char * CEval::evalExpression(const char *s, int & line, CScope *scope_info, vector<tInfoAsmOpCompiler> 	* asm_op){
		// PRE: s is current string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.

		vector<tTokenNode> vExpressionTokens;
		PRE_OPERATOR_TYPE pre_op = PRE_OPERATOR_TYPE::UNKNOWN_PRE_OPERATOR;
		OPERATOR_TYPE op = OPERATOR_TYPE::UNKNOWN_OPERATOR;
		tTokenNode *root;
		bool error=false;

		PASTNode ast_node_to_be_evaluated=NULL;
		char *aux_p=IGNORE_BLANKS(s,line);

		while(!isEndSeparator(*aux_p)){

			tTokenNode 	symbol_token_node
						,operator_token_node;

			// check pre operator (-,+,!)...
			switch(pre_op=evalPreOperator(aux_p)){

				default:
					break;
				case PRE_OPERATOR_TYPE::PRE_SUB_OPERATOR:
				case PRE_OPERATOR_TYPE::PRE_ADD_OPERATOR:
				case PRE_OPERATOR_TYPE::PRE_LOGIC_NOT_OPERATOR:
					aux_p+=strlen(defined_operator[pre_op].str);
					break;
			}

			aux_p=IGNORE_BLANKS(aux_p,line);

			// parenthesis (evals another expression)
			if(*aux_p=='('){
				vector<tInfoAsmOpCompiler> 	asm_op_inner;
				if((aux_p=evalExpression(aux_p+1, line, scope_info, &asm_op_inner))==NULL){
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p,line);

				if(*aux_p != ')'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected ')'");
					return NULL;
				}

			}
			else{ // get a symbol is obligated...

				if((aux_p = evalSymbol(aux_p,line,&symbol_token_node))==NULL){
					return NULL;
				}

				vExpressionTokens.push_back(symbol_token_node);
			}

			aux_p=IGNORE_BLANKS(aux_p,line);

			if(!isEndSeparator(*aux_p)){ // a operator is expected!
				op=evalOperator(aux_p);

				if(op==OPERATOR_TYPE::UNKNOWN_OPERATOR){
					writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected operator");
					return NULL;
				}

				operator_token_node.operator_type=op;
				operator_token_node.token_type=TOKEN_TYPE::OPERATOR_TOKEN;

				vExpressionTokens.push_back(operator_token_node);
				aux_p+=strlen(defined_operator[op].str);
			}


			aux_p=IGNORE_BLANKS(aux_p,line);

		}


		// build ast three
		if(vExpressionTokens.size()>=3){ // there're ast three...
			if(!buildAstExpression(&root,&vExpressionTokens,0,vExpressionTokens.size()-1,error)){
				return NULL;
			}
		}else{ // one symbol ? ...
			root = &vExpressionTokens[0];
		}

		// last character is a separator so it return increments by 1
		return aux_p+1;
	}

	//---------------------------------------------------------------------------------------------------------------
	// PARSE OBJECT FUNCTIONS


	//---------------------------------------------------------------------------------------------------------------
	// PARSE KEYWORDS



	char * CEval::evalKeywordDelete(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_p;
		string symbol_value;
		KEYWORD_TYPE key_w;
		PASTNode   value;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::DELETE_KEYWORD){

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),line);


				if((aux_p=getIdentifierToken(aux_p,symbol_value))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
					 return NULL;
				}

				 aux_p=IGNORE_BLANKS(aux_p,line);

				 if(*aux_p != ';'){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ;");
					 return NULL;
				 }

				return aux_p;
			}
		}
		return NULL;
	}

	char * CEval::isClassMember(const char *s,int & line,short & idxScopeClass, bool & error){

		char *aux_p = (char *)s;
		char *end_var;
		string class_name;
		idxScopeClass=-1;
		CScriptClass *sc=NULL;

		error = true;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check whwther the function is anonymous or not.
		if((aux_p=getIdentifierToken(aux_p,class_name))==NULL){
			 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
			 return NULL;
		}

		aux_p=IGNORE_BLANKS(aux_p,line);

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


	char * CEval::evalKeywordClass(const char *s,int & line, CScope *scope_info, bool & error){
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

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::CLASS_KEYWORD){

				if(scope_info->getIdxParent()!=ZS_UNDEFINED_IDX){
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"class keyword is not allowed");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),line);

				// check for symbol's name
				if((aux_p=getIdentifierToken(aux_p,class_name))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
					 return NULL;
				}

				// try to register class...
				class_line = line;


				aux_p=IGNORE_BLANKS(aux_p,line);

				if(*aux_p == ':' ){

					aux_p=IGNORE_BLANKS(aux_p+1,line);

					if((aux_p=getIdentifierToken(aux_p,base_class_name))==NULL){
						 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
						 return NULL;
					}

					aux_p=IGNORE_BLANKS(end_p, line);
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

					aux_p=IGNORE_BLANKS(aux_p+1,line);

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
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected \"var\" or \"function\" keyword");
								return NULL;
								break;
							case KEYWORD_TYPE::FUNCTION_KEYWORD:

								if((aux_p = evalKeywordFunction(aux_p, line,class_scope_info,&local_ifc)) == NULL){
									return NULL;
								}
								break;
							case KEYWORD_TYPE::VAR_KEYWORD:
								if((aux_p = evalKeywordVar(aux_p, line,class_scope_info,NULL,error)) == NULL){
									return NULL;
								}
								break;
							}
						}else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected \"var\" or \"function\" keyword");
							return NULL;
						}
						aux_p=IGNORE_BLANKS(aux_p,line);
					}

					if(*aux_p != '}'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p+1,line);

					if(*aux_p != ';'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,class_line ,"class \"%s\" not end with ;",class_name.c_str());
						return NULL;
					}

					return aux_p+1;

				}else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '{'");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalExpressionArgs(char c1,char c2,const char *s,int & line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		PASTNode   node_arg_expression=NULL;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		if(*aux_p == c1){
			aux_p++;
			aux_p=IGNORE_BLANKS(aux_p,line);


			if(*aux_p != c2 ){
				if(*aux_p == ',' ){
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Unexpected %c",c2);
					return NULL;
				}

				do{
					if((aux_p = evalExpression(aux_p,line,scope_info,asm_op))==NULL){
						return NULL;
					}

					if(*aux_p == ',' ){
						aux_p++;
						aux_p=IGNORE_BLANKS(aux_p,line);
					}else{
						if(*aux_p != c2 ){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected %c",c2);
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

	char * CEval::evalKeywordFunction(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile ** ifc){

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
				aux_p=IGNORE_BLANKS(aux_p,line);


				bool named_function = *aux_p!='(';

				if(named_function){ // is named function..

					if((end_var=isClassMember(aux_p,line, idxScopeClass, error))!=NULL){ // check if particular case extension attribute class

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

							if((end_var=getIdentifierToken(aux_p,function_name)) == NULL){
								 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
								 return NULL;
							}
						}
					}
					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,line);
				}
				else{ //function node
					// TODO manage function object
					printf("TODO manage function object\n");
				}


				// eval function args...
				if(*aux_p == '('){ // push arguments...

					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,line);
					vector<string> arg;


					// grab words separated by ,
					while(*aux_p != 0 && *aux_p != ')'){


						aux_p=IGNORE_BLANKS(aux_p,line);

						if(arg.size()>0){
							if(*aux_p != ','){
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ',' ");
								return NULL;
							}
							aux_p=IGNORE_BLANKS(aux_p+1,line);
						}


						if(*aux_p == ')' || *aux_p == ','){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected arg");
							return NULL;
						}

						int m_start_arg=line;
						if((end_var=getIdentifierToken(aux_p,arg_value)) == NULL){
							 writeErrorMsg(CURRENT_PARSING_FILENAME,line ,"Expected symbol");
							 return NULL;
						}



						// check if repeats...
						for(unsigned k = 0; k < arg.size(); k++){
							if(arg[k] == arg_value){
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Repeated argument '%s' argument ",arg_value.c_str());
								return NULL;
							}
						}

						// check whether parameter name's matches with some global variable...
						if((irv=body_scope->getInfoRegisteredSymbol(symbol_value,-1,false)) != NULL){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Ambiguous symbol argument \"%s\" name with var defined at %i", symbol_value, -1);
							return NULL;
						}
							// ok register symbol into the object function ...
						arg.push_back(symbol_value);

						aux_p=end_var;
						aux_p=IGNORE_BLANKS(aux_p,line);

						if(*aux_p != ')'){


						}

					}


					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,line);

					if(*aux_p != '{'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '{'");
						return NULL;
					}

					// ok let's go to body..
					if((aux_p = evalBlock(
							aux_p,
							line,
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
											writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Function name \"%s\" is already defined with same args at %s:%i", function_name.c_str(),GET_AST_FILENAME_LINE(irv->idxAstNode));
										}else{
											writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Function name \"%s\" is no allowed it has conflict with name of already registered function in C/C++", function_name.c_str());
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
					writeErrorMsg(CURRENT_PARSING_FILENAME,line," Expected '('");
				}
			}else{
				writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected operator or function operator");
			}
		}
		return NULL;
	}

	char *  CEval::evalKeywordReturn(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		string s_aux;

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...
				PASTNode child_node=NULL;
				aux_p += strlen(defined_keyword[key_w].str);


				aux_p=IGNORE_BLANKS(aux_p,line);


				if((aux_p = evalExpression(aux_p, line, scope_info,&ifc->asm_op))!= NULL){


					if(*aux_p!=';'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ';'");
						return NULL;
					}
					aux_p++;
					return aux_p;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordWhile(const char *s,int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

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
				aux_p=IGNORE_BLANKS(aux_p,line);
				if(*aux_p == '('){

					if((end_expr = evalExpression(aux_p+1,line,_currentScope,&ifc->asm_op)) != NULL){

						if(*end_expr != ')'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')'");
							return NULL;
						}
						if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
							return NULL;
						}


						aux_p=IGNORE_BLANKS(end_expr+1,line);
						if(*aux_p != '{'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected while-block open block ('{') ");
							return NULL;
						}
						if((aux_p=evalBlock(aux_p
								,line
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
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')' while ");
						return NULL;
					}

				}else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '(' while ");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordDoWhile(const char *s,int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

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
				aux_p=IGNORE_BLANKS(aux_p,line);
				if(*aux_p != '{'){
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected open block ('{') in do-while expression");
					return NULL;
				}
				if((aux_p=evalBlock(aux_p
						,line
						,_currentScope
						,ifc
						,error

						))!= NULL){
					if(!error){

						// Finally evaluate conditional line ...
						aux_p=IGNORE_BLANKS(aux_p,line);

						// check for keyword ...
						key_w = isKeyword(aux_p);

						if(key_w!=WHILE_KEYWORD){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"expected while keyword");
							return NULL;
						}

						aux_p += strlen(defined_keyword[key_w].str);

						aux_p=IGNORE_BLANKS(aux_p,line);

						if(*aux_p == '('){

							if((end_expr = evalExpression(aux_p+1,line,_currentScope,&ifc->asm_op)) != NULL){

								if(*end_expr != ')'){
									writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')'");
									return NULL;
								}
								if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
									return NULL;
								}


							}else{
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')' do-while expression");
								return NULL;
							}

						}else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '(' do-while expression");
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

	char * CEval::evalKeywordIf(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

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
					aux_p=IGNORE_BLANKS(aux_p,line);

					if(*aux_p != '('){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '(' if");
						return NULL;
					}

					conditional_line=line;

					if((end_expr = evalExpression(aux_p+1,line,scope_info,&ifc->asm_op)) == NULL){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')' if ");
						return NULL;
					}

					if(*end_expr != ')'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')'");
						return NULL;
					}

					if(IGNORE_BLANKS(aux_p+1,dl)==end_expr){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"no conditional expression");
						return NULL;
					}

					if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					conditional_str=start_symbol;

					aux_p=IGNORE_BLANKS(end_expr+1,line);
					if(*aux_p != '{'){

						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected if-block open block ('{')");
						return NULL;
					}


					if((aux_p=evalBlock(aux_p
							,line
							,scope_info
							,ifc
							,error
							))== NULL){
						return NULL;
					}

					if(error){
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p,line);

					bool else_key = false;
					if((key_w = isKeyword(aux_p)) != KEYWORD_TYPE::UNKNOWN_KEYWORD){
						else_key = (key_w == KEYWORD_TYPE::ELSE_KEYWORD);
					}

					if(else_key){
						aux_p += strlen(defined_keyword[key_w].str);

						if(*aux_p != '{'){
							aux_p++;
						}

						aux_p=IGNORE_BLANKS(aux_p,line);

						bool if_key = false;
						if((key_w = isKeyword(aux_p)) != KEYWORD_TYPE::UNKNOWN_KEYWORD){
							if_key = (key_w == KEYWORD_TYPE::IF_KEYWORD);
						}

						if(!if_key){

							if(*aux_p != '{'){
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected else-block open block ('{')");
								return NULL;
							}

							if((aux_p=evalBlock(aux_p
									,line
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

	char * CEval::evalKeywordFor(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

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
				aux_p=IGNORE_BLANKS(aux_p,line);

				if(*aux_p == '('){ // ready ...

					// save scope pointer ...
					_currentScope =scope_info->pushScope(); // push current scope


					aux_p=IGNORE_BLANKS(aux_p+1,line);

					if(*aux_p != ';'){ // there's some var init...
						// init node ...
						KEYWORD_TYPE key_w = isKeyword(aux_p);

						if(key_w == VAR_KEYWORD){
							if((aux_p = evalKeywordVar(aux_p,line, _currentScope,ifc,error))==NULL){
								return NULL;
							}

							aux_p = aux_p - 1; // redirect aux_p to ';'
						}
						else{

							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected 'var' keyword");
							return NULL;
						}

					}

					aux_p=IGNORE_BLANKS(aux_p,line);

					key_w = isKeyword(aux_p);
					if(key_w == KEYWORD_TYPE::IN_KEYWORD){

						PASTNode node_for_in_right_op_expression=NULL;

						aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[KEYWORD_TYPE::IN_KEYWORD].str),line);


						if((aux_p = evalExpression((const char *)aux_p,line,_currentScope,&ifc->asm_op)) == NULL){
							return NULL;
						}
					}

					else{ // expects conditional and post (i.e for(;;) )


						if(*aux_p != ';'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ';'");
							return NULL;

						}

						aux_p=IGNORE_BLANKS(aux_p+1,line);

						if(*aux_p != ';'){ // conditional...
							char * end_p=IGNORE_BLANKS(aux_p+1,line);

							if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

								if((aux_p = evalExpression((const char *)aux_p,line,_currentScope,&ifc->asm_op)) == NULL){
									return NULL;
								}

							}
						}

						aux_p=IGNORE_BLANKS(aux_p,line);

						if(*aux_p != ';'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ';'");
							return NULL;

						}

						aux_p=IGNORE_BLANKS(aux_p+1,line);


						if(*aux_p != ')' ){ // finally do post op...

							if(*aux_p == ',' ){
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Unexpected ) ");
								return NULL;
							}

							do{
								if((aux_p = evalExpression(aux_p,line,_currentScope,&ifc->asm_op))==NULL){
									return NULL;
								}


								if(*aux_p == ',' ){
									aux_p=IGNORE_BLANKS(aux_p+1,line);
								}else{
									if(*aux_p != ')' ){
										writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')'");
										return NULL;
									}
								}

							}while(*aux_p != ')' && *aux_p != 0);
						}
					}

					if(*aux_p != ')'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')'");
						return NULL;

					}

					aux_p=IGNORE_BLANKS(aux_p+1,line);
					if(*aux_p != '{'){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '{' for-block");
						return NULL;
					}

					// eval block ...
					if((aux_p=evalBlock(aux_p
							,line
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
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '(' for");
					return NULL;
				}
			}
		}
		return NULL;
	}


	char * CEval::evalKeywordSwitch(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_symbol,*start_symbol;
		PASTNode switch_node=NULL,
					 group_cases=NULL,
					 case_value_node=NULL,
					 default_switch_node=NULL;

		CScope *scope_case=NULL;
		PASTNode body_switch=NULL;

		OPERATOR_TYPE ip;
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
				aux_p=IGNORE_BLANKS(aux_p,line);

				if(*aux_p == '('){
						aux_p=IGNORE_BLANKS(aux_p+1,line);

						// evaluate switch vale expression ...
						PASTNode condition_expression_to_evaluate = NULL;
						//static char * evalExpression_Recursive(const char *s, int & line, CScope *scope_info, PASTNode *ast_node_to_be_evaluated=NULL,GROUP_TYPE type_group=GROUP_TYPE::GROUP_0,PASTNode parent=NULL);
						if((aux_p = CEval::evalExpression(
								aux_p,
								line,
								scope_info,
								&ifc->asm_op
								))==NULL)
						{
							return NULL;
						}


						if(*aux_p != ')'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ')' switch");
							error = true;
							return NULL;
						}

						aux_p=IGNORE_BLANKS(aux_p+1,line);

						if(*aux_p == '{'){

							aux_p++;

							if((aux_p=eval_Recursive(aux_p, line, currentScope,ifc, error))==NULL){
								return NULL;
							}

							aux_p=IGNORE_BLANKS(aux_p,line);

							if(*aux_p != '}'){
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '}' switch");
								return NULL;
							}


							scope_info->popScope();


							return aux_p+1;
						}
						else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '{' switch");
							return NULL;
						}
				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected '(' switch ");
					return NULL;
				}
			}
		}
		return NULL;
	}


	char * CEval::evalKeywordVar(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

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
				aux_p=IGNORE_BLANKS(aux_p,line);


				while(*aux_p != ';' && *aux_p != 0 && !end){ // JE: added multivar feature.

					bool is_class_member=parent_scope_is_class;

					aux_p=IGNORE_BLANKS(aux_p,line);
					start_var=aux_p;
					m_startLine = line;
					vars_collection_node=NULL;

					//parent_eval_to_insert_var=NULL;
					if(scope_info != NULL){ // main as default
						idxScope=scope_info->getCurrentScopePointer()->idxScope;
					}

					if((end_var=isClassMember(aux_p,line,idxScopeClass, error))!=NULL){ // check if particular case extension attribute class
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

							line = m_startLine;

							// check whwther the function is anonymous with a previous arithmetic operation ....
							if((end_var=getIdentifierToken(aux_p,variable_name))==NULL){
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected symbol");
								return NULL;
							}
						}
					}

					KEYWORD_TYPE keyw = isKeyword(variable_name.c_str());

					if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Cannot use symbol name as reserverd symbol \"%s\"",defined_keyword[keyw].str);
						return NULL;
					}

					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,line);
					//}
					bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;
					if(is_class_member && *aux_p == '='){
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Variable member is not assignable on its declaration. Should be initialized within constructor.");
						return NULL;
					}

					if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
						allow_for_in=false;
						var_node = NULL;


						if(*aux_p == '='){ // only for variables (not class members)

							PASTNode children_node=NULL;


							// try to evaluate expression...
							aux_p=IGNORE_BLANKS(aux_p,line);

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

							line = m_startLine;
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
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"'in' keyword should be used with an uninitialized variable (example: for ( var e in v) {...} )", *aux_p);
								return NULL;
							}
							end=true;
						}
						else{
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"unexpected '%c'", *aux_p);
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
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected ';'");
					return NULL;
				}

				return aux_p;
			}
		}
		return NULL;
	}

	char * CEval::evalBlock(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;

		//CScope *_localScope =  scope_info != NULL ? scope_info->symbol_info.ast->scope_info_ptr: NULL;
		CScope *currentScope=  NULL;
		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		if(*aux_p == '{'){
			aux_p++;

			currentScope =scope_info->getCurrentScopePointer();
			currentScope = scope_info->pushScope(); // special case... ast is created later ...


			if((aux_p = eval_Recursive(aux_p, line, currentScope,ifc,error)) != NULL){
				if(error){
					return NULL;
				}

				if(*aux_p != '}'){
					error = true;
					writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected } ");
					return NULL;
				}


				scope_info->popScope();

				return aux_p+1;
			}
		}
		return NULL;
	}

	char *CEval::evalBreak(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"

		char *aux_p=(char *)s;

		fprintf(stderr,"find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n");

		{ // ok break is valid in current scope...

			writeErrorMsg(CURRENT_PARSING_FILENAME,line,"\"break\" allowed within loop or case-switch statements");
			error = true;
			return NULL;
		}


		if(*aux_p != ';'){
			writeErrorMsg(CURRENT_PARSING_FILENAME,line,"expected ';'");
			error = true;
			return NULL;
		}
	}

	char *CEval::evalContinue(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
		char *aux_p=(char*)s;
		{ // ok break is valid in current scope...

			writeErrorMsg(CURRENT_PARSING_FILENAME,line,"\"continue\" allowed within loop or case-switch statements");
			error = true;
			return NULL;
		}


		if(*aux_p != ';'){
			writeErrorMsg(CURRENT_PARSING_FILENAME,line,"expected ';'");
			error = true;
			return NULL;
		}
	}

	char *CEval::evalDefaultCase(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){
		char *aux_p=(char *)s;
		string value_to_eval;
		tTokenNode token_node;

		vector<tInfoAsmOpCompiler> *tokenCompiled = NULL;

		aux_p=IGNORE_BLANKS(aux_p, line);

		KEYWORD_TYPE keyw = isKeyword(aux_p);

		if(keyw == KEYWORD_TYPE::CASE_KEYWORD){ // a keyword was detected...


			aux_p=IGNORE_BLANKS(aux_p,line);

			// get the symbol...
			if(*aux_p=='-'){
				// INSERT NEGATE OP.
			}

			aux_p++;

			aux_p=evalSymbol(aux_p,line,&token_node);

			if(aux_p==NULL){ return NULL;}

		}

		aux_p=IGNORE_BLANKS(aux_p,line);

		if(*aux_p != ':'){
			writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Expected  ':' ");
			return NULL;
		}



		return aux_p+1;
	}


	char *CEval::evalKeyword(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p= (char *)s;
		tInfoFunctionCompile *ifc_new = NULL;

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD,keyw2nd=KEYWORD_TYPE::UNKNOWN_KEYWORD;

		aux_p=IGNORE_BLANKS(aux_p, line);

		// check if condition...
		keyw = isKeyword(aux_p);

		if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...

			switch(keyw){
			case CASE_KEYWORD:
			case DEFAULT_KEYWORD:
				return evalDefaultCase(s,line,scope_info,ifc,error);

			case FUNCTION_KEYWORD:
				if((aux_p = evalKeywordFunction(s,line,scope_info,&ifc_new)) != NULL){
					return aux_p;
				}
				error = true;
				return NULL;
			case CLASS_KEYWORD:
				if((aux_p = CEval::evalKeywordClass(s,line,scope_info,error)) != NULL){
					return aux_p;
				}
				error = true;
				return NULL;

			default:

				if(defined_keyword[keyw].eval_fun != NULL){

					return  defined_keyword[keyw].eval_fun(s,line,scope_info,ifc,error);
				}
				writeErrorMsg(CURRENT_PARSING_FILENAME,line,"Not implemented");
				error = true;
				return NULL;
			}

			// something wrong was happen..
		}

		return NULL;
	}

	char * CEval::eval_Recursive(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error){

		// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD;
		bool custom_quit = false;
		char *aux = (char *)s;
		char *end_expr=0;
		PASTNode children=NULL;

		bool processed_directive=false;


		aux=IGNORE_BLANKS(aux, line);

		while(*aux != 0 && !custom_quit){

			processed_directive=false;
			children = NULL;
			// ignore all ;
			while(*aux==';' && *aux != 0){
				aux =IGNORE_BLANKS(aux+1, line);
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
						aux = IGNORE_BLANKS(aux,line);
						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"expected starting \" directive");
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						aux++;
						start_var=aux;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILENAME,line,"expected end \" directive");
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
								writeErrorMsg(CURRENT_PARSING_FILENAME,line,"\"%s\" already evald",file_to_eval.c_str());
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
						writeErrorMsg(CURRENT_PARSING_FILENAME,line,"directive \"%s\" not supported",defined_directive[directive].str);
						break;
					}

					processed_directive = true;
					end_expr=aux;
				}
			}

			// 0st special case member class extension ...
			if(children==NULL && !processed_directive){ // not processed yet ...
				// 1st. check whether eval a keyword...
				if((end_expr = evalKeyword(aux, line, scope_info,ifc, error)) == NULL){

					// If was unsuccessful then try to eval expression.
					if(error){

						THROW_SCRIPT_ERROR();
						return NULL;
					}
					// 2nd. check whether eval a block
					if((end_expr = evalBlock(aux
							,line
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
						int starting_expression=line;

						if((end_expr = evalExpression(aux,line, scope_info,&ifc->asm_op)) == NULL){ // something wrong was happen.
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
			aux=IGNORE_BLANKS(aux, line);
		}
		return aux;
	}

}

