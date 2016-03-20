#include "zg_script.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100

CUndefined *CScope::m_defaultVar=NULL;

void CScope::createSingletons(){
	m_defaultVar = new CUndefined();
}


void CScope::destroySingletons(){
	delete m_defaultVar;
}



CScope::CScope(CScriptFunction * _fs, CScope * _parent){
	m_parentScope = _parent;

	m_scriptFunction = _fs;

	if(_parent == NULL){
		m_mainScope = this;
	}else{
		m_mainScope = _parent->getMainScope();
	}
}

CScriptFunction * CScope::getScriptFunction(){
	return m_scriptFunction;
}


CScope * CScope::getMainScope(){
	return m_mainScope;
}

void CScope::addLocalScope(CScope *_ls){
	m_scopeList.push_back(_ls);
}

CScope * CScope::getParent(){
	return m_parentScope;
}

//-----------------------------------------------------------------------------------------------------------
//
// SCOPE VARIABLE MANAGEMENT
//
bool CScope::registerVariable(const string & var_name, int m_line){
	tInfoRegisteredVar * irv;
	if((irv = existRegisteredVariable(var_name))==NULL){ // check whether is local var registered scope ...
		irv = new tInfoRegisteredVar;
		irv->m_line = m_line;
		irv->m_obj = m_defaultVar;
		m_registeredVariable[var_name]=irv;
		return true;
	}else{
		print_error_cr("error var \"%s\" already registered at line %i!", var_name.c_str(), irv->m_line);
	}

	return false;
}

bool CScope::defineVariable(const string & var_name, CObject *obj){

	tInfoRegisteredVar * irv;
	if((irv = existRegisteredVariable(var_name))!=NULL){ // check whether is local var registered scope ...
		irv->m_obj=obj;
		irv->m_obj->setName(var_name);
		return true;
	}else{
		print_error_cr("error var \"%s\" is not registered!", var_name.c_str());
	}
	return false;
}

CScope::tInfoRegisteredVar * CScope::existRegisteredVariable(const string & var_name){
	if(m_registeredVariable.count(var_name)==0){ // not exit but we will deepth through parents ...
		CScope * parent =  getParent();
		if(parent != NULL){
			return parent->existRegisteredVariable(var_name);
		}
		return NULL;
	}else{
		return m_registeredVariable[var_name];
		//print_error_cr("variable %s already registered at line %i",var_name.c_str(),lc->m_registeredVariable[var_name]->m_line);
	}

	return NULL;

}


CObject *CScope::getRegisteredVariable(const string & var_name, bool print_msg){
	tInfoRegisteredVar * irv;
	if((irv = existRegisteredVariable(var_name))!=NULL){ // check whether is local var registered scope ...

		return irv->m_obj;
	}else{
		if(print_msg){
			print_error_cr("var \"%s\" doesn't exist!", var_name.c_str());
		}
	}

	return NULL;

}
//-----------------------------------------------------------------------------------------------------------

