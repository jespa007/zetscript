#include "script/zg_script.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100

CUndefined *CScopeInfo::UndefinedSymbol=NULL;
CVoid *CScopeInfo::VoidSymbol=NULL;
int n_anonymouse_func=0;

void CScopeInfo::createSingletons(){
	UndefinedSymbol = new CUndefined();
	VoidSymbol = new CVoid();
}


void CScopeInfo::destroySingletons(){
	delete UndefinedSymbol;
	delete VoidSymbol;
}



CScopeInfo::CScopeInfo(CScopeInfo * _parent){
	m_parentScope = _parent;

	m_baseScope = this;


	if(_parent == NULL){
		m_mainScope = this;
	}else{
		m_mainScope = _parent->getMainScope();
	}

	m_currentScopePointer=m_baseScope;
}


CScopeInfo * CScopeInfo::getMainScope(){
	return m_mainScope;
}

void CScopeInfo::addLocalScope(CScopeInfo *_ls){
	m_scopeList.push_back(_ls);
}

CScopeInfo * CScopeInfo::getParent(){
	return m_parentScope;
}

CScopeInfo * CScopeInfo::getCurrentScopePointer(){
	return m_currentScopePointer;
}

void CScopeInfo::resetScopePointer(){
	m_currentScopePointer = m_baseScope;
}

CScopeInfo * CScopeInfo::pushScope(){

	CScopeInfo *new_scope = new CScopeInfo(m_currentScopePointer);
	m_currentScopePointer->m_scopeList.push_back(new_scope);
	m_currentScopePointer = new_scope;
	return m_currentScopePointer;

}

CScopeInfo * CScopeInfo::popScope(){

	if(m_currentScopePointer->m_parentScope != NULL){
		m_currentScopePointer = m_currentScopePointer->m_parentScope;
		return m_currentScopePointer;
	}

	return NULL;
}


//-----------------------------------------------------------------------------------------------------------
//
// SCOPE VARIABLE MANAGEMENT
//
tInfoScopeVar * CScopeInfo::registerAnonymouseFunction(PASTNode ast){ // register anonymous function in the top of scope ...

	tInfoScopeVar * irv = new tInfoScopeVar;


	string var_name = "_afun_"+CStringUtils::intToString(n_anonymouse_func++);
	string symbol_ref = "_"+var_name;

	irv->name=var_name;
	irv->ast=ast;
	m_baseScope->m_registeredVariable[symbol_ref]=irv;

	return irv;
}


tInfoScopeVar * CScopeInfo::registerSymbol(const string & var_name, PASTNode ast){
	tInfoScopeVar * irv;


	if((irv = existRegisteredSymbol(var_name))==NULL){ // check whether is local var registered scope ...

		string symbol_ref = var_name;
		symbol_ref = "_"+symbol_ref;
		irv = new tInfoScopeVar;
		//irv->m_obj=NULL;
		m_registeredVariable[symbol_ref]=irv;
		irv->name=var_name;
		irv->ast=ast;
		/*if(!m_mainScope->addIndexedSymbol(irv)){
			return NULL;
		}*/
		return irv;
	}else{
		int m_line=-1;
		/*if(irv->ast != NULL) {
			m_line = irv->ast->definedValueline;
		}*/
		print_error_cr("(line %i): error var \"%s\" already registered at line %i!", m_line,var_name.c_str(), m_line);
	}

	return NULL;
}
/*
bool CScopeInfo::addIndexedSymbol(tInfoScopeVar *isv){
	if(m_parentScope == NULL){ // ok is the main scope...

		isv->index_var=m_indexedSymbol.size();
		m_indexedSymbol.push_back(isv);

		return true;
	}
	else{
		print_error_cr("You must call this function from parent scope");
	}

	return false;
}
*/

/*
bool CScopeInfo::defineSymbol(const string & var_name, CObject *obj){

	tInfoScopeVar * irv;
	if((irv = existRegisteredSymbol(var_name))!=NULL){ // check whether is local var registered scope ...
		irv->m_obj=obj;
		irv->m_obj->setName(var_name);
		return true;
	}else{
		print_error_cr("error var \"%s\" is not registered!", var_name.c_str());
	}
	return false;
}*/

