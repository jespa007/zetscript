#include "../../CZetScript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100

int n_anonymouse_func=0;



//------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------

CScope::CScope(CScope * _parent){//, int _index){
	m_parentScope = _parent;
	//m_index = _index;
	m_currentScopePointer=NULL;
	//m_baseScope = this;


	if(_parent == NULL){
		m_baseScope = this;
		m_currentScopePointer=this;
		//totalScopes = 0;
	//	m_index = 0;
	}else{
		m_baseScope = _parent->getBaseScope();
		//m_baseScope->getScopeList()->push_back(this);
		//m_index = m_baseScope->getScopeList()->size()-1;
	}





}
/*
int CScope::incTotalScopes(){
	totalScopes++;
	return totalScopes;
}
*/
CScope * CScope::getBaseScope(){
	return m_baseScope;
}



CScope * CScope::getParent(){
	return m_parentScope;
}

CScope * CScope::getCurrentScopePointer(){
	return m_baseScope->m_currentScopePointer;
}

void CScope::resetScopePointer(){
	m_baseScope->m_currentScopePointer = m_baseScope;
}

CScope * CScope::pushScope(){



	CScope *new_scope = CScopeFactory::newScope(m_baseScope->m_currentScopePointer);//, m_baseScope->incTotalScopes());
	m_baseScope->m_currentScopePointer->m_scopeList.push_back(new_scope);
	m_baseScope->m_currentScopePointer = new_scope;
	return m_baseScope->m_currentScopePointer;

}

CScope * CScope::popScope(){

	if(m_baseScope->m_currentScopePointer->m_parentScope != NULL){
		m_currentScopePointer = m_baseScope->m_currentScopePointer->m_parentScope;
		return m_baseScope->m_currentScopePointer;
	}

	return NULL;
}

/*void CScope::generateScopeListRecursive(CScope * scope, vector<CScope *> & lstScope){

	lstScope.push_back(scope);

	vector<CScope *> * scope_list = scope->getScopeList();
	for(unsigned i = 0; i < scope_list->size(); i++){
		generateScopeListRecursive(scope_list->at(i), lstScope);
	}

}

void CScope::generateScopeList(vector<CScope *> & lstScope){

	//return &m_baseScope->m_scopeList;

	generateScopeListRecursive(m_baseScope,lstScope);
}

int          CScope::getIndex(){
	return m_index;
}*/

vector<CScope *> * CScope::getScopeList(){

	return &m_scopeList;
}
/*
vector<CScope *> * CScope::getVecIndexScopeNode(){

	return vec_index_scope_node;
}

vector<tScopeVar *> * CScope::getRegisteredVariableList(){
	return &m_registeredVariable;
}*/

vector<tScopeVar *> * CScope::getRegisteredSymbolsList(){
	return m_baseScope->m_registeredVariable;
}

void CScope::destroySingletons(){
	/*if(vec_index_scope_node != NULL){
		delete vec_index_scope_node;
		delete vec_index_registered_var;
	}*/
}
/*
//-----------------------------------------------------------------------------------------------------------
int CScope::getScopeIndex(){

	return m_index;
	//int index=0;
	//return CScope::getScopeIndexRecursive(scope_to_find->getBaseScope(),scope_to_find, index);
}*/
//-----------------------------------------------------------------------------------------------------------
/*int CScope::getScopeIndexRecursive(CScope * current_scope, CScope * scope_to_find, int & _index){

	int r=0;
	if(current_scope == scope_to_find){
		return _index;
	}
	vector<CScope *> * scope_list = current_scope->getScopeList();

	for(unsigned i = 0; i < scope_list->size(); i++){
		_index++;
		if((r=getScopeIndexRecursive(scope_list->at(i), scope_to_find,_index))!=-1){
			return r;
		}

	}

	return -1;
}*/

//-----------------------------------------------------------------------------------------------------------
//
// SCOPE VARIABLE MANAGEMENT
//
tScopeVar * CScope::registerAnonymouseFunction(PASTNode ast){ // register anonymous function in the top of scope ...

	tScopeVar * irv = new tScopeVar;


	string var_name = "_afun_"+CStringUtils::intToString(n_anonymouse_func++);
	string symbol_ref = "_"+var_name;


	irv->symbol_ref = symbol_ref;
	irv->name=var_name;
	irv->idxAstNode=-1;
	if(ast != NULL){
		irv->idxAstNode=ast->idxAstNode;
	}
	m_baseScope->m_registeredVariable.push_back(irv);

	return irv;
}

bool CScope::registerSymbol(const string & var_name, PASTNode ast){
	int idxAstNode=-1;// * irv;


	if((idxAstNode = existRegisteredSymbol(var_name))==-1){ // check whether is local var registered scope ...

		string symbol_ref = var_name;
		symbol_ref = "_"+symbol_ref;
		tScopeVar * irv = new tScopeVar;
		//irv->m_obj=NULL;

		irv->symbol_ref = symbol_ref;
		irv->name=var_name;
		irv->idxAstNode=-1;
		if(ast != NULL){
			irv->idxAstNode=ast->idxAstNode;
		}
		m_registeredVariable.push_back(irv);
		return true;//irv->idxScopeVar;
	}else{

		int m_line=-1;
		if(ast != NULL) {
			m_line = ast->line_value;

		}

		if(idxAstNode != -1) { // if not null is defined in script scope, else is C++ var
			print_error_cr("(line %i): error var \"%s\" already registered at line %i!", m_line,var_name.c_str(), AST_LINE_VALUE(idxAstNode));
		}else{
			print_error_cr("(line %i): error var \"%s\" already registered as C++!", m_line,var_name.c_str());
		}
	}
	return false;//-1;
}

bool CScope::existRegisteredSymbol(const string & var_name){

	string symbol_ref = var_name;
	symbol_ref = "_"+symbol_ref;

	for(unsigned i = 0; i < m_registeredVariable.size(); i++){
		if(m_registeredVariable[i]->symbol_ref==symbol_ref){
			return true;//m_registeredVariable[i].idxScopeVar; // ptr scope ?
		}
	}

	CScope * parent =  getParent();
	if(parent != NULL){
		return parent->existRegisteredSymbol(var_name);
	}

	return false;//-1;

}

/*
int CScope::getInfoRegisteredSymbol(const string & var_name, bool print_msg){
	int idxScopeVar=-1;// * irv;

	if((idxScopeVar = existRegisteredSymbol(var_name))!=-1){ // check whether is local var registered scope ...

		return idxScopeVar;
	}else{
		if(print_msg){
			print_error_cr("var \"%s\" doesn't exist!", var_name.c_str());
		}
	}
	return -1;
}*/
/*
void CScope::deleteScopeRecursive(CScope *scope_info){

	for(unsigned i = 0; i < scope_info->getRegisteredVariableList()->size(); i++){
		delete scope_info->getRegisteredVariableList()->at(i);
	}


	for(unsigned i = 0; i < scope_info->getScopeList()->size(); i++){

		deleteScopeRecursive(scope_info->getScopeList()->at(i));
		delete scope_info->getScopeList()->at(i);
	}

}

void CScope::destroyScopes(){
	//deleteScopeRecursive(this);
}
*/
//-----------------------------------------------------------------------------------------------------------
CScope::~CScope(){

	//deleteScope(this);
	/*for(unsigned i = 0; i < m_scopeList.size(); i++){
		delete m_scopeList[i];
	}*/

}
