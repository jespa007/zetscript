#include "../CZetScript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100
/*
//	 _____                        _____       __
//	/  ___|                      |_   _|     / _|
//	\ `--.  ___ ___  _ __   ___    | | _ __ | |_ ___
//	 `--. \/ __/ _ \| '_ \ / _ \   | || '_ \|  _/ _ \
//	/\__/ / (_| (_) | |_) |  __/  _| || | | | || (_) |
//	\____/ \___\___/| .__/ \___|  \___/_| |_|_| \___/
//               | |
//               |_|
// _________________________________________________
//  __________________________________
*/


vector<CScope *> 						* CScope::vec_scope_node=NULL;
int n_anonymouse_func=0;


void 						CScope::setVectorScopeNode(vector<CScope *> 	* set_vec){
	vec_scope_node = set_vec;
}


vector<CScope *> 	*		CScope::getVectorScopeNode(){
	return vec_scope_node;
}

CScope *	 CScope::newScope(CScope *parent_scope){
	CScope * scope_node = new CScope(parent_scope);
	vec_scope_node->push_back(scope_node);
	scope_node->idxScope = vec_scope_node->size()-1;
	return scope_node;
}

CScope 		* CScope::getScopeNodeByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= vec_scope_node->size()){
		print_error_cr("CScope node out of bound");
		return NULL;
	}

	return vec_scope_node->at(idx);
}
//------------------------------------------------------------------------------------------------
CScope::CScope(CScope * _parent){//, int _index){
	m_parentScope = _parent;
	//m_index = _index;
	m_currentScopePointer=NULL;
	//m_baseScope = this;
	idxScope = -1;

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

	CScope *new_scope = CScope::newScope(m_baseScope->m_currentScopePointer);//, m_baseScope->incTotalScopes());
	m_baseScope->m_currentScopePointer->m_localScopeList.push_back(new_scope->idxScope);
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

vector<int> * CScope::getLocalScopeList(){

	return &m_localScopeList;
}


//-----------------------------------------------------------------------------------------------------------
//
// SCOPE VARIABLE MANAGEMENT
//
tScopeVar * CScope::registerAnonymouseFunction(PASTNode ast){ // register anonymous function in the top of scope ...

	tScopeVar irv;// = new tScopeVar;


	string var_name = "_afun_"+CStringUtils::intToString(n_anonymouse_func++);
	string symbol_ref = "_"+var_name;

	irv.symbol_ref = symbol_ref;
	irv.name=var_name;
	irv.idxAstNode=-1;
	if(ast != NULL){
		irv.idxAstNode=ast->idxAstNode;
	}
	m_baseScope->m_registeredVariable.push_back(irv);

	return &m_baseScope->m_registeredVariable[m_baseScope->m_registeredVariable.size()-1];
}

tScopeVar * CScope::registerSymbol(const string & var_name, PASTNode ast){
	tScopeVar *p_irv=NULL;//idxAstNode=-1;// * irv;


	if((p_irv = existRegisteredSymbol(var_name))==NULL){ // check whether is local var registered scope ...

		string symbol_ref = var_name;
		symbol_ref = "_"+symbol_ref;
		tScopeVar irv;// = new tScopeVar;
		//irv->m_obj=NULL;

		irv.symbol_ref = symbol_ref;
		irv.name=var_name;
		irv.idxAstNode=-1;
		if(ast != NULL){
			irv.idxAstNode=ast->idxAstNode;
		}
		m_registeredVariable.push_back(irv);
		return &m_registeredVariable[m_registeredVariable.size()-1];// irv->idxScopeVar;
	}else{

		int m_line=-1;
		if(ast != NULL) {
			m_line = ast->line_value;
		}

		if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
			print_error_cr("(line %i): error var \"%s\" already registered at line %i!", m_line,var_name.c_str(), AST_LINE_VALUE(p_irv->idxAstNode));
		}else{
			print_error_cr("(line %i): error var \"%s\" already registered as C++!", m_line,var_name.c_str());
		}
	}
	return NULL;//false;//-1;
}

tScopeVar * CScope::existRegisteredSymbol(const string & var_name){

	string symbol_ref = var_name;
	symbol_ref = "_"+symbol_ref;

	for(unsigned i = 0; i < m_registeredVariable.size(); i++){
		if(m_registeredVariable[i].symbol_ref==symbol_ref){
			return &m_registeredVariable[i];//.idxScopeVar; // ptr scope ?
		}
	}

	CScope * parent =  getParent();
	if(parent != NULL){
		return parent->existRegisteredSymbol(var_name);
	}

	return NULL;//false;//-1;

}


tScopeVar * CScope::getInfoRegisteredSymbol(const string & v, bool print_msg){
	tScopeVar *irv = existRegisteredSymbol(v);
	if(irv == NULL && print_msg){
		print_error_cr("%s not exist",v.c_str());
	}

	return irv;
}

//-----------------------------------------------------------------------------------------------------------
CScope::~CScope(){

	//deleteScope(this);
	/*for(unsigned i = 0; i < m_scopeList.size(); i++){
		delete m_scopeList[i];
	}*/

}