tInfoScopeVar * CScopeInfo::existRegisteredSymbol(const string & var_name){

	string symbol_ref = var_name;
	symbol_ref = "_"+symbol_ref;

	if(m_registeredVariable.count(symbol_ref)==0){ // not exit but we will deepth through parents ...
		CScopeInfo * parent =  getParent();
		if(parent != NULL){
			return parent->existRegisteredSymbol(var_name);
		}
		return NULL;
	}else{
		return m_registeredVariable[symbol_ref];
		//print_error_cr("variable %s already registered at line %i",var_name.c_str(),lc->m_registeredVariable[var_name]->m_line);
	}

	return NULL;

}



tInfoScopeVar *CScopeInfo::getInfoRegisteredSymbol(const string & var_name, bool print_msg){
	tInfoScopeVar * irv;

	if((irv = existRegisteredSymbol(var_name))!=NULL){ // check whether is local var registered scope ...

		return irv;
	}else{
		if(print_msg){
			print_error_cr("var \"%s\" doesn't exist!", var_name.c_str());
		}
	}

	return NULL;

}


//-----------------------------------------------------------------------------------------------------------
/*
bool CScopeInfo::isVarDeclarationStatment(const char *statment, bool & error, char ** eval_expression,int & m_line, CScopeInfo * _localScope){
	// PRE: length(statment) < MAX_STATMENT_LENGTH
	char *aux = (char *)statment;

	//string var_name;
	//PASTNode expression;
	tInfoKeyword *key_w;
	char *start_var,*end_var;
	*eval_expression=NULL;
	error=false;
	char stat[MAX_STATMENT_LENGTH]={0};
	string s_aux;
	int var_length;
	int registeredVariableLine;

	aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

	if(strncmp(aux,"var",3)==0){ // possible variable...
		aux+=3;

		if(*aux!=' '){ // is not var word...
			return NULL;
		}

		aux=CStringUtils::IGNORE_BLANKS(aux,m_line);
		*eval_expression=aux;
		registeredVariableLine = m_line;

		if(*aux!=0 && (
		   ('a' <= *aux && *aux <='z') ||
		   ('A' <= *aux && *aux <='Z'))
		){ // let's see it has right chars...
			start_var=aux;
			aux++;
			while((*aux!=0 || *aux!= '=') && (('a' <= *aux && *aux <='z') ||
				  ('0' <= *aux && *aux <='9') ||
				  (*aux=='_') ||
				  ('A' <= *aux && *aux <='Z'))){
				aux++;
			}
			end_var=aux;

			var_length=end_var-start_var;
			strncpy(stat,start_var,var_length);

			if((key_w = is_keyword(stat)) != NULL){
				print_error_cr("unexpected keyword \"%s\" at line %i",stat, m_line);
				error = true;
				return false;
			}

			aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

			if((*aux == 0 || *aux == '=')){


				print_info_cr("registered \"%s\" variable: ",stat);

				s_aux=stat;
				if(!_localScope->registerSymbol(s_aux,registeredVariableLine)){
					error=true;
				}

				// rewind line when the variable was declared because we pass the expression "variable=expression"
				m_line = registeredVariableLine;

				return true;
			}
			else{
				print_error_cr("variable cannot cannot contain char '%c' at line",*aux, m_line);
				error=true;
			}

		}else{
			print_error_cr("variable cannot start with char %c",*aux);
			error=true;
		}
	}
	return false;
}
*/

//-----------------------------------------------------------------------------------------------------------
typedef struct{
	int m_line;
	string str;
}tInfoKeyWord;


typedef struct{
	char *str_begin;
	char *str_end;
	int m_begin_line;
}tInfoStartEndString;


typedef struct {

	string str_conditional_value; // values that executes the scope...
	tInfoStartEndString str_scope;
	tInfoAsmOp *jt_asm=NULL;
	tInfoAsmOp *jmp_asm=NULL;
}tInfoCase;

typedef struct {

	vector<tInfoCase> conditional_case;
	tInfoStartEndString str_default_case_scope;
	tInfoAsmOp *jmp_default_asm=NULL;

}tInfoSwitch;

