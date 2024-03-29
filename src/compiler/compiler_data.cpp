/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define IGNORE_BLANKS(aux_p,compiler_data,s,line) 								if((aux_p=zetscript::compiler_ignore_blanks(compiler_data,(s),line))==NULL) return 0
#define IGNORE_BLANKS_AND_GOTO_ON_ERROR(my_goto,aux_p,compiler_data,s,line) 	if((aux_p=zetscript::compiler_ignore_blanks(compiler_data,(s),line))==NULL) goto my_goto

#define RESULT_LITERAL_VALUE 							(number_part[0]+number_part[1]+number_part[2]).toConstChar()

#define EVAL_ERROR_FILE_LINE(_file,_line,_str_error,...) \
	compiler_data->error=true;\
	compiler_data->error_file=_file;\
	compiler_data->error_line=_line;\
	compiler_data->str_error=String::format(_str_error, __VA_ARGS__);\
	return 0;


#define EVAL_ERROR_FILE_LINEF(_file,_line,_str_error_in) \
		EVAL_ERROR_FILE_LINE(_file,_line,_str_error_in,NULL)


#define EVAL_ERROR_FILE_LINE_GOTO_NO_AUX(file,line,my_goto,_str_error,...)	\
		compiler_data->error=true;\
		compiler_data->error_file=file;\
		compiler_data->error_line=line;\
		compiler_data->str_error=String::format(_str_error, __VA_ARGS__);\
		goto my_goto;

#define EVAL_ERROR_FILE_LINE_GOTO_NO_AUXF(file,line,my_goto,_str_error)	\
		EVAL_ERROR_FILE_LINE_GOTO_NO_AUX(file,line,my_goto,_str_error,NULL)


#define EVAL_ERROR(_str_error,...) \
	compiler_data->error=true;\
	compiler_data->str_error=String::format(_str_error, __VA_ARGS__);\
	return 0;

#define EVAL_ERRORF(file,line,_str_error) \
	EVAL_ERROR(_str_error,NULL)





#define EVAL_ERROR_FILE_LINE_GOTO(_file,_line,_my_goto,_str_error,...) \
	compiler_data->error=true;\
	aux_p=NULL;\
	compiler_data->error_file=_file;\
	compiler_data->error_line=_line;\
	compiler_data->str_error=String::format(_str_error, __VA_ARGS__);\
	goto _my_goto;

#define EVAL_ERROR_FILE_LINE_GOTOF(_file,_line,_my_goto,_str_error)	\
	EVAL_ERROR_FILE_LINE_GOTO(_file,_line,_my_goto,_str_error,NULL);

#define EVAL_ERROR_BYTE_CODE(_str_error,...)	\
	compiler_data->error=true;\
	aux_p=NULL;\
	compiler_data->str_error=String::format(_str_error, __VA_ARGS__);\
	goto compiler_error_byte_code;

#define EVAL_ERROR_BYTE_CODEF(_str_error) \
	EVAL_ERROR_BYTE_CODE(_str_error,NULL)


#define IS_OPERATOR_TYPE_ASSIGN_WITH_OPERATION(c) (Operator::ZS_OPERATOR_ASSIGN_ADD<=(c) && (c)<=Operator::ZS_OPERATOR_ASSIGN_SHIFT_RIGHT)
#define IS_OPERATOR_TYPE_ASSIGN(c) (IS_OPERATOR_TYPE_ASSIGN_WITH_OPERATION(c) || (c)==Operator::ZS_OPERATOR_ASSIGN)

namespace zetscript{

	typedef enum{
		EVAL_EXPRESSION_ALLOW_SEQUENCE_EXPRESSION		=0x1<<1, // it tells is a regular expression in compiler or in post operation for
		EVAL_EXPRESSION_ALLOW_SEQUENCE_ASSIGNMENT		=0x1<<2, // do not allow a,b,c=0,0,0
		EVAL_EXPRESSION_BREAK_ON_ASSIGNMENT_OPERATOR	=0x1<<3, // break when any assign operator (i.e, =, +=, -=, ...) is found
		//EVAL_EXPRESSION_ON_MAIN_BLOCK					=0x1<<4,
		EVAL_EXPRESSION_FOR_IN_VARIABLES				=0x1<<4,
		EVAL_EXPRESSION_ONLY_TOKEN_SYMBOL				=0x1<<5,
		EVAL_EXPRESSION_RESET_STACK_LAST_LOAD			=0x1<<6

	}EvalExpressionProperty;

