/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define IGNORE_BLANKS(aux_p,eval_data,s,line) 			if((aux_p=zetscript::eval_ignore_blanks(eval_data,(s),line))==NULL) return 0
#define IGNORE_BLANKS_AND_GOTO(my_goto,aux_p,eval_data,s,line) 	if((aux_p=zetscript::eval_ignore_blanks(eval_data,(s),line))==NULL) goto my_goto

#define RESULT_LITERAL_VALUE 					(number_part[0]+number_part[1]+number_part[2]).c_str()
#define EVAL_ERROR_FILE_LINE(file,line,s,...)	eval_data->error=true;\
												eval_data->error_file=file;\
												eval_data->error_line=line;\
												eval_data->error_str=zetscript::zs_strutils::format(s, ##__VA_ARGS__);\
												return 0;

#define EVAL_ERROR_TOKEN_SYMBOL(file,line,s,...)	eval_data->error=true;\
													eval_data->error_file=file;\
													eval_data->error_line=line;\
													eval_data->error_str=zetscript::zs_strutils::format(s, ##__VA_ARGS__);\
													goto lbl_eval_error_token_symbol;


#define EVAL_ERROR(s,...)						eval_data->error=true;\
												eval_data->error_str=zetscript::zs_strutils::format(s, ##__VA_ARGS__);\
												return 0;





#define EVAL_ERROR_FILE_LINE_AND_GOTO(my_goto,file,line,s,...)			eval_data->error=true;\
																		eval_data->error_file=file;\
																		eval_data->error_line=line;\
																		eval_data->error_str=zetscript::zs_strutils::format(s, ##__VA_ARGS__);\
																		goto my_goto;

#define EVAL_ERROR_BYTE_CODE(s,...)							eval_data->error=true;\
															aux_p=NULL;\
															eval_data->error_str=zetscript::zs_strutils::format(s, ##__VA_ARGS__);\
															goto eval_error_byte_code;\


#define IS_OPERATOR_TYPE_ASSIGN_WITH_OPERATION(c) (Operator::OPERATOR_ASSIGN_ADD<=(c) && (c)<=Operator::OPERATOR_ASSIGN_SHIFT_RIGHT)
#define IS_OPERATOR_TYPE_ASSIGN(c) (IS_OPERATOR_TYPE_ASSIGN_WITH_OPERATION(c) || (c)==Operator::OPERATOR_ASSIGN)

namespace zetscript{

	typedef enum{
		EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION		=0x1<<1, // it tells is a regular expression in eval or in post operation for
		EVAL_EXPRESSION_ALLOW_SEQUENCE_ASSIGNMENT		=0x1<<2, // do not allow a,b,c=0,0,0
		EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR	=0x1<<3, // break when any assign operator (i.e, =, +=, -=, ...) is found
		EVAL_EXPRESSION_ON_MAIN_BLOCK					=0x1<<4,
		EVAL_EXPRESSION_FOR_IN_VARIABLES				=0x1<<5,
		EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL				=0x1<<6
	}EvalExpressionProperty;

	typedef enum
		:unsigned char {
		KEYWORD_UNKNOWN = 0,
		KEYWORD_IF,
		KEYWORD_ELSE,
		KEYWORD_FOR,
		KEYWORD_WHILE,
		KEYWORD_DO_WHILE,
		KEYWORD_VAR,
		KEYWORD_CONST,
		KEYWORD_STATIC,
		KEYWORD_SWITCH,
		KEYWORD_CASE,
		KEYWORD_DEFAULT,
		KEYWORD_BREAK,
		KEYWORD_CONTINUE,
		KEYWORD_RETURN,
		KEYWORD_FUNCTION,
		KEYWORD_CLASS,
		KEYWORD_DELETE,
		KEYWORD_REF,
		KEYWORD_VARIABLE_ARGS,
		KEYWORDS_WITHIN_EXPRESSIONS,
		//-------------------
		// Put here keywords are considered within expressions!...
		KEYWORD_THIS=KEYWORDS_WITHIN_EXPRESSIONS,
		KEYWORD_NEW,
		KEYWORD_MAX
	}Keyword;

	typedef enum
		:unsigned char {
		DIRECTIVE_UNKNOWN = 0,
		DIRECTIVE_IMPORT,
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
		TOKEN_TYPE_OBJECT,
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

	typedef enum :unsigned char {
		PRE_OPERATION_UNKNOWN=0,
		// two chars here!
		PRE_OPERATION_DEC    ,	// --
		PRE_OPERATION_INC	,	// ++
		PRE_OPERATION_DEC_INC_INVALID,	// -+ or +-
		// one chars here!
		PRE_OPERATION_NOT, 		// ! (for boolean)
		PRE_OPERATION_TYPEOF, 	//
		PRE_OPERATION_POS, 		// + (just ignore)
		PRE_OPERATION_NEG	, 	// -
		PRE_OPERATION_MAX
	}PreOperation;

	typedef enum :unsigned char {
		POST_OPERATION_UNKNOWN=0,
		POST_OPERATION_DEC	,	// --
		POST_OPERATION_INC	,	// ++
		POST_OPERATION_DEC_INC_INVALID,	// -+ or +-
		POST_OPERATION_MAX
	}PostOperation;

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

	struct TokenNode{
		TokenType	  					token_type; // can be operator, literal, identifier, object. (separator are not take account)
		PreOperation   					pre_operation; // !,+,-,--,++
		Operator  						operator_type;

		zs_string 						value; // token value content
		int 							line;
		zs_vector 						eval_instructions; // byte code load literal/identifier(can be anonymous function), zs_vector/struct.
		bool are_instructions_moved;

		TokenNode(){
			reset();
		}

		void reset(){
			are_instructions_moved=false;
			value="";
			line=-1;
			token_type=TokenType::TOKEN_TYPE_UNKNOWN;
			operator_type=Operator::OPERATOR_UNKNOWN;
			pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;
			eval_instructions.clear();
		}
	};


	struct EvalFunction{

		zs_vector						 		eval_instructions;
		ScriptFunction 						*  	script_function;
		int										parsing_loop;
		int										parsing_switch;

		// a set of instructions that relates with jmps instructions in current scope, just in case we have to insert push instruction later
		EvalFunction(ScriptFunction	* _script_function){
			script_function=_script_function;
			parsing_loop=0;
			parsing_switch=0;
		}

		~EvalFunction(){
			for(unsigned i=0; i< eval_instructions.count; i++){
				delete (EvalInstruction *)eval_instructions.items[i];
			}
		}
	};


	typedef struct {
		Directive id;
		const char *str;
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
		PreOperation id;
		const char *str;
		bool (*eval_fun)(const char *);
	} EvalPreOperation;

	typedef struct {
		PostOperation id;
		const char *str;
		bool (*eval_fun)(const char *);
	} EvalPostOperation;

	typedef struct {
		Separator id;
		const char *str;
		bool (*eval_fun)(const char *);
	} EvalSeparator;

	typedef struct{
		zs_vector 						ei_break_jmps;
		int								idx_instruction_start_loop;
	}LoopBreakContinueInfo;


	struct UnresolvedInstructionInfo{
		Instruction *instruction;
		ScriptFunction *calling_function;

		UnresolvedInstructionInfo(Instruction *_instruction, ScriptFunction *_calling_function){
			instruction = _instruction;
			calling_function = _calling_function;
		}

	};

	struct EvalData{
		ZetScript 						* 		zs;
		ScopeFactory 					* 		scope_factory;
		ScriptFunctionFactory 			* 		script_function_factory;
		ScriptClassFactory 				* 		script_class_factory;
		EvalFunction					* 		current_function;
		zs_vector				 	  			eval_functions;
		zs_vector								unresolved_symbols; // UnresolvedInstructionInfo
		zs_vector				 				global_ref_instructions; // Eval Instruction
		int										parsing_loop;

		const char *					 		current_parsing_file;
		bool							  		error;
		zs_string								error_str;
		zs_string								error_file;
		int 									error_line;

		EvalData(ZetScript * _zs){
			current_parsing_file="";
			current_function=NULL;
			this->zs=_zs;
			this->script_function_factory=zs->getScriptFunctionFactory();
			this->scope_factory=zs->getScopeFactory();
			this->script_class_factory=zs->getScriptClassFactory();
			error=false;
			error_str="";
			parsing_loop=0;
			error_file="";
			error_line=-1;
		}
	};

	EvalOperator eval_data_operators[OPERATOR_MAX];
	EvalPreOperation eval_data_pre_operations[PRE_OPERATION_MAX];
	EvalPostOperation eval_data_post_operations[POST_OPERATION_MAX];
	EvalSeparator eval_data_separators[SEPARATOR_MAX];
	EvalKeyword eval_data_keywords[KEYWORD_MAX];
	EvalDirective eval_data_directives[DIRECTIVE_MAX];


	bool g_init_eval=false;
	zs_map	 	*compiled_symbol_name=NULL;


	char *  eval_symbol(EvalData *eval_data,const char *start_word, int line,  Scope *scope_info,TokenNode * token_node, PreOperation pre_operation, PostOperation post_operation);
	Symbol *eval_find_local_symbol(EvalData *eval_data,Scope *scope, const zs_string & symbol_to_find);
	Symbol *eval_find_global_symbol(EvalData *eval_data, const zs_string & symbol_to_find);

	bool	is_operator_ternary_if(const char *s)				{return *s=='?';}
	bool 	is_operator_ternary_else(const char *s)				{return *s==':';}
	bool 	is_operator_add(const char *s)						{return	((*s=='+') && (*(s+1)!='+'));}// && (*(s+1)!='+') && (*(s+1)!='='));}
	bool 	is_operator_sub(const char *s)						{return	((*s=='-') && (*(s+1)!='-'));}// && (*(s+1)!='-') && (*(s+1)!='='));}
	bool 	is_operator_mul(const char *s)						{return ((*s=='*') && (*(s+1)!='='));}
	bool 	is_operator_div(const char *s)						{return ((*s=='/') && (*(s+1)!='='));}
	bool 	is_operator_mod(const char *s)						{return ((*s=='%') && (*(s+1)!='='));}
	bool 	is_operator_assign(const char *s)					{return	((*s=='=') && (*(s+1)!='='));}
	bool 	is_operator_assign_add(const char *s)				{return ((*s=='+') && (*(s+1)=='='));}
	bool 	is_operator_assign_sub(const char *s)				{return ((*s=='-') && (*(s+1)=='='));}
	bool 	is_operator_assign_mul(const char *s)				{return ((*s=='*') && (*(s+1)=='='));}
	bool 	is_operator_assign_div(const char *s)				{return ((*s=='/') && (*(s+1)=='='));}
	bool 	is_operator_assign_mod(const char *s)				{return ((*s=='%') && (*(s+1)=='='));}
	bool 	is_operator_assign_xor(const char *s)				{return ((*s=='^') && (*(s+1)=='='));}		// ^=
	bool 	is_operator_assign_and(const char *s)				{return ((*s=='&') && (*(s+1)=='='));}		// &=
	bool 	is_operator_assign_or(const char *s)				{return ((*s=='|') && (*(s+1)=='='));}  	// |=
	bool 	is_operator_assign_shift_left(const char *s)		{return ((*s=='<') && (*(s+1)=='<')&& (*(s+2)=='='));} 	// <<=
	bool 	is_operator_assign_shift_right(const char *s)		{return ((*s=='>') && (*(s+1)=='>')&& (*(s+2)=='='));} 	// >>=
	bool 	is_operator_xor(const char *s)						{return ((*s=='^') && (*(s+1)!='='));}
	bool 	is_operator_and(const char *s)						{return ((*s=='&') && (*(s+1)!='&') && (*(s+1)!='='));}
	bool 	is_operator_or(const char *s)						{return ((*s=='|') && (*(s+1)!='|') && (*(s+1)!='='));}
	bool 	is_operator_shift_left(const char *s)				{return ((*s=='<') && (*(s+1)=='<') && (*(s+2)!='='));}
	bool 	is_operator_shift_right(const char *s)				{return	((*s=='>') && (*(s+1)=='>') && (*(s+2)!='='));}
	bool 	is_operator_logic_and(const char *s)				{return ((*s=='&') && (*(s+1)=='&'));}
	bool 	is_operator_logic_or(const char *s)					{return ((*s=='|') && (*(s+1)=='|'));}
	bool 	is_operator_logic_equal(const char *s) 				{return ((*s=='=') && (*(s+1)=='='));}
	bool 	is_operator_logic_not_equal(const char *s)			{return ((*s=='!') && (*(s+1)=='='));}
	bool 	is_operator_logic_gt(const char *s)					{return ((*s=='>') && (*(s+1)!='>'));}
	bool 	is_operator_logic_lt(const char *s)					{return ((*s=='<') && (*(s+1)!='<'));}
	bool 	is_operator_logic_gte(const char *s)				{return ((*s=='>') && (*(s+1)=='='));}
	bool 	is_operator_logic_lte(const char *s)				{return ((*s=='<') && (*(s+1)=='='));}
	bool 	is_operator_logic_not(const char *s)				{return ((*s=='!') && (*(s+1)!='='));}

	bool 	is_operation_dec(const char *s)						{return ((*s=='-') && (*(s+1)=='-'));}
	bool 	is_operation_inc(const char *s)						{return ((*s=='+') && (*(s+1)=='+'));}
	bool 	is_operation_dec_inc_invalid(const char *s)			{return ((*s=='-') && (*(s+1)=='+')) || ((*s=='+') && (*(s+1)=='-'));}
	bool 	is_comment_single_line(char *s)						{return	((*s=='/') && (*(s+1)=='/'));}
	bool 	is_comment_block_start(char *s)						{return ((*s=='/') && (*(s+1)=='*'));}
	bool 	is_comment_block_end(char *s)						{return ((*s=='*') && (*(s+1)=='/'));}

	// complex operators...
	bool 	is_operator_typeof(const char *s){

		if(strncmp("typeof",s,6)==0){
			char *aux_p=((char *)s+6);
			return *aux_p==' ' || *aux_p=='\r' || *aux_p=='\n';
		}
		return false;
	}
	bool 	is_operator_instanceof(const char *s){
		if(strncmp("instanceof",s,10) == 0){
			char *aux_p=((char *)s+10);
			return *aux_p==' ' || *aux_p=='\r' || *aux_p=='\n';
		}
		return false;
	}
	bool 	is_operator_in(const char *s)						{
		if(strncmp("in",s,2) == 0){
			char *aux_p=((char *)s+2);
			return *aux_p==' ' || *aux_p=='\r' || *aux_p=='\n';
		}
		return false;
	}
	Keyword eval_is_keyword(const char *c);

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

	char *eval_ignore_blanks(EvalData *eval_data,const char *str, int &line) {
		char *aux_p = (char *)str;
		bool end = false;
		while(!end){
			end = true;
			while(*aux_p!=0 && ((*aux_p==' ') || (*aux_p=='\t'))) aux_p++;

			if( is_comment_single_line(aux_p)) // ignore line
				while(*aux_p!=0 && *aux_p!='\n') aux_p++;

			else if( is_comment_block_start(aux_p)){
				// ignore until get the end of the comment...
				aux_p = advance_to_end_block_comment(aux_p, line);

				if( is_comment_block_end(aux_p))
					aux_p+=2;

				end=false;
			}else if( is_comment_block_end(aux_p)){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line," Unexpected end comment block");
			}
			// make compatible windows format...
			if(*aux_p == '\r'){
				aux_p++;
			}

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
			if(*eval_data_separators[i].str == *s){
				return eval_data_separators[i].id;
			}
		}
		return Separator::SEPARATOR_UNKNOWN;
	}

	Operator   is_operator(const char *s){
		for(unsigned char i = 1; i < OPERATOR_MAX; i++){
			if(eval_data_operators[i].eval_fun(s)){
				return eval_data_operators[i].id;
			}
		}
		return Operator::OPERATOR_UNKNOWN;
	}

	unsigned char get_operator_type_group(Operator operator_type){
		if(OPERATOR_GROUP_0(operator_type)){
			return 0;
		}else if (OPERATOR_GROUP_1(operator_type)){
			return 1;
		}else if (OPERATOR_GROUP_2(operator_type)){
			return 2;
		}else if (OPERATOR_GROUP_3(operator_type)){
			return 3;
		}else if (OPERATOR_GROUP_4(operator_type)){
			return 4;
		}else if (OPERATOR_GROUP_5(operator_type)){
			return 5;
		}

		return OPERATOR_GROUP_MAX;

	}

	PreOperation   	is_pre_operation(const char *s){
		for(unsigned char i = 1; i < PRE_OPERATION_MAX; i++){
			if(eval_data_pre_operations[i].eval_fun(s)){
				return eval_data_pre_operations[i].id;
			}
		}
		return PreOperation::PRE_OPERATION_UNKNOWN;
	}

	PostOperation   is_post_operation(const char *s){
		for(unsigned char i = 1; i < POST_OPERATION_MAX; i++){
			if(eval_data_post_operations[i].eval_fun(s)){
				return eval_data_post_operations[i].id;
			}
		}
		return PostOperation::POST_OPERATION_UNKNOWN;
	}

	bool is_special_char(char *aux){
		return *aux == 0  || // carry end
		*aux == ' '  || // space
		*aux == '\t'  || // tab
		*aux=='.' ||
		*aux=='(' ||
		*aux==')' ||
		*aux=='{' ||
		*aux=='}' ||
		*aux=='[' ||
		*aux==']' ||
		*aux == '\n' || // carry return
		*aux == '\r' || // compatible windows format
		*aux == ':' ||  // mostly after case/default
		*aux == ';'; // continue/break
	}

	Keyword eval_is_keyword(const char *c){
		// PRE: The first char must be starting symbol.
		char *str=(char *)c;
		//char *aux=str;

		for(int i = 0; i < KEYWORD_MAX; i++){
			if(eval_data_keywords[i].str!=NULL){
				size_t size = strlen(eval_data_keywords[i].str);
				char *aux = str+size;
				if((strncmp(str,eval_data_keywords[i].str,size)==0) && (
						is_special_char(aux) ||

					   is_comment_block_start(aux)) //start block comment
				){
					return eval_data_keywords[i].id;
				}
			}
		}
		return Keyword::KEYWORD_UNKNOWN;
	}

	Directive is_directive(const char *c){
		// PRE: The first char must be starting symbol.
		char *str=(char *)c;

		for(int i = 0; i < DIRECTIVE_MAX; i++){
			if(eval_data_directives[i].str){
				size_t size = strlen(eval_data_directives[i].str);

				if(strncmp(str,eval_data_directives[i].str,size)==0)
				{
					return eval_data_directives[i].id;
				}
			}
		}
		return Directive::DIRECTIVE_UNKNOWN;
	}

	zs_string * get_mapped_name(EvalData *eval_data,const zs_string & _mapped_name){
		zs_int e=compiled_symbol_name->get(_mapped_name.c_str());
		if(e==0){
			zs_string *s=new zs_string (_mapped_name);
			compiled_symbol_name->set(s->c_str(),(zs_int)(s));
			e=(zs_int)s;
		}
		return ((zs_string *)e);
	}

	bool  is_end_symbol_token(char *s, char pre=0){

		return    is_operator(s)!=Operator::OPERATOR_UNKNOWN
			   || is_post_operation(s)!=PostOperation::POST_OPERATION_UNKNOWN
			   || is_separator(s)!=Separator::SEPARATOR_UNKNOWN
			   || is_special_char(s)
			   || (*s=='\"' && pre!='\\');
	}

	bool  is_anonymous_function(EvalData *eval_data,const char *s, int line){
		Keyword key_w;
		char *aux_p = (char *)s;

		key_w = eval_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_FUNCTION){
			IGNORE_BLANKS(aux_p,eval_data,aux_p+strlen(eval_data_keywords[key_w].str),line);

			if(*aux_p == '('){
				return true;
			}
		}

		return false;
	}

	int  check_identifier_name_expression_ok(EvalData *eval_data,const zs_string & symbol, int line){

		char *aux_p = (char *)symbol.c_str();
		Keyword kw;
		Operator op;

		if((kw=eval_is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line," Unexpected '%s' keyword", eval_data_keywords[kw].str);
		}

		if((op=is_operator(aux_p))!=Operator::OPERATOR_UNKNOWN){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line," Unexpected '%s' operator",  eval_data_operators[op].str);
		}

		// avoid special literal words
		if(symbol=="true"
			|| symbol == "false"
			|| symbol=="null"
		){
			EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line," Unexpected '%s'", aux_p);
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
			return FALSE;
		}

		return TRUE;
	}

	char * parse_literal_number(EvalData *eval_data,const char *s, int & line, zs_string & value){
		// PRE: a zs_string given...
		char *aux_p = NULL;
		bool end=false;
		int current_part=0;
		zs_string number_part[3];
		//value="";
		bool is_hexa=false;
		bool is01s=true;
		//bool isInt=true;
		bool is_char=false;
		bool is_float=false;

		IGNORE_BLANKS(aux_p,eval_data,s,line);


		if(eval_is_keyword(s)){
			 return NULL;
		}

		if(*aux_p == '\''){ // is char
			int i_char=0;
			aux_p++;
			if(*aux_p == '\\'){ // is special char
				aux_p++;
				switch(*aux_p){
				default:
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"unknown special char \\%c",*aux_p);
					break;
				case 'n':
					i_char='\n';
					break;
				case 'r':
					i_char='\r';
					break;
				case 't':
					i_char='\t';
					break;
				}

			}else if(*aux_p != '\''){

				if(*aux_p==0){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Unterminated char \'");
				}

				i_char=*aux_p;
			}else{
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"empty character constant");
			}

			aux_p++;
			if(*aux_p != '\''){
				while(*aux_p!=0 && *aux_p!='\n' && *aux_p!='\'' ) aux_p++;

				if(*aux_p == '\''){
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"multi-character character constant");
				}else{
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Unterminated char \'");
				}
			}

			aux_p++;

			// convert i_char to value...
			value=zs_strutils::zs_int_to_str(i_char);
			return aux_p;
		}

		if(*aux_p == '0' && ((*(aux_p+1) == 'X') || (*(aux_p+1) == 'x'))){
			is_hexa=true;
			number_part[current_part]+=*aux_p++;
			number_part[current_part]+=*aux_p++;
		}


		if(!(('0'<=*aux_p&&*aux_p<='9') || is_hexa || (*aux_p == '.'))){ // is no number directly
			return NULL;
		}

		// else is candidate to number ...
		do{

			if(is_float && (*aux_p == 'e' || *aux_p == 'E')){ // exponencial part ?

				if(current_part == 1 && number_part[current_part].length()>1){
					current_part=2;
					number_part[current_part]+=*aux_p++;
					if(*(aux_p)=='+' || *(aux_p)=='-'){
						number_part[current_part]+=*aux_p++;
					}
				}else{ // error
					number_part[current_part]+=*aux_p; // save conflicting character
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
				}
			}

			if('0'<=*aux_p&&*aux_p<='9'){
				number_part[current_part]+=*aux_p;
			}
			else if((('a'<=*aux_p&& *aux_p<='f') || ('A'<=*aux_p && *aux_p<='F')) && is_hexa){ // hexa ?
				number_part[current_part]+=*aux_p;
			}
			else if(*aux_p == '.'){ // fraccional part ?
				if(!is_hexa && (current_part==0)){
					current_part++;
					number_part[current_part]+=".";
					is_float=true;
				}else{ // error
					number_part[current_part]+=*aux_p; // save conflicting character
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
				}
			}

			else if(*aux_p == 'b'){ // is end binary format?
				if(!is01s || (current_part != 0)){
					number_part[current_part]+=*aux_p; // save conflicting character
					EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
				}

				number_part[current_part]+=*aux_p;
				end=true;
			}
			else{
				number_part[current_part]+=*aux_p; // save conflicting character
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line ,"Invalid number format \"%s\"",RESULT_LITERAL_VALUE);
			}
			is01s&=(('0'==*aux_p) || ('1'==*aux_p));

			aux_p++;

		}while(!end && !((is_end_symbol_token(aux_p) || (is_special_char(aux_p))) && *aux_p != '.'));

		// check and eval token ?
		value = number_part[0]+number_part[1]+number_part[2];

		if( is_end_symbol_token(aux_p) || eval_is_keyword(s)){
			 return aux_p;
		}

		return NULL;
		// POST: detects integer/binary/fractional/hexa
	}

	char *  get_name_identifier_token(EvalData *eval_data,const char *s, int line, zs_string & name, bool _throw_error=true){

		char *aux_p = (char *)s;
		name="";

		if(*aux_p!=0 && (
				   ('a' <= *aux_p && *aux_p <='z') ||
				   ('A' <= *aux_p && *aux_p <='Z') ||
				   *aux_p == '_'
				 )
			){ // let's see it has right chars...

				do{
					name+=*aux_p;
					aux_p++;
				}while(  (*aux_p!=0) && (
							('a' <= *aux_p && *aux_p <='z')
						||  ('0' <= *aux_p && *aux_p <='9')
						||  (*aux_p=='_')
						||  ('A' <= *aux_p && *aux_p <='Z')));

				return aux_p;
		}else{
			if(_throw_error){
				EVAL_ERROR_FILE_LINE(eval_data->current_parsing_file,line,"Invalid or unexpected token");
			}
		}
		return NULL;
	}

	// PROTOTYPES
	void eval_init(){

		if(g_init_eval) return;

		// Init operator punctuators...
		memset(eval_data_operators,0,sizeof(eval_data_operators));
		memset(eval_data_pre_operations,0,sizeof(eval_data_pre_operations));
		memset(eval_data_post_operations,0,sizeof(eval_data_post_operations));
		memset(eval_data_separators,0,sizeof(eval_data_separators));
		memset(eval_data_keywords,0,sizeof(eval_data_keywords));

		eval_data_operators[OPERATOR_UNKNOWN]={OPERATOR_UNKNOWN, "unknown-operator",NULL};

		eval_data_operators[OPERATOR_ADD]={OPERATOR_ADD, "+",is_operator_add};
		eval_data_operators[OPERATOR_SUB]={OPERATOR_SUB, "-",is_operator_sub};
		eval_data_operators[OPERATOR_MUL]={OPERATOR_MUL, "*",is_operator_mul};
		eval_data_operators[OPERATOR_DIV]={OPERATOR_DIV, "/",is_operator_div};
		eval_data_operators[OPERATOR_MOD]={OPERATOR_MOD, "%",is_operator_mod};

		eval_data_operators[OPERATOR_TERNARY_IF]={OPERATOR_TERNARY_IF, "?",is_operator_ternary_if};
		eval_data_operators[OPERATOR_TERNARY_ELSE]={OPERATOR_TERNARY_ELSE, ":",is_operator_ternary_else};
		eval_data_operators[OPERATOR_ASSIGN]={OPERATOR_ASSIGN, "=",is_operator_assign};
		eval_data_operators[OPERATOR_ASSIGN_ADD]={OPERATOR_ASSIGN_ADD, "+=",is_operator_assign_add};
		eval_data_operators[OPERATOR_ASSIGN_SUB]={OPERATOR_ASSIGN_SUB, "-=",is_operator_assign_sub};
		eval_data_operators[OPERATOR_ASSIGN_MUL]={OPERATOR_ASSIGN_MUL, "*=",is_operator_assign_mul};
		eval_data_operators[OPERATOR_ASSIGN_DIV]={OPERATOR_ASSIGN_DIV, "/=",is_operator_assign_div};
		eval_data_operators[OPERATOR_ASSIGN_MOD]={OPERATOR_ASSIGN_MOD, "%=",is_operator_assign_mod};
		eval_data_operators[OPERATOR_ASSIGN_XOR]={OPERATOR_ASSIGN_XOR,"^=",is_operator_assign_xor};
		eval_data_operators[OPERATOR_ASSIGN_AND]={OPERATOR_ASSIGN_AND,"&=",is_operator_assign_and};
		eval_data_operators[OPERATOR_ASSIGN_OR]={OPERATOR_ASSIGN_OR,"|=",is_operator_assign_or};
		eval_data_operators[OPERATOR_ASSIGN_SHIFT_LEFT]={OPERATOR_ASSIGN_SHIFT_LEFT,"<<=",is_operator_assign_shift_left};
		eval_data_operators[OPERATOR_ASSIGN_SHIFT_RIGHT]={OPERATOR_ASSIGN_SHIFT_RIGHT,">>=",is_operator_assign_shift_right};

		eval_data_operators[OPERATOR_XOR]={OPERATOR_XOR, "^",is_operator_xor};
		eval_data_operators[OPERATOR_AND]={OPERATOR_AND, "&",is_operator_and};
		eval_data_operators[OPERATOR_OR]={OPERATOR_OR, "|",is_operator_or};
		eval_data_operators[OPERATOR_SHIFT_LEFT]={OPERATOR_SHIFT_LEFT, "<<",is_operator_shift_left};
		eval_data_operators[OPERATOR_SHIFT_RIGHT]={OPERATOR_SHIFT_RIGHT, ">>",is_operator_shift_right};
		eval_data_operators[OPERATOR_LOGIC_AND]={OPERATOR_LOGIC_AND, "&&",is_operator_logic_and};
		eval_data_operators[OPERATOR_LOGIC_OR]={OPERATOR_LOGIC_OR, "||",is_operator_logic_or};
		eval_data_operators[OPERATOR_EQUAL]={OPERATOR_EQUAL, "==",is_operator_logic_equal};
		eval_data_operators[OPERATOR_NOT_EQUAL]={OPERATOR_NOT_EQUAL, "!=",is_operator_logic_not_equal};
		eval_data_operators[OPERATOR_GT]={OPERATOR_GT, ">",is_operator_logic_gt};
		eval_data_operators[OPERATOR_LT]={OPERATOR_LT, "<",is_operator_logic_lt};
		eval_data_operators[OPERATOR_GTE]={OPERATOR_GTE, ">=",is_operator_logic_gte};
		eval_data_operators[OPERATOR_LTE]={OPERATOR_LTE, "<=",is_operator_logic_lte};
		eval_data_operators[OPERATOR_INSTANCEOF]={OPERATOR_INSTANCEOF, "instanceof",is_operator_instanceof};
		eval_data_operators[OPERATOR_IN]={OPERATOR_IN, "in",is_operator_in};


		eval_data_pre_operations[PRE_OPERATION_NOT]={PRE_OPERATION_NOT, "!",is_operator_logic_not};
		eval_data_pre_operations[PRE_OPERATION_TYPEOF]={PRE_OPERATION_TYPEOF, "typeof",is_operator_typeof};
		eval_data_pre_operations[PRE_OPERATION_POS]={PRE_OPERATION_POS, "+",is_operator_add};
		eval_data_pre_operations[PRE_OPERATION_NEG]={PRE_OPERATION_NEG, "-",is_operator_sub};
		eval_data_pre_operations[PRE_OPERATION_DEC]={PRE_OPERATION_DEC, "--",is_operation_dec};
		eval_data_pre_operations[PRE_OPERATION_INC]={PRE_OPERATION_INC, "++",is_operation_inc};
		eval_data_pre_operations[PRE_OPERATION_DEC_INC_INVALID]={PRE_OPERATION_DEC_INC_INVALID, "+-/-+",is_operation_dec_inc_invalid};


		eval_data_post_operations[POST_OPERATION_INC]={POST_OPERATION_INC, "++",is_operation_inc};
		eval_data_post_operations[POST_OPERATION_DEC]={POST_OPERATION_DEC, "--",is_operation_dec};
		eval_data_post_operations[POST_OPERATION_DEC_INC_INVALID]={POST_OPERATION_DEC_INC_INVALID, "+-/-+",is_operation_dec_inc_invalid};


		// special punctuators...
		eval_data_separators[SEPARATOR_COMA]={SEPARATOR_COMA, ",",NULL};
		eval_data_separators[SEPARATOR_SEMICOLON]={SEPARATOR_SEMICOLON, ";",NULL};
		eval_data_separators[SEPARATOR_PARENTHESIS_OPEN]={SEPARATOR_PARENTHESIS_OPEN, "(",NULL};
		eval_data_separators[SEPARATOR_PARENTHESIS_CLOSE]={SEPARATOR_PARENTHESIS_CLOSE, ")",NULL};
		eval_data_separators[SEPARATOR_SQUARE_BRAKET_OPEN]={SEPARATOR_SQUARE_BRAKET_OPEN, "[",NULL};
		eval_data_separators[SEPARATOR_SQUARE_BRAKET_CLOSE]={SEPARATOR_SQUARE_BRAKET_CLOSE, "]",NULL};

		// Init special punctuators...
		// Init keywords...

		eval_data_keywords[KEYWORD_UNKNOWN] = {KEYWORD_UNKNOWN, NULL,NULL};
		eval_data_keywords[KEYWORD_VAR] = {KEYWORD_VAR,"var",NULL};
		eval_data_keywords[KEYWORD_CONST] = {KEYWORD_CONST,"const",NULL};
		eval_data_keywords[KEYWORD_STATIC] = {KEYWORD_STATIC,"static",NULL};
		eval_data_keywords[KEYWORD_IF] = {KEYWORD_IF,"if",eval_keyword_if_else};
		eval_data_keywords[KEYWORD_ELSE] = {KEYWORD_ELSE,"else",NULL};
		eval_data_keywords[KEYWORD_FOR] = {KEYWORD_FOR,"for",eval_keyword_for};
		eval_data_keywords[KEYWORD_WHILE] = {KEYWORD_WHILE,"while",eval_keyword_while};
		eval_data_keywords[KEYWORD_DO_WHILE] = {KEYWORD_DO_WHILE,"do",eval_keyword_do_while}; // while is expected in the end ...

		eval_data_keywords[KEYWORD_SWITCH] = {KEYWORD_SWITCH,"switch",eval_keyword_switch};
		eval_data_keywords[KEYWORD_CASE] = {KEYWORD_CASE,"case",NULL};
		eval_data_keywords[KEYWORD_DEFAULT] = {KEYWORD_DEFAULT,"default",NULL};
		eval_data_keywords[KEYWORD_BREAK] = {KEYWORD_BREAK,"break",eval_keyword_break};
		eval_data_keywords[KEYWORD_CONTINUE] = {KEYWORD_CONTINUE,"continue",eval_keyword_continue};
		eval_data_keywords[KEYWORD_FUNCTION] = {KEYWORD_FUNCTION,"function",NULL};
		//eval_data_keywords[KEYWORD_ANONYMOUS_FUNCTION]={KEYWORD_ANONYMOUS_FUNCTION,"function",NULL};
		//eval_data_keywords[KEYWORD_ATTRIB] = {KEYWORD_ATTRIB,"attrib",NULL};
		eval_data_keywords[KEYWORD_RETURN] = {KEYWORD_RETURN,"return",eval_keyword_return};
		eval_data_keywords[KEYWORD_THIS] = {KEYWORD_THIS,"this", NULL};
		eval_data_keywords[KEYWORD_CLASS] = {KEYWORD_CLASS,"class",NULL};
		eval_data_keywords[KEYWORD_NEW] = {KEYWORD_NEW,"new", NULL};
		eval_data_keywords[KEYWORD_DELETE] = {KEYWORD_DELETE,"delete",eval_keyword_delete};
		eval_data_keywords[KEYWORD_REF] = {KEYWORD_REF,"ref",NULL};
		eval_data_keywords[KEYWORD_VARIABLE_ARGS] = {KEYWORD_VARIABLE_ARGS,"...",NULL};

		// DIRECTIVES
		eval_data_directives[DIRECTIVE_UNKNOWN]={DIRECTIVE_UNKNOWN, NULL};
		eval_data_directives[DIRECTIVE_IMPORT]={DIRECTIVE_IMPORT, "import"};

		compiled_symbol_name=new zs_map();//std::map<zs_string,zs_string *>;

		g_init_eval=true;
	}

	void eval_deinit(){
		if(g_init_eval){

			for(auto it=compiled_symbol_name->begin();!it.end(); it.next()){
				delete (zs_string *)it.value;
			}

			delete compiled_symbol_name;
			compiled_symbol_name=NULL;

		}
	}


}

