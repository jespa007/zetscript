/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

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


namespace zetscript{


	void  write_error(const char *filename, int line, const  char  *string_text, ...);

	//int CScope::n_anonymouse_func=0;


	//------------------------------------------------------------------------------------------------

	CScope::CScope(CZetScript * _zs, short _idx_this, short _idx_parent, bool _is_c_node){//, int _index){
		idxParentScope = _idx_parent;
		idxCurrentScopePointer=ZS_UNDEFINED_IDX;
		idxScope = _idx_this;
		is_c_node = _is_c_node;
		script_class=NULL;
		zs=_zs;
		scope_factory=_zs->getScopeFactory();


		if(_idx_parent == ZS_UNDEFINED_IDX){ // first node...

			idxBaseScope = _idx_this;
			idxCurrentScopePointer=_idx_this;
		}else{
			idxBaseScope = GET_SCOPE(_idx_parent)->getIdxBaseScope();
		}
	}

	void CScope::setScriptClass(CScriptClass *sc){
		if(idxParentScope != ZS_UNDEFINED_IDX){
			THROW_RUNTIME_ERROR("set scriptclass must when scope is root");
			return;
		}

		script_class=sc;
	}

	CScriptClass * CScope::getScriptClass(){
		return GET_SCOPE(idxBaseScope)->script_class;
	}

	short CScope::getIdxBaseScope(){
		return idxBaseScope;
	}

	short CScope::getIdxParent(){
		return idxParentScope;
	}

	short CScope::getIdxCurrentScopePointer(){
		return GET_SCOPE(idxBaseScope)->idxCurrentScopePointer;
	}

	CScope * CScope::getCurrentScopePointer(){
		return GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);
	}


	void CScope::resetScopePointer(){
		GET_SCOPE(idxBaseScope)->idxCurrentScopePointer = idxBaseScope;
	}

	CScope * CScope::pushScope(){

		CScope *new_scope = NEW_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);//, m_baseScope->incTotalScopes());
		GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer)->m_localScopeList.push_back(new_scope->idxScope);
		GET_SCOPE(idxBaseScope)->idxCurrentScopePointer = new_scope->idxScope;
		return new_scope;
	}

	CScope * CScope::popScope(){

		CScope *current_scope = GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);
		if(current_scope->idxParentScope != ZS_UNDEFINED_IDX){

			GET_SCOPE(idxBaseScope)->idxCurrentScopePointer = current_scope->idxParentScope;
			return GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);
		}

		return NULL;
	}

	std::vector<short> * CScope::getLocalScopeList(){

		return &m_localScopeList;
	}

	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//

	Symbol * CScope::registerSymbol(const std::string & file,short line,const std::string & var_name, char n_params){
		Symbol *p_irv=NULL;//idxAstNode=-1;// * irv;


		if((p_irv = getSymbol(var_name,n_params))==NULL){ // check whether is local var registered scope ...

			Symbol *irv = new Symbol();
			irv->name = var_name;
			irv->file	 = file;
			irv->line 	 = line;
			irv->idxScope=this->idxScope;
			irv->n_params=n_params;

			m_scopeSymbol.push_back(irv);
			return irv;
		}else{

			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				write_error(file.c_str(),line," error var \"%s\" already registered at %s:%i", var_name.c_str(),p_irv->file.c_str(),p_irv->line);
			}else{
				write_error(NULL,0," error var \"%s\" already registered as C++", var_name.c_str());
			}

			THROW_SCRIPT_ERROR();
		}
		return NULL;
	}

	Symbol * CScope::getSymbolRecursiveDownScope(const std::string & symbol_name, char n_params){


		for(unsigned i = 0; i < m_scopeSymbol.size(); i++){

			if(m_scopeSymbol[i]->name==symbol_name && (m_scopeSymbol[i]->n_params >= 0 && n_params>=0 ?(m_scopeSymbol[i]->n_params == n_params):true)){
					return m_scopeSymbol[i];//.idxScopeVar; // ptr scope ?
			}
		}

		int parent =  getIdxParent();
		if(parent != ZS_UNDEFINED_IDX){
			return GET_SCOPE(parent)->getSymbolRecursiveDownScope(symbol_name,n_params);
		}

		return NULL;

	}

	Symbol * CScope::getSymbolRecursiveUpScope(const std::string & symbol_name, char n_params){
		// only blocks within functions...
		Symbol *sv;

		// for each variable in current scope ...
		for(unsigned i = 0; i < m_scopeSymbol.size(); i++){

			if(m_scopeSymbol[i]->name==symbol_name && (m_scopeSymbol[i]->n_params >= 0 && n_params>=0 ?(m_scopeSymbol[i]->n_params == n_params):true)){
				return m_scopeSymbol[i];//.idxScopeVar; // ptr scope ?
			}
		}

		// ok lets iterate through current scope list
		for(unsigned i = 0; i < m_localScopeList.size(); i++){
			CScope *s=GET_SCOPE(m_localScopeList[i]);
			sv=s->getSymbolRecursiveUpScope(symbol_name,n_params);

			if(sv != NULL) return sv;
		}

		return NULL;//false;//-1;

	}

	Symbol * CScope::getSymbol(const std::string & var_name, char n_params){
		//return getSymbolRecursive(var_name, n_params);
		Symbol *sv;

		if((sv=getSymbolRecursiveDownScope(var_name,n_params))!=NULL){
					return sv;
		}
		return getSymbolRecursiveUpScope(var_name, n_params);
	}

	//-----------------------------------------------------------------------------------------------------------
	CScope::~CScope(){
		for(unsigned i = 0; i < m_scopeSymbol.size(); i++){
			delete m_scopeSymbol[i];
		}

		m_scopeSymbol.clear();
	}
}