	typedef enum
		:uint8_t {
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
	typedef enum :int8_t{
		TOKEN_TYPE_UNKNOWN=0,
		TOKEN_TYPE_IDENTIFIER,  // a,b,c,d
		TOKEN_TYPE_LITERAL, // true, false, 0.1, -12e-12
		TOKEN_TYPE_OPERATOR, // +,-,%, ...
		TOKEN_TYPE_SUBEXPRESSION,
		TOKEN_TYPE_OBJECT_ARRAY,
		TOKEN_TYPE_OBJECT_SCRIPT_OBJECT,
		TOKEN_TYPE_OBJECT_FUNCTION,
		TOKEN_TYPE_NEW_OBJECT, // =new ob(); | op (new obj()) op
		TOKEN_TYPE_MAX
	}TokenType;


	typedef enum:int8_t{
		ACCESSOR_UNKNOWN=0,
		ACCESSOR_CALL, // v(1)
		ACCESSOR_ARRAY, // v[0]
		ACCESSOR_MEMBER, // a.b.c
		ACCESSOR_MAX
	}AccessorType;

	typedef enum :uint8_t {
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
		PRE_OPERATION_BWC, // ~ (for integer)
		PRE_OPERATION_MAX
	}PreOperation;

	typedef enum :uint8_t {
		POST_OPERATION_UNKNOWN=0,
		POST_OPERATION_DEC	,	// --
		POST_OPERATION_INC	,	// ++
		POST_OPERATION_DEC_INC_INVALID,	// -+ or +-
		POST_OPERATION_MAX
	}PostOperation;

	typedef enum :uint8_t {
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

		String 						value; // token value content
		int 							line;
		Vector<CompilerInstruction *> 						compiler_instructions; // byte code load literal/identifier(can be anonymous function), Vector/struct.
		bool are_instructions_moved;

		TokenNode(){
			reset();
		}

		void reset(){
			are_instructions_moved=false;
			value="";
			line=-1;
			token_type=TokenType::TOKEN_TYPE_UNKNOWN;
			operator_type=Operator::ZS_OPERATOR_UNKNOWN;
			pre_operation=PreOperation::PRE_OPERATION_UNKNOWN;
			compiler_instructions.clear();
		}
	};


	struct EvalFunction{

		Vector<CompilerInstruction *>						 		compiler_instructions;
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
			for(int i=0; i< compiler_instructions.length(); i++){
				delete compiler_instructions.get(i);
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
		char * (* compiler_fun)(CompilerData *compiler_data,const char *, int &, ScriptScope *);
	} EvalKeyword;

	typedef struct {
		Operator id;
		const char *str;
		bool (*compiler_fun)(const char *);
	} EvalOperator;

	typedef struct {
		PreOperation id;
		const char *str;
		bool (*compiler_fun)(const char *);
	} EvalPreOperation;

	typedef struct {
		PostOperation id;
		const char *str;
		bool (*compiler_fun)(const char *);
	} EvalPostOperation;

	typedef struct {
		Separator id;
		const char *str;
		bool (*compiler_fun)(const char *);
	} EvalSeparator;

	typedef struct{
		Vector<CompilerInstruction *> 						ei_break_jmps;
		int								idx_instruction_start_loop;
	}LoopBreakContinueInfo;

	struct CompilerData{
		zetscript::ScriptEngine 					* 		script_engine;
		ScriptScopesFactory 			* 		scope_factory;
		ScriptFunctionsFactory 			* 		script_function_factory;
		ScriptTypesFactory 				* 		script_types_factory;
		EvalFunction					* 		current_function;
		Vector<EvalFunction *>					compiler_functions;

		//Vector				 				global_ref_instructions; // Eval Instruction
		int										parsing_loop;

		const char *					 		current_parsing_file;
		bool							  		error;
		String									str_error;
		String									error_file;
		int 									error_line;

		CompilerData(ScriptEngine * _script_engine){
			current_parsing_file="";
			current_function=NULL;
			this->script_engine=_script_engine;
			this->script_function_factory=script_engine->getScriptFunctionsFactory();
			this->scope_factory=script_engine->getScriptScopesFactory();
			this->script_types_factory=script_engine->getScriptTypesFactory();
			error=false;
			parsing_loop=0;
			error_line=-1;
		}
	};

	EvalOperator compiler_data_operators[ZS_OPERATOR_MAX];
	EvalPreOperation compiler_data_pre_operations[PRE_OPERATION_MAX];
	EvalPostOperation compiler_data_post_operations[POST_OPERATION_MAX];
	EvalSeparator compiler_data_separators[SEPARATOR_MAX];
	EvalKeyword compiler_data_keywords[KEYWORD_MAX];
	EvalDirective compiler_data_directives[DIRECTIVE_MAX];


	bool g_init_compiler=false;

	char *  compiler_symbol(CompilerData *compiler_data,const char *start_word, int line,  ScriptScope *scope_info,TokenNode * token_node, PreOperation pre_operation, PostOperation post_operation);
	Symbol *compiler_find_local_symbol(CompilerData *compiler_data,ScriptScope *scope, const String & symbol_to_find);
	Symbol *compiler_find_global_symbol(CompilerData *compiler_data, const String & symbol_to_find);

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
	bool 	is_operator_bwc(const char *s)						{return (*s=='~');}

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
	Keyword compiler_is_keyword(const char *c);

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

	char *compiler_ignore_blanks(CompilerData *compiler_data,const char *str, int &line) {
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
				EVAL_ERROR_FILE_LINEF(
					compiler_data->current_parsing_file
					,line
					," Unexpected end comment block"
				);
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
			if(*compiler_data_separators[i].str == *s){
				return compiler_data_separators[i].id;
			}
		}
		return Separator::SEPARATOR_UNKNOWN;
	}

	Operator   is_operator(const char *s){
		for(unsigned char i = 1; i < ZS_OPERATOR_MAX; i++){
			if(compiler_data_operators[i].compiler_fun(s)){
				return compiler_data_operators[i].id;
			}
		}
		return Operator::ZS_OPERATOR_UNKNOWN;
	}

	unsigned char get_preference_operator_group(Operator _operator){
		if(ZS_PREFERENCE_OPERATOR_GROUP_0(_operator)){
			return 0;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_1(_operator)){
			return 1;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_2(_operator)){
			return 2;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_3(_operator)){
			return 3;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_4(_operator)){
			return 4;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_5(_operator)){
			return 5;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_6(_operator)){
			return 6;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_7(_operator)){
			return 7;
		}else if (ZS_PREFERENCE_OPERATOR_GROUP_8(_operator)){
			return 8;
		}

		return ZS_PREFERENCE_OPERATOR_GROUP_MAX;

	}

	PreOperation   	is_pre_operation(const char *s){
		for(unsigned char i = 1; i < PRE_OPERATION_MAX; i++){
			if(compiler_data_pre_operations[i].compiler_fun(s)){
				return compiler_data_pre_operations[i].id;
			}
		}
		return PreOperation::PRE_OPERATION_UNKNOWN;
	}

	PostOperation   is_post_operation(const char *s){
		for(unsigned char i = 1; i < POST_OPERATION_MAX; i++){
			if(compiler_data_post_operations[i].compiler_fun(s)){
				return compiler_data_post_operations[i].id;
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

	Keyword compiler_is_keyword(const char *c){
		// PRE: The first char must be starting symbol.
		char *str=(char *)c;
		//char *aux=str;

		for(int i = 0; i < KEYWORD_MAX; i++){
			if(compiler_data_keywords[i].str!=NULL){
				size_t size = strlen(compiler_data_keywords[i].str);
				char *aux = str+size;
				if((strncmp(str,compiler_data_keywords[i].str,size)==0) && (
						is_special_char(aux) ||

					   is_comment_block_start(aux)) //start block comment
				){
					return compiler_data_keywords[i].id;
				}
			}
		}
		return Keyword::KEYWORD_UNKNOWN;
	}

	Directive is_directive(const char *c){
		// PRE: The first char must be starting symbol.
		char *str=(char *)c;

		for(int i = 0; i < DIRECTIVE_MAX; i++){
			if(compiler_data_directives[i].str){
				size_t size = strlen(compiler_data_directives[i].str);

				if(strncmp(str,compiler_data_directives[i].str,size)==0)
				{
					return compiler_data_directives[i].id;
				}
			}
		}
		return Directive::DIRECTIVE_UNKNOWN;
	}

	const char * get_mapped_name(CompilerData *compiler_data, const String & _mapped_name){
		ZS_UNUSUED_PARAM(compiler_data);
		char * key=(char *)compiler_data->script_engine->getCompiledSymbolName()->getKey(_mapped_name.toConstChar());
		if(key==NULL){
			auto node=compiler_data->script_engine->getCompiledSymbolName()->set(_mapped_name.toConstChar(),0);
			key=node->key;
		}
		return key;
	}

	bool  is_end_symbol_token(char *s, char pre=0){

		Operator _operator=is_operator(s);
		return    (_operator!=Operator::ZS_OPERATOR_UNKNOWN && _operator!=Operator::ZS_OPERATOR_INSTANCEOF && _operator!=Operator::ZS_OPERATOR_IN)
			   || is_post_operation(s)!=PostOperation::POST_OPERATION_UNKNOWN
			   || is_separator(s)!=Separator::SEPARATOR_UNKNOWN
			   || is_special_char(s)
			   || (*s=='\"' && pre!='\\');
	}

	bool  is_anonymous_function(CompilerData *compiler_data,const char *s, int line){
		Keyword key_w;
		char *aux_p = (char *)s;

		key_w = compiler_is_keyword(aux_p);

		if(key_w == Keyword::KEYWORD_FUNCTION){
			IGNORE_BLANKS(aux_p,compiler_data,aux_p+strlen(compiler_data_keywords[key_w].str),line);

			if(*aux_p == '('){
				return true;
			}
		}

		return false;
	}

	int  check_identifier_name_expression_ok(CompilerData *compiler_data,const String & symbol, int line){

		char *aux_p = (char *)symbol.toConstChar();
		Keyword kw;
		Operator op;

		if((kw=compiler_is_keyword(aux_p))!=Keyword::KEYWORD_UNKNOWN){
			EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line," Unexpected '%s' keyword", compiler_data_keywords[kw].str);
		}

		if((op=is_operator(aux_p))!=Operator::ZS_OPERATOR_UNKNOWN){
			EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line," Unexpected '%s' operator",  compiler_data_operators[op].str);
		}

		// avoid special literal words
		if(symbol=="true"
			|| symbol == "false"
			|| symbol=="null"
		){
			EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line," Unexpected '%s'", aux_p);
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

	char * parse_literal_number(CompilerData *compiler_data,const char *s, int & line, String & value){
		// PRE: a String given...
		char *aux_p = NULL;
		bool end=false;
		int current_part=0;
		String number_part[3];
		//value="";
		bool is_hexa=false;
		bool is01s=true;
		//bool isInt=true;
		bool is_float=false;

		IGNORE_BLANKS(aux_p,compiler_data,s,line);


		if(compiler_is_keyword(s)){
			 return NULL;
		}

		if(*aux_p == '\''){ // is char
			int i_char=0;
			aux_p++;
			if(*aux_p == '\\'){ // is special char
				aux_p++;
				switch(*aux_p){
				default:
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"unknown special char \\%c",*aux_p);
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
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"Unterminated char \'");
				}

				i_char=*aux_p;
			}else{
				EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"empty character constant");
			}

			aux_p++;
			if(*aux_p != '\''){
				while(*aux_p!=0 && *aux_p!='\n' && *aux_p!='\'' ) aux_p++;

				if(*aux_p == '\''){
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"multi-character character constant");
				}else{
					EVAL_ERROR_FILE_LINEF(compiler_data->current_parsing_file,line ,"Unterminated char \'");
				}
			}

			aux_p++;

			// convert i_char to value...
			value=Integer::toString(i_char);
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
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Invalid number format '%s'",RESULT_LITERAL_VALUE);
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
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Invalid number format '%s'",RESULT_LITERAL_VALUE);
				}
			}

			else if(*aux_p == 'b'){ // is end binary format?
				if(!is01s || (current_part != 0)){
					number_part[current_part]+=*aux_p; // save conflicting character
					EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Invalid number format '%s'",RESULT_LITERAL_VALUE);
				}

				number_part[current_part]+=*aux_p;
				end=true;
			}
			else{
				number_part[current_part]+=*aux_p; // save conflicting character
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line ,"Invalid number format '%s'",RESULT_LITERAL_VALUE);
			}
			is01s&=(('0'==*aux_p) || ('1'==*aux_p));

			aux_p++;

		}while(!end && !((is_end_symbol_token(aux_p) || (is_special_char(aux_p))) && *aux_p != '.'));

		// check and compiler token ?
		value = number_part[0]+number_part[1]+number_part[2];

		if( is_end_symbol_token(aux_p) || compiler_is_keyword(s)){
			 return aux_p;
		}

		return NULL;
		// POST: detects integer/binary/fractional/hexa
	}

	char *  get_name_identifier_token(CompilerData *compiler_data,const char *s, int line, String & name, bool _throw_error=true){

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
				EVAL_ERROR_FILE_LINE(compiler_data->current_parsing_file,line,"Unexpected char '%c'",*aux_p);
			}
		}
		return NULL;
	}

	InstructionSourceInfo compiler_instruction_source_info(
			CompilerData *_compiler_data
			,const char *_file
			,int _line
			,const String & _name
	){
		InstructionSourceInfo is=InstructionSourceInfo(_file,_line);
		is.ptr_str_symbol_name=get_mapped_name(_compiler_data,_name);
		return is;
	}

	// PROTOTYPES
	void compiler_init(){

		if(g_init_compiler) return;

		// Init operator punctuators...
		memset(compiler_data_operators,0,sizeof(compiler_data_operators));
		memset(compiler_data_pre_operations,0,sizeof(compiler_data_pre_operations));
		memset(compiler_data_post_operations,0,sizeof(compiler_data_post_operations));
		memset(compiler_data_separators,0,sizeof(compiler_data_separators));
		memset(compiler_data_keywords,0,sizeof(compiler_data_keywords));

		compiler_data_operators[ZS_OPERATOR_UNKNOWN]={ZS_OPERATOR_UNKNOWN, "unknown-operator",NULL};

		compiler_data_operators[ZS_OPERATOR_ADD]={ZS_OPERATOR_ADD, "+",is_operator_add};
		compiler_data_operators[ZS_OPERATOR_SUB]={ZS_OPERATOR_SUB, "-",is_operator_sub};
		compiler_data_operators[ZS_OPERATOR_MUL]={ZS_OPERATOR_MUL, "*",is_operator_mul};
		compiler_data_operators[ZS_OPERATOR_DIV]={ZS_OPERATOR_DIV, "/",is_operator_div};
		compiler_data_operators[ZS_OPERATOR_MOD]={ZS_OPERATOR_MOD, "%",is_operator_mod};

		compiler_data_operators[ZS_OPERATOR_TERNARY_IF]={ZS_OPERATOR_TERNARY_IF, "?",is_operator_ternary_if};
		compiler_data_operators[ZS_OPERATOR_TERNARY_ELSE]={ZS_OPERATOR_TERNARY_ELSE, ":",is_operator_ternary_else};
		compiler_data_operators[ZS_OPERATOR_ASSIGN]={ZS_OPERATOR_ASSIGN, "=",is_operator_assign};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_ADD]={ZS_OPERATOR_ASSIGN_ADD, "+=",is_operator_assign_add};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_SUB]={ZS_OPERATOR_ASSIGN_SUB, "-=",is_operator_assign_sub};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_MUL]={ZS_OPERATOR_ASSIGN_MUL, "*=",is_operator_assign_mul};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_DIV]={ZS_OPERATOR_ASSIGN_DIV, "/=",is_operator_assign_div};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_MOD]={ZS_OPERATOR_ASSIGN_MOD, "%=",is_operator_assign_mod};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_XOR]={ZS_OPERATOR_ASSIGN_XOR,"^=",is_operator_assign_xor};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_AND]={ZS_OPERATOR_ASSIGN_AND,"&=",is_operator_assign_and};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_OR]={ZS_OPERATOR_ASSIGN_OR,"|=",is_operator_assign_or};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_SHIFT_LEFT]={ZS_OPERATOR_ASSIGN_SHIFT_LEFT,"<<=",is_operator_assign_shift_left};
		compiler_data_operators[ZS_OPERATOR_ASSIGN_SHIFT_RIGHT]={ZS_OPERATOR_ASSIGN_SHIFT_RIGHT,">>=",is_operator_assign_shift_right};

		compiler_data_operators[ZS_OPERATOR_XOR]={ZS_OPERATOR_XOR, "^",is_operator_xor};
		compiler_data_operators[ZS_OPERATOR_AND]={ZS_OPERATOR_AND, "&",is_operator_and};
		compiler_data_operators[ZS_OPERATOR_OR]={ZS_OPERATOR_OR, "|",is_operator_or};
		compiler_data_operators[ZS_OPERATOR_SHIFT_LEFT]={ZS_OPERATOR_SHIFT_LEFT, "<<",is_operator_shift_left};
		compiler_data_operators[ZS_OPERATOR_SHIFT_RIGHT]={ZS_OPERATOR_SHIFT_RIGHT, ">>",is_operator_shift_right};
		compiler_data_operators[ZS_OPERATOR_LOGIC_AND]={ZS_OPERATOR_LOGIC_AND, "&&",is_operator_logic_and};
		compiler_data_operators[ZS_OPERATOR_LOGIC_OR]={ZS_OPERATOR_LOGIC_OR, "||",is_operator_logic_or};
		compiler_data_operators[ZS_OPERATOR_EQUAL]={ZS_OPERATOR_EQUAL, "==",is_operator_logic_equal};
		compiler_data_operators[ZS_OPERATOR_NOT_EQUAL]={ZS_OPERATOR_NOT_EQUAL, "!=",is_operator_logic_not_equal};
		compiler_data_operators[ZS_OPERATOR_GT]={ZS_OPERATOR_GT, ">",is_operator_logic_gt};
		compiler_data_operators[ZS_OPERATOR_LT]={ZS_OPERATOR_LT, "<",is_operator_logic_lt};
		compiler_data_operators[ZS_OPERATOR_GTE]={ZS_OPERATOR_GTE, ">=",is_operator_logic_gte};
		compiler_data_operators[ZS_OPERATOR_LTE]={ZS_OPERATOR_LTE, "<=",is_operator_logic_lte};
		compiler_data_operators[ZS_OPERATOR_INSTANCEOF]={ZS_OPERATOR_INSTANCEOF, "instanceof",is_operator_instanceof};
		compiler_data_operators[ZS_OPERATOR_IN]={ZS_OPERATOR_IN, "in",is_operator_in};


		compiler_data_pre_operations[PRE_OPERATION_NOT]={PRE_OPERATION_NOT, "!",is_operator_logic_not};
		compiler_data_pre_operations[PRE_OPERATION_TYPEOF]={PRE_OPERATION_TYPEOF, "typeof",is_operator_typeof};
		compiler_data_pre_operations[PRE_OPERATION_POS]={PRE_OPERATION_POS, "+",is_operator_add};
		compiler_data_pre_operations[PRE_OPERATION_NEG]={PRE_OPERATION_NEG, "-",is_operator_sub};
		compiler_data_pre_operations[PRE_OPERATION_BWC]={PRE_OPERATION_BWC, "~",is_operator_bwc};
		compiler_data_pre_operations[PRE_OPERATION_DEC]={PRE_OPERATION_DEC, "--",is_operation_dec};
		compiler_data_pre_operations[PRE_OPERATION_INC]={PRE_OPERATION_INC, "++",is_operation_inc};
		compiler_data_pre_operations[PRE_OPERATION_DEC_INC_INVALID]={PRE_OPERATION_DEC_INC_INVALID, "+-/-+",is_operation_dec_inc_invalid};


		compiler_data_post_operations[POST_OPERATION_INC]={POST_OPERATION_INC, "++",is_operation_inc};
		compiler_data_post_operations[POST_OPERATION_DEC]={POST_OPERATION_DEC, "--",is_operation_dec};
		compiler_data_post_operations[POST_OPERATION_DEC_INC_INVALID]={POST_OPERATION_DEC_INC_INVALID, "+-/-+",is_operation_dec_inc_invalid};


		// special punctuators...
		compiler_data_separators[SEPARATOR_COMA]={SEPARATOR_COMA, ",",NULL};
		compiler_data_separators[SEPARATOR_SEMICOLON]={SEPARATOR_SEMICOLON, ";",NULL};
		compiler_data_separators[SEPARATOR_PARENTHESIS_OPEN]={SEPARATOR_PARENTHESIS_OPEN, "(",NULL};
		compiler_data_separators[SEPARATOR_PARENTHESIS_CLOSE]={SEPARATOR_PARENTHESIS_CLOSE, ")",NULL};
		compiler_data_separators[SEPARATOR_SQUARE_BRAKET_OPEN]={SEPARATOR_SQUARE_BRAKET_OPEN, "[",NULL};
		compiler_data_separators[SEPARATOR_SQUARE_BRAKET_CLOSE]={SEPARATOR_SQUARE_BRAKET_CLOSE, "]",NULL};

		// Init special punctuators...
		// Init keywords...

		compiler_data_keywords[KEYWORD_UNKNOWN] = {KEYWORD_UNKNOWN, NULL,NULL};
		compiler_data_keywords[KEYWORD_VAR] = {KEYWORD_VAR,"var",NULL};
		compiler_data_keywords[KEYWORD_CONST] = {KEYWORD_CONST,"const",NULL};
		compiler_data_keywords[KEYWORD_STATIC] = {KEYWORD_STATIC,"static",NULL};
		compiler_data_keywords[KEYWORD_IF] = {KEYWORD_IF,"if",compiler_keyword_if_else};
		compiler_data_keywords[KEYWORD_ELSE] = {KEYWORD_ELSE,"else",NULL};
		compiler_data_keywords[KEYWORD_FOR] = {KEYWORD_FOR,"for",compiler_keyword_for};
		compiler_data_keywords[KEYWORD_WHILE] = {KEYWORD_WHILE,"while",compiler_keyword_while};
		compiler_data_keywords[KEYWORD_DO_WHILE] = {KEYWORD_DO_WHILE,"do",compiler_keyword_do_while}; // while is expected in the end ...

		compiler_data_keywords[KEYWORD_SWITCH] = {KEYWORD_SWITCH,"switch",compiler_keyword_switch};
		compiler_data_keywords[KEYWORD_CASE] = {KEYWORD_CASE,"case",NULL};
		compiler_data_keywords[KEYWORD_DEFAULT] = {KEYWORD_DEFAULT,"default",NULL};
		compiler_data_keywords[KEYWORD_BREAK] = {KEYWORD_BREAK,"break",compiler_keyword_break};
		compiler_data_keywords[KEYWORD_CONTINUE] = {KEYWORD_CONTINUE,"continue",compiler_keyword_continue};
		compiler_data_keywords[KEYWORD_FUNCTION] = {KEYWORD_FUNCTION,"function",NULL};
		//compiler_data_keywords[KEYWORD_ANONYMOUS_FUNCTION]={KEYWORD_ANONYMOUS_FUNCTION,"function",NULL};
		//compiler_data_keywords[KEYWORD_ATTRIB] = {KEYWORD_ATTRIB,"attrib",NULL};
		compiler_data_keywords[KEYWORD_RETURN] = {KEYWORD_RETURN,"return",compiler_keyword_return};
		compiler_data_keywords[KEYWORD_THIS] = {KEYWORD_THIS,"this", NULL};
		compiler_data_keywords[KEYWORD_CLASS] = {KEYWORD_CLASS,"class",NULL};
		compiler_data_keywords[KEYWORD_NEW] = {KEYWORD_NEW,"new", NULL};
		compiler_data_keywords[KEYWORD_DELETE] = {KEYWORD_DELETE,"delete",compiler_keyword_delete};
		compiler_data_keywords[KEYWORD_REF] = {KEYWORD_REF,"ref",NULL};
		compiler_data_keywords[KEYWORD_VARIABLE_ARGS] = {KEYWORD_VARIABLE_ARGS,"...",NULL};

		// DIRECTIVES
		compiler_data_directives[DIRECTIVE_UNKNOWN]={DIRECTIVE_UNKNOWN, NULL};
		compiler_data_directives[DIRECTIVE_IMPORT]={DIRECTIVE_IMPORT, "import"};


		g_init_compiler=true;
	}

	void compiler_deinit(){
		if(g_init_compiler){
			g_init_compiler=false;
		}
	}


}

