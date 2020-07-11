namespace zetscript{
	namespace eval{

		class EvalData;

		typedef enum
			:unsigned char {
			KEYWORD_TYPE_UNKNOWN = 0,
			KEYWORD_TYPE_IF,
			KEYWORD_TYPE_ELSE,
			KEYWORD_TYPE_FOR,
			//FOREACH_KEYWORD,
			KEYWORD_TYPE_IN,
			KEYWORD_TYPE_WHILE,
			KEYWORD_TYPE_DO_WHILE,
			KEYWORD_TYPE_VAR,
			KEYWORD_TYPE_SWITCH,
			KEYWORD_TYPE_CASE,
			KEYWORD_TYPE_DEFAULT,
			KEYWORD_TYPE_BREAK,
			KEYWORD_TYPE_CONTINUE,
			KEYWORD_TYPE_RETURN,
			KEYWORD_TYPE_FUNCTION,
			KEYWORD_TYPE_CLASS,
			KEYWORD_TYPE_THIS,
			//	SUPER_KEYWORD,
			KEYWORD_TYPE_NEW,
			KEYWORD_TYPE_DELETE,
			KEYWORD_TYPE_MAX
		}KeywordType;

		typedef enum
			:unsigned char {
			DIRECTIVE_TYPE_UNKNOWN = 0,
			DIRECTIVE_TYPE_INCLUDE,
			DIRECTIVE_TYPE_MAX
		}DirectiveType;

		// In a expression we can find three types of tokens: an identifier, a separator, an operator, a literal or object token.
		typedef enum :char{
			TOKEN_TYPE_UNKNOWN=0,
			TOKEN_TYPE_IDENTIFIER,  // a,b,c,d
			TOKEN_TYPE_LITERAL, // true, false, 0.1, -12e-12
			TOKEN_TYPE_OPERATOR, // +,-,%, ...
			TOKEN_TYPE_SUBEXPRESSION,
			TOKEN_TYPE_VECTOR,
			TOKEN_TYPE_DICTIONARY,
			TOKEN_TYPE_FUNCTION_OBJECT,
			TOKEN_TYPE_NEW_OBJECT, // =new ob(); | op (new obj()) op
			TOKEN_TYPE_MAX
		}TokenType;


		typedef enum:char{
			ACCESSOR_TYPE_UNKNOWN=0,
			ACCESSOR_TYPE_CALL, // v(1)
			ACCESSOR_TYPE_VECTOR, // v[0]
			ACCESSOR_TYPE_MEMBER, // a.b.c
			ACCESSOR_TYPE_MAX
		}AccessorType;

		typedef enum :unsigned char {

			OPERATOR_TYPE_UNKNOWN = 0,

			// ASSIGN
			OPERATOR_TYPE_ASSIGN, 				// =
			OPERATOR_TYPE_ASSIGN_ADD, 			// +=
			OPERATOR_TYPE_ASSIGN_SUB, 			// -=
			OPERATOR_TYPE_ASSIGN_MUL, 			// *=
			OPERATOR_TYPE_ASSIGN_DIV, 			// /=
			OPERATOR_TYPE_ASSIGN_MOD, 			// %=
			OPERATOR_TYPE_ASSIGN_BINARY_XOR,	// ^=
			OPERATOR_TYPE_ASSIGN_BINARY_AND,	// &=
			OPERATOR_TYPE_ASSIGN_BINARY_OR,  	// |=
			OPERATOR_TYPE_ASSIGN_SHIFT_LEFT, 	// <<=
			OPERATOR_TYPE_ASSIGN_SHIFT_RIGHT, 	// >>=

			// LOGIC
			OPERATOR_TYPE_LOGIC_AND, 			// &&
			OPERATOR_TYPE_LOGIC_OR, 			// ||

			// RELATIONAL
			OPERATOR_TYPE_LOGIC_EQUAL, 			// ==
			OPERATOR_TYPE_LOGIC_NOT_EQUAL, 		// !=
			OPERATOR_TYPE_LOGIC_GTE, 			// >=
			OPERATOR_TYPE_LOGIC_LTE, 			// <=
			OPERATOR_TYPE_LOGIC_GT, 			// >
			OPERATOR_TYPE_LOGIC_LT, 			// <

			// ARITHMETIC
			OPERATOR_TYPE_ADD, 					// +
			OPERATOR_TYPE_OR, 			// |
			OPERATOR_TYPE_XOR, 			// ^
			OPERATOR_TYPE_SUB, 					// -
			OPERATOR_TYPE_MUL, 					// *
			OPERATOR_TYPE_BINARY_AND, 			// &
			OPERATOR_TYPE_DIV, 					// /
			OPERATOR_TYPE_MOD, 					// %
			OPERATOR_TYPE_SHIFT_LEFT, 			// <<
			OPERATOR_TYPE_SHIFT_RIGHT, 			// >>

			OPERATOR_TYPE_INSTANCEOF, 			// instanceof

			OPERATOR_TYPE_TERNARY_IF, 			// ?
			OPERATOR_TYPE_TERNARY_ELSE, 		// :
			OPERATOR_TYPE_MAX
		}OperatorType;


		typedef enum :unsigned char {
			PRE_OPERATOR_TYPE_UNKNOWN=0,
			PRE_OPERATOR_TYPE_NOT, 				// !
			PRE_OPERATOR_TYPE_ADD, 				// -
			PRE_OPERATOR_TYPE_SUB	, 			// +
			PRE_OPERATOR_TYPE_MAX
		}PreOperatorType;

		typedef enum :unsigned char {
			IDENTITY_OPERATOR_TYPE_UNKNOWN=0,
			IDENTITY_OPERATOR_TYPE_INC,	// ++
			IDENTITY_OPERATOR_TYPE_DEC,	// --
			IDENTITY_OPERATOR_TYPE_MAX
		}IdentityOperatorType;

		typedef enum :unsigned char {
			SEPARATOR_TYPE_UNKNOWN=0,
			SEPARATOR_TYPE_COMA,					// ,
			SEPARATOR_TYPE_SEMICOLON,    		// ;
			SEPARATOR_TYPE_PARENTHESIS_OPEN, 	// (
			SEPARATOR_TYPE_PARENTHESIS_CLOSE, 	// )
			SEPARATOR_TYPE_SQUARE_BRAKET_OPEN, 	// [
			SEPARATOR_TYPE_SQUARE_BRAKET_CLOSE, 	// ]
			SEPARATOR_TYPE_MAX
		}SeparatorType;

		struct EvaluatedInstruction:Instruction{

			LinkSymbolFirstAccess			link_symbol_first_access;
			InstructionSourceInfo 			instruction_source_info;

			EvaluatedInstruction(ByteCode _byte_code
						 ,unsigned char _index_op1=ZS_UNDEFINED_IDX
						 ,intptr_t _index_op2=ZS_UNDEFINED_IDX
						 ,unsigned short _properties=0
						 ):Instruction(_byte_code,_index_op1,_index_op2,_properties){
			}
		};

		struct TokenNode{

			TokenType	  		token_type; // can be operator, literal, identifier, object. (separator are not take account)
			PreOperatorType   	pre_operator_type; // !,+,-
			OperatorType  		operator_type;
			IdentityOperatorType  identity_pre_operator_type; // ++i,--i
			IdentityOperatorType  identity_post_operator_type; // i++,i--

			std::string value; // first access
			int line;
			std::vector<EvaluatedInstruction> evaluated_instructions; // byte code load literal/identifier(can be anonymous function), std::vector/struct.
			//std::map<short,ScriptFunction::InstructionSourceInfo> instruction_source_info;
			// access info like function call, std::vector access and variable memeber
			//std::vector<tTokenNodeAccessor> accessor;

			// AST info operator.
			TokenNode *token_node_left;
			TokenNode *token_node_right;

			TokenNode(){
				line=-1;

				token_type=TokenType::TOKEN_TYPE_UNKNOWN;
				operator_type=OperatorType::OPERATOR_TYPE_UNKNOWN;
				token_node_left=token_node_right=NULL;
				pre_operator_type=PreOperatorType::PRE_OPERATOR_TYPE_UNKNOWN;
				identity_pre_operator_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
				identity_post_operator_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
			}
		};

		typedef struct {
			std::vector<EvaluatedInstruction> evaluated_instruction;
		}ContinueInstructionScope,BreakInstructionScope;

		struct EvaluatedFunction{

			std::vector<EvaluatedInstruction>	 	evaluated_instructions;
			ScriptFunction 						*  	script_function;

			EvaluatedFunction(ScriptFunction	* _script_function){
				script_function = _script_function;
			}
		};

		typedef struct {
			DirectiveType id;
			const char *str;
			//char * (*parse_fun)(const char *, int &, Scope *, PASTNode *);
		} EvalInfoDirective;


		typedef struct {
			KeywordType id;
			const char *str;
			char * (* eval_fun)(EvalData *eval_data,const char *, int &, Scope *, bool &);
		} EvalInfoKeyword;

		typedef struct {
			OperatorType id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalInfoOperator;

		typedef struct {
			PreOperatorType id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalInfoPreOperator;

		typedef struct {
			IdentityOperatorType id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalInfoIdentityOperator;


		typedef struct {
			SeparatorType id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalInfoSeparator;

		struct EvalData{
			ZetScript 						* 		zs;
			ScopeFactory 					* 		scope_factory;
			ScriptFunctionFactory 			* 		script_function_factory;
			ScriptClassFactory 				* 		script_class_factory;
			EvaluatedFunction				* 		evaluated_function_current;
			std::vector<EvaluatedFunction *> 	  	evaluated_functions;
			const char 						* 		current_parsing_file;
			std::map<std::string,std::string *>	 	compiled_symbol_name;
			bool							  		error;
			std::string								error_str;

			EvalData(ZetScript * _zs){
				current_parsing_file=NULL;
				evaluated_function_current=NULL;
				this->zs=_zs;
				this->script_function_factory=zs->getScriptFunctionFactory();
				this->scope_factory=zs->getScopeFactory();
				this->script_class_factory=zs->getScriptClassFactory();
				error=false;
				error_str="";

			}
		};

		EvalInfoOperator eval_info_operators[OPERATOR_TYPE_MAX];
		EvalInfoPreOperator eval_info_pre_operators[PRE_OPERATOR_TYPE_MAX];
		EvalInfoIdentityOperator eval_info_identity_operators[IDENTITY_OPERATOR_TYPE_MAX];
		EvalInfoSeparator eval_info_separators[SEPARATOR_TYPE_MAX];
		EvalInfoKeyword eval_info_keywords[KEYWORD_TYPE_MAX];
		EvalInfoDirective eval_info_directives[DIRECTIVE_TYPE_MAX];

		bool g_init_eval=false;

		char * 	evalKeywordTypeFunction(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeVar(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeIf(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeWhile(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeDoWhile(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeReturn(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeSwitch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);
		char * 	evalKeywordTypeFor(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool & error);


		bool	isOperatorTypeTernaryIf(const char *s)			{return ((*s=='?'));}
		bool 	isOperatorTypeTernaryElse(const char *s)		{return ((*s==':'));}
		bool 	isOperatorAdd(const char *s)					{return	((*s=='+') && (*(s+1)!='+') && (*(s+1)!='='));}
		bool 	isOperatorTypeSub(const char *s)				{return	((*s=='-') && (*(s+1)!='-') && (*(s+1)!='='));}
		bool 	isOperatorTypeMul(const char *s)				{return ((*s=='*') && (*(s+1)!='='));}
		bool 	isOperatorTypeDiv(const char *s)				{return ((*s=='/') && (*(s+1)!='='));}
		bool 	isOperatorTypeMod(const char *s)				{return ((*s=='%') && (*(s+1)!='='));}
		bool 	isOperatorTypeAssign(const char *s)				{return	((*s=='=') && (*(s+1)!='='));}
		bool 	isOperatorTypeAssignAdd(const char *s)			{return ((*s=='+') && (*(s+1)=='='));}
		bool 	isOperatorTypeAssignSub(const char *s)			{return ((*s=='-') && (*(s+1)=='='));}
		bool 	isOperatorTypeAssignMul(const char *s)			{return ((*s=='*') && (*(s+1)=='='));}
		bool 	isOperatorTypeAssignDiv(const char *s)			{return ((*s=='/') && (*(s+1)=='='));}
		bool 	isOperatorTypeAssignMod(const char *s)			{return ((*s=='%') && (*(s+1)=='='));}
		bool 	isOperatorTypeAssignXor(const char *s)			{return ((*s=='^') && (*(s+1)=='='));}		// ^=
		bool 	isOperatorTypeAssignBinaryAnd(const char *s)	{return ((*s=='&') && (*(s+1)=='='));}		// &=
		bool 	isOperatorTypeAssignBinaryOr(const char *s)		{return ((*s=='|') && (*(s+1)=='='));}  	// |=
		bool 	isOperatorTypeAssignShiftLeft(const char *s)	{return ((*s=='<') && (*(s+1)=='<')&& (*(s+2)=='='));} 	// <<=
		bool 	isOperatorTypeAssignShiftRight(const char *s)	{return ((*s=='>') && (*(s+1)=='>')&& (*(s+2)=='='));} 	// >>=
		bool 	isOperatorTypeXor(const char *s)				{return ((*s=='^') && (*(s+1)!='='));}
		bool 	isOperatorTypeBinaryAnd(const char *s)			{return ((*s=='&') && (*(s+1)!='&') && (*(s+1)!='='));}
		bool 	isOperatorTypeBinaryOr(const char *s)			{return ((*s=='|') && (*(s+1)!='|') && (*(s+1)!='='));}
		bool 	isOperatorTypeShiftLeft(const char *s)			{return ((*s=='<') && (*(s+1)=='<') && (*(s+2)!='='));}
		bool 	isOperatorTypeShiftRight(const char *s)			{return	((*s=='>') && (*(s+1)=='>') && (*(s+2)!='='));}
		bool 	isOperatorTypeLogicAnd(const char *s)			{return ((*s=='&') && (*(s+1)=='&'));}
		bool 	isOperatorTypeLogicOr(const char *s)			{return ((*s=='|') && (*(s+1)=='|'));}
		bool 	isOperatorTypeLogicEqual(const char *s) 		{return ((*s=='=') && (*(s+1)=='='));}
		bool 	isOperatorTypeLogicNotEqual(const char *s)		{return ((*s=='!') && (*(s+1)=='='));}
		bool 	isOperatorTypeLogicGt(const char *s)			{return ((*s=='>') && (*(s+1)!='>'));}
		bool 	isOperatorTypeLogicLt(const char *s)			{return ((*s=='<') && (*(s+1)!='<'));}
		bool 	isOperatorTypeLogicGte(const char *s)			{return ((*s=='>') && (*(s+1)=='='));}
		bool 	isOperatorTypeLogicLte(const char *s)			{return ((*s=='<') && (*(s+1)=='='));}
		bool 	isOperatorTypeLogicNot(const char *s)			{return ((*s=='!') && (*(s+1)!='='));}
		bool 	isOperatorTypeInc(const char *s)				{return ((*s=='+') && (*(s+1)=='+'));}
		bool 	isOperatorTypeDec(const char *s)				{return ((*s=='-') && (*(s+1)=='-'));}
		bool 	isCommentSingleLine(char *s)					{return	((*s=='/') && (*(s+1)=='/'));}
		bool 	isCommentBlockStart(char *s)					{return ((*s=='/') && (*(s+1)=='*'));}
		bool 	isCommentBlockEnd(char *s)						{return ((*s=='*') && (*(s+1)=='/'));}
		bool 	isOperatorTypeInstanceOf(const char *s)			{return strncmp("instanceof",s,10) == 0;}
		bool    isEndExpression(const char * s){
			return *s==')' || *s==','||  *s==']' ||  *s==']' ||  *s==';' || *s == 0;
		}

		char *advanceToEndBlockComment(char *aux_p, int &line){

			if( isCommentBlockStart(aux_p)){
				aux_p+=2; //advance first
				bool end = false;
				while(*aux_p != 0  && !end){//! isCommentBlockEnd(aux_p) && *aux_p != 0){

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

		char *ignoreBlanks(const char *str, int &line) {
			char *aux_p = (char *)str;
			bool end = false;
			while(!end){
				end = true;
				while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

				if( isCommentSingleLine(aux_p)) // ignore line
					while(*aux_p!=0 && *aux_p!='\n') aux_p++;

				else if( isCommentBlockStart(aux_p)){
					// ignore until get the end of the comment...
					aux_p = advanceToEndBlockComment(aux_p, line);

					if( isCommentBlockEnd(aux_p))
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

		bool  isLiteral(const std::string & symbol){
			return symbol=="true" || symbol == "false" || symbol=="undefined" || symbol == "null";
		}


		SeparatorType   isSeparatorType(const char *s){
			for(unsigned char i = 1; i < SEPARATOR_TYPE_MAX; i++){
				if(*eval_info_separators[i].str == *s){
					return eval_info_separators[i].id;
				}
			}
			return SeparatorType::SEPARATOR_TYPE_UNKNOWN;
		}

		OperatorType   isOperatorType(const char *s){
			for(unsigned char i = 1; i < OPERATOR_TYPE_MAX; i++){
				if(eval_info_operators[i].eval_fun(s)){
					return eval_info_operators[i].id;
				}
			}
			return OperatorType::OPERATOR_TYPE_UNKNOWN;
		}

		PreOperatorType   	isPreOperatorType(const char *s){
			for(unsigned char i = 1; i < PRE_OPERATOR_TYPE_MAX; i++){
				if(*eval_info_pre_operators[i].str == *s){
					return eval_info_pre_operators[i].id;
				}
			}
			return PreOperatorType::PRE_OPERATOR_TYPE_UNKNOWN;
		}

		IdentityOperatorType   isIdentityOperatorType(const char *s){
			for(unsigned char i = 1; i < IDENTITY_OPERATOR_TYPE_MAX; i++){
				if(*eval_info_identity_operators[i].str == *s){
					return eval_info_identity_operators[i].id;
				}
			}
			return IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
		}

		KeywordType isKeywordType(const char *c){
			// PRE: The first char must be starting symbol.
			char *str=(char *)c;
			//char *aux=str;

			for(int i = 0; i < KEYWORD_TYPE_MAX; i++){
				if(eval_info_keywords[i].str!=NULL){
					int size = strlen(eval_info_keywords[i].str);
					char *aux = str+size;
					if((strncmp(str,eval_info_keywords[i].str,size)==0) && (
							*aux == 0  || // carry end
							*aux == ' '  || // space
							*aux == '\t'  || // tab
							*aux == '('  || // ( // mostly if,for,while,switch
							*aux == '{'  || // ( // mostly else,
							*aux == '\n' || // carry return

						   isCommentBlockStart(aux)) //start block comment
						   ){
						return eval_info_keywords[i].id;
					}
				}
			}
			return KeywordType::KEYWORD_TYPE_UNKNOWN;
		}

		DirectiveType isDirectiveType(const char *c){
			// PRE: The first char must be starting symbol.
			char *str=(char *)c;

			for(int i = 0; i < DIRECTIVE_TYPE_MAX; i++){
				if(eval_info_directives[i].str){
					int size = strlen(eval_info_directives[i].str);

					if(strncmp(str,eval_info_directives[i].str,size)==0)
					{
						return eval_info_directives[i].id;
					}
				}
			}
			return DirectiveType::DIRECTIVE_TYPE_UNKNOWN;
		}

		bool  isAccessPunctuator(char *s){
			return *s=='.' || *s=='[' || *s=='(';
		}

		bool  isEndSymbolToken(char *s, char pre=0){
			return isOperatorType(s)!=OperatorType::OPERATOR_TYPE_UNKNOWN
				   || isSeparatorType(s)!=SeparatorType::SEPARATOR_TYPE_UNKNOWN
				   || *s=='.' // to separate access identifiers.
				   || *s==' '
				   || *s==0
				   || (*s=='\"' && pre!='\\');
		}

		bool  isIdentifierNameExpressionOk(EvalData *eval_data,const std::string & symbol, int line){

			char *aux_p = (char *)symbol.c_str();
			KeywordType kw;

			if((kw=isKeywordType(aux_p))!=KeywordType::KEYWORD_TYPE_UNKNOWN){
				writeError(eval_data->current_parsing_file,line," Unexpected \"%s\" keyword", aux_p);
				return false;
			}

			if( isLiteral(symbol)){
				writeError(eval_data->current_parsing_file,line," Unexpected \"%s\"", aux_p);
				return false;
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
				writeError(eval_data->current_parsing_file,line," identifier cannot begin with %c", *aux_p);
				return false;
			}

			return true;
		}

		char * parseLiteralNumber(EvalData *eval_data,const char *c, int & line, std::string & value, bool & error){
			// PRE: a std::string given...
			char *aux_p = ignoreBlanks(c,line);
			bool end=false;
			int current_part=0;
			std::string number_part[3];
			value="";
			bool is01s=true;
			//bool isInt=true;
			bool isHexa=(*aux_p == 'x' || *aux_p == 'X') || ((*aux_p == '0' && *(aux_p+1) == 'X') || (*aux_p == '0' && *(aux_p+1) == 'x'));

			if(!(('0'<=*aux_p&&*aux_p<='9') || isHexa || (*aux_p == '.'))){ // is no number directly
				return NULL;
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
							aux_p=ignoreBlanks(aux_p+1,line);
							if(*(aux_p)=='+' || *(aux_p)=='-'){
								number_part[current_part]+=*aux_p;
								aux_p=ignoreBlanks(aux_p+1,line);
							}
						}
						else{ // error

							 writeError(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
							 error=true;
							 return NULL;
						}
					}
					else{ // error
						 writeError(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
						 error=true;
						 return NULL;
					}

				}
				else if(*aux_p == '.'){ // fraccional part ?

					if(isHexa){
						 writeError(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
							 error=true;
							 return NULL;
					}

					if(current_part==0){
						current_part++;
						number_part[current_part]+=".";
					}
					else{ // error
						 writeError(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
						 error=true;
						 return NULL;
					}
				}

				else if(*aux_p == 'b'){ // is end binary format?
					if(!is01s || (current_part != 0)){
						 writeError(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
						 error=true;
						 return NULL;
					}

					number_part[current_part]+=*aux_p;
					end=true;
				}
				else{
					 writeError(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
					 error=true;
					 return NULL;
				}

				is01s&=(('0'==*aux_p) || ('1'==*aux_p));

				aux_p++;


				//if(evalSpecialPunctuator(aux)==OperatorType::FIELD_OPERATOR);
			}while(!end && !( isEndSymbolToken(aux_p)&&!(*aux_p=='.')));

			// check and eval token ?
			value = number_part[0]+number_part[1]+number_part[2];

			if( isEndSymbolToken(aux_p)){
				return aux_p;
			}

			return NULL;

			// POST: detects integer/binary/fractional/hexa
		}

		char *  getIdentifierToken(EvalData *eval_data,const char *s, std::string & symbol){

			char *aux_p = (char *)s;
			symbol="";

			if(*aux_p!=0 && (
					   ('a' <= *aux_p && *aux_p <='z') ||
					   ('A' <= *aux_p && *aux_p <='Z') ||
					   *aux_p == '_'
					 )
				){ // let's see it has right chars...

					do{
						symbol+=*aux_p;
						aux_p++;
					}while(  (*aux_p!=0) && (
								('a' <= *aux_p && *aux_p <='z')
							||  ('0' <= *aux_p && *aux_p <='9')
							||  (*aux_p=='_')
							||  ('A' <= *aux_p && *aux_p <='Z')));

					return aux_p;
			}else{
				writeError(eval_data->current_parsing_file,-1," identifier cannot begin with %c", *aux_p);
			}
			return NULL;
		}

		// PROTOTYPES
		void initEval(){

			if(g_init_eval) return;

			// Init operator punctuators...
			memset(eval_info_operators,0,sizeof(eval_info_operators));
			memset(eval_info_pre_operators,0,sizeof(eval_info_pre_operators));
			memset(eval_info_identity_operators,0,sizeof(eval_info_identity_operators));
			memset(eval_info_separators,0,sizeof(eval_info_separators));
			memset(eval_info_keywords,0,sizeof(eval_info_keywords));

			eval_info_operators[OPERATOR_TYPE_UNKNOWN]={OPERATOR_TYPE_UNKNOWN, "none",NULL};

			eval_info_operators[OPERATOR_TYPE_ADD]={OPERATOR_TYPE_ADD, "+",isOperatorAdd};
			eval_info_operators[OPERATOR_TYPE_SUB]={OPERATOR_TYPE_SUB, "-",isOperatorTypeSub};
			eval_info_operators[OPERATOR_TYPE_MUL]={OPERATOR_TYPE_MUL, "*",isOperatorTypeMul};
			eval_info_operators[OPERATOR_TYPE_DIV]={OPERATOR_TYPE_DIV, "/",isOperatorTypeDiv};
			eval_info_operators[OPERATOR_TYPE_MOD]={OPERATOR_TYPE_MOD, "%",isOperatorTypeMod};

			eval_info_operators[OPERATOR_TYPE_TERNARY_IF]={OPERATOR_TYPE_TERNARY_IF, "?",isOperatorTypeTernaryIf};
			eval_info_operators[OPERATOR_TYPE_TERNARY_ELSE]={OPERATOR_TYPE_TERNARY_ELSE, ":",isOperatorTypeTernaryElse};
			eval_info_operators[OPERATOR_TYPE_ASSIGN]={OPERATOR_TYPE_ASSIGN, "=",isOperatorTypeAssign};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_ADD]={OPERATOR_TYPE_ASSIGN_ADD, "+=",isOperatorTypeAssignAdd};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_SUB]={OPERATOR_TYPE_ASSIGN_SUB, "-=",isOperatorTypeAssignSub};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_MUL]={OPERATOR_TYPE_ASSIGN_MUL, "*=",isOperatorTypeAssignMul};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_DIV]={OPERATOR_TYPE_ASSIGN_DIV, "/=",isOperatorTypeAssignDiv};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_MOD]={OPERATOR_TYPE_ASSIGN_MOD, "%=",isOperatorTypeAssignMod};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_BINARY_XOR]={OPERATOR_TYPE_ASSIGN_BINARY_XOR,"^=",isOperatorTypeAssignXor};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_BINARY_AND]={OPERATOR_TYPE_ASSIGN_BINARY_AND,"&=",isOperatorTypeAssignBinaryAnd};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_BINARY_OR]={OPERATOR_TYPE_ASSIGN_BINARY_OR,"|=",isOperatorTypeAssignBinaryOr};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_SHIFT_LEFT]={OPERATOR_TYPE_ASSIGN_SHIFT_LEFT,"<<=",isOperatorTypeAssignShiftLeft};
			eval_info_operators[OPERATOR_TYPE_ASSIGN_SHIFT_RIGHT]={OPERATOR_TYPE_ASSIGN_SHIFT_RIGHT,">>=",isOperatorTypeAssignShiftRight};

			eval_info_operators[OPERATOR_TYPE_XOR]={OPERATOR_TYPE_XOR, "^",isOperatorTypeXor};
			eval_info_operators[OPERATOR_TYPE_BINARY_AND]={OPERATOR_TYPE_BINARY_AND, "&",isOperatorTypeBinaryAnd};
			eval_info_operators[OPERATOR_TYPE_OR]={OPERATOR_TYPE_OR, "|",isOperatorTypeBinaryOr};
			eval_info_operators[OPERATOR_TYPE_SHIFT_LEFT]={OPERATOR_TYPE_SHIFT_LEFT, "<<",isOperatorTypeShiftLeft};
			eval_info_operators[OPERATOR_TYPE_SHIFT_RIGHT]={OPERATOR_TYPE_SHIFT_RIGHT, ">>",isOperatorTypeShiftRight};
			eval_info_operators[OPERATOR_TYPE_LOGIC_AND]={OPERATOR_TYPE_LOGIC_AND, "&&",isOperatorTypeLogicAnd};
			eval_info_operators[OPERATOR_TYPE_LOGIC_OR]={OPERATOR_TYPE_LOGIC_OR, "||",isOperatorTypeLogicOr};
			eval_info_operators[OPERATOR_TYPE_LOGIC_EQUAL]={OPERATOR_TYPE_LOGIC_EQUAL, "==",isOperatorTypeLogicEqual};
			eval_info_operators[OPERATOR_TYPE_LOGIC_NOT_EQUAL]={OPERATOR_TYPE_LOGIC_NOT_EQUAL, "!=",isOperatorTypeLogicNotEqual};
			eval_info_operators[OPERATOR_TYPE_LOGIC_GT]={OPERATOR_TYPE_LOGIC_GT, ">",isOperatorTypeLogicGt};
			eval_info_operators[OPERATOR_TYPE_LOGIC_LT]={OPERATOR_TYPE_LOGIC_LT, "<",isOperatorTypeLogicLt};
			eval_info_operators[OPERATOR_TYPE_LOGIC_GTE]={OPERATOR_TYPE_LOGIC_GTE, ">=",isOperatorTypeLogicGte};
			eval_info_operators[OPERATOR_TYPE_LOGIC_LTE]={OPERATOR_TYPE_LOGIC_LTE, "<=",isOperatorTypeLogicLte};
			eval_info_operators[OPERATOR_TYPE_INSTANCEOF]={OPERATOR_TYPE_INSTANCEOF, "instanceof",isOperatorTypeInstanceOf};


			eval_info_pre_operators[PRE_OPERATOR_TYPE_NOT]={PRE_OPERATOR_TYPE_NOT, "!",isOperatorTypeLogicNot};
			eval_info_pre_operators[PRE_OPERATOR_TYPE_ADD]={PRE_OPERATOR_TYPE_ADD, "+",isOperatorAdd};
			eval_info_pre_operators[PRE_OPERATOR_TYPE_SUB]={PRE_OPERATOR_TYPE_SUB, "-",isOperatorTypeSub};


			eval_info_identity_operators[IDENTITY_OPERATOR_TYPE_INC]={IDENTITY_OPERATOR_TYPE_INC, "++",isOperatorTypeInc};
			eval_info_identity_operators[IDENTITY_OPERATOR_TYPE_DEC]={IDENTITY_OPERATOR_TYPE_DEC, "--",isOperatorTypeDec};


			// special punctuators...
			eval_info_separators[SEPARATOR_TYPE_COMA]={SEPARATOR_TYPE_COMA, ",",NULL};
			eval_info_separators[SEPARATOR_TYPE_SEMICOLON]={SEPARATOR_TYPE_SEMICOLON, ";",NULL};
			eval_info_separators[SEPARATOR_TYPE_PARENTHESIS_OPEN]={SEPARATOR_TYPE_PARENTHESIS_OPEN, "(",NULL};
			eval_info_separators[SEPARATOR_TYPE_PARENTHESIS_CLOSE]={SEPARATOR_TYPE_PARENTHESIS_CLOSE, ")",NULL};
			eval_info_separators[SEPARATOR_TYPE_SQUARE_BRAKET_OPEN]={SEPARATOR_TYPE_SQUARE_BRAKET_OPEN, "[",NULL};
			eval_info_separators[SEPARATOR_TYPE_SQUARE_BRAKET_CLOSE]={SEPARATOR_TYPE_SQUARE_BRAKET_CLOSE, "]",NULL};

			// Init special punctuators...
			// Init keywords...

			eval_info_keywords[KEYWORD_TYPE_UNKNOWN] = {KEYWORD_TYPE_UNKNOWN, NULL,NULL};
			eval_info_keywords[KEYWORD_TYPE_VAR] = {KEYWORD_TYPE_VAR,"var",evalKeywordTypeVar};
			eval_info_keywords[KEYWORD_TYPE_IF] = {KEYWORD_TYPE_IF,"if",evalKeywordTypeIf};
			eval_info_keywords[KEYWORD_TYPE_ELSE] = {KEYWORD_TYPE_ELSE,"else"};
			eval_info_keywords[KEYWORD_TYPE_FOR] = {KEYWORD_TYPE_FOR,"for",evalKeywordTypeFor};
			eval_info_keywords[KEYWORD_TYPE_WHILE] = {KEYWORD_TYPE_WHILE,"while",evalKeywordTypeWhile};
			eval_info_keywords[KEYWORD_TYPE_DO_WHILE] = {KEYWORD_TYPE_DO_WHILE,"do",evalKeywordTypeDoWhile}; // while is expected in the end ...

			eval_info_keywords[KeywordType::KEYWORD_TYPE_SWITCH] = {KEYWORD_TYPE_SWITCH,"switch",evalKeywordTypeSwitch};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_CASE] = {KEYWORD_TYPE_CASE,"case",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_BREAK] = {KEYWORD_TYPE_BREAK,"break",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_CONTINUE] = {KEYWORD_TYPE_CONTINUE,"continue",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_DEFAULT] = {KEYWORD_TYPE_DEFAULT,"default",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_FUNCTION] = {KEYWORD_TYPE_FUNCTION,"function",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_RETURN] = {KEYWORD_TYPE_RETURN,"return",evalKeywordTypeReturn};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_THIS] = {KEYWORD_TYPE_THIS,"this", NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_CLASS] = {KEYWORD_TYPE_CLASS,"class",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_NEW] = {KEYWORD_TYPE_NEW,"new", NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_DELETE] = {KEYWORD_TYPE_DELETE,"delete",NULL};
			eval_info_keywords[KeywordType::KEYWORD_TYPE_IN] = {KEYWORD_TYPE_IN,"in",NULL};

			// DIRECTIVES
			eval_info_directives[DIRECTIVE_TYPE_UNKNOWN]={DIRECTIVE_TYPE_UNKNOWN, NULL};
			eval_info_directives[DIRECTIVE_TYPE_INCLUDE]={DIRECTIVE_TYPE_INCLUDE, "import"};

			g_init_eval=true;
		}
	}
}
