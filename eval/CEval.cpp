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
S <= "[all characters]" --> token std::string


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
		� break;
	case C1:
		� break;
		� ..
	case CN:
		� break;
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

#include "zetscript.h"



#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr zs_print_debug_cr
#else
#define print_eval_cr(s,...)
#endif


namespace zs{

	#define FORMAT_PRINT_INSTRUCTION "%04i"

	void  		writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);

	CEval * CEval::eval_singleton = NULL;


	bool IS_TERNARY_IF_OPERATOR(const char *s)			{return ((*s=='?'));}
	bool IS_TERNARY_ELSE_OPERATOR(const char *s)		{return ((*s==':'));}
	bool IS_ADD_OPERATOR(const char *s)					{return	((*s=='+') && (*(s+1)!='+') && (*(s+1)!='='));}
	bool IS_SUB_OPERATOR(const char *s)					{return	((*s=='-') && (*(s+1)!='-') && (*(s+1)!='='));}
	bool IS_MUL_OPERATOR(const char *s)					{return ((*s=='*') && (*(s+1)!='='));}
	bool IS_DIV_OPERATOR(const char *s)					{return ((*s=='/') && (*(s+1)!='='));}
	bool IS_MOD_OPERATOR(const char *s)					{return ((*s=='%') && (*(s+1)!='='));}
	bool IS_ASSIGN_OPERATOR(const char *s)				{return	((*s=='=') && (*(s+1)!='='));}
	bool IS_ASSIGN_ADD_OPERATOR(const char *s)			{return ((*s=='+') && (*(s+1)=='='));}
	bool IS_ASSIGN_SUB_OPERATOR(const char *s)			{return ((*s=='-') && (*(s+1)=='='));}
	bool IS_ASSIGN_MUL_OPERATOR(const char *s)			{return ((*s=='*') && (*(s+1)=='='));}
	bool IS_ASSIGN_DIV_OPERATOR(const char *s)			{return ((*s=='/') && (*(s+1)=='='));}
	bool IS_ASSIGN_MOD_OPERATOR(const char *s)			{return ((*s=='%') && (*(s+1)=='='));}
	bool IS_ASSIGN_BINARY_XOR_OPERATOR(const char *s)	{return ((*s=='^') && (*(s+1)=='='));}		// ^=
	bool IS_ASSIGN_BINARY_AND_OPERATOR(const char *s)	{return ((*s=='&') && (*(s+1)=='='));}		// &=
	bool IS_ASSIGN_BINARY_OR_OPERATOR(const char *s)	{return ((*s=='|') && (*(s+1)=='='));}  	// |=
	bool IS_ASSIGN_SHIFT_LEFT_OPERATOR(const char *s)	{return ((*s=='<') && (*(s+1)=='<')&& (*(s+2)=='='));} 	// <<=
	bool IS_ASSIGN_SHIFT_RIGHT_OPERATOR(const char *s)	{return ((*s=='>') && (*(s+1)=='>')&& (*(s+2)=='='));} 	// >>=
	bool IS_BINARY_XOR_OPERATOR(const char *s)			{return ((*s=='^') && (*(s+1)!='='));}
	bool IS_BINARY_AND_OPERATOR(const char *s)			{return ((*s=='&') && (*(s+1)!='&') && (*(s+1)!='='));}
	bool IS_BINARY_OR_OPERATOR(const char *s)			{return ((*s=='|') && (*(s+1)!='|') && (*(s+1)!='='));}
	bool IS_SHIFT_LEFT_OPERATOR(const char *s)			{return ((*s=='<') && (*(s+1)=='<') && (*(s+2)!='='));}
	bool IS_SHIFT_RIGHT_OPERATOR(const char *s)			{return	((*s=='>') && (*(s+1)=='>') && (*(s+2)!='='));}
	bool IS_LOGIC_AND_OPERATOR(const char *s)			{return ((*s=='&') && (*(s+1)=='&'));}
	bool IS_LOGIC_OR_OPERATOR(const char *s)			{return ((*s=='|') && (*(s+1)=='|'));}
	bool IS_LOGIC_EQUAL_OPERATOR(const char *s) 		{return ((*s=='=') && (*(s+1)=='='));}
	bool IS_LOGIC_NOT_EQUAL_OPERATOR(const char *s)		{return ((*s=='!') && (*(s+1)=='='));}
	bool IS_LOGIC_GT_OPERATOR(const char *s)			{return ((*s=='>') && (*(s+1)!='>'));}
	bool IS_LOGIC_LT_OPERATOR(const char *s)			{return ((*s=='<') && (*(s+1)!='<'));}
	bool IS_LOGIC_GTE_OPERATOR(const char *s)			{return ((*s=='>') && (*(s+1)=='='));}
	bool IS_LOGIC_LTE_OPERATOR(const char *s)			{return ((*s=='<') && (*(s+1)=='='));}
	bool IS_LOGIC_NOT_OPERATOR(const char *s)			{return ((*s=='!') && (*(s+1)!='='));}
	bool IS_INC_OPERATOR(const char *s)					{return ((*s=='+') && (*(s+1)=='+'));}
	bool IS_DEC_OPERATOR(const char *s)					{return ((*s=='-') && (*(s+1)=='-'));}
	bool IS_SINGLE_COMMENT(char *s)						{return	((*s=='/') && (*(s+1)=='/'));}
	bool IS_START_BLOCK_COMMENT(char *s)				{return ((*s=='/') && (*(s+1)=='*'));}
	bool IS_END_BLOCK_COMMENT(char *s)					{return ((*s=='*') && (*(s+1)=='/'));}
	bool IS_INSTANCEOF_OPERATOR(const char *s)			{return strncmp("instanceof",s,10) == 0				 ;}


	char *ADVANCE_TO_END_COMMENT(char *aux_p, int &line){

		if(IS_START_BLOCK_COMMENT(aux_p)){
			aux_p+=2; //advance first
			bool end = false;
			while(*aux_p != 0  && !end){//!IS_END_BLOCK_COMMENT(aux_p) && *aux_p != 0){

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

			else if(IS_START_BLOCK_COMMENT(aux_p)){
				// ignore until get the end of the comment...
				aux_p = ADVANCE_TO_END_COMMENT(aux_p, line);

				if(IS_END_BLOCK_COMMENT(aux_p))
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

	const char * 		CEval::getOpCodeStr(OP_CODE  op){
		if(op < OP_CODE::MAX_OP_CODES){
			return eval_singleton->defined_opcode[op].str;

		}

		return "unknow_op";
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO


	 std::string CEval::getStrTypeLoadValue(CScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction){

		 char print_aux_load_value[512] = {0};
		 char object_access[512] = "";

		 tInstruction * instruction =&m_listStatements[current_instruction];
		 tInfoConstantValue *icv;
		 std::string symbol_value=INSTRUCTION_GET_SYMBOL_NAME(current_function,instruction);
		 if(instruction->op_code != LOAD){
			 return "ERROR";
		 }



		 sprintf(print_aux_load_value,"UNDEFINED");

		 if(instruction->properties & INS_PROPERTY_ACCESS_SCOPE){

			 sprintf(object_access,
					"[" FORMAT_PRINT_INSTRUCTION "]."

					,(int)instruction->index_op2);
		 }
		 else if(instruction->properties & INS_PROPERTY_THIS_SCOPE){
			sprintf(object_access,"this.");
		 }

		 switch(instruction->index_op1){

			case LOAD_TYPE::LOAD_TYPE_CONSTANT:
				icv=(tInfoConstantValue *)instruction->index_op2;
				switch(icv->properties){
				case STK_PROPERTY_TYPE_BOOLEAN:
				case STK_PROPERTY_TYPE_INTEGER:
					sprintf(print_aux_load_value,"CONST(%i)",(int)((intptr_t)icv->stkValue));
					break;
				case STK_PROPERTY_TYPE_NUMBER:
					sprintf(print_aux_load_value,"CONST(%f)",*((float *)&icv->stkValue));
					break;
				case STK_PROPERTY_TYPE_STRING:
					sprintf(print_aux_load_value,"CONST(%s)",((const char *)icv->stkValue));
					break;

				}
				break;

			case LOAD_TYPE::LOAD_TYPE_VARIABLE:
				sprintf(print_aux_load_value,"%sVAR(%s)",object_access,symbol_value.c_str());
				break;
			case LOAD_TYPE::LOAD_TYPE_FUNCTION:

				sprintf(print_aux_load_value,"%sFUN(%s)",object_access,symbol_value.c_str());
				break;

			case LOAD_TYPE::LOAD_TYPE_ARGUMENT:
				sprintf(print_aux_load_value,"ARG(%s)",symbol_value.c_str());
				break;
			default:

				break;
		}
		return print_aux_load_value;
	 }

	 void CEval::printGeneratedCode(CScriptFunction *sfo){

		// PRE: it should printed after compile and updateReferences.
		std::string pre="";
		std::string post="";

		unsigned idx_instruction=0;
		for(tInstruction * instruction=sfo->instruction; instruction->op_code!= END_FUNCTION; instruction++,idx_instruction++){

			int n_ops=0;
			int index_op1 = instruction->index_op1;
			int index_op2 = instruction->index_op2;

			if(index_op1 != -1)
				n_ops++;

			 if(index_op2 != -1)
				 n_ops++;

			 pre="";
			 post="";

				switch(GET_INS_PROPERTY_PRE_POST_OP(instruction->properties)){
				case INS_PROPERTY_PRE_NEG:
					pre="-";
					break;
				case INS_PROPERTY_PRE_INC:
					pre="++";
					break;
				case INS_PROPERTY_PRE_DEC:
					pre="--";
					break;
				case INS_PROPERTY_POST_INC:
					post="++";
					break;
				case INS_PROPERTY_POST_DEC:
					post="--";
					break;
				default:
					// check whether is constant and numeric
					if(instruction->op_code==OP_CODE::LOAD && instruction->index_op1==LOAD_TYPE_CONSTANT){
						tInfoConstantValue *icv = (((tInfoConstantValue *)instruction->index_op2));
						float n;

						// change the sign
						switch(icv->properties){
						default:
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if(((intptr_t)icv->stkValue)<0){
								pre="-";
							}
							break;
						case STK_PROPERTY_TYPE_NUMBER:
							memcpy(&n,&icv->stkValue,sizeof(float));
							if(n<0){
								pre="-";
							}
							break;
						}
					}
					break;

				}
			switch(instruction->op_code){

			case  NEW:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n",idx_instruction,CEval::getOpCodeStr(instruction->op_code),instruction->index_op1!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(instruction->index_op1):"???");
				break;
			case  LOAD:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s\n"
						,idx_instruction,
						CEval::getOpCodeStr(instruction->op_code),
						pre.c_str(),
						getStrTypeLoadValue(sfo,sfo->instruction,idx_instruction).c_str(),
						post.c_str());
				break;
			case JNT:
			case JT:
			case JMP:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%03i\n"
						,idx_instruction
						,CEval::getOpCodeStr(instruction->op_code)
						,(int)instruction->index_op2);
				break;
			case PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,CEval::getOpCodeStr(instruction->op_code)
						,instruction->index_op1!=0?'(':' '
						,instruction->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
						,instruction->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
						,instruction->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
						,instruction->index_op1!=0?')':' '
						);
				break;
			case POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,CEval::getOpCodeStr(instruction->op_code)
						,instruction->index_op1!=0?'(':' '
						,instruction->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
						,instruction->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
						,instruction->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
						,instruction->index_op1!=0?')':' '
						);
				break;
			default:

				if(n_ops==0){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n",idx_instruction,CEval::getOpCodeStr(instruction->op_code));
				}else if(n_ops==1){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\n"
							,idx_instruction
							,CEval::getOpCodeStr(instruction->op_code)
							,(instruction->properties & STK_PROPERTY_READ_TWO_POP_ONE)?"_CS":""
							);
				}else{
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
							,idx_instruction
							,CEval::getOpCodeStr(instruction->op_code)
							);
				}
				break;
			}
		}


		// and then print its functions ...
		std::vector<CScriptFunction *> * m_vf = &sfo->m_function;

		for(unsigned j =0; j < m_vf->size(); j++){

			CScriptFunction *local_irfs = (*m_vf)[j];

			if(( local_irfs->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
				std::string symbol_ref="????";


				//strcpy(symbol_ref,AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

				if(local_irfs->idxClass!=ZS_INVALID_CLASS){
					CScriptClass *sc = GET_SCRIPT_CLASS(local_irfs->idxClass);
					if(sc->idxClass == IDX_CLASS_MAIN){
						symbol_ref="Main";
					}else{
						symbol_ref=sfo->symbol_info.symbol->name+std::string("::")+std::string("????");
					}
				}

				printf("-------------------------------------------------------\n");
				printf("\nCode for function \"%s\"\n\n",symbol_ref.c_str());
				printGeneratedCode(m_vf->at(j));
			}
		}
	 }

	 void CEval::printGeneratedCode(){

		 std::vector<CScriptClass *> *vec_script_class_node=CScriptClassFactory::getInstance()->getVectorScriptClassNode();
		 // for all classes print code...
		 for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			 CScriptClass *rc=vec_script_class_node->at(i);
			 for(unsigned f = 0; f < rc->m_function.size(); f++){
				 printGeneratedCode(rc->m_function[f]);
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------



	CEval * CEval::getInstance(){
		if(eval_singleton == NULL){
			eval_singleton = new CEval();
			eval_singleton->iniVars();
		}

		return eval_singleton;
	}


	void CEval::destroySingleton(){
		if(eval_singleton != NULL){
			delete eval_singleton;
		}
	}

	bool CEval::evalString(const std::string & expression){
		return getInstance()->evalStringInternal(expression);
	}

	bool CEval::evalFile(const std::string & filename){
		return getInstance()->evalFileInternal(filename);
	}

	//--------------------------------
	 // FILE MANAGEMENT


	bool CEval::isFilenameAlreadyParsed(const std::string & filename){
		for(unsigned i = 0; i < m_parsedSource.size(); i++){
			if(m_parsedSource.at(i).filename==filename){
				return true;
			}
		}
		return false;
	}

	const char * CEval::getParsedFilenameFromIdx(unsigned idx){

		if(idx >= m_parsedSource.size()){
			THROW_RUNTIME_ERROR("out of bounds");
			return DEFAULT_NO_FILENAME;
		}

		return m_parsedSource.at(idx).filename.c_str();
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONSTANT MANAGEMENT

	tInfoConstantValue *CEval::getConstant(const std::string & const_name){

		if((m_contantPool).count(const_name) == 1){
			return (m_contantPool)[const_name];
		}
		return NULL;
	}

	tInfoConstantValue * CEval::addConstant(const std::string & const_name, void *obj, unsigned short properties){

		tInfoConstantValue * info_ptr=NULL;

		if(getConstant(const_name) == NULL){
			info_ptr=new tInfoConstantValue;
			*info_ptr={obj,NULL,properties};
			(m_contantPool)[const_name]=info_ptr;
		}else{
			THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("internal:constant %s already exist",const_name.c_str()));
		}

		return info_ptr;
	}

	tInfoConstantValue * CEval::addConstant(const std::string & const_name, int _value){
		intptr_t value = _value;
		unsigned short type=STK_PROPERTY_TYPE_INTEGER;
		tStackElement *stk;

		if((stk = getConstant(const_name))!=NULL){
			return stk;
		}

		return addConstant(const_name,(void *)value,type);

	}

	// CONSTANT MANAGEMENT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------

	std::string * CEval::getCompiledSymbol(const std::string & s){
		if(this->m_compiledSymbolName.count(s)==0){
			this->m_compiledSymbolName[s]=new std::string (s);
		}
		return this->m_compiledSymbolName[s];
	}

	void CEval::iniVars(){

		using namespace std::placeholders;

		pCurrentFunctionInfo=NULL;
		CURRENT_PARSING_FILE_IDX=-1;
		CURRENT_PARSING_FILE_STR=NULL;

		// init operator punctuators...
		memset(defined_operator,0,sizeof(defined_operator));
		memset(defined_pre_operator,0,sizeof(defined_pre_operator));
		memset(defined_identity_operator,0,sizeof(defined_identity_operator));
		memset(defined_separator,0,sizeof(defined_separator));
		memset(defined_keyword,0,sizeof(defined_keyword));
		memset(defined_opcode,0,sizeof(defined_opcode));


		defined_operator[UNKNOWN_OPERATOR]={UNKNOWN_OPERATOR, "none",NULL};

		defined_operator[ADD_OPERATOR]={ADD_OPERATOR, "+",IS_ADD_OPERATOR};
		defined_operator[SUB_OPERATOR]={SUB_OPERATOR, "-",IS_SUB_OPERATOR};
		defined_operator[MUL_OPERATOR]={MUL_OPERATOR, "*",IS_MUL_OPERATOR};
		defined_operator[DIV_OPERATOR]={DIV_OPERATOR, "/",IS_DIV_OPERATOR};
		defined_operator[MOD_OPERATOR]={MOD_OPERATOR, "%",IS_MOD_OPERATOR};

		defined_operator[TERNARY_IF_OPERATOR]={TERNARY_IF_OPERATOR, "?",IS_TERNARY_IF_OPERATOR};
		defined_operator[TERNARY_ELSE_OPERATOR]={TERNARY_ELSE_OPERATOR, ":",IS_TERNARY_ELSE_OPERATOR};
		defined_operator[ASSIGN_OPERATOR]={ASSIGN_OPERATOR, "=",IS_ASSIGN_OPERATOR};
		defined_operator[ASSIGN_ADD_OPERATOR]={ASSIGN_ADD_OPERATOR, "+=",IS_ASSIGN_ADD_OPERATOR};
		defined_operator[ASSIGN_SUB_OPERATOR]={ASSIGN_SUB_OPERATOR, "-=",IS_ASSIGN_SUB_OPERATOR};
		defined_operator[ASSIGN_MUL_OPERATOR]={ASSIGN_MUL_OPERATOR, "*=",IS_ASSIGN_MUL_OPERATOR};
		defined_operator[ASSIGN_DIV_OPERATOR]={ASSIGN_DIV_OPERATOR, "/=",IS_ASSIGN_DIV_OPERATOR};
		defined_operator[ASSIGN_MOD_OPERATOR]={ASSIGN_MOD_OPERATOR, "%=",IS_ASSIGN_MOD_OPERATOR};
		defined_operator[ASSIGN_BINARY_XOR_OPERATOR]={ASSIGN_BINARY_XOR_OPERATOR,"^=",IS_ASSIGN_BINARY_XOR_OPERATOR};
		defined_operator[ASSIGN_BINARY_AND_OPERATOR]={ASSIGN_BINARY_AND_OPERATOR,"&=",IS_ASSIGN_BINARY_AND_OPERATOR};
		defined_operator[ASSIGN_BINARY_OR_OPERATOR]={ASSIGN_BINARY_OR_OPERATOR,"|=",IS_ASSIGN_BINARY_OR_OPERATOR};
		defined_operator[ASSIGN_SHIFT_LEFT_OPERATOR]={ASSIGN_SHIFT_LEFT_OPERATOR,"<<=",IS_ASSIGN_SHIFT_LEFT_OPERATOR};
		defined_operator[ASSIGN_SHIFT_RIGHT_OPERATOR]={ASSIGN_SHIFT_RIGHT_OPERATOR,">>=",IS_ASSIGN_SHIFT_RIGHT_OPERATOR};

		defined_operator[BINARY_XOR_OPERATOR]={BINARY_XOR_OPERATOR, "^",IS_BINARY_XOR_OPERATOR};
		defined_operator[BINARY_AND_OPERATOR]={BINARY_AND_OPERATOR, "&",IS_BINARY_AND_OPERATOR};
		defined_operator[BINARY_OR_OPERATOR]={BINARY_OR_OPERATOR, "|",IS_BINARY_OR_OPERATOR};
		defined_operator[SHIFT_LEFT_OPERATOR]={SHIFT_LEFT_OPERATOR, "<<",IS_SHIFT_LEFT_OPERATOR};
		defined_operator[SHIFT_RIGHT_OPERATOR]={SHIFT_RIGHT_OPERATOR, ">>",IS_SHIFT_RIGHT_OPERATOR};
		defined_operator[LOGIC_AND_OPERATOR]={LOGIC_AND_OPERATOR, "&&",IS_LOGIC_AND_OPERATOR};
		defined_operator[LOGIC_OR_OPERATOR]={LOGIC_OR_OPERATOR, "||",IS_LOGIC_OR_OPERATOR};
		defined_operator[LOGIC_EQUAL_OPERATOR]={LOGIC_EQUAL_OPERATOR, "==",IS_LOGIC_EQUAL_OPERATOR};
		defined_operator[LOGIC_NOT_EQUAL_OPERATOR]={LOGIC_NOT_EQUAL_OPERATOR, "!=",IS_LOGIC_NOT_EQUAL_OPERATOR};
		defined_operator[LOGIC_GT_OPERATOR]={LOGIC_GT_OPERATOR, ">",IS_LOGIC_GT_OPERATOR};
		defined_operator[LOGIC_LT_OPERATOR]={LOGIC_LT_OPERATOR, "<",IS_LOGIC_LT_OPERATOR};
		defined_operator[LOGIC_GTE_OPERATOR]={LOGIC_GTE_OPERATOR, ">=",IS_LOGIC_GTE_OPERATOR};
		defined_operator[LOGIC_LTE_OPERATOR]={LOGIC_LTE_OPERATOR, "<=",IS_LOGIC_LTE_OPERATOR};
		defined_operator[INSTANCEOF_OPERATOR]={INSTANCEOF_OPERATOR, "instanceof",IS_INSTANCEOF_OPERATOR};


		defined_pre_operator[PRE_LOGIC_NOT_OPERATOR]={PRE_LOGIC_NOT_OPERATOR, "!",IS_LOGIC_NOT_OPERATOR};
		defined_pre_operator[PRE_ADD_OPERATOR]={PRE_ADD_OPERATOR, "+",IS_ADD_OPERATOR};
		defined_pre_operator[PRE_SUB_OPERATOR]={PRE_SUB_OPERATOR, "-",IS_SUB_OPERATOR};


		defined_identity_operator[IDENTITY_INC_OPERATOR]={IDENTITY_INC_OPERATOR, "++",IS_INC_OPERATOR};
		defined_identity_operator[IDENTITY_DEC_OPERATOR]={IDENTITY_DEC_OPERATOR, "--",IS_DEC_OPERATOR};


		// special punctuators...
		defined_separator[COMA_SEPARATOR]={COMA_SEPARATOR, ",",NULL};
		defined_separator[SEMICOLON_SEPARATOR]={SEMICOLON_SEPARATOR, ";",NULL};
		defined_separator[OPEN_PARENTHESIS_SEPARATOR]={OPEN_PARENTHESIS_SEPARATOR, "(",NULL};
		defined_separator[CLOSE_PARENTHESIS_SEPARATOR]={CLOSE_PARENTHESIS_SEPARATOR, ")",NULL};
		defined_separator[OPEN_SQUARE_BRAKET_SEPARATOR]={OPEN_SQUARE_BRAKET_SEPARATOR, "[",NULL};
		defined_separator[CLOSE_SQUARE_BRAKET_SEPARATOR]={CLOSE_SQUARE_BRAKET_SEPARATOR, "]",NULL};

		// init special punctuators...
		// init keywords...



		defined_keyword[KEYWORD_TYPE::UNKNOWN_KEYWORD] = {UNKNOWN_KEYWORD, NULL,NULL};
		defined_keyword[KEYWORD_TYPE::VAR_KEYWORD] = {VAR_KEYWORD,"var",new tKeywordFunction(std::bind(&CEval::evalKeywordVar,eval_singleton,_1,_2,_3,_4))};
		defined_keyword[KEYWORD_TYPE::IF_KEYWORD] = {IF_KEYWORD,"if",new tKeywordFunction(std::bind(&CEval::evalKeywordIf,eval_singleton,_1,_2,_3,_4))};
		defined_keyword[KEYWORD_TYPE::ELSE_KEYWORD] = {ELSE_KEYWORD,"else"};
		defined_keyword[KEYWORD_TYPE::FOR_KEYWORD] = {FOR_KEYWORD,"for",new tKeywordFunction(std::bind(&CEval::evalKeywordFor,eval_singleton,_1,_2,_3,_4))};
		defined_keyword[KEYWORD_TYPE::WHILE_KEYWORD] = {WHILE_KEYWORD,"while",new tKeywordFunction(std::bind(&CEval::evalKeywordWhile,eval_singleton,_1,_2,_3,_4))};
		defined_keyword[KEYWORD_TYPE::DO_WHILE_KEYWORD] = {DO_WHILE_KEYWORD,"do",new tKeywordFunction(std::bind(&CEval::evalKeywordDoWhile,eval_singleton,_1,_2,_3,_4))}; // while is expected in the end ...

		defined_keyword[KEYWORD_TYPE::SWITCH_KEYWORD] = {SWITCH_KEYWORD,"switch",new tKeywordFunction(std::bind(&CEval::evalKeywordSwitch,eval_singleton,_1,_2,_3,_4))};
		defined_keyword[KEYWORD_TYPE::CASE_KEYWORD] = {CASE_KEYWORD,"case",NULL};
		defined_keyword[KEYWORD_TYPE::BREAK_KEYWORD] = {BREAK_KEYWORD,"break",NULL};
		defined_keyword[KEYWORD_TYPE::CONTINUE_KEYWORD] = {CONTINUE_KEYWORD,"continue",NULL};
		defined_keyword[KEYWORD_TYPE::DEFAULT_KEYWORD] = {DEFAULT_KEYWORD,"default",NULL};
		defined_keyword[KEYWORD_TYPE::FUNCTION_KEYWORD] = {FUNCTION_KEYWORD,"function",NULL};
		defined_keyword[KEYWORD_TYPE::RETURN_KEYWORD] = {RETURN_KEYWORD,"return",new tKeywordFunction(std::bind(&CEval::evalKeywordReturn,eval_singleton,_1,_2,_3,_4))};
		defined_keyword[KEYWORD_TYPE::THIS_KEYWORD] = {THIS_KEYWORD,"this", NULL};
		defined_keyword[KEYWORD_TYPE::CLASS_KEYWORD] = {CLASS_KEYWORD,"class",NULL};
		defined_keyword[KEYWORD_TYPE::NEW_KEYWORD] = {NEW_KEYWORD,"new", NULL};
		defined_keyword[KEYWORD_TYPE::DELETE_KEYWORD] = {DELETE_KEYWORD,"delete",NULL};
		defined_keyword[KEYWORD_TYPE::IN_KEYWORD] = {IN_KEYWORD,"in",NULL};

		// DIRECTIVES
		defined_directive[UNKNOWN_DIRECTIVE]={UNKNOWN_DIRECTIVE, NULL};
		defined_directive[INCLUDE_DIRECTIVE]={INCLUDE_DIRECTIVE, "import"};

		// OP CODES
		//		VAR  			|	STR   | ID | NUM OP
		//----------------------+---------+----+-------

		defined_opcode[EQU]         ={EQU,"EQU" };  // ==
		defined_opcode[INSTANCEOF]  ={INSTANCEOF,"INSTANCEOF"};  // ==
		defined_opcode[NOT_EQU]     ={NOT_EQU,"NOT_EQU" };  // !=
		defined_opcode[LT]          ={LT,"LT"  };  // <
		defined_opcode[LTE]         ={LTE,"LTE"};  // <=
		defined_opcode[NOT]         ={NOT,"NOT"}; // !
		defined_opcode[GT]          ={GT,"GT"};  // >
		defined_opcode[GTE]         ={GTE,"GTE"}; // >=
		defined_opcode[NEG]         ={NEG,"NEG"}; // !
		defined_opcode[ADD]         ={ADD,"ADD"}; // +
		defined_opcode[LOGIC_AND]   ={LOGIC_AND,"LOGIC_AND"}; // &&
		defined_opcode[LOGIC_OR]    ={LOGIC_OR,"LOGIC_OR"};  // ||
		defined_opcode[DIV]         ={DIV,"DIV"}; // /
		defined_opcode[MUL]         ={MUL,"MUL"}; // *
		defined_opcode[MOD]         ={MOD,"MOD"};  // %

		defined_opcode[AND]         ={AND,"AND"}; // bitwise logic and
		defined_opcode[OR]          ={OR,"OR"}; // bitwise logic or
		defined_opcode[XOR]         ={XOR,"XOR"}; // logic xor
		defined_opcode[SHL]         ={SHL,"SHL"}; // shift left
		defined_opcode[SHR]         ={SHR,"SHR"}; // shift right

		defined_opcode[STORE]       ={STORE,"STORE"}; // mov expression to var
		defined_opcode[LOAD]        ={LOAD,"LOAD"}; // primitive value like number/std::string or boolean...

		defined_opcode[JMP]         ={JMP,"JMP"}; // Unconditional jump.
		defined_opcode[JNT]         ={JNT,"JNT"}; // goto if not true ... goes end to conditional.
		defined_opcode[JT]          ={JT,"JT"}; // goto if true ... goes end to conditional.

		defined_opcode[CALL]={CALL,"CALL"}; // calling function after all of arguments are processed...


		defined_opcode[VGET]={VGET,"VGET"}; // std::vector access after each index is processed...

		defined_opcode[DECL_VEC]={DECL_VEC,"DECL_VEC"}; // Vector object (CREATE)

		defined_opcode[VPUSH]={VPUSH,"VPUSH"}; // Value push for std::vector
		defined_opcode[RET]={RET,"RET"}; // Value pop for std::vector

		defined_opcode[NEW]={NEW,"NEW"}; // New object (CREATE)
		defined_opcode[DELETE_OP]={DELETE_OP,"DELETE_OP"};

		defined_opcode[POP_SCOPE]={POP_SCOPE,"POP_SCOPE"}; // New object (CREATE)
		defined_opcode[PUSH_SCOPE]={PUSH_SCOPE,"PUSH_SCOPE"}; // New object (CREATE)
		defined_opcode[PUSH_ATTR]={PUSH_ATTR,"PUSH_ATTR"}; // New object (CREATE)
		defined_opcode[DECL_STRUCT]={DECL_STRUCT,"DECL_STRUCT"}; // New object (CREATE)

		defined_opcode[IT_INI]={IT_INI,"IT_INI"}; // IT_INI
		defined_opcode[IT_CHK_END]={IT_CHK_END,"IT_CHK_END"}; // IT_CHK_END
		defined_opcode[IT_SET_AND_NEXT]={IT_SET_AND_NEXT,"IT_SET_AND_NEXT"}; // IT_SET_AND_NEXT

		// create main ast management
		// init variables...
	}

	CEval::CEval(){
		CURRENT_PARSING_FILE_IDX=-1;
		CURRENT_PARSING_FILE_STR="";
		pCurrentFunctionInfo=NULL;
	}

	bool   CEval::endExpression(const char * s){
		return *s==')' || *s==','||  *s==']' ||  *s==']' ||  *s==';' || *s == 0;
	}

	CEval::SEPARATOR_TYPE   CEval::evalSeparator(const char *s){

		for(unsigned char i = 1; i < MAX_SEPARATOR_TYPES; i++){

			if(*defined_separator[i].str == *s){
				return defined_separator[i].id;
			}
		}

		return SEPARATOR_TYPE::UNKNOWN_SEPARATOR;
	}

	CEval::OPERATOR_TYPE   CEval::evalOperator(const char *s){

		for(unsigned char i = 1; i < MAX_OPERATOR_TYPES; i++){

			if(defined_operator[i].eval_fun(s)){
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

	CEval::IDENTITY_OPERATOR_TYPE   CEval::evalPrePostIdentityOperator(const char *s){
		for(unsigned char i = 1; i < MAX_IDENTITY_OPERATOR_TYPES; i++){

			if(*defined_identity_operator[i].str == *s){
				return defined_identity_operator[i].id;
			}
		}
		return IDENTITY_OPERATOR_TYPE::UNKNOWN_IDENTITY_OPERATOR;
	}

	KEYWORD_TYPE CEval::isKeyword(const char *c){
		// PRE: The first char must be starting symbol.
		char *str=(char *)c;
		char *aux=str;

		for(int i = 0; i < MAX_KEYWORD; i++){
			if(defined_keyword[i].str!=NULL){
				int size = strlen(defined_keyword[i].str);
				char *aux = str+size;
				if((strncmp(str,defined_keyword[i].str,size)==0) && (
						*aux == 0  || // carry end
						*aux == ' '  || // space
						*aux == '\t'  || // tab
						*aux == '('  || // ( // mostly if,for,while,switch
						*aux == '{'  || // ( // mostly else,
						*aux == '\n' || // carry return

					   IS_START_BLOCK_COMMENT(aux)) //start block comment
					   ){
					return defined_keyword[i].id;
				}
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


	char * CEval::evalLiteralNumber(const char *c, int & line, std::string & value, bool & error){
		// PRE: a std::string given...
		char *aux_p = IGNORE_BLANKS(c,line);
		bool end=false;
		int current_part=0;
		std::string number_part[3];
		value="";
		bool is01s=true;
		bool isInt=true;
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
						aux_p=IGNORE_BLANKS(aux_p+1,line);
						if(*(aux_p)=='+' || *(aux_p)=='-'){
							number_part[current_part]+=*aux_p;
							aux_p=IGNORE_BLANKS(aux_p+1,line);
						}
					}
					else{ // error
						 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Invalid number format \"%s\"",value.c_str());
						 error=true;
						 return NULL;
					}
				}
				else{ // error
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Invalid number format \"%s\"",value.c_str());
					 error=true;
					 return NULL;
				}

			}
			else if(*aux_p == '.'){ // fraccional part ?

				if(isHexa){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Invalid number format \"%s\"",value.c_str());
						 error=true;
						 return NULL;
				}

				if(current_part==0){
					current_part++;
					number_part[current_part]+=".";
				}
				else{ // error
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Invalid number format \"%s\"",value.c_str());
					 error=true;
					 return NULL;
				}
			}

			else if(*aux_p == 'b'){ // is end binary format?
				if(!is01s || (current_part != 0)){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Invalid number format \"%s\"",value.c_str());
					 error=true;
					 return NULL;
				}

				number_part[current_part]+=*aux_p;
				end=true;
			}
			else{
				 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Invalid number format \"%s\"",value.c_str());
				 error=true;
				 return NULL;
			}

			is01s&=(('0'==*aux_p) || ('1'==*aux_p));

			aux_p++;


			//if(evalSpecialPunctuator(aux)==OPERATOR_TYPE::FIELD_OPERATOR);
		}while(!end && !(isEndSymbolToken(aux_p)&&!(*aux_p=='.')));

		// check and eval token ?
		value = number_part[0]+number_part[1]+number_part[2];

		if(isEndSymbolToken(aux_p)){
			return aux_p;
		}

		return NULL;

		// POST: detects integer/binary/fractional/hexa
	}


	bool CEval::isAccessPunctuator(char *s){
		return *s=='.' || *s=='[' || *s=='(';
	}

	bool CEval::isEndSymbolToken(char *s, char pre){
		return evalOperator(s)!=OPERATOR_TYPE::UNKNOWN_OPERATOR
			   || evalSeparator(s)!=SEPARATOR_TYPE::UNKNOWN_SEPARATOR
			   || *s=='.' // to separate access identifiers.
			   || *s==' '
			   || *s==0
			   || (*s=='\"' && pre!='\\');
	}

	// to std::string utils ...
	char * CEval::evalSymbol(const char *start_word, int line,CEval::tTokenNode * token_node){
		// PRE:
		unsigned short type=STK_PROPERTY_TYPE_SCRIPTVAR;
		LOAD_TYPE load_type=LOAD_TYPE_NOT_DEFINED;

		unsigned int scope_type=0;
		void *obj=NULL,*get_obj=NULL,*const_obj=NULL;
		char *start_str=(char *)start_word;
		char *aux=(char *)start_word;
		std::string v="";
		 bool is_possible_number=false;
		 int i=0;
		 bool error=false;
		 bool start_digit = false;
		 token_node->token_type = TOKEN_TYPE::UNKNOWN_TOKEN;
		 KEYWORD_TYPE kw=isKeyword(aux);
		 int start_line = line;
		 bool is_constant_string=false;

		 if((aux=evalLiteralNumber(start_word,line,v ,error))==NULL){ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

			 if(error){
				 return NULL;
			 }

			 aux=(char *)start_word;
			 // try eval identifier, boolean, std::string ...
			 token_node->token_type = TOKEN_TYPE::LITERAL_TOKEN;


			char pre=0;
			if(*aux=='\"'){
				is_constant_string = true;
				aux++;
			}
			while(!isEndSymbolToken(aux,pre)){
				pre=*aux;
				v += (*aux++);
			}

			if(*aux==0){
				return NULL;
			}

			if(*aux=='\"'){
				 if(!is_constant_string){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Unexpected '\"'");
					 return NULL;
				 }
				 aux++;


				 type=STK_PROPERTY_TYPE_STRING;
				 load_type=LOAD_TYPE_CONSTANT;


				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{

					CStringScriptVariable *s=new CStringScriptVariable(v);
					obj=addConstant(v,NULL,type);
					((tStackElement *)obj)->stkValue=((void *)(s->m_strValue.c_str()));
					((tStackElement *)obj)->varRef=s;

				 }
			}
			 // add load std::string constant
			// compile constant ...
		 }

		 if(!is_constant_string){
			 // try parse value...
			if(v=="null"){
				type=STK_PROPERTY_TYPE_NULL;
				load_type=LOAD_TYPE_NULL;
				obj=NULL;//CScriptVariable::NullSymbol;


			}else if(v=="undefined"){
					type=STK_PROPERTY_TYPE_UNDEFINED;
					load_type=LOAD_TYPE_UNDEFINED;
					obj=NULL;// CScriptVariable::UndefinedSymbol;
			}else if((const_obj=CZetScriptUtils::ParseInteger(v))!=NULL){
				int value = *((int *)const_obj);
				delete (int *)const_obj;
				load_type=LOAD_TYPE_CONSTANT;
				obj=addConstant(v,value);
			}
			else if((const_obj=CZetScriptUtils::ParseFloat(v))!=NULL){
				float value = *((float *)const_obj);
				delete (float *)const_obj;
				void *value_ptr;
				memcpy(&value_ptr,&value,sizeof(float));

				type=STK_PROPERTY_TYPE_NUMBER;
				load_type=LOAD_TYPE_CONSTANT;

				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{
					obj=addConstant(v,value_ptr,type);
				}
			}
			else if((const_obj=CZetScriptUtils::ParseBoolean(v))!=NULL){

				bool value = *((bool *)const_obj);
				delete (bool *)const_obj;

				type=STK_PROPERTY_TYPE_BOOLEAN;
				load_type=LOAD_TYPE_CONSTANT;

				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{
					obj=addConstant(v,(void *)value,type);
				}
			}else{ // it should be an identifier token  ...

				token_node->token_type = TOKEN_TYPE::IDENTIFIER_TOKEN;
				intptr_t idx_local_var=ZS_UNDEFINED_IDX;

				for(unsigned i = 0; i < pCurrentFunctionInfo->function_info_object->m_arg.size() && idx_local_var == ZS_UNDEFINED_IDX; i++){

					if(pCurrentFunctionInfo->function_info_object->m_arg[i].arg_name == v){
						idx_local_var=i;
					}
				}

				if(idx_local_var!=ZS_UNDEFINED_IDX){ // is arg...
					load_type=LOAD_TYPE_ARGUMENT;
					obj=(void *)idx_local_var;
				}
				else if(v == "super"){
					scope_type=INS_PROPERTY_SUPER_SCOPE;
				}else if(v == "this"){
					scope_type=INS_PROPERTY_THIS_SCOPE;
				}else if((get_obj = getConstant(v)) != NULL){  // check if symbol is constant ...
					obj=get_obj;
					load_type=LOAD_TYPE_CONSTANT;
				}else{
					// should be an identifier...
					if(!isIdentifierNameExpressionOk(v,line)){
						return NULL;
					}

					// search local var into current function...


					// search global ...

					// else access scope  var ?
				}

			}
		 }
		token_node->value = v;
		token_node->instruction.push_back(tInstructionEval(OP_CODE::LOAD,load_type,(intptr_t)obj,scope_type));

		return aux;
		// POST: token as literal or identifier
	}

	char *  CEval::getIdentifierToken(const char *s, std::string & symbol){

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
			writeErrorMsg(CURRENT_PARSING_FILE_STR,-1," identifier cannot begin with %c", *aux_p);
		}
		return NULL;
	}

	bool CEval::isLiteral(const std::string & symbol){
		return symbol=="true" || symbol == "false" || symbol=="undefined" || symbol == "null";
	}

	bool CEval::isIdentifierNameExpressionOk(const std::string & symbol, int line){

		char *aux_p = (char *)symbol.c_str();
		KEYWORD_TYPE kw;

		if((kw=isKeyword(aux_p))!=KEYWORD_TYPE::UNKNOWN_KEYWORD){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line," Unexpected \"%s\" keyword", aux_p);
			return false;
		}

		if(isLiteral(symbol)){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line," Unexpected \"%s\"", aux_p);
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
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line," identifier cannot begin with %c", *aux_p);
			return false;
		}

		return true;
	}
	//------------------------------------------------------------------------------------------------------------
	char * CEval::evalFunctionObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction){

		// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

		return NULL;
	}

	char * CEval::evalStructObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;
			std::string symbol_value;
			int lineSymbol;

			if(*aux_p == '{'){ // go for final ...

				// this solve problem void structs...
				aux_p=IGNORE_BLANKS(aux_p+1,line);

				while (*aux_p != '}' && *aux_p != 0){

					lineSymbol = line;

					if((aux_p=getIdentifierToken(aux_p,symbol_value))==NULL){
						 writeErrorMsg(CURRENT_PARSING_FILE_STR,lineSymbol ,"Expected symbol");
						 return NULL;

					}

					 aux_p=IGNORE_BLANKS(aux_p,line);

					 if(*aux_p != ':'){ // expected : ...
						 writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ':'");
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
						 writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected '}' or ','");
						 return NULL;
					 }
				}
			}
			else{
				writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '{'");
			}
			return aux_p;
	}


	char * CEval::evalVectorObject(const char *s,int & line,  CScope *scope_info,  std::vector<tInstructionEval> *	instruction){

		char * aux_p=IGNORE_BLANKS(s,line);

		if(*aux_p != '['){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '['");
			return NULL;
		}

		aux_p=IGNORE_BLANKS(aux_p+1,line);
		unsigned v_elements=0;

		while(*aux_p!=0 && *aux_p != ']'){

			aux_p=IGNORE_BLANKS(aux_p,line);

			// expression expected ...
			if(v_elements > 0){
				if(*aux_p != ','){
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,",");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+1,line);
			}

			aux_p=evalExpression(aux_p,line,scope_info,NULL);

			v_elements++;
		}

		return aux_p;
	}

	char * CEval::evalNewObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		std::string symbol_value;

		KEYWORD_TYPE key_w;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::NEW_KEYWORD){
				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),line);
				// try get symbol ...

				if((aux_p=getIdentifierToken(aux_p,symbol_value))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected symbol");
					 return NULL;
				}

				 aux_p=IGNORE_BLANKS(aux_p,line);

				 if(*aux_p != '('){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '(' after \'%s\'",defined_keyword[key_w].str);
					 return NULL;
				 }

				 do{
					if((aux_p = evalExpression(IGNORE_BLANKS(aux_p+1,line),line,scope_info,instruction))==NULL){
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p,line);

					if(*aux_p != ',' && *aux_p != ')'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected ',' or ')'");
						return NULL;
					}

				}while(*aux_p != ')');

				 //aux_p = evalExpressionArgs('(', ')',aux_p,line,scope_info,instruction);
				 if(aux_p == NULL){
					 return NULL;
				 }

				return aux_p;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------------------------------------------

	OP_CODE CEval::puntuator2instruction(OPERATOR_TYPE op){

		switch(op){
		default:
			break;
		case OPERATOR_TYPE::SUB_OPERATOR:
			return OP_CODE::NEG;
		case OPERATOR_TYPE::ADD_OPERATOR:
			return OP_CODE::ADD;
		case OPERATOR_TYPE::MUL_OPERATOR:
			return OP_CODE::MUL;
		case OPERATOR_TYPE::DIV_OPERATOR:
			return OP_CODE::DIV;
		case OPERATOR_TYPE::MOD_OPERATOR:
			return OP_CODE::MOD;
		case OPERATOR_TYPE::ASSIGN_OPERATOR:
			return OP_CODE::STORE;
		case OPERATOR_TYPE::BINARY_XOR_OPERATOR:
			return OP_CODE::XOR;
		case OPERATOR_TYPE::BINARY_AND_OPERATOR:
			return OP_CODE::AND;
		case OPERATOR_TYPE::BINARY_OR_OPERATOR:
			return OP_CODE::OR;
		case OPERATOR_TYPE::SHIFT_LEFT_OPERATOR:
			return OP_CODE::SHL;
		case OPERATOR_TYPE::SHIFT_RIGHT_OPERATOR:
			return OP_CODE::SHR;
		case OPERATOR_TYPE::LOGIC_AND_OPERATOR:
			return OP_CODE::LOGIC_AND;
		case OPERATOR_TYPE::LOGIC_OR_OPERATOR:
			return OP_CODE::LOGIC_OR;
		case OPERATOR_TYPE::LOGIC_EQUAL_OPERATOR:
			return OP_CODE::EQU;
		case OPERATOR_TYPE::LOGIC_NOT_EQUAL_OPERATOR:
			return OP_CODE::NOT_EQU;
		case OPERATOR_TYPE::LOGIC_GT_OPERATOR:
			return OP_CODE::GT;
		case OPERATOR_TYPE::LOGIC_LT_OPERATOR:
			return OP_CODE::LT;
		case OPERATOR_TYPE::LOGIC_GTE_OPERATOR:
			return OP_CODE::GTE;
		case OPERATOR_TYPE::LOGIC_LTE_OPERATOR:
			return OP_CODE::LTE;
		case OPERATOR_TYPE::INSTANCEOF_OPERATOR:
			return OP_CODE::INSTANCEOF;
		}


		THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("operator %i not implemented",op));
		return OP_CODE::END_FUNCTION;
	}


	bool CEval::makeOperatorPrecedence(std::vector<tTokenNode> * vExpressionTokens,std::vector<tInstructionEval> *instruction, int idx_start,int idx_end, bool & error){

		OPERATOR_TYPE 	op_split=OPERATOR_TYPE::MAX_OPERATOR_TYPES;
		int 			idx_split=-1;
		tTokenNode      *split_node;

		// trivial case (no operator left, append byte code )...
		if(idx_start>=idx_end){

			// concatenate instructions ...
			instruction->insert(
					  instruction->end()
					, vExpressionTokens->at(idx_start).instruction.begin()
					, vExpressionTokens->at(idx_start).instruction.end() );

			return true;
		}

		// search for the most priority operator...
		for(int i=idx_start; i < idx_end; i++){

			if( (vExpressionTokens->at(i).token_type == TOKEN_TYPE::OPERATOR_TOKEN)
				&& (vExpressionTokens->at(i).operator_type < op_split))
			{
				idx_split=i;
				op_split=vExpressionTokens->at(i).operator_type;
			}
		}

		if(idx_split == -1){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,-1,"Unexpected error");
			error =true;
			return false;
		}


		// split left/right by operator precedence...
		split_node=&vExpressionTokens->at(idx_split);

		bool result= makeOperatorPrecedence(vExpressionTokens,instruction,idx_start,idx_split-1, error) // left branches...
									&&
					makeOperatorPrecedence(vExpressionTokens,instruction,idx_split+1,idx_end,error); // right branches...

		if(result){ // if result was ok, push op_code...
			instruction->push_back(tInstructionEval(puntuator2instruction(split_node->operator_type)));
		}

		return result;
	}

	char * CEval::evalExpression(const char *s, int & line, CScope *scope_info, std::vector<tInstructionEval> 	* instruction){
		// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
		// If this functions finds ';' then the function will generate ast.

		std::vector<tTokenNode> vExpressionTokens;
		PRE_OPERATOR_TYPE pre_op = PRE_OPERATOR_TYPE::UNKNOWN_PRE_OPERATOR;
		OPERATOR_TYPE op = OPERATOR_TYPE::UNKNOWN_OPERATOR;
		KEYWORD_TYPE kw;
		bool error=false;

		bool is_first_access=false;
		//int instruction_first_access=-1;
		int instruction_identifier=-1;
		unsigned char params=0;

		//PASTNode ast_node_to_be_evaluated=NULL;
		char *aux_p=IGNORE_BLANKS(s,line);

		while(!endExpression(aux_p)){

			tTokenNode 	symbol_token_node
						,operator_token_node;
			kw=KEYWORD_TYPE::UNKNOWN_KEYWORD;

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
			kw=isKeyword(aux_p);

			// parenthesis (evals another expression)
			if(*aux_p=='('){ // inner expression (priority)
				std::vector<tInstructionEval> 	instruction_inner;
				if((aux_p=evalExpression(aux_p+1, line, scope_info, &instruction_inner))==NULL){
					return NULL;
				}

				// concatenate instruction ...
				instruction->insert(
									  instruction->end()
									, instruction_inner.begin()
									, instruction_inner.end() );

				if(*aux_p != ')'){
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected ')'");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+1,line);

				operator_token_node.token_type=TOKEN_TYPE::SUB_EXPRESSION_TOKEN;

			}
			else{

				IDENTITY_OPERATOR_TYPE pre_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE::UNKNOWN_IDENTITY_OPERATOR;
				IDENTITY_OPERATOR_TYPE post_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE::UNKNOWN_IDENTITY_OPERATOR;

				if(IS_INC_OPERATOR(aux_p)){
					pre_inline_operator_identity_type=IDENTITY_INC_OPERATOR;
					aux_p=IGNORE_BLANKS(aux_p+2,line);
				}
				else if(IS_DEC_OPERATOR(aux_p)){
					pre_inline_operator_identity_type=IDENTITY_DEC_OPERATOR;
					aux_p=IGNORE_BLANKS(aux_p+2,line);
				}

				// pre operator identifier ...

				// first call..
				if(*aux_p=='['){ // std::vector object...

					if((aux_p=evalVectorObject(aux_p,line,scope_info,instruction)) == NULL){
						return NULL;
					}
					symbol_token_node.token_type = TOKEN_TYPE::VECTOR_OBJECT_TOKEN;

				}else if(*aux_p=='{'){ // struct object ...

					if((aux_p=evalStructObject(aux_p,line,scope_info,instruction)) == NULL){
						return NULL;
					}
					symbol_token_node.token_type = TOKEN_TYPE::STRUCT_OBJECT_TOKEN;
				}else if(kw == KEYWORD_TYPE::NEW_KEYWORD){

					if((aux_p=evalNewObject(aux_p,line,scope_info,instruction)) == NULL){
						return NULL;
					}
					symbol_token_node.token_type = TOKEN_TYPE::NEW_OBJECT_TOKEN;

				}else if(kw == KEYWORD_TYPE::FUNCTION_KEYWORD){ // can be after instanceof or a function object..
					if((aux_p=evalFunctionObject(aux_p,line,scope_info,instruction)) == NULL){
						return NULL;
					}
					symbol_token_node.token_type = TOKEN_TYPE::FUNCTION_OBJECT_TOKEN;
				}
				else if((aux_p = evalSymbol(aux_p,line,&symbol_token_node))==NULL){ // finally try evalSymbol (it should be an identifier or literal)
						return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p,line);
				is_first_access=false;
				params=NO_PARAMS_IS_VARIABLE;

				// check valid access punctuator...
				if(isAccessPunctuator(aux_p) || symbol_token_node.token_type == TOKEN_TYPE::IDENTIFIER_TOKEN){

					if(!(      symbol_token_node.token_type == TOKEN_TYPE::IDENTIFIER_TOKEN
						  || ((symbol_token_node.token_type == TOKEN_TYPE::FUNCTION_OBJECT_TOKEN)&& *aux_p == '(')// cannot be a number/boolean or std::string and then accessor like . / ( / [
						  || ((symbol_token_node.token_type == TOKEN_TYPE::VECTOR_OBJECT_TOKEN  )&& *aux_p == '[')// inline function object like this: 1+function(a,b){ return a+b;} + 0
						  || ((symbol_token_node.token_type == TOKEN_TYPE::STRUCT_OBJECT_TOKEN  )&& *aux_p == '.')// inline struct object like this: 1+{"a":0,"b":1}.a + 0

						))
					{
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Unexpected '%c' after literal",*aux_p);
							return NULL;
					}

					is_first_access=true;

					//instruction_first_access=0;//symbol_token_node.instruction.size()-1;
					//instruction_identifier=instruction_first_access;

					// eval accessor element (supose that was a preinsert a load instruction for identifier )...
					while(isAccessPunctuator(aux_p)){
						OP_CODE op_code=OP_CODE::INVALID_OP;
						std::string accessor_value="";

						switch(*aux_p){
						case '(': // function call
							if(is_first_access){
								params=0;
							}
							aux_p = IGNORE_BLANKS(aux_p+1,line);
							if(*aux_p != ')'){
								do{


									if((aux_p = evalExpression(aux_p,line,scope_info,&symbol_token_node.instruction))==NULL){
										return NULL;
									}

									if(*aux_p != ',' && *aux_p != ')'){
										writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected ',' or ')'");
										return NULL;
									}

									if(*aux_p == ','){
										aux_p=IGNORE_BLANKS(aux_p+1,line);
									}

									if(is_first_access){
										params++;
									}

								}while(*aux_p != ')');
							}
							//accessor_token.params=params;
							op_code=OP_CODE::CALL;
							is_first_access=false;

							aux_p++;
							break;
						case '[': // std::vector access
							if((aux_p = evalExpression(IGNORE_BLANKS(aux_p+1,line),line,scope_info,&symbol_token_node.instruction))==NULL){
								return NULL;
							}
							if(*aux_p != ']'){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected ']'");
								return NULL;
							}
							aux_p=IGNORE_BLANKS(aux_p+1,line);

							//symbol_token_node.instruction.push_back(tInstruction(OP_CODE::VGET));
							op_code=OP_CODE::VGET;
							break;
						case '.': // member access
							aux_p=IGNORE_BLANKS(aux_p+1,line);
							accessor_value="";
							while(!isEndSymbolToken(aux_p)){ // get name...
								accessor_value += (*aux_p++);
							}

							if(!isIdentifierNameExpressionOk(accessor_value,line)){
								return NULL;
							}

							//save_symbol_name_value=true;
							//pCurrentFunctionInfo->function_info_object->instruction_info[symbol_token_node.instruction.size()]=CScriptFunction::tInstructionInfo(accessor_value,CURRENT_PARSING_FILE_STR,line);
							//symbol_token_node.instruction.push_back(tInstruction(OP_CODE::LOAD)); //--> must add symbol value instruction...
							op_code=OP_CODE::LOAD;
							break;
						}



						if(op_code==OP_CODE::LOAD){
							//instruction_identifier=symbol_token_node.instruction.size();
							symbol_token_node.instruction[instruction_identifier].instructionInfo= tInstructionInfo(CURRENT_PARSING_FILE_STR,line,getCompiledSymbol(symbol_token_node.value));
						}

						tInstructionEval instruction_token=tInstructionEval(op_code);
						symbol_token_node.instruction.push_back(instruction_token);

						aux_p=IGNORE_BLANKS(aux_p,line);



					}


					// add info to solve symbols first access (we need to put here because we have to know n params if function related)
					symbol_token_node.instruction[0].linkSymbolFirstAccess=tLinkSymbolFirstAccess(
							pCurrentFunctionInfo->function_info_object->idxScriptFunction
							,scope_info->idxScope
							,symbol_token_node.value
							,params // only if first access is a function...
					);

					// add info to add as symbol value ...
					symbol_token_node.instruction[0].instructionInfo = tInstructionInfo(CURRENT_PARSING_FILE_STR,line,getCompiledSymbol(symbol_token_node.value));


					if(IS_INC_OPERATOR(aux_p)){
						post_inline_operator_identity_type=IDENTITY_INC_OPERATOR;
						aux_p=IGNORE_BLANKS(aux_p+2,line);
					}
					else if(IS_DEC_OPERATOR(aux_p)){
						post_inline_operator_identity_type=IDENTITY_DEC_OPERATOR;
						aux_p=IGNORE_BLANKS(aux_p+2,line);
					}

					// pre/post operator...
					symbol_token_node.pre_inline_operator_identity_type = pre_inline_operator_identity_type;
					symbol_token_node.post_inline_operator_identity_type = post_inline_operator_identity_type;
				}


			}

			vExpressionTokens.push_back(symbol_token_node);

			aux_p=IGNORE_BLANKS(aux_p,line);

			if(!endExpression(aux_p)){ // if not end expression then a operator is expected...
				op=evalOperator(aux_p);

				if(op==OPERATOR_TYPE::UNKNOWN_OPERATOR){
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected operator");
					return NULL;
				}

				operator_token_node.operator_type=op;
				operator_token_node.token_type=TOKEN_TYPE::OPERATOR_TOKEN;

				vExpressionTokens.push_back(operator_token_node);
				aux_p+=strlen(defined_operator[op].str);
			}

			aux_p=IGNORE_BLANKS(aux_p,line);
		}

		if(aux_p==0){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Unexpected end of file");
			return NULL;
		}


		// make operator precedence from the AST built before...
		if(!makeOperatorPrecedence(&vExpressionTokens,instruction,0,vExpressionTokens.size()-1,error)){
			return NULL;
		}

		// last character is a separator so it return increments by 1
		return aux_p;
	}
	//---------------------------------------------------------------------------------------------------------------
	// PARSE KEYWORDS

	char * CEval::evalKeywordDelete(const char *s,int & line,  CScope *scope_info, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		std::string symbol_value;
		KEYWORD_TYPE key_w;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::DELETE_KEYWORD){

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),line);


				if((aux_p=getIdentifierToken(aux_p,symbol_value))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected symbol");
					 return NULL;
				}

				 aux_p=IGNORE_BLANKS(aux_p,line);

				 if(*aux_p != ';'){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ;");
					 return NULL;
				 }

				return aux_p;
			}
		}
		return NULL;
	}

	char * CEval::isClassMember(const char *s,int & line,CScriptClass **sc,std::string & member_symbol, bool & error){

		char *aux_p = (char *)s;
		std::string class_name;
		*sc=NULL;

		error = false;

		aux_p=IGNORE_BLANKS(aux_p,line);

		// check whwther the function is anonymous or not.
		if((aux_p=getIdentifierToken(aux_p,class_name))==NULL){
			 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected class symbol");
			 return NULL;
		}

		aux_p=IGNORE_BLANKS(aux_p,line);

		if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

			if((*sc=GET_SCRIPT_CLASS(class_name)) != NULL){

				if((aux_p=getIdentifierToken(aux_p+2,member_symbol))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected class member symbol");
					 return NULL;
				}
				//idxScopeClass=sc->idxScope;

				return aux_p;

			}else{
				error=true;
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordClass(const char *s,int & line, CScope *scope_info, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		CScope *class_scope_info=NULL;
		int class_line;
		std::string class_name;
		std::string base_class_name="";
		CScriptClass *sc;
		KEYWORD_TYPE key_w;
		aux_p=IGNORE_BLANKS(aux_p,line);

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::CLASS_KEYWORD){

				if(scope_info->getIdxParent()!=ZS_UNDEFINED_IDX){
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"class keyword is not allowed");
					return NULL;
				}

				aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[key_w].str),line);

				// check for symbol's name
				if((aux_p=getIdentifierToken(aux_p,class_name))==NULL){
					 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected symbol");
					 return NULL;
				}

				// try to register class...
				class_line = line;


				aux_p=IGNORE_BLANKS(aux_p,line);

				if(*aux_p == ':' ){

					aux_p=IGNORE_BLANKS(aux_p+1,line);

					if((aux_p=getIdentifierToken(aux_p,base_class_name))==NULL){
						 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected symbol");
						 return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p, line);
				}


				// register class
				if((sc=zs::CScriptClassFactory::getInstance()->registerClass(__FILE__, __LINE__, class_name,base_class_name))==NULL){
					return NULL;
				}

				zs_print_debug_cr("registered class \"%s\" line %i ",class_name.c_str(), class_line);

				if(*aux_p == '{' ){

					aux_p=IGNORE_BLANKS(aux_p+1,line);

					// TODO: Register class and baseof

					// register info class ...
					// check for named functions or vars...
					while(*aux_p != '}' && *aux_p != 0){

						tFunctionInfo *local_ifc=NULL;

						// 1st. check whether eval a keyword...
						key_w = isKeyword(aux_p);
						if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
							switch(key_w){
							default:
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected \"var\" or \"function\" keyword");
								return NULL;
								break;
							case KEYWORD_TYPE::FUNCTION_KEYWORD:

								if((aux_p = evalKeywordFunction(aux_p, line,class_scope_info,error)) == NULL){
									return NULL;
								}
								break;
							case KEYWORD_TYPE::VAR_KEYWORD:
								if((aux_p = evalKeywordVar(aux_p, line,class_scope_info,error)) == NULL){
									return NULL;
								}
								break;
							}
						}else{
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected \"var\" or \"function\" keyword");
							return NULL;
						}
						aux_p=IGNORE_BLANKS(aux_p,line);
					}

					if(*aux_p != '}'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p+1,line);

					if(*aux_p != ';'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,class_line ,"class \"%s\" not end with ;",class_name.c_str());
						return NULL;
					}

					return aux_p+1;

				}else{
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '{'");
					return NULL;
				}
			}
		}
		return NULL;
	}

	//
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	//  KEYWORDS
	//

	char * CEval::evalKeywordFunction(const char *s,int & line,  CScope *scope_info, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *symbol_value,*end_var;
		KEYWORD_TYPE key_w;
		std::vector<tArgumentInfo> arg;
		std::string conditional_str;
		CScriptClass *sc=NULL;

		tSymbol * irv=NULL;
		std::string str_name,arg_value;
		static int n_anonymous_function=0;
		//std::string class_member,class_name,
		std::string function_name="";


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

					if((end_var=isClassMember(aux_p,line,&sc, function_name, error))!=NULL){ // check if particular case extension attribute class

						idxScope = sc->symbol_info.symbol->idxScope;
						//symbol_value = (char *)class_member.c_str();

					}
					else{
						if(error){
							return NULL;
						}
						else{ // get normal name...

							// check whwther the function is anonymous with a previous arithmetic operation ....

							if((end_var=getIdentifierToken(aux_p,function_name)) == NULL){
								 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected symbol");
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

					// grab words separated by ,
					while(*aux_p != 0 && *aux_p != ')'){

						aux_p=IGNORE_BLANKS(aux_p,line);

						if(arg.size()>0){
							if(*aux_p != ','){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ',' ");
								return NULL;
							}
							aux_p=IGNORE_BLANKS(aux_p+1,line);
						}

						if(*aux_p == ')' || *aux_p == ','){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected arg");
							return NULL;
						}

						int m_start_arg=line;
						if((end_var=getIdentifierToken(aux_p,arg_value)) == NULL){
							 writeErrorMsg(CURRENT_PARSING_FILE_STR,line ,"Expected symbol");
							 return NULL;
						}

						// check if repeats...
						for(unsigned k = 0; k < arg.size(); k++){
							if(arg[k].arg_name == arg_value){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Repeated argument '%s' argument ",arg_value.c_str());
								return NULL;
							}
						}

						// check whether parameter name's matches with some global variable...
						if((irv=body_scope->getSymbol(symbol_value)) != NULL){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Ambiguous symbol argument \"%s\" name with var defined at %i", symbol_value, -1);
							return NULL;
						}
							// ok register symbol into the object function ...
						tArgumentInfo arg_info;
						arg_info.arg_name=symbol_value;
						arg.push_back(arg_info);

						aux_p=end_var;
						aux_p=IGNORE_BLANKS(aux_p,line);

						if(*aux_p != ')'){

						}
					}

					aux_p++;
					aux_p=IGNORE_BLANKS(aux_p,line);

					if(*aux_p != '{'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '{'");
						return NULL;
					}

					// ok let's go to body..
					if((aux_p = evalBlock(
							aux_p,
							line,
							GET_SCOPE(idxScope),
							error
						)) != NULL){

						if(!error){
							// register function symbol...
							int n_params=arg.size();

							/*if(named_function){ // register named function...
								if((irv=GET_SCOPE(idxScope)->getSymbol(function_name,n_params)) != NULL){

									writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Function name \"%s\" is already defined with same args at %s:%i", function_name.c_str(),irv->file.c_str(),irv->line);
									return NULL;
								}

								if((irv=GET_SCOPE(idxScope)->registerSymbol(CURRENT_PARSING_FILE_STR,line,function_name,n_params))==NULL){
									return NULL;
								}

							}else{ // register anonymouse function at global scope...
								irv=GET_SCOPE(IDX_GLOBAL_SCOPE)->registerAnonymouseFunction(CURRENT_PARSING_FILE_STR,line,n_params);
							}*/

							if(!named_function){ // register named function...
								function_name="_afun_"+CZetScriptUtils::intToString(n_anonymous_function++);
							}
							//--- OP
							if(sc!=NULL){ // register as variable member...
								sc->registerFunction(CURRENT_PARSING_FILE_STR, line, function_name,arg);
							}
							else{ // register as local variable in the function...
								pCurrentFunctionInfo->function_info_object->registerFunction(CURRENT_PARSING_FILE_STR, line, function_name,arg);
							}

							//---


							GET_SCOPE(idxScope)->popScope();
							return aux_p;
						}
					}
				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line," Expected '('");
				}
			}else{
				writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected operator or function operator");
			}
		}
		return NULL;
	}

	char *  CEval::evalKeywordReturn(const char *s,int & line,  CScope *scope_info, bool & error){
		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		std::string s_aux;

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){

			if(key_w == KEYWORD_TYPE::RETURN_KEYWORD){ // possible variable...
				//PASTNode child_node=NULL;
				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,line);
				if((aux_p = evalExpression(aux_p, line, scope_info,&pCurrentFunctionInfo->instruction))!= NULL){

					if(*aux_p!=';'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ';'");
						return NULL;
					}
					aux_p=IGNORE_BLANKS(aux_p+1,line);
					return aux_p;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordWhile(const char *s,int & line, CScope *scope_info,  bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		CScope *_currentScope=NULL;

		//PASTNode conditional_expression=NULL, while_node=NULL;
		std::string conditional_str;
		error = false;

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::WHILE_KEYWORD){

				aux_p += strlen(defined_keyword[key_w].str);
				// evaluate conditional line ...
				aux_p=IGNORE_BLANKS(aux_p,line);
				if(*aux_p == '('){

					if((end_expr = evalExpression(aux_p+1,line,_currentScope,&pCurrentFunctionInfo->instruction)) != NULL){

						if(*end_expr != ')'){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')'");
							return NULL;
						}

						if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
							return NULL;
						}

						aux_p=IGNORE_BLANKS(end_expr+1,line);
						if(*aux_p != '{'){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected while-block open block ('{') ");
							return NULL;
						}
						if((aux_p=evalBlock(aux_p
								,line
								,_currentScope
								,error
								))!= NULL){
							if(!error){
								scope_info->popScope();
								return aux_p;
							}
						}
					}else{
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')' while ");
						return NULL;
					}

				}else{
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '(' while ");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordDoWhile(const char *s,int & line, CScope *scope_info,  bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		KEYWORD_TYPE key_w;
		CScope *_currentScope=NULL;
		std::string conditional_str;
		error = false;

		// check for keyword ...
		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::DO_WHILE_KEYWORD){


				aux_p += strlen(defined_keyword[key_w].str);

				//1st evaluate body ..
				aux_p=IGNORE_BLANKS(aux_p,line);
				if(*aux_p != '{'){
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected open block ('{') in do-while expression");
					return NULL;
				}
				if((aux_p=evalBlock(aux_p
						,line
						,_currentScope
						,error

						))!= NULL){
					if(!error){

						// Finally evaluate conditional line ...
						aux_p=IGNORE_BLANKS(aux_p,line);

						// check for keyword ...
						key_w = isKeyword(aux_p);

						if(key_w!=WHILE_KEYWORD){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected while keyword");
							return NULL;
						}

						aux_p += strlen(defined_keyword[key_w].str);

						aux_p=IGNORE_BLANKS(aux_p,line);

						if(*aux_p == '('){

							if((end_expr = evalExpression(aux_p+1,line,_currentScope,&pCurrentFunctionInfo->instruction)) != NULL){

								if(*end_expr != ')'){
									writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')'");
									return NULL;
								}
								if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
									return NULL;
								}
							}else{
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')' do-while expression");
								return NULL;
							}

						}else{
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '(' do-while expression");
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

	char * CEval::evalKeywordIf(const char *s,int & line,  CScope *scope_info, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		char *end_expr,*start_symbol;
		int dl=-1;
		KEYWORD_TYPE key_w;
		std::string conditional_str;
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
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '(' if");
						return NULL;
					}

					conditional_line=line;

					if((end_expr = evalExpression(aux_p+1,line,scope_info,&pCurrentFunctionInfo->instruction)) == NULL){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')' if ");
						return NULL;
					}

					if(*end_expr != ')'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')'");
						return NULL;
					}

					if(IGNORE_BLANKS(aux_p+1,dl)==end_expr){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"no conditional expression");
						return NULL;
					}

					if((start_symbol = CZetScriptUtils::copyStringFromInterval(aux_p+1, end_expr))==NULL){
						return NULL;
					}

					conditional_str=start_symbol;

					aux_p=IGNORE_BLANKS(end_expr+1,line);
					if(*aux_p != '{'){

						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected if-block open block ('{')");
						return NULL;
					}

					if((aux_p=evalBlock(aux_p
							,line
							,scope_info
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
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected else-block open block ('{')");
								return NULL;
							}

							if((aux_p=evalBlock(aux_p
									,line
									,scope_info
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

	char * CEval::evalKeywordFor(const char *s,int & line,  CScope *scope_info,  bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		error=false;
		std::string eval_for;
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
							if((aux_p = evalKeywordVar(aux_p,line, _currentScope,error))==NULL){
								return NULL;
							}

							aux_p = aux_p - 1; // redirect aux_p to ';'
						}
						else{

							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected 'var' keyword");
							return NULL;
						}

					}

					aux_p=IGNORE_BLANKS(aux_p,line);

					key_w = isKeyword(aux_p);
					if(key_w == KEYWORD_TYPE::IN_KEYWORD){

						//PASTNode node_for_in_right_op_expression=NULL;

						aux_p=IGNORE_BLANKS(aux_p+strlen(defined_keyword[KEYWORD_TYPE::IN_KEYWORD].str),line);


						if((aux_p = evalExpression((const char *)aux_p,line,_currentScope,&pCurrentFunctionInfo->instruction)) == NULL){
							return NULL;
						}
					}

					else{ // expects conditional and post (i.e for(;;) )


						if(*aux_p != ';'){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ';'");
							return NULL;

						}

						aux_p=IGNORE_BLANKS(aux_p+1,line);

						if(*aux_p != ';'){ // conditional...
							char * end_p=IGNORE_BLANKS(aux_p+1,line);

							if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

								if((aux_p = evalExpression((const char *)aux_p,line,_currentScope,&pCurrentFunctionInfo->instruction)) == NULL){
									return NULL;
								}
							}
						}

						aux_p=IGNORE_BLANKS(aux_p,line);

						if(*aux_p != ';'){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ';'");
							return NULL;

						}
						aux_p=IGNORE_BLANKS(aux_p+1,line);

						if(*aux_p != ')' ){ // finally do post op...

							if(*aux_p == ',' ){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Unexpected ) ");
								return NULL;
							}

							do{
								if((aux_p = evalExpression(aux_p,line,_currentScope,&pCurrentFunctionInfo->instruction))==NULL){
									return NULL;
								}

								if(*aux_p == ',' ){
									aux_p=IGNORE_BLANKS(aux_p+1,line);
								}else{
									if(*aux_p != ')' ){
										writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')'");
										return NULL;
									}
								}

							}while(*aux_p != ')' && *aux_p != 0);
						}
					}

					if(*aux_p != ')'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')'");
						return NULL;
					}

					aux_p=IGNORE_BLANKS(aux_p+1,line);
					if(*aux_p != '{'){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '{' for-block");
						return NULL;
					}

					// eval block ...
					if((aux_p=evalBlock(aux_p
							,line
							,_currentScope
							,error
						))!= NULL){ // true: We treat declared variables into for as another scope.
						if(!error){

							scope_info->popScope(); // push current scope
							return aux_p;
						}
					}
				}else{
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '(' for");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordSwitch(const char *s,int & line,  CScope *scope_info,  bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p = (char *)s;
		CScope *scope_case=NULL;
		std::string val;
		KEYWORD_TYPE key_w;//,key_w2;
		CScope *currentScope=scope_info;

		error=false;
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
						if((aux_p = CEval::evalExpression(
								aux_p,
								line,
								scope_info,
								&pCurrentFunctionInfo->instruction
								))==NULL)
						{
							return NULL;
						}


						if(*aux_p != ')'){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ')' switch");
							error = true;
							return NULL;
						}

						aux_p=IGNORE_BLANKS(aux_p+1,line);

						if(*aux_p == '{'){

							aux_p++;

							if((aux_p=eval_Recursive(aux_p, line, currentScope, error))==NULL){
								return NULL;
							}

							aux_p=IGNORE_BLANKS(aux_p,line);

							if(*aux_p != '}'){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '}' switch");
								return NULL;
							}

							scope_info->popScope();
							return aux_p+1;
						}
						else{
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '{' switch");
							return NULL;
						}
				}
				else{
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected '(' switch ");
					return NULL;
				}
			}
		}
		return NULL;
	}

	char * CEval::evalKeywordVar(const char *s,int & line,  CScope *scope_info, bool & error){

		// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass

		char *aux_p = (char *)s;
		KEYWORD_TYPE key_w;
		char *start_var,*end_var;
		std::string s_aux,variable_name;
		//char *symbol_value;
		bool end=false;
		bool allow_for_in=true;
		short idxScopeClass=-1;
		short idxScope=scope_info->getCurrentScopePointer()->idxScope;
		//bool is_class_member=false;
		int m_startLine=0;

		CScriptClass *sc=NULL;
		CScriptClass *sc_come_from=NULL;

		if(scope_info->idxScope!=IDX_GLOBAL_SCOPE){
			sc_come_from=scope_info->getScriptClass();//) { // NOT GLOBAL
		}
		//is_class_member = scope_info->getIdxBaseScope() == scope_info->getCurrentScopePointer()->idxScope;

		key_w = isKeyword(aux_p);

		if(key_w != KEYWORD_TYPE::UNKNOWN_KEYWORD){
			if(key_w == KEYWORD_TYPE::VAR_KEYWORD){ // possible variable...

				aux_p += strlen(defined_keyword[key_w].str);
				aux_p=IGNORE_BLANKS(aux_p,line);


				while(*aux_p != ';' && *aux_p != 0 && !end){ // JE: added multivar feature.

					//bool is_class_member=parent_scope_is_class;

					aux_p=IGNORE_BLANKS(aux_p,line);
					start_var=aux_p;
					m_startLine = line;
					//is_class_member=false;
					idxScope=scope_info->getCurrentScopePointer()->idxScope;
					sc=NULL;

					if(sc_come_from != NULL){ // it comes from class declaration itself
						sc=sc_come_from;
						idxScope=sc->symbol_info.symbol->idxScope;
						//is_class_member=true;

					}
					else{ // check if type var ClasS::v1 or v1
						if((end_var=isClassMember(aux_p,line,&sc,variable_name, error))!=NULL){ // check if particular case extension attribute class
							idxScope = idxScopeClass; // override scope info
						}
						else{ // causal variable
							if(error){
								return NULL;
							}
							else{ // get normal name...

								line = m_startLine;

								// check whwther the function is anonymous with a previous arithmetic operation ....
								if((end_var=getIdentifierToken(aux_p,variable_name))==NULL){
									writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected symbol");
									return NULL;
								}
							}
						}
					}

					KEYWORD_TYPE keyw = isKeyword(variable_name.c_str());

					if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Cannot use symbol name as reserverd symbol \"%s\"",defined_keyword[keyw].str);
						return NULL;
					}

					aux_p=end_var;
					aux_p=IGNORE_BLANKS(aux_p,line);
					//}
					bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;
					if(sc!=NULL && *aux_p == '='){
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Variable member is not assignable on its declaration. Should be initialized within constructor.");
						return NULL;
					}

					if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
						allow_for_in=false;
						if(*aux_p == '='){ // only for variables (not class members)
							// try to evaluate expression...
							aux_p=IGNORE_BLANKS(aux_p,line);

							if((aux_p = evalExpression(start_var,m_startLine,scope_info,&pCurrentFunctionInfo->instruction)) == NULL){
								return NULL;
							}
							line = m_startLine;
						}
						 // define as many vars is declared within ','

						//--- OP
						if(sc!=NULL){ // register as variable member...
							sc->registerVariable(CURRENT_PARSING_FILE_STR, line, variable_name);
						}
						else{ // register as local variable in the function...
							pCurrentFunctionInfo->function_info_object->registerVariable(CURRENT_PARSING_FILE_STR, line, variable_name);
						}

						//---

						zs_print_debug_cr("registered symbol \"%s\" line %i ",variable_name.c_str(), line);
					}
					else{

						KEYWORD_TYPE keyw = isKeyword(variable_name.c_str());
						if(keyw == KEYWORD_TYPE::IN_KEYWORD){ // in keyword was detected (return to evalr)...
							if(!allow_for_in){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"'in' keyword should be used with an uninitialized variable (example: for ( var e in v) {...} )", *aux_p);
								return NULL;
							}
							end=true;
						}
						else{
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"unexpected '%c'", *aux_p);
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
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected ';'");
					return NULL;
				}
				return aux_p;
			}
		}
		return NULL;
	}

	char * CEval::evalBlock(const char *s,int & line,  CScope *scope_info, bool & error){
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

			if((aux_p = eval_Recursive(aux_p, line, currentScope,error)) != NULL){
				if(error){
					return NULL;
				}

				if(*aux_p != '}'){
					error = true;
					writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected } ");
					return NULL;
				}

				scope_info->popScope();
				return aux_p+1;
			}
		}
		return NULL;
	}

	char *CEval::evalBreak(const char *s, int & line, CScope *scope_info, bool & error){
		// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
		char *aux_p=(char *)s;

		fprintf(stderr,"find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n");

		{ // ok break is valid in current scope...

			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"\"break\" allowed within loop or case-switch statements");
			error = true;
			return NULL;
		}

		if(*aux_p != ';'){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected ';'");
			error = true;
			return NULL;
		}
	}

	char *CEval::evalContinue(const char *s, int & line, CScope *scope_info, bool & error){
		// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
		char *aux_p=(char*)s;
		{ // ok break is valid in current scope...

			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"\"continue\" allowed within loop or case-switch statements");
			error = true;
			return NULL;
		}

		if(*aux_p != ';'){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected ';'");
			error = true;
			return NULL;
		}
	}

	char *CEval::evalDefaultCase(const char *s, int & line, CScope *scope_info,  bool & error){
		char *aux_p=(char *)s;
		std::string value_to_eval;
		tTokenNode token_node;

		std::vector<tInstructionEval> *tokenCompiled = NULL;

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
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Expected  ':' ");
			return NULL;
		}
		return aux_p+1;
	}

	char *CEval::evalKeyword(const char *s, int & line, CScope *scope_info, bool & error){

		// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
		char *aux_p= (char *)s;
		tFunctionInfo *ifc_new = NULL;
		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD,keyw2nd=KEYWORD_TYPE::UNKNOWN_KEYWORD;

		aux_p=IGNORE_BLANKS(aux_p, line);

		// check if condition...
		keyw = isKeyword(aux_p);

		if(keyw != KEYWORD_TYPE::UNKNOWN_KEYWORD){ // a keyword was detected...

			switch(keyw){
			case CASE_KEYWORD:
			case DEFAULT_KEYWORD:
				return evalDefaultCase(s,line,scope_info,error);

			case FUNCTION_KEYWORD:
				if((aux_p = evalKeywordFunction(s,line,scope_info,error)) != NULL){
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

					return  (*defined_keyword[keyw].eval_fun)(s,line,scope_info,error);
				}
				writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Not implemented");
				error = true;
				return NULL;
			}
		}
		return NULL;
	}

	char * CEval::eval_Recursive(const char *s, int & line, CScope *scope_info,  bool & error){

		// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)

		KEYWORD_TYPE keyw=KEYWORD_TYPE::UNKNOWN_KEYWORD;
		bool custom_quit = false;
		char *aux = (char *)s;
		char *end_expr=0;
		bool processed_directive=false;

		aux=IGNORE_BLANKS(aux, line);

		while(*aux != 0 && !custom_quit){

			processed_directive=false;
			//children = NULL;
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
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected starting \" directive");
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						aux++;
						start_var=aux;

						while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

						if(*aux != '\"'){
							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected end \" directive");
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
							std::string current_file_str=CEval::CURRENT_PARSING_FILE_STR;
							int current_file_idx=CEval::CURRENT_PARSING_FILE_IDX;
							std::string file_to_eval=symbol_name;

							if(isFilenameAlreadyParsed(file_to_eval.c_str())){
								writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"\"%s\" already evald",file_to_eval.c_str());
								THROW_SCRIPT_ERROR();
								return NULL;
							}

							try{
								CZetScript::getInstance()->evalFile(file_to_eval);
							}catch(exception::script_error & error){
								THROW_EXCEPTION(error);
								return NULL;
							}

							//restore current file info...
							CEval::CURRENT_PARSING_FILE_STR=current_file_str.c_str();
							CEval::CURRENT_PARSING_FILE_IDX=current_file_idx;
						}

						aux++;// advance ..
						break;
					default:
						writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"directive \"%s\" not supported",defined_directive[directive].str);
						break;
					}

					processed_directive = true;
					end_expr=aux;
				}
			}

			// 0st special case member class extension ...
			if(!processed_directive){ // not processed yet ...
				// 1st. check whether eval a keyword...
				if((end_expr = evalKeyword(aux, line, scope_info, error)) == NULL){

					// If was unsuccessful then try to eval expression.
					if(error){

						THROW_SCRIPT_ERROR();
						return NULL;
					}
					// 2nd. check whether eval a block
					if((end_expr = evalBlock(aux
							,line
							, scope_info
							, error
							))==NULL){

						// If was unsuccessful then try to eval expression.
						if(error){
							THROW_SCRIPT_ERROR();
							return NULL;
						}
						// 2nd. try expression
						int starting_expression=line;

						if((end_expr = evalExpression(aux,line, scope_info,&pCurrentFunctionInfo->instruction)) == NULL){ // something wrong was happen.
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						if(*end_expr != ';'){

							writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"expected end ;");
							THROW_SCRIPT_ERROR();
							return NULL;
						}

						end_expr=IGNORE_BLANKS(end_expr+1, line);
					}
				}
			}

			aux=end_expr;
			aux=IGNORE_BLANKS(aux, line);
		}
		return aux;
	}

	void CEval::pushFunction(CScriptFunction *sf){
		vFunctionInfo.push_back(pCurrentFunctionInfo=new tFunctionInfo(sf));
	}

	void CEval::popFunction(){

		pCurrentFunctionInfo->function_info_object->instruction=NULL;


		// get total size op + 1 ends with NULL
		unsigned size = (pCurrentFunctionInfo->instruction.size() + 1) * sizeof(tInstruction);
		pCurrentFunctionInfo->function_info_object->instruction = (PtrInstruction)malloc(size);
		memset(pCurrentFunctionInfo->function_info_object->instruction, 0, size);


		for(unsigned i=0; i < pCurrentFunctionInfo->instruction.size(); i++){

			tVariableSymbolInfo *vis=NULL;
			tInstructionEval *instruction = &pCurrentFunctionInfo->instruction[i];
			tLinkSymbolFirstAccess *ls=&instruction->linkSymbolFirstAccess;


			if(ls->idxScriptFunction != ZS_UNDEFINED_IDX){ // solve first symbol first access...

				CScriptFunction *sf=GET_SCRIPT_FUNCTION(ls->idxScriptFunction);
				CScriptClass *sc = GET_SCRIPT_CLASS(sf->idxClass);

				if(instruction->properties & INS_PROPERTY_THIS_SCOPE){ // trivial this.

					if(ls->n_params==NO_PARAMS_IS_VARIABLE){
						if((vis=sc->getVariable(ls->value,sc->symbol_info.symbol->idxScope))==0){
							THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("Cannot find variable %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
							return;
						}

						instruction->index_op2=vis->idxSymbol;
					}
					else{
						if((instruction->index_op2=(intptr_t)sc->getFunction(ls->value,sc->symbol_info.symbol->idxScope,ls->n_params))==0){
							THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("Cannot find function %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
							return;
						}
					}

				}else if(instruction->properties & INS_PROPERTY_SUPER_SCOPE){ // trivial super.

					CScriptFunction *sf_found=NULL;
					std::string symbol_name_to_find = sf->symbol_info.symbol->name;

					for(int i = sf->idxLocalFunction-1; i >=0 && sf_found==NULL; i--){

						if(
								(sc->m_function[i]->m_arg.size() == ls->n_params)
							&& (symbol_name_to_find == sc->m_function[i]->symbol_info.symbol->name)
							){
							sf_found = sc->m_function[i];
						}
					}

					// ok get the super function...
					if(sf_found == NULL){

						THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("Cannot find super function %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
						return;
					}

					instruction->index_op2=(intptr_t)sf_found;


				}else{ // find local/global var/function ...
					bool local_found=false;
					LOAD_TYPE load_type=LOAD_TYPE::LOAD_TYPE_UNDEFINED;

					// try find local symbol  ...
					CScope *scope=GET_SCOPE(ls->idxScope);
					tSymbol * sc_var = scope->getSymbol(ls->value, ls->n_params);
					if(ls->n_params==NO_PARAMS_IS_VARIABLE){
						if((vis=sf->getVariable(ls->value,sc_var->idxScope))!=NULL){
							load_type=LOAD_TYPE::LOAD_TYPE_VARIABLE;
							instruction->index_op2=vis->idxSymbol;
							local_found=true;
						}
					}
					else{

						if((instruction->index_op2=(intptr_t)sf->getFunction(ls->value,sc_var->idxScope,ls->n_params))!=0){
							load_type=LOAD_TYPE::LOAD_TYPE_FUNCTION;
							local_found =true;
						}
					}

					if(!local_found){ // try global...
						tSymbol * sc_var = MAIN_SCOPE->getSymbol(ls->value, ls->n_params);

						if(sc_var != NULL){
							if(ls->n_params==NO_PARAMS_IS_VARIABLE){

								if((vis=MAIN_FUNCTION->getVariable(ls->value,sc_var->idxScope))==NULL){
									THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("Cannot find variable \"%s\"",ls->value.c_str()));
									return;
								}

								load_type=LOAD_TYPE::LOAD_TYPE_VARIABLE;
								instruction->index_op2=vis->idxSymbol;
							}
							else{

								if((instruction->index_op2=(intptr_t)MAIN_FUNCTION->getFunction(ls->value,sc_var->idxScope,ls->n_params))==0){
									THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("Cannot find function \"%s\"",ls->value.c_str()));
									return;
								}

								load_type=LOAD_TYPE::LOAD_TYPE_FUNCTION;

							}
						}
					}



					instruction->index_op1=load_type;

					if(load_type==LOAD_TYPE::LOAD_TYPE_FUNCTION){
						CScriptFunction *sf = ((CScriptFunction *)instruction->index_op2);
						if((sf->symbol_info.properties & PROPERTY_C_OBJECT_REF) != 0){ // function will be solved at run time because it has to check param type
							instruction->index_op2=ZS_SOLVE_AT_RUNTIME; // late binding, solve at runtime...
						}
					}

				}
			}

			// save instruction ...
			pCurrentFunctionInfo->function_info_object->instruction[i].op_code = instruction->op_code;
			pCurrentFunctionInfo->function_info_object->instruction[i].index_op1 = instruction->index_op1;
			pCurrentFunctionInfo->function_info_object->instruction[i].index_op2 = instruction->index_op2;
			pCurrentFunctionInfo->function_info_object->instruction[i].properties = instruction->properties;


			// symbol value to save at runtime ...
			if(instruction->instructionInfo.symbol_name != NULL){
				pCurrentFunctionInfo->function_info_object->instruction_info[i]=instruction->instructionInfo;
			}

		}

		pCurrentFunctionInfo->function_info_object->buildLutScopeSymbols();

		// delete and popback function information...
		delete(pCurrentFunctionInfo);
		vFunctionInfo.pop_back();

		pCurrentFunctionInfo = NULL;
		if(vFunctionInfo.size() > 0){
			pCurrentFunctionInfo = vFunctionInfo.at(vFunctionInfo.size()-1);
		}


	}

	char * CEval::eval(const char * str, bool & error){
		int line =1;
		char *end_char = NULL;

		pushFunction(MAIN_FUNCTION);

		end_char=eval_Recursive(str,line,MAIN_SCOPE,error);

		popFunction();

		if(*end_char != 0){
			writeErrorMsg(CURRENT_PARSING_FILE_STR,line,"Unexpected \'%c\' ",*end_char);
			THROW_SCRIPT_ERROR();
			error=true;
		}

		if(!error){
			CURRENT_VM->buildCache();
		}

		return end_char;
	}

	bool CEval::evalStringInternal(const std::string & expression){

		bool error=false;
		char *end_char=NULL;

		try{
			eval(expression.c_str(),error);
		}
		catch(exception::script_error & e)
		{
			THROW_EXCEPTION(e);
			error=true;
		}

		return !error;
	}

	bool CEval::evalFileInternal(const std::string & filename){
		int idx_file=-1;
		bool error=false;
		char *buf_tmp=NULL;

		if(!isFilenameAlreadyParsed(filename)){
			tInfoParsedSource ps;
			ps.filename = filename;
			m_parsedSource.push_back(ps);
			idx_file=m_parsedSource.size()-1;
			int n_bytes;

			if((buf_tmp=CZetScriptUtils::readFile(filename, n_bytes))!=NULL){
				CURRENT_PARSING_FILE_STR=filename.c_str();
				CURRENT_PARSING_FILE_IDX=idx_file;
				try{
					eval(buf_tmp,error);
				}
				catch(exception::script_error & e){
					free(buf_tmp);
					buf_tmp=NULL;
					THROW_EXCEPTION(e);
					error=true;
				}

				CURRENT_PARSING_FILE_STR=NULL;
				CURRENT_PARSING_FILE_IDX=-1;
			}

		}else{
			// already parsed
			THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("Filename \"%s\" already parsed",filename.c_str()));
			error=true;
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}
		return !error;
	}

	CEval::~CEval() {
		for(int i=0;i<MAX_KEYWORD;i++){
			if(defined_keyword[i].eval_fun != NULL ){
				delete defined_keyword[i].eval_fun;

			}
		}

		for(std::map<std::string,tInfoConstantValue *>::iterator it=m_contantPool.begin();it!=m_contantPool.end();it++){
			tInfoConstantValue *icv=it->second;
			switch(GET_INS_PROPERTY_VAR_TYPE(icv->properties)){
			default:
				break;
			case STK_PROPERTY_TYPE_INTEGER:
				//delete (int *)icv->varRef;
				break;
			case STK_PROPERTY_TYPE_BOOLEAN:
				//delete (bool *)icv->stkValue;
				break;
			case STK_PROPERTY_TYPE_NUMBER:
				//delete (float *)icv->stkValue;
				break;
			case STK_PROPERTY_TYPE_STRING:
				delete (CStringScriptVariable *)icv->varRef;
				break;

			}

			delete icv;
		}

		m_contantPool.clear();
	}

}

