
#define IGNORE_BLANKS(aux_p,eval_data,s,line) aux_p=zetscript::eval::ignore_blanks(eval_data,(s),line);

namespace zetscript{
	namespace eval{

		struct EvalData;

		typedef enum
			:unsigned char {
			KEYWORD_UNKNOWN = 0,
			KEYWORD_IF,
			KEYWORD_ELSE,
			KEYWORD_FOR,
			KEYWORD_IN,
			KEYWORD_WHILE,
			KEYWORD_DO_WHILE,
			KEYWORD_VAR,
			KEYWORD_CONST,
			KEYWORD_SWITCH,
			KEYWORD_CASE,
			KEYWORD_DEFAULT,
			KEYWORD_BREAK,
			KEYWORD_CONTINUE,
			KEYWORD_RETURN,
			KEYWORD_FUNCTION,
			KEYWORD_CLASS,
			KEYWORD_THIS,
			KEYWORD_NEW,
			KEYWORD_DELETE,
			KEYWORD_REF,
			KEYWORD_MAX
		}Keyword;

		typedef enum
			:unsigned char {
			DIRECTIVE_UNKNOWN = 0,
			DIRECTIVE_INCLUDE,
			DIRECTIVE_MAX
		}Directive;

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
			ACCESSOR_UNKNOWN=0,
			ACCESSOR_CALL, // v(1)
			ACCESSOR_VECTOR, // v[0]
			ACCESSOR_MEMBER, // a.b.c
			ACCESSOR_MAX
		}AccessorType;

		// operator enumerated as its precedence
		typedef enum :unsigned char {

			OPERATOR_UNKNOWN = 0,

			// ASSIGN
			OPERATOR_ASSIGN, 									// =
			OPERATOR_ASSIGN_FIRST,
			OPERATOR_ASSIGN_ADD=OPERATOR_ASSIGN_FIRST, 			// +=
			OPERATOR_ASSIGN_SUB, 								// -=
			OPERATOR_ASSIGN_MUL, 								// *=
			OPERATOR_ASSIGN_DIV, 								// /=
			OPERATOR_ASSIGN_MOD, 								// %=
			OPERATOR_ASSIGN_XOR,								// ^=
			OPERATOR_ASSIGN_BINARY_AND,							// &=
			OPERATOR_ASSIGN_BINARY_OR,  						// |=
			OPERATOR_ASSIGN_SHIFT_LEFT, 						// <<=
			OPERATOR_ASSIGN_SHIFT_RIGHT, 						// >>=
			OPERATOR_ASSIGN_LAST,

			// LOGIC
			OPERATOR_LOGIC_AND=OPERATOR_ASSIGN_LAST, 			// &&
			OPERATOR_LOGIC_OR, 									// ||

			// RELATIONAL
			OPERATOR_LOGIC_EQUAL, 								// ==
			OPERATOR_LOGIC_NOT_EQUAL, 							// !=
			OPERATOR_LOGIC_GTE, 								// >=
			OPERATOR_LOGIC_LTE, 								// <=
			OPERATOR_LOGIC_GT, 									// >
			OPERATOR_LOGIC_LT, 									// <

			// ARITHMETIC
			OPERATOR_ADD, 										// +
			OPERATOR_OR, 										// |
			OPERATOR_XOR, 										// ^
			OPERATOR_SUB, 										// -
			OPERATOR_MUL, 										// *
			OPERATOR_BINARY_AND, 								// &
			OPERATOR_DIV, 										// /
			OPERATOR_MOD, 										// %
			OPERATOR_SHIFT_LEFT, 								// <<
			OPERATOR_SHIFT_RIGHT, 								// >>

			OPERATOR_INSTANCEOF, 								// instanceof

			OPERATOR_TERNARY_IF, 								// ?
			OPERATOR_TERNARY_ELSE, 								// :
			OPERATOR_MAX
		}Operator;


		typedef enum :unsigned char {
			PRE_OPERATOR_UNKNOWN=0,
			PRE_OPERATOR_NOT, 		// ! (for boolean)
			PRE_OPERATOR_POS, 		// + (just ignore)
			PRE_OPERATOR_NEG	, 	// -
			PRE_OPERATOR_MAX
		}PreOperator;