char *parseKeyword_IfElseForWhile(const char *str, tInfoKeyword **keyw, int & m_line, tInfoKeyWord & condition, tInfoKeyWord & pre_for, tInfoKeyWord & post_for, bool & error ){
	char *current = (char *)str,*next,*next_aux, *begin_ptr, *end_ptr;

	int length;
	char expr[MAX_STATMENT_LENGTH];
	string cond_expr;
	error = false;
	int m_line2;


	*keyw=NULL;


	condition.str = "";
	post_for.str="";
	pre_for.str="";

	//goto_statment = -1;

	// check if condition...
	*keyw = CAst::isKeyword(str);

	if(*keyw == NULL){
		return NULL;
	}

	if((*keyw)->id == VAR_KEYWORD){ // special case that is managed later..
		return NULL;
	}


	if(*keyw != NULL){

		current+=strlen((*keyw)->str);
		current=CStringUtils::IGNORE_BLANKS(current,m_line);

		if((*keyw)->id == ELSE_KEYWORD){ // special case that is managed later..
			return current;
		}

		if(*current == '('){
			current++;

		}else{
			print_error_cr("Expected '(' at line %i",m_line);
			error = true;
			return NULL;
		}

		current = CStringUtils::IGNORE_BLANKS(current,m_line);
		// search for )
		next = current;
		int n_open_parentesis = 1;
		while(*next!=0 && *next!='{' && n_open_parentesis != 0 ){
			next=CStringUtils::IGNORE_BLANKS(next,m_line);
			if(*next == ')')	{n_open_parentesis--;}
			if(*next == '(')	{n_open_parentesis++;}

			if(n_open_parentesis!=0)
				next++;
		}

		if(n_open_parentesis != 0){
			print_error_cr("Unclosed expression at line %i",m_line);
			error = true;
			return NULL;
		}
		//next = strstr(current,")");

		length=next-current;
		if(length>=MAX_STATMENT_LENGTH){
			print_error_cr("Max statment length!");
			error = true;
			return NULL;
		}

		memset(expr,0,sizeof(expr));

		next_aux=CStringUtils::IGNORE_BLANKS_REVERSE(next-1,current,m_line2);
		strncpy(expr,current,(next_aux-current+1));


		cond_expr=expr;
		condition.str = expr;
		condition.m_line = m_line;

		if((*keyw)->id == FOR_KEYWORD){

			/*_scope->m_scopeList.push_back(for_scope=new CScopeInfo(_scope->getScriptFunction(),_scope));
			CCompiler::getInstance()->insertPushScopeInstruction(for_scope);*/

			end_ptr = begin_ptr = (char *)cond_expr.c_str();
			pre_for.m_line = m_line;
			//end_ptr = CStringUtils::ADVANCE_TO_CHAR(begin_ptr,';', m_line);
			while((*end_ptr!=0 || *end_ptr==';')){
				current=CStringUtils::IGNORE_BLANKS(end_ptr,m_line);
				end_ptr++;
			}


			if(*end_ptr != ';'){
				print_error_cr("Expected ';' at line %i",m_line);
				error = true;
				return NULL;
			}

			length=end_ptr-begin_ptr;
			if(length>=MAX_STATMENT_LENGTH){
				print_error_cr("Max statment length!");
				error = true;
				return NULL;
			}
			memset(expr,0,sizeof(expr));

			strncpy(expr,begin_ptr,(end_ptr-begin_ptr));
			strcat(expr,";");
			pre_for.str=expr;

			// evaluating init for;
		/*	if(evalRecursive(pre_for, m_line, for_scope, level_scope+1)==NULL){
				return NULL;
			}*/

			end_ptr++;
			begin_ptr = end_ptr;
			condition.m_line = m_line;
			//end_ptr = CStringUtils::ADVANCE_TO_CHAR(begin_ptr,';', m_line);
			while((*end_ptr!=0 || *end_ptr==';')){
				current=CStringUtils::IGNORE_BLANKS(end_ptr,m_line);
				end_ptr++;
			}


			if(*end_ptr != ';'){
				print_error_cr("Expected ';' at line %i",m_line);
				error = true;
				return NULL;
			}

			length=end_ptr-begin_ptr;
			if(length>=MAX_STATMENT_LENGTH){
				print_error_cr("Max statment length!");
				error = true;
				return NULL;
			}

			memset(expr,0,sizeof(expr));
			strncpy(expr,begin_ptr,(end_ptr-begin_ptr));
			if(*expr == 0){
				strcpy(expr,"true");
			}
			condition.str=expr;

			end_ptr=CStringUtils::IGNORE_BLANKS(end_ptr+1,m_line);
			post_for.m_line = m_line;
			memset(expr,0,sizeof(expr));
			if(*end_ptr!=0){
				strcpy(expr,end_ptr);
				post_for.str=expr;
			}
			post_for.str+=";"; // add a end end semicolon

			//condition = expr;
			//strcpy(condition,cond_for);

		}

	}
	else{
		return NULL;
	}
	current = next + 1;
	return current;
}

