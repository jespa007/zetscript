#include "core/zg_core.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100

int n_anonymouse_func=0;


CScopeInfo::CScopeInfo(CScopeInfo * _parent){
	m_parentScope = _parent;

	//m_baseScope = this;


	if(_parent == NULL){
		m_baseScope = this;
	}else{
		m_baseScope = _parent->getBaseScope();
	}

	m_currentScopePointer=m_baseScope;
}

CScopeInfo * CScopeInfo::getBaseScope(){
	return m_baseScope;
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

void CScopeInfo::generateScopeListRecursive(CScopeInfo * scope, vector<CScopeInfo *> & lstScope){

	lstScope.push_back(scope);

	vector<CScopeInfo *> * scope_list = scope->getScopeList();
	for(unsigned i = 0; i < scope_list->size(); i++){
		generateScopeListRecursive(scope_list->at(i), lstScope);
	}

}

void CScopeInfo::generateScopeList(vector<CScopeInfo *> & lstScope){

	//return &m_baseScope->m_scopeList;

	generateScopeListRecursive(m_baseScope,lstScope);
}

vector<CScopeInfo *> * CScopeInfo::getScopeList(){

	return &m_baseScope->m_scopeList;
}

vector<tInfoScopeVar *> * CScopeInfo::getRegisteredSymbolsList(){
	return &m_baseScope->m_registeredVariable;
}
//-----------------------------------------------------------------------------------------------------------
int CScopeInfo::getScopeIndex(CScopeInfo * scope_to_find){
	int index=0;
	return CScopeInfo::getScopeIndexRecursive(scope_to_find->getBaseScope(),scope_to_find, index);
}
//-----------------------------------------------------------------------------------------------------------
int CScopeInfo::getScopeIndexRecursive(CScopeInfo * current_scope, CScopeInfo * scope_to_find, int & _index){

	int r=0;
	if(current_scope == scope_to_find){
		return _index;
	}
	vector<CScopeInfo *> * scope_list = current_scope->getScopeList();

	for(unsigned i = 0; i < scope_list->size(); i++){
		_index++;
		if((r=getScopeIndexRecursive(scope_list->at(i), scope_to_find,_index))!=-1){
			return r;
		}

	}

	return -1;
}

//-----------------------------------------------------------------------------------------------------------
//
// SCOPE VARIABLE MANAGEMENT
//
tInfoScopeVar * CScopeInfo::registerAnonymouseFunction(PASTNode ast){ // register anonymous function in the top of scope ...

	tInfoScopeVar * irv = new tInfoScopeVar;


	string var_name = "_afun_"+CStringUtils::intToString(n_anonymouse_func++);
	string symbol_ref = "_"+var_name;


	irv->symbol_ref = symbol_ref;
	irv->name=var_name;
	irv->ast=ast;
	m_baseScope->m_registeredVariable.push_back(irv);

	return irv;
}

tInfoScopeVar * CScopeInfo::registerSymbol(const string & var_name, PASTNode ast){
	tInfoScopeVar * irv;


	if((irv = existRegisteredSymbol(var_name))==NULL){ // check whether is local var registered scope ...

		string symbol_ref = var_name;
		symbol_ref = "_"+symbol_ref;
		irv = new tInfoScopeVar;
		//irv->m_obj=NULL;

		irv->symbol_ref = symbol_ref;
		irv->name=var_name;
		irv->ast=ast;
		m_registeredVariable.push_back(irv);
		return irv;
	}else{

		int m_line=-1;
		if(ast != NULL) {
			m_line = ast->definedValueline;

		}

		if(irv->ast != NULL) { // if not null is defined in script scope, else is C++ var
			print_error_cr("(line %i): error var \"%s\" already registered at line %i!", m_line,var_name.c_str(), irv->ast->definedValueline);
		}else{
			print_error_cr("(line %i): error var \"%s\" already registered as C++!", m_line,var_name.c_str());
		}
	}
	return NULL;
}

tInfoScopeVar * CScopeInfo::existRegisteredSymbol(const string & var_name){

	string symbol_ref = var_name;
	symbol_ref = "_"+symbol_ref;

	for(unsigned i = 0; i < m_registeredVariable.size(); i++){
		if(m_registeredVariable[i]->symbol_ref==symbol_ref){
			return m_registeredVariable[i];
		}
	}

	CScopeInfo * parent =  getParent();
	if(parent != NULL){
		return parent->existRegisteredSymbol(var_name);
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
CScopeInfo::~CScopeInfo(){

	for(unsigned i = 0; i < m_registeredVariable.size(); i++){
		delete m_registeredVariable[i];
	}

	/*for(unsigned i = 0; i < m_scopeList.size(); i++){
		delete m_scopeList[i];
	}*/

}