bool CScope::isVarDeclarationStatment(const char *statment, bool & error, char ** eval_expression,int & m_line, CScope * _localScope){
	// PRE: length(statment) < MAX_STATMENT_LENGTH
	char *aux = (char *)statment;

	//string var_name;
	//PASTNode expression;
	char *start_var,*end_var;
	*eval_expression=NULL;
	error=false;
	char stat[MAX_STATMENT_LENGTH]={0};
	string s_aux;
	int var_length;

	aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

	if(strncmp(aux,"var",3)==0){ // possible variable...
		aux+=3;

		if(*aux!=' '){ // is not var word...
			return NULL;
		}

		*eval_expression=aux;

		aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

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
			aux=CStringUtils::IGNORE_BLANKS(aux,m_line);

			if((*aux == 0 || *aux == '=')){

				var_length=end_var-start_var;

				strncpy(stat,start_var,var_length);
				print_info_cr("registered \"%s\" variable: ",stat);

				s_aux=stat;
				if(!_localScope->registerVariable(s_aux,m_line)){
					error=true;
				}

				return true;
			}
			else{
				print_error_cr("variable cannot cannot contain char '%c'",*aux);
				error=true;
			}

		}else{
			print_error_cr("variable cannot start with char %c",*aux);
			error=true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------------------------------------

enum TYPE_SPECIAL_OP{
	NONE=0,
	IF,
	ELSE,
	FOR,
	WHILE
};


struct{
	TYPE_SPECIAL_OP id;
	const char *str;
}str_op[]{
		{NONE, "None"},
		{IF,"if"},
		{ELSE,"else"},
		{FOR,"for"},
		{WHILE,"while"}
};



char * CScope::evalRecursive(const char *str_to_eval, int & m_line, CScope * _scope, int level_scope){

	char *current=(char *) str_to_eval, *begin_ptr, *end_ptr;
	string var_name;
	//int m_line=0;
	bool error;
	char statment[MAX_STATMENT_LENGTH];
	char condition[MAX_STATMENT_LENGTH], cond_for[MAX_STATMENT_LENGTH];
	char pre_for[MAX_STATMENT_LENGTH];
	char post_for[MAX_STATMENT_LENGTH];
	char *next;
	TYPE_SPECIAL_OP s_op;
	int length;
	//int goto_statment;


	CScope * new_local_scope = NULL,*for_scope=NULL;

	current=CStringUtils::IGNORE_BLANKS(current,m_line);

	while(*current!=0){
		s_op = NONE;
		//goto_statment = -1;

		// check if condition...
		if(strncmp(current,"if",2)==0) {// conditional detected...
			current+=2;
			s_op=IF;
		}
		else if(strncmp(current,"for",3)==0){  // check for condition...
			current+=3;
			s_op=FOR;
		}else if(strncmp(current,"while",5)==0){  // check while condition...
			current+=5;
			s_op=WHILE;
		}


		if(s_op != NONE){

			current=CStringUtils::IGNORE_BLANKS(current,m_line);
			if(*current == '('){
				current++;
			}else{
				print_error_cr("Expected '(' at line %i",m_line);
				return NULL;
			}

			// search for )
			next = strstr(current,")");


			if(next == NULL){
				print_error_cr("Expected ')' at line %i",m_line);
				return NULL;
			}

			length=next-current;
			if(length>=MAX_STATMENT_LENGTH){
				print_error_cr("Max statment length!");
				return NULL;
			}

			memset(condition,0,sizeof(statment));
			strncpy(condition,current,(next-current));

			if(s_op == FOR){

				_scope->m_scopeList.push_back(for_scope=new CScope(_scope->getScriptFunction(),_scope));
				CCompiler::getInstance()->insertPushScopeInstruction(for_scope);


				begin_ptr = condition;
				end_ptr = strstr(begin_ptr,";");
				if(end_ptr == NULL){
					print_error_cr("Expected ';' at line %i",m_line);
					return NULL;
				}

				length=end_ptr-begin_ptr;
				if(length>=MAX_STATMENT_LENGTH){
					print_error_cr("Max statment length!");
					return NULL;
				}
				memset(pre_for,0,sizeof(pre_for));
				strncpy(pre_for,begin_ptr,(end_ptr-begin_ptr));
				strcat(pre_for,";");

				// evaluating init for;
				if(evalRecursive(pre_for, m_line, for_scope, level_scope+1)==NULL){
					return NULL;
				}

				end_ptr++;
				begin_ptr = end_ptr;
				end_ptr = strstr(begin_ptr,";");
				if(end_ptr == NULL){
					print_error_cr("Expected ';' at line %i",m_line);
					return NULL;
				}

				length=end_ptr-begin_ptr;
				if(length>=MAX_STATMENT_LENGTH){
					print_error_cr("Max statment length!");
					return NULL;
				}

				memset(cond_for,0,sizeof(cond_for));
				strncpy(cond_for,begin_ptr,(end_ptr-begin_ptr));
				if(*cond_for == 0){
					strcpy(cond_for,"true");
				}


				memset(post_for,0,sizeof(post_for));
				if(*end_ptr!=0){
					strcpy(post_for,end_ptr+1);
				}

				strcpy(condition,cond_for);

			}

			current = next + 1;

			switch(s_op){
			default:
				print_error_cr("op not recognized at line %i",m_line);
				return NULL;
				break;
			case FOR:
				if(!CCompiler::getInstance()->compileExpression(condition, m_line,_scope->getScriptFunction(), for_scope)){
					return NULL;
				}
				break;
			case IF: // evaluate conditional...
			case WHILE:

				if(!CCompiler::getInstance()->compileExpression(condition, m_line,_scope->getScriptFunction(), _scope)){
					return NULL;
				}

				break;
			}

		}

		if(s_op!=NONE && *current != '{'){
			print_error_cr("Expected '{' on '%s' at line %s",str_op[s_op].str,m_line);
		}


		if(*current == '{'){ // begin scope --> inc scope level ...
			// add into localscope
			if(s_op == FOR) {
				new_local_scope = for_scope;
			}else{
				new_local_scope=new CScope(_scope->getScriptFunction(),_scope);
			}

			_scope->m_scopeList.push_back(new_local_scope);
			CCompiler::getInstance()->insertPushScopeInstruction(new_local_scope);

			if((current = evalRecursive(current+1, m_line, new_local_scope, level_scope+1)) == NULL){
				return NULL;
			}

			if(*(current-1) != '}'){ // closed claudator (from if/while/for ?)
				print_error_cr("Expected '}' at line %i",m_line);
				return NULL;
			}


				/*if(s_op == FOR){ //
					if(level_scope == 0){ // download scope level
						CCompiler::getInstance()->insertPopScopeInstruction();//_scope->getIndexLastInsertedLocalScope());
					}else{
						print_error_cr("Unclosed for at line %i ?",m_line);
						return NULL;
					}
				}
				else{
					print_info_cr("CLOSED CLAUDATOR FROM %i",s_op);
				}*/
			//}


		}else if(*current == '}'){ // end scope...
			if(level_scope > 0){ // download scope level
				CCompiler::getInstance()->insertPopScopeInstruction();//_scope->getIndexLastInsertedLocalScope());
				return (current+1);
			}else{
				print_error_cr("Unexpected } at line %i",m_line);
				return NULL;
			}
		}
		else{ // try normal expressions (with or not var declaration)...

			char *advanced_expression=NULL;
			next=strstr(current,";");//getStatment(current);
			if(next ==0){
				print_error_cr("Expected ;");
				return NULL;
			}

			length=next-current;
			if(length>=MAX_STATMENT_LENGTH){
				print_error_cr("Max statment length!");
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

			if(!CCompiler::getInstance()->compileExpression(advanced_expression, m_line,_scope->getScriptFunction(),_scope)){
				return NULL;
			}
		}




		// next statment...
		current=CStringUtils::IGNORE_BLANKS(current,m_line);

	}

	// the generating code has been terminated... let's execute it...
	//
	return current;

}

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

bool CScope::eval (const string & s){
	int m_line = 1;
	return evalRecursive((const char *)s.c_str(), m_line,this) != NULL;
}







CScope::~CScope(){

}