char *getKeywordHeader(const char *str, string & header, bool & error, CScopeInfo * _scope){
	return NULL;
}

char *processKeywordHeader(const char *str, string & header, bool & error, CScopeInfo * _scope){
	return NULL;
}

char *processKeywordBody(const char *str, string & header, bool & error, CScopeInfo * _scope){
	return NULL;
}

/*
char * CScopeInfo::evalRecursive(const char *str_to_eval, int & m_line, bool & error, CScopeInfo * _scope, int level_scope){

	char *current=(char *) str_to_eval;
	string var_name;
	//int m_line=0;

	char statment[MAX_STATMENT_LENGTH];
	tInfoKeyWord condition;
	tInfoKeyWord pre_for;
	tInfoKeyWord post_for;
	tInfoSwitch info_switch;
	int index_st_jump=-1; // not conditional jump;
	CVirtualMachine::tInfoAsmOp *jnt_asm=NULL;;
	CVirtualMachine::tInfoAsmOp *jmp_asm=NULL;


	char *next;
	tInfoKeyword * key_w=NULL, *key_w_last=NULL;
	int length;

	CScopeInfo * new_local_scope = NULL,*for_scope=NULL;

	current=CStringUtils::IGNORE_BLANKS(current,m_line);

	while(*current!=0){


		// reset info_switch...
		memset(&info_switch.str_default_case_scope,0,sizeof(info_switch.str_default_case_scope));
		info_switch.conditional_case.clear();

		next = parseKeyword_IfElseForWhile(current,&key_w,m_line,condition,pre_for, post_for,error);

		if(error){
			return NULL;
		}


		//------------------------------------------------------------------------------------------------------------------------------------------------
		//
		// CHECK SYNTAX KEYWORD
		//
		// recognized special op
		if(next != NULL){

			switch(key_w->id){
			default:
				print_error_cr("op %s is not implemented at line %i",key_w->str,m_line);
				error = true;
				return NULL;
				break;
			case SWITCH_KEYWORD:

				if(!IS_WORD(condition.str.c_str())){
					print_error_cr("invalid expression %s at line %i",condition.str.c_str(),condition.m_line);
					error = true;
					return NULL;
				}

				if(_scope->getInfoRegisteredSymbol(condition.str,false)==NULL){
					print_error("symbol %s not declared at line %i",condition.str.c_str(), condition.m_line);
					error = true;
					return NULL;
				}
				break;
			case FOR_KEYWORD:

				// eval pre and post...
				_scope->m_scopeList.push_back(for_scope=new CScopeInfo(_scope->getScriptFunction(),_scope));

				// pre for ...
				if(evalRecursive(pre_for.str.c_str(),pre_for.m_line, error, for_scope, level_scope+1)==NULL){
						return NULL;
				}

				// current statment index after prefor ...
				if(CCompiler::getInstance()->compileExpression(condition.str.c_str(), condition.m_line,_scope->getScriptFunction(), for_scope)){
					index_st_jump =  CCompiler::getInstance()->getCurrentStatmentIndex();
					// check whether the expression is boolean ...
					bool * b = CCompiler::getInstance()->getObjectResultCurrentStatmentAsBoolean();
					if(b){ // insert special goto asm instruction but put as not assigned yet ...
						jnt_asm = CCompiler::getInstance()->insert_JNT_Instruction();
					}
					else{
						print_error("Error FOR expression not boolean at line %i",m_line);
						error = true;
						return NULL;
					}
				}else{
					return NULL;
				}

				break;
			case ELSE_KEYWORD: // no conditional only evaluatues its scope later...
				break;
			case IF_KEYWORD: // evaluate conditional...
			case WHILE_KEYWORD:

				// jmp for while


				if(CCompiler::getInstance()->compileExpression(condition.str.c_str(), condition.m_line,_scope->getScriptFunction(), _scope)){
					index_st_jump =  CCompiler::getInstance()->getCurrentStatmentIndex();
					if(key_w->id == IF_KEYWORD){
						index_st_jump = -1; // we will set it later...
					}

					// check whether the expression is boolean ...
					bool * b = CCompiler::getInstance()->getObjectResultCurrentStatmentAsBoolean();
					if(b){ // reserve jnt whether if not true...

						jnt_asm = CCompiler::getInstance()->insert_JNT_Instruction(); // if-else
					}
					else{
						print_error("Error %s expression not boolean at line %i",key_w->str,m_line);
						error = true;
						return NULL;
					}
				}else{
					return NULL;
				}

				break;
			}

			current = next;

			if(*current != '{'){
				print_error_cr("Expected '{' on '%s' at line %i",key_w->str,m_line);
			}
		}
		//
		// KEYWORD
		//
		//------------------------------------------------------------------------------------------------------------------------------------------------


		if(*current == '{'){ // begin scope --> inc scope level ...

			//------------------------------------------------------------------------------------------------------------------------------------------------
			//
			// EVAL FOR SWITCH
			//
			if(key_w->id == SWITCH_KEYWORD) {

				tInfoKeyword *key_w_switch;
				current = CStringUtils::IGNORE_BLANKS(current+1, m_line);
				char *begin_ptr, *end_ptr, *begin_scope=NULL,*end_scope=NULL;
				long size_value;
				tInfoStartEndString *eval_scope=NULL;
				char value[100]={0};
				bool already_processed_default=false;// char case_scope[];
				// detect tokens ...
				while(*current!= 0 && *(current-1) != '}'){

					current = CStringUtils::IGNORE_BLANKS(current, m_line);

					if((key_w_switch=is_keyword(current))!=NULL){


						switch(key_w_switch->id){
							default:
								print_error_cr("unexpected keyword %s at line",key_w_switch->str, m_line);
								error=true;
								return NULL;
								break;
							case DEFAULT_KEYWORD:
							case CASE_KEYWORD: // begin scope...

								if(eval_scope!=NULL){
										if(begin_scope != NULL){ // last case had no break;
											eval_scope->str_begin = begin_scope;
											eval_scope->str_end = current;
											begin_scope=NULL;
										}
								}

								current+=strlen(key_w_switch->str);
								current = CStringUtils::IGNORE_BLANKS(current, m_line);

								if(key_w_switch->id == DEFAULT_KEYWORD){
									if(already_processed_default){
										print_error_cr("repeat default at line %i", m_line);
										error = true;
										return NULL;
									}
									already_processed_default = true;
									end_ptr=current;
									eval_scope = &info_switch.str_default_case_scope;
								}else{

									// get value ...
									begin_ptr=current;
									end_ptr=GET_END_WORD(begin_ptr);
									size_value = end_ptr-begin_ptr;

									if(size_value >= (int)sizeof(value)){
										print_error_cr("Case value to long %100s at line %i",begin_ptr, m_line);
										error = true;
										return NULL;
									}

									strncpy(value,begin_ptr,size_value);

									for(unsigned j=0; j < info_switch.conditional_case.size(); j++){
										if(info_switch.conditional_case[j].str_conditional_value == value){
											print_error_cr("duplicated switch-value %s at line %i",value,m_line);
											return NULL;
										}
									}

									print_info_cr("CASE with value %s",value);

									tInfoCase *info_case_ptr;

									tInfoCase info_case;
									info_switch.conditional_case.push_back(info_case);
									info_case_ptr = &info_switch.conditional_case[info_switch.conditional_case.size()-1];
									info_case_ptr->str_conditional_value=value;

									eval_scope=&info_case_ptr->str_scope;
								}

								current = CStringUtils::IGNORE_BLANKS(end_ptr, m_line);

								if(*current != ':'){
									print_error_cr("expected : at line %i",m_line);
									error=true;
									return NULL;
								}

								eval_scope->m_begin_line = m_line;

								// scope begins here..
								begin_scope = current+1;
								end_scope = NULL;

								break;
							case BREAK_KEYWORD: // end scope ...

								if(begin_scope==NULL){
									print_error_cr("unexpected break at line %i",m_line);
									error=true;
									return NULL;
								}
								end_scope = current;
								current+=strlen(key_w_switch->str);

								int m_line2=m_line;
								current = CStringUtils::IGNORE_BLANKS(current, m_line);
								if(*current != ';'){
									print_error_cr("expected ; %s at line",key_w_switch->str, m_line2);
									error=true;
									return NULL;
								}

								eval_scope->str_begin = begin_scope;
								eval_scope->str_end = end_scope;

								begin_scope=NULL;
								break;

						}

					}

					current++;

				}


			}
			//
			// SWITCH
			//
			//------------------------------------------------------------------------------------------------------------------------------------------------
			else
			//------------------------------------------------------------------------------------------------------------------------------------------------
			//
			// EVAL FOR IF-ELSE-FOR-WHILE
			//
			{


				// add into localscope
				if(key_w->id == FOR_KEYWORD) {
					new_local_scope = for_scope;
				}else{
					new_local_scope=new CScopeInfo(_scope->getScriptFunction(),_scope);
					_scope->m_scopeList.push_back(new_local_scope);
				}

				//CCompiler::getInstance()->insertPushScopeInstruction(new_local_scope);

				if((current = evalRecursive(current+1, m_line,error, new_local_scope, level_scope+1)) == NULL){
					return NULL;
				}
			}
			//
			// EVAL FOR IF-ELSE-FOR-WHILE
			//
			//------------------------------------------------------------------------------------------------------------------------------------------------

			// check whether the eval str ends with } or not.
			if(*(current-1) != '}'){ // closed claudator (from if/while/for ?)
				print_error_cr("Expected '}' at line %i",m_line);
				error = true;
				return NULL;
			}

			//------------------------------------------------------------------------------------------------------------------------------------------------
			//
			// GENERATE ASM FOR SWITCH
			//
			if(key_w->id == SWITCH_KEYWORD)
			{ // generata code for all conditionals...

				// design ...
				// jt case 1 or case 2
				// jt case 2
				// jt ...
				// jmp default
				// scope1:
				//...
				// jmp end
				// scope2:
				//...
				// jmp end
				// scope3:
				//...
				// jmp end
				// scope4:
				//...
				// jmp end
				// default:
				//... whatever
				// end:

				// Load symbol or value (0)...

				string error_str;
				bool ok=true;
				string type_value_condition;
				//CCompiler::getInstance()->insertLoadValueInstruction(condition.str,type_value_condition,_scope,condition.m_line);
				// get current index ...

				// create new statment...
				CCompiler::getInstance()->newStatment();

				print_info_cr("st:%i",CCompiler::getInstance()->getCurrentStatmentIndex());

				ok&=CCompiler::getInstance()->insertLoadValueInstruction(condition.str,type_value_condition,_scope,condition.m_line);

				// 0
				int condition_index  = CCompiler::getInstance()->getCurrentInstructionIndex();


				for(unsigned i = 0; i< info_switch.conditional_case.size();i++){ // for all cases...
					// 1...
					int case_index=CCompiler::getInstance()->getCurrentInstructionIndex();
					int last_compare_index=0;

					// other cases...

					// load case value ...
					ok&=CCompiler::getInstance()->insertLoadValueInstruction(info_switch.conditional_case[i].str_conditional_value,type_value_condition,_scope,condition.m_line);
					case_index++;

					// is equal ? == j+(4)
					ok&=CCompiler::getInstance()->insertOperatorInstruction("==",error_str, condition_index ,case_index);
					case_index++;



					if(!ok){
						print_error_cr("%s",error_str.c_str());
						error =true;
						return NULL;
					}
					info_switch.conditional_case[i].jt_asm = CCompiler::getInstance()->insert_JT_Instruction();
				}

				// insert a jmp to default st...
				info_switch.jmp_default_asm = CCompiler::getInstance()->insert_JMP_Instruction();
				char v_scope[1000];

				print_info_cr("st:%i",CCompiler::getInstance()->getCurrentStatmentIndex());

				// evaluate all case scope ...
				for(unsigned i = 0; i< info_switch.conditional_case.size();i++){ // for all cases...

					new_local_scope=new CScopeInfo(_scope->getScriptFunction(),_scope);
					_scope->m_scopeList.push_back(new_local_scope);
					// 1. generate asm scope.
					// link asm conditional jmp...
					info_switch.conditional_case[i].jt_asm->result_obj=(void*)(CCompiler::getInstance()->getCurrentStatmentIndex()+1);

					int size = info_switch.conditional_case[i].str_scope.str_end-info_switch.conditional_case[i].str_scope.str_begin;

					if(size >= (int)sizeof(v_scope)){
						error = true;
						print_error_cr("switch-case at line %i to long",info_switch.conditional_case[i].str_scope.m_begin_line);
						return NULL;
					}

					memset(v_scope,0,sizeof(v_scope));
					strncpy(v_scope,info_switch.conditional_case[i].str_scope.str_begin,size);
					if((evalRecursive(v_scope, m_line,error, new_local_scope, level_scope+1)) == NULL){
						return NULL;
					}

					// 2. generate jmp without statment.
					info_switch.conditional_case[i].jmp_asm=CCompiler::getInstance()->insert_JMP_Instruction();
					print_info_cr("st:%i",CCompiler::getInstance()->getCurrentStatmentIndex());

				}

				print_info_cr("st:%i",CCompiler::getInstance()->getCurrentStatmentIndex());

				info_switch.jmp_default_asm->result_obj=(void *)(CCompiler::getInstance()->getCurrentStatmentIndex()+1);

				// last, we evaluates the default scope...
				if(info_switch.str_default_case_scope.str_begin != NULL){

					new_local_scope=new CScopeInfo(_scope->getScriptFunction(),_scope);
					_scope->m_scopeList.push_back(new_local_scope);

					int size = info_switch.str_default_case_scope.str_end-info_switch.str_default_case_scope.str_begin;

						if(size >= (int)sizeof(v_scope)){
							error = true;
							print_error_cr("switch-defualt at line %i to long",info_switch.str_default_case_scope.m_begin_line);
							return NULL;
						}

					strncpy(v_scope,info_switch.str_default_case_scope.str_begin,size);
					if((evalRecursive(v_scope, m_line,error, new_local_scope, level_scope+1)) == NULL){
						return NULL;
					}
				}

				// link jmp cases to current instruction ...
				for(unsigned i = 0; i< info_switch.conditional_case.size();i++){
					info_switch.conditional_case[i].jmp_asm->result_obj=(void*)(CCompiler::getInstance()->getCurrentStatmentIndex()+1);
				}

				//

				// 0. if there's default then generate statment (is the last)...

				//

			}
			//
			// GENERATE ASM FOR SWITCH
			//
			//------------------------------------------------------------------------------------------------------------------------------------------------
			else
			//------------------------------------------------------------------------------------------------------------------------------------------------
			//
			// GENERATE ASM FOR IF-ELSE-FOR-WHILE
			//
			{
				bool if_else = false;
				bool check = key_w->id == FOR_KEYWORD || key_w->id == WHILE_KEYWORD || key_w->id == IF_KEYWORD;

				if(key_w->id == ELSE_KEYWORD){ // check whether last keyword was "if"
					if(key_w_last!=NULL){
						if_else=key_w_last->id==IF_KEYWORD;
						check=if_else;
					}else{
						check=false;
					}
				}

				if(check){

					if(key_w->id == FOR_KEYWORD && post_for.str != ";"){ // eval post
						if(evalRecursive(post_for.str.c_str(), post_for.m_line, error,for_scope, level_scope+1)==NULL){
							return NULL;
						}
					}

					if(key_w->id == FOR_KEYWORD || key_w->id == WHILE_KEYWORD || key_w->id == IF_KEYWORD ){ // set statment jmp instrucction

						jmp_asm=CCompiler::getInstance()->insert_JMP_Instruction();

						if(key_w->id == IF_KEYWORD){ // if "if" then update short jmp to next instruction...
							jmp_asm->result_obj = (void *)(CCompiler::getInstance()->getCurrentStatmentIndex()+1);
						}else{
							jmp_asm->result_obj = (void *)(index_st_jump);
						}

						jnt_asm->result_obj=(void *)(CCompiler::getInstance()->getCurrentStatmentIndex()+1); // +1 because ww want over pass the end of last statment ...
					}
					else{
						if(if_else){ // update jmp...
							if(jmp_asm!=NULL){
								jmp_asm->result_obj=(void *)(CCompiler::getInstance()->getCurrentStatmentIndex()+1);
							}else{
								print_error_cr("Error jmp_asm NULL");
								error = true;
								return NULL;
							}
						}


					}


					// set goto statment (if was if, overrides its jnt_asm...

				}else{
					print_error_cr("Unexpected keyword %s at line %i",key_w->str,m_line);
					error = true;
					return NULL;
				}
			}
			//
			// GENERATE ASM FOR IF-ELSE-FOR-WHILE
			//
			//------------------------------------------------------------------------------------------------------------------------------------------------

		}else if(*current == '}'){ // end scope...
			if(level_scope > 0){ // download scope level
				CCompiler::getInstance()->insertPopScopeInstruction();//_scope->getIndexLastInsertedLocalScope());
				return (current+1);
			}else{
				print_error_cr("Unexpected } at line %i",m_line);
				error = true;
				return NULL;
			}
		}
		else{ // try normal expressions (with or not var declaration)...

			char *advanced_expression=NULL;

			next=strstr(current,";");//getStatment(current);
			if(next ==0){
				print_error_cr("Expected ;");
				error = true;
				return NULL;
			}

			length=next-current;
			if(length>=MAX_STATMENT_LENGTH){
				print_error_cr("Max statment length!");
				error = true;
				return NULL;
			}

			memset(statment,0,sizeof(statment));
			strncpy(statment,current,(next-current));

			print_info_cr("eval:%s",statment);


			// advance current pointer...
			current=next+1; // ignore ;

			if(isVarDeclarationStatment(statment,error, &advanced_expression, m_line,_scope)){

				if(advanced_expression!=NULL){
					print_info_cr("eval expression %s",advanced_expression);
				}

				if(error) {
					return NULL;
				}
			}else{
				advanced_expression = statment;
			}

			if(error)
				return NULL;

			if(!CCompiler::getInstance()->compileExpression(advanced_expression, m_line,_scope->getScriptFunction(),_scope)){
				return NULL;
			}
		}
		// next statment...
		key_w_last = key_w;
		current=CStringUtils::IGNORE_BLANKS(current,m_line);
	}

	// the generating code has been terminated... let's execute it...
	//
	return current;

}*/

int getLineBeginScope(const string  & s, int m_scope){
	// PRE s: expression to be evaluated.

	int m_line = 1;
	char *current = (char *)s.c_str();
	int current_scope=0;

	while (*current != 0){

		if(*current == '\n') m_line ++;

		if(*current == '{'){

			current_scope++;
			if(current_scope==m_scope){
				return m_line;
			}
		}
		else if(*current == '}'){
			current_scope--;
		}

		current++;
	}

	return 0;

}
/*
bool CScopeInfo::eval (const string & s){
	int m_line = 1;
	bool error;
	return evalRecursive((const char *)s.c_str(), m_line,error,this) != NULL;
}
*/






CScopeInfo::~CScopeInfo(){
	for(map<string,tInfoScopeVar *>::iterator it = m_registeredVariable.begin();it!= m_registeredVariable.end();it++){
			delete it->second;
	}

	for(unsigned i = 0; i < m_scopeList.size(); i++){
		delete m_scopeList[i];
	}

}