		typedef enum :unsigned char {
			PRE_POST_SELF_OPERATION_UNKNOWN=0,
			PRE_POST_SELF_OPERATION_INC,		// ++
			PRE_POST_SELF_OPERATION_DEC,		// --
			PRE_POST_SELF_OPERATION_INVALID,	// +- or -+
			PRE_POST_SELF_OPERATION_MAX
		}PrePostSelfOperation;

		typedef enum :unsigned char {
			SEPARATOR_UNKNOWN=0,
			SEPARATOR_COMA,				// ,
			SEPARATOR_SEMICOLON,    		// ;
			SEPARATOR_PARENTHESIS_OPEN, 	// (
			SEPARATOR_PARENTHESIS_CLOSE, 	// )
			SEPARATOR_SQUARE_BRAKET_OPEN, 	// [
			SEPARATOR_SQUARE_BRAKET_CLOSE, // ]
			SEPARATOR_MAX
		}Separator;

		struct EvalInstruction{
			Instruction 					vm_instruction;
			LinkSymbolFirstAccess			link_symbol_first_access;
			InstructionSourceInfo 			instruction_source_info;
			bool 							is_symbol_super_method;

			EvalInstruction(
				ByteCode _byte_code
				,unsigned char _index_op1=ZS_IDX_INSTRUCTION_OP2_UNDEFINED
				,intptr_t _index_op2=ZS_IDX_INSTRUCTION_OP2_UNDEFINED
				,unsigned short _properties=0
			 ){
				is_symbol_super_method=false;
				vm_instruction=Instruction(_byte_code,_index_op1,_index_op2,_properties);
			}
		};

		struct TokenNode{
			TokenType	  					token_type; // can be operator, literal, identifier, object. (separator are not take account)
			PreOperator   					pre_operator; // !,+,-
			Operator  						operator_type;

			std::string 					value; // token value content
			int 							line;
			std::vector<EvalInstruction *> 	instructions; // byte code load literal/identifier(can be anonymous function), std::vector/struct.


			TokenNode(){
				line=-1;
				token_type=TokenType::TOKEN_TYPE_UNKNOWN;
				operator_type=Operator::OPERATOR_UNKNOWN;
				pre_operator=PreOperator::PRE_OPERATOR_UNKNOWN;
			}
		};


		struct EvalFunction{

			std::vector<EvalInstruction *>	 		instructions;
			ScriptFunction 						*  	script_function;

			// a set of instructions that relates with jmps instructions in current scope, just in case we have to insert push instruction later
			//std::vector<EvalInstruction *>	 		jmp_instructions;

			EvalFunction(ScriptFunction	* _script_function){
				script_function=_script_function;
			}

			~EvalFunction(){
				for(unsigned i=0; i< instructions.size(); i++){
					delete instructions[i];
				}
			}
		};


		typedef struct {
			Directive id;
			const char *str;
			//char * (*parse_fun)(const char *, int &, Scope *, PASTNode *);
		} EvalDirective;


		typedef struct {
			Keyword id;
			const char *str;
			char * (* eval_fun)(EvalData *eval_data,const char *, int &, Scope *);
		} EvalKeyword;

