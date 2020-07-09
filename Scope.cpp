/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

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


	void  WriteError(const char *filename, int line, const  char  *string_text, ...);

	//int Scope::n_anonymouse_func=0;


	//------------------------------------------------------------------------------------------------

	Scope::Scope(ZetScript * _zs, short _idx_this, short _idx_parent, bool _is_c_node){//, int _index){
		idxParentScope = _idx_parent;
		idxCurrentScopePointer=ZS_UNDEFINED_IDX;
		idx_scope = _idx_this;
		is_c_node = _is_c_node;
		script_class=NULL;
		zs=_zs;
		scope_factory=_zs->GetScopeFactory();


		if(_idx_parent == ZS_UNDEFINED_IDX){ // first node...

			idxBaseScope = _idx_this;
			idxCurrentScopePointer=_idx_this;
		}else{
			idxBaseScope = GET_SCOPE(_idx_parent)->getIdxBaseScope();
		}
	}

	void Scope::setScriptClass(ScriptClass *sc){
		if(idxParentScope != ZS_UNDEFINED_IDX){
			THROW_RUNTIME_ERROR("set scriptclass must when scope is root");
			return;
		}

		script_class=sc;
	}

	ScriptClass * Scope::getScriptClass(){
		return GET_SCOPE(idxBaseScope)->script_class;
	}

	short Scope::getIdxBaseScope(){
		return idxBaseScope;
	}

	short Scope::getIdxParent(){
		return idxParentScope;
	}

	short Scope::getIdxCurrentScopePointer(){
		return GET_SCOPE(idxBaseScope)->idxCurrentScopePointer;
	}

	Scope * Scope::getCurrentScopePointer(){
		return GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);
	}


	void Scope::resetScopePointer(){
		GET_SCOPE(idxBaseScope)->idxCurrentScopePointer = idxBaseScope;
	}

	Scope * Scope::pushScope(){

		Scope *new_scope = NEW_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);//, m_baseScope->incTotalScopes());
		GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer)->m_localScopeList.push_back(new_scope->idx_scope);
		GET_SCOPE(idxBaseScope)->idxCurrentScopePointer = new_scope->idx_scope;
		return new_scope;
	}

	Scope * Scope::popScope(){

		Scope *current_scope = GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);
		if(current_scope->idxParentScope != ZS_UNDEFINED_IDX){

			GET_SCOPE(idxBaseScope)->idxCurrentScopePointer = current_scope->idxParentScope;
			return GET_SCOPE(GET_SCOPE(idxBaseScope)->idxCurrentScopePointer);
		}

		return NULL;
	}

	std::vector<short> * Scope::getLocalScopeList(){

		return &m_localScopeList;
	}

	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//

	Symbol * Scope::registerSymbol(const std::string & file,short line,const std::string & var_name, char n_params){
		Symbol *p_irv=NULL;//idxAstNode=-1;// * irv;


		if((p_irv = getSymbol(var_name,n_params))==NULL){ // check whether is local var registered scope ...

			Symbol *irv = new Symbol();
			irv->name = var_name;
			irv->file	 = file;
			irv->line 	 = line;
			irv->idx_scope=this->idx_scope;
			irv->n_params=n_params;

			m_scopeSymbol.push_back(irv);
			return irv;
		}else{

			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				WriteError(file.c_str(),line," error var \"%s\" already registered at %s:%i", var_name.c_str(),p_irv->file.c_str(),p_irv->line);
			}else{
				WriteError(NULL,0," error var \"%s\" already registered as C++", var_name.c_str());
			}

			THROW_SCRIPT_ERROR();
		}
		return NULL;
	}

	Symbol * Scope::getSymbolRecursiveDownScope(const std::string & str_symbol, char n_params){


		for(unsigned i = 0; i < m_scopeSymbol.size(); i++){

			if(m_scopeSymbol[i]->name==str_symbol && (m_scopeSymbol[i]->n_params >= 0 && n_params>=0 ?(m_scopeSymbol[i]->n_params == n_params):true)){
					return m_scopeSymbol[i];//.idxScopeVar; // ptr scope ?
			}
		}

		int parent =  getIdxParent();
		if(parent != ZS_UNDEFINED_IDX){
			return GET_SCOPE(parent)->getSymbolRecursiveDownScope(str_symbol,n_params);
		}

		return NULL;

	}

	Symbol * Scope::getSymbolRecursiveUpScope(const std::string & str_symbol, char n_params){
		// only blocks within functions...
		Symbol *sv;

		// for each variable in current scope ...
		for(unsigned i = 0; i < m_scopeSymbol.size(); i++){

			if(m_scopeSymbol[i]->name==str_symbol && (m_scopeSymbol[i]->n_params >= 0 && n_params>=0 ?(m_scopeSymbol[i]->n_params == n_params):true)){
				return m_scopeSymbol[i];//.idxScopeVar; // ptr scope ?
			}
		}

		// ok lets iterate through current scope list
		for(unsigned i = 0; i < m_localScopeList.size(); i++){
			Scope *s=GET_SCOPE(m_localScopeList[i]);
			sv=s->getSymbolRecursiveUpScope(str_symbol,n_params);

			if(sv != NULL) return sv;
		}

		return NULL;//false;//-1;

	}

	Symbol * Scope::getSymbol(const std::string & var_name, char n_params){
		//return getSymbolRecursive(var_name, n_params);
		Symbol *sv;

		if((sv=getSymbolRecursiveDownScope(var_name,n_params))!=NULL){
					return sv;
		}
		return getSymbolRecursiveUpScope(var_name, n_params);
	}

	//-----------------------------------------------------------------------------------------------------------
	Scope::~Scope(){
		for(unsigned i = 0; i < m_scopeSymbol.size(); i++){
			delete m_scopeSymbol[i];
		}

		m_scopeSymbol.clear();
	}
}