		typedef struct {
			Operator id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalOperator;

		typedef struct {
			PreOperator id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalPreOperator;

		typedef struct {
			PrePostSelfOperation id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalIdentityOperator;

		typedef struct {
			Separator id;
			const char *str;
			bool (*eval_fun)(const char *);
		} EvalSeparator;

		struct EvalData{
			ZetScript 						* 		zs;
			ScopeFactory 					* 		scope_factory;
			ScriptFunctionFactory 			* 		script_function_factory;
			ScriptClassFactory 				* 		script_class_factory;
			EvalFunction					* 		current_function;
			std::vector<EvalFunction *> 	  		functions;
			std::vector<EvalInstruction *>			break_jmp_instructions; // number of break_jmp_instructions collected (should managed on loops or switches)
			std::vector<EvalInstruction *>			continue_jmp_instructions; // number of continue_jmp_instructions collected (should managed only on loops)
			const char 						* 		current_parsing_file;
			std::map<std::string,std::string *>	 	compiled_symbol_name;
			bool							  		error;
			std::string								error_str;

			EvalData(ZetScript * _zs){
				current_parsing_file=NULL;
				current_function=NULL;
				this->zs=_zs;
				this->script_function_factory=zs->getScriptFunctionFactory();
				this->scope_factory=zs->getScopeFactory();
				this->script_class_factory=zs->getScriptClassFactory();
				error=false;
				error_str="";
			}
			~EvalData(){
				for(auto it=compiled_symbol_name.begin();it!=compiled_symbol_name.end() ;it++){
					delete it->second;
				}
			}
		};

		EvalOperator eval_info_operators[OPERATOR_MAX];
		EvalPreOperator eval_info_pre_operators[PRE_OPERATOR_MAX];
		EvalIdentityOperator eval_info_pre_post_self_operations[PRE_POST_SELF_OPERATION_MAX];
		EvalSeparator eval_info_separators[SEPARATOR_MAX];
		EvalKeyword eval_info_keywords[KEYWORD_MAX];
		EvalDirective eval_info_directives[DIRECTIVE_MAX];

		bool g_init_eval=false;

		char * 	eval_keyword_delete(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_function(EvalData *eval_data,const char *s,int & line,  Scope *scope_info, bool check_anonymous_function=false, std::string * function_name_result=NULL);
		char * 	eval_keyword_var_or_const(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_if_else(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_while(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_do_while(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_return(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_switch(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char * 	eval_keyword_for(EvalData *eval_data,const char *s,int & line,  Scope *scope_info);
		char *	eval_keyword_break(EvalData *eval_data,const char *s, int & line, Scope *scope_info);
		char *	eval_keyword_continue(EvalData *eval_data,const char *s, int & line, Scope *scope_info);
		char *  eval_symbol(EvalData *eval_data,const char *start_word, int line,TokenNode * token_node, PreOperator pre_operator, PrePostSelfOperation pre_self_operation);


		bool	is_operator_ternary_if(const char *s)			{return ((*s=='?'));}
		bool 	is_operator_ternary_else(const char *s)		{return ((*s==':'));}
		bool 	is_operator_add(const char *s)					{return	(*s=='+'); }// && (*(s+1)!='+') && (*(s+1)!='='));}
		bool 	is_operator_sub(const char *s)				{return	(*s=='-');}// && (*(s+1)!='-') && (*(s+1)!='='));}
		bool 	is_operator_mul(const char *s)				{return ((*s=='*') && (*(s+1)!='='));}
		bool 	is_operator_div(const char *s)				{return ((*s=='/') && (*(s+1)!='='));}
		bool 	is_operator_mod(const char *s)				{return ((*s=='%') && (*(s+1)!='='));}
		bool 	is_operator_assign(const char *s)				{return	((*s=='=') && (*(s+1)!='='));}
		bool 	is_operator_assign_add(const char *s)			{return ((*s=='+') && (*(s+1)=='='));}
		bool 	is_operator_assign_sub(const char *s)			{return ((*s=='-') && (*(s+1)=='='));}
		bool 	is_operator_assign_mul(const char *s)			{return ((*s=='*') && (*(s+1)=='='));}
		bool 	is_operator_assign_div(const char *s)			{return ((*s=='/') && (*(s+1)=='='));}
		bool 	is_operator_assign_mod(const char *s)			{return ((*s=='%') && (*(s+1)=='='));}
		bool 	is_operator_assign_xor(const char *s)			{return ((*s=='^') && (*(s+1)=='='));}		// ^=
		bool 	is_operator_assign_binary_and(const char *s)	{return ((*s=='&') && (*(s+1)=='='));}		// &=
		bool 	is_operator_assign_binary_or(const char *s)		{return ((*s=='|') && (*(s+1)=='='));}  	// |=
		bool 	is_operator_assign_shift_left(const char *s)	{return ((*s=='<') && (*(s+1)=='<')&& (*(s+2)=='='));} 	// <<=
		bool 	is_operator_assign_shift_right(const char *s)	{return ((*s=='>') && (*(s+1)=='>')&& (*(s+2)=='='));} 	// >>=
		bool 	is_operator_xor(const char *s)				{return ((*s=='^') && (*(s+1)!='='));}
		bool 	is_operator_binari_and(const char *s)			{return ((*s=='&') && (*(s+1)!='&') && (*(s+1)!='='));}
		bool 	is_operator_binari_or(const char *s)			{return ((*s=='|') && (*(s+1)!='|') && (*(s+1)!='='));}
		bool 	is_operator_shift_left(const char *s)			{return ((*s=='<') && (*(s+1)=='<') && (*(s+2)!='='));}
		bool 	is_operator_shift_right(const char *s)			{return	((*s=='>') && (*(s+1)=='>') && (*(s+2)!='='));}
		bool 	is_operator_logic_and(const char *s)			{return ((*s=='&') && (*(s+1)=='&'));}
		bool 	is_operator_logic_or(const char *s)			{return ((*s=='|') && (*(s+1)=='|'));}
		bool 	is_operator_logic_equal(const char *s) 		{return ((*s=='=') && (*(s+1)=='='));}
		bool 	is_operator_logic_not_equal(const char *s)		{return ((*s=='!') && (*(s+1)=='='));}
		bool 	is_operator_logic_gt(const char *s)			{return ((*s=='>') && (*(s+1)!='>'));}
		bool 	is_operator_logic_lt(const char *s)			{return ((*s=='<') && (*(s+1)!='<'));}
		bool 	is_operator_logic_gte(const char *s)			{return ((*s=='>') && (*(s+1)=='='));}
		bool 	is_operator_logic_lte(const char *s)			{return ((*s=='<') && (*(s+1)=='='));}
		bool 	is_operator_logic_not(const char *s)			{return ((*s=='!') && (*(s+1)!='='));}
		bool 	is_pre_post_self_operation_inc(const char *s)		{return ((*s=='+') && (*(s+1)=='+'));}
		bool 	is_pre_post_self_operation_dec(const char *s)		{return ((*s=='-') && (*(s+1)=='-'));}
		bool 	is_pre_post_self_operation_invalid(const char *s)		{return ((*s=='-') && (*(s+1)=='+')) || ((*s=='+') && (*(s+1)=='-'));}
		bool 	is_comment_single_line(char *s)					{return	((*s=='/') && (*(s+1)=='/'));}
		bool 	is_comment_block_start(char *s)					{return ((*s=='/') && (*(s+1)=='*'));}
		bool 	is_comment_block_end(char *s)						{return ((*s=='*') && (*(s+1)=='/'));}
		bool 	is_operator_instanceof(const char *s)			{return strncmp("instanceof",s,10) == 0;}
		bool    is_end_expression(const char * s){
			return *s==')' || *s==','||  *s==']' ||  *s==']' ||  *s==';' || *s == 0 || *s=='}';
		}

		char *advance_to_end_block_comment(char *aux_p, int &line){

			if( is_comment_block_start(aux_p)){
				aux_p+=2; //advance first
				bool end = false;
				while(*aux_p != 0  && !end){//! is_comment_block_end(aux_p) && *aux_p != 0){

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

		char *ignore_blanks(EvalData *eval_data,const char *str, int &line) {
			char *aux_p = (char *)str;
			bool end = false;
			while(!end){
				end = true;
				while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

				if( is_comment_single_line(aux_p)) // ignore line
					while(*aux_p!=0 && *aux_p!='\n') aux_p++;

				else if( is_comment_block_start(aux_p)){
					// ignore until get the end of the comment...
					aux_p = advance_to_end_block_comment(aux_p, line);

					if( is_comment_block_end(aux_p))
						aux_p+=2;

					end=false;
				}else if( is_comment_block_end(aux_p)){
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line," Unexpected end comment block");
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

		Separator   is_separator(const char *s){
			for(unsigned char i = 1; i < SEPARATOR_MAX; i++){
				if(*eval_info_separators[i].str == *s){
					return eval_info_separators[i].id;
				}
			}
			return Separator::SEPARATOR_UNKNOWN;
		}

		Operator   is_operator(const char *s){
			for(unsigned char i = 1; i < OPERATOR_MAX; i++){
				if(eval_info_operators[i].eval_fun(s)){
					return eval_info_operators[i].id;
				}
			}
			return Operator::OPERATOR_UNKNOWN;
		}

		PreOperator   	is_pre_operator_type(const char *s){
			for(unsigned char i = 1; i < PRE_OPERATOR_MAX; i++){
				if(*eval_info_pre_operators[i].str == *s){
					return eval_info_pre_operators[i].id;
				}
			}
			return PreOperator::PRE_OPERATOR_UNKNOWN;
		}

		PrePostSelfOperation   is_pre_post_self_operation(const char *s){
			if(is_pre_post_self_operation_inc(s)){
				return PrePostSelfOperation::PRE_POST_SELF_OPERATION_INC;

			}else if(is_pre_post_self_operation_dec(s)){
				return PrePostSelfOperation::PRE_POST_SELF_OPERATION_DEC;
			}
			else if(is_pre_post_self_operation_invalid(s)){
				return PrePostSelfOperation::PRE_POST_SELF_OPERATION_INVALID;
			}

			return PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN;
		}

		Keyword is_keyword(const char *c){
			// PRE: The first char must be starting symbol.
			char *str=(char *)c;
			//char *aux=str;

			for(int i = 0; i < KEYWORD_MAX; i++){
				if(eval_info_keywords[i].str!=NULL){
					size_t size = strlen(eval_info_keywords[i].str);
					char *aux = str+size;
					if((strncmp(str,eval_info_keywords[i].str,size)==0) && (
							*aux == 0  || // carry end
							*aux == ' '  || // space
							*aux == '\t'  || // tab
							*aux == '('  || // ( // mostly if,for,while,switch
							*aux == '{'  || // ( // mostly else,
							*aux == '\n' || // carry return
							*aux == ':' ||  // mostly after case/default
							*aux == ';' || // continue/break

						   is_comment_block_start(aux)) //start block comment
						   ){
						return eval_info_keywords[i].id;
					}
				}
			}
			return Keyword::KEYWORD_UNKNOWN;
		}

		Directive is_directive(const char *c){
			// PRE: The first char must be starting symbol.
			char *str=(char *)c;

			for(int i = 0; i < DIRECTIVE_MAX; i++){
				if(eval_info_directives[i].str){
					size_t size = strlen(eval_info_directives[i].str);

					if(strncmp(str,eval_info_directives[i].str,size)==0)
					{
						return eval_info_directives[i].id;
					}
				}
			}
			return Directive::DIRECTIVE_UNKNOWN;
		}

		bool  is_access_punctuator(char *s){
			return *s=='.' || *s=='[' || *s=='(';
		}

		bool  is_end_symbol_token(char *s, char pre=0){
			return is_operator(s)!=Operator::OPERATOR_UNKNOWN
				   || is_pre_post_self_operation(s)!=PrePostSelfOperation::PRE_POST_SELF_OPERATION_UNKNOWN
				   || is_separator(s)!=Separator::SEPARATOR_UNKNOWN
				   || *s=='.' // to separate access identifiers.
				   || *s==' '
				   || *s==0
				   || *s=='\r'
				   || *s=='\n'
				   || (*s=='\"' && pre!='\\');
		}

		void  check_identifier_name_expression_ok(EvalData *eval_data,const std::string & symbol, int line){

			char *aux_p = (char *)symbol.c_str();
			Keyword kw;

			if((kw=is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line," Unexpected \"%s\" keyword", aux_p);
			}

			// avoid special literal words
			if(symbol=="true"
				|| symbol == "false"
				|| symbol=="undefined"
				|| symbol == "null"
			){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line," Unexpected \"%s\"", aux_p);
			}

			// the rest is checked here...
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
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line," expected symbol");
			}
		}

#define RESULT_LITERAL_VALUE 	(number_part[0]+number_part[1]+number_part[2]).c_str()

		char * parse_literal_number(EvalData *eval_data,const char *c, int & line, std::string & value){
			// PRE: a std::string given...
			char *aux_p = NULL;
			IGNORE_BLANKS(aux_p,eval_data,c,line);

			bool end=false;
			int current_part=0;
			std::string number_part[3];
			//value="";
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
							IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);

							if(*(aux_p)=='+' || *(aux_p)=='-'){
								number_part[current_part]+=*aux_p;
								IGNORE_BLANKS(aux_p,eval_data,aux_p+1,line);
							}
						}
						else{ // error
							number_part[current_part]+=*aux_p; // save conflicting character
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
						}
					}
					else{ // error
						number_part[current_part]+=*aux_p; // save conflicting character
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
					}
				}
				else if(*aux_p == '.'){ // fraccional part ?

					if(isHexa){ // error
						number_part[current_part]+=*aux_p; // save conflicting character
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
					}

					if(current_part==0){
						current_part++;
						number_part[current_part]+=".";
					}
					else{ // error
						number_part[current_part]+=*aux_p; // save conflicting character
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
					}
				}

				else if(*aux_p == 'b'){ // is end binary format?
					if(!is01s || (current_part != 0)){
						number_part[current_part]+=*aux_p; // save conflicting character
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
					}

					number_part[current_part]+=*aux_p;
					end=true;
				}
				else{
					number_part[current_part]+=*aux_p; // save conflicting character
					THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
				}

				is01s&=(('0'==*aux_p) || ('1'==*aux_p));

				aux_p++;

				//if(evalSpecialPunctuator(aux)==Operator::FIELD_OPERATOR);
			}while(!end && !( is_end_symbol_token(aux_p)&&!(*aux_p=='.')));

			// check and eval token ?
			value = number_part[0]+number_part[1]+number_part[2];

			if( is_end_symbol_token(aux_p)){
				return aux_p;
			}
			return NULL;
			// POST: detects integer/binary/fractional/hexa
		}

		char *  get_identifier_token(EvalData *eval_data,const char *s, int line, std::string & symbol){

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
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected symbol", *aux_p);
			}
			return NULL;
		}

		// PROTOTYPES
		void init_eval(){

			if(g_init_eval) return;

			// Init operator punctuators...
			memset(eval_info_operators,0,sizeof(eval_info_operators));
			memset(eval_info_pre_operators,0,sizeof(eval_info_pre_operators));
			memset(eval_info_pre_post_self_operations,0,sizeof(eval_info_pre_post_self_operations));
			memset(eval_info_separators,0,sizeof(eval_info_separators));
			memset(eval_info_keywords,0,sizeof(eval_info_keywords));

			eval_info_operators[OPERATOR_UNKNOWN]={OPERATOR_UNKNOWN, "none",NULL};

			eval_info_operators[OPERATOR_ADD]={OPERATOR_ADD, "+",is_operator_add};
			eval_info_operators[OPERATOR_SUB]={OPERATOR_SUB, "-",is_operator_sub};
			eval_info_operators[OPERATOR_MUL]={OPERATOR_MUL, "*",is_operator_mul};
			eval_info_operators[OPERATOR_DIV]={OPERATOR_DIV, "/",is_operator_div};
			eval_info_operators[OPERATOR_MOD]={OPERATOR_MOD, "%",is_operator_mod};

			eval_info_operators[OPERATOR_TERNARY_IF]={OPERATOR_TERNARY_IF, "?",is_operator_ternary_if};
			eval_info_operators[OPERATOR_TERNARY_ELSE]={OPERATOR_TERNARY_ELSE, ":",is_operator_ternary_else};
			eval_info_operators[OPERATOR_ASSIGN]={OPERATOR_ASSIGN, "=",is_operator_assign};
			eval_info_operators[OPERATOR_ASSIGN_ADD]={OPERATOR_ASSIGN_ADD, "+=",is_operator_assign_add};
			eval_info_operators[OPERATOR_ASSIGN_SUB]={OPERATOR_ASSIGN_SUB, "-=",is_operator_assign_sub};
			eval_info_operators[OPERATOR_ASSIGN_MUL]={OPERATOR_ASSIGN_MUL, "*=",is_operator_assign_mul};
			eval_info_operators[OPERATOR_ASSIGN_DIV]={OPERATOR_ASSIGN_DIV, "/=",is_operator_assign_div};
			eval_info_operators[OPERATOR_ASSIGN_MOD]={OPERATOR_ASSIGN_MOD, "%=",is_operator_assign_mod};
			eval_info_operators[OPERATOR_ASSIGN_XOR]={OPERATOR_ASSIGN_XOR,"^=",is_operator_assign_xor};
			eval_info_operators[OPERATOR_ASSIGN_BINARY_AND]={OPERATOR_ASSIGN_BINARY_AND,"&=",is_operator_assign_binary_and};
			eval_info_operators[OPERATOR_ASSIGN_BINARY_OR]={OPERATOR_ASSIGN_BINARY_OR,"|=",is_operator_assign_binary_or};
			eval_info_operators[OPERATOR_ASSIGN_SHIFT_LEFT]={OPERATOR_ASSIGN_SHIFT_LEFT,"<<=",is_operator_assign_shift_left};
			eval_info_operators[OPERATOR_ASSIGN_SHIFT_RIGHT]={OPERATOR_ASSIGN_SHIFT_RIGHT,">>=",is_operator_assign_shift_right};

			eval_info_operators[OPERATOR_XOR]={OPERATOR_XOR, "^",is_operator_xor};
			eval_info_operators[OPERATOR_BINARY_AND]={OPERATOR_BINARY_AND, "&",is_operator_binari_and};
			eval_info_operators[OPERATOR_OR]={OPERATOR_OR, "|",is_operator_binari_or};
			eval_info_operators[OPERATOR_SHIFT_LEFT]={OPERATOR_SHIFT_LEFT, "<<",is_operator_shift_left};
			eval_info_operators[OPERATOR_SHIFT_RIGHT]={OPERATOR_SHIFT_RIGHT, ">>",is_operator_shift_right};
			eval_info_operators[OPERATOR_LOGIC_AND]={OPERATOR_LOGIC_AND, "&&",is_operator_logic_and};
			eval_info_operators[OPERATOR_LOGIC_OR]={OPERATOR_LOGIC_OR, "||",is_operator_logic_or};
			eval_info_operators[OPERATOR_LOGIC_EQUAL]={OPERATOR_LOGIC_EQUAL, "==",is_operator_logic_equal};
			eval_info_operators[OPERATOR_LOGIC_NOT_EQUAL]={OPERATOR_LOGIC_NOT_EQUAL, "!=",is_operator_logic_not_equal};
			eval_info_operators[OPERATOR_LOGIC_GT]={OPERATOR_LOGIC_GT, ">",is_operator_logic_gt};
			eval_info_operators[OPERATOR_LOGIC_LT]={OPERATOR_LOGIC_LT, "<",is_operator_logic_lt};
			eval_info_operators[OPERATOR_LOGIC_GTE]={OPERATOR_LOGIC_GTE, ">=",is_operator_logic_gte};
			eval_info_operators[OPERATOR_LOGIC_LTE]={OPERATOR_LOGIC_LTE, "<=",is_operator_logic_lte};
			eval_info_operators[OPERATOR_INSTANCEOF]={OPERATOR_INSTANCEOF, "instanceof",is_operator_instanceof};


			eval_info_pre_operators[PRE_OPERATOR_NOT]={PRE_OPERATOR_NOT, "!",is_operator_logic_not};
			eval_info_pre_operators[PRE_OPERATOR_POS]={PRE_OPERATOR_POS, "+",is_operator_add};
			eval_info_pre_operators[PRE_OPERATOR_NEG]={PRE_OPERATOR_NEG, "-",is_operator_sub};


			eval_info_pre_post_self_operations[PRE_POST_SELF_OPERATION_INC]={PRE_POST_SELF_OPERATION_INC, "++",is_pre_post_self_operation_inc};
			eval_info_pre_post_self_operations[PRE_POST_SELF_OPERATION_DEC]={PRE_POST_SELF_OPERATION_DEC, "--",is_pre_post_self_operation_dec};
			eval_info_pre_post_self_operations[PRE_POST_SELF_OPERATION_INVALID]={PRE_POST_SELF_OPERATION_INVALID, "+-",is_pre_post_self_operation_invalid};

			// special punctuators...
			eval_info_separators[SEPARATOR_COMA]={SEPARATOR_COMA, ",",NULL};
			eval_info_separators[SEPARATOR_SEMICOLON]={SEPARATOR_SEMICOLON, ";",NULL};
			eval_info_separators[SEPARATOR_PARENTHESIS_OPEN]={SEPARATOR_PARENTHESIS_OPEN, "(",NULL};
			eval_info_separators[SEPARATOR_PARENTHESIS_CLOSE]={SEPARATOR_PARENTHESIS_CLOSE, ")",NULL};
			eval_info_separators[SEPARATOR_SQUARE_BRAKET_OPEN]={SEPARATOR_SQUARE_BRAKET_OPEN, "[",NULL};
			eval_info_separators[SEPARATOR_SQUARE_BRAKET_CLOSE]={SEPARATOR_SQUARE_BRAKET_CLOSE, "]",NULL};

			// Init special punctuators...
			// Init keywords...

			eval_info_keywords[KEYWORD_UNKNOWN] = {KEYWORD_UNKNOWN, NULL,NULL};
			eval_info_keywords[KEYWORD_VAR] = {KEYWORD_VAR,"var",eval_keyword_var_or_const};
			eval_info_keywords[KEYWORD_CONST] = {KEYWORD_CONST,"const",eval_keyword_var_or_const};
			eval_info_keywords[KEYWORD_IF] = {KEYWORD_IF,"if",eval_keyword_if_else};
			eval_info_keywords[KEYWORD_ELSE] = {KEYWORD_ELSE,"else"};
			eval_info_keywords[KEYWORD_FOR] = {KEYWORD_FOR,"for",eval_keyword_for};
			eval_info_keywords[KEYWORD_WHILE] = {KEYWORD_WHILE,"while",eval_keyword_while};
			eval_info_keywords[KEYWORD_DO_WHILE] = {KEYWORD_DO_WHILE,"do",eval_keyword_do_while}; // while is expected in the end ...

			eval_info_keywords[KEYWORD_SWITCH] = {KEYWORD_SWITCH,"switch",eval_keyword_switch};
			eval_info_keywords[KEYWORD_CASE] = {KEYWORD_CASE,"case",NULL};
			eval_info_keywords[KEYWORD_DEFAULT] = {KEYWORD_DEFAULT,"default",NULL};
			eval_info_keywords[KEYWORD_BREAK] = {KEYWORD_BREAK,"break",eval_keyword_break};
			eval_info_keywords[KEYWORD_CONTINUE] = {KEYWORD_CONTINUE,"continue",eval_keyword_continue};
			eval_info_keywords[KEYWORD_FUNCTION] = {KEYWORD_FUNCTION,"function",NULL};
			eval_info_keywords[KEYWORD_RETURN] = {KEYWORD_RETURN,"return",eval_keyword_return};
			eval_info_keywords[KEYWORD_THIS] = {KEYWORD_THIS,"this", NULL};
			eval_info_keywords[KEYWORD_CLASS] = {KEYWORD_CLASS,"class",NULL};
			eval_info_keywords[KEYWORD_NEW] = {KEYWORD_NEW,"new", NULL};
			eval_info_keywords[KEYWORD_DELETE] = {KEYWORD_DELETE,"delete",eval_keyword_delete};
			eval_info_keywords[KEYWORD_IN] = {KEYWORD_IN,"in",NULL};
			eval_info_keywords[KEYWORD_REF] = {KEYWORD_REF,"ref",NULL};

			// DIRECTIVES
			eval_info_directives[DIRECTIVE_UNKNOWN]={DIRECTIVE_UNKNOWN, NULL};
			eval_info_directives[DIRECTIVE_INCLUDE]={DIRECTIVE_INCLUDE, "import"};

			g_init_eval=true;
		}
	}
}
