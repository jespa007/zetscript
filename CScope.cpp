/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

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


	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);

	int CScope::n_anonymouse_func=0;


	//------------------------------------------------------------------------------------------------

	CScope::CScope( short _idx_this, short _idx_parent, bool _is_c_node){//, int _index){
		idxParentScope = _idx_parent;
		idxCurrentScopePointer=ZS_UNDEFINED_IDX;
		idxScope = _idx_this;
		is_c_node = _is_c_node;
		script_class=NULL;


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

	vector<short> * CScope::getLocalScopeList(){

		return &m_localScopeList;
	}

	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//
	tScopeVar * CScope::registerAnonymouseFunction(const string & file, int line, int n_args){ // register anonymous function in the top of scope ...

		tScopeVar irv;// = new tScopeVar;
		string var_name ="_afun"+CZetScriptUtils::intToString(n_anonymouse_func++);
		string symbol_ref = "@funp"+CZetScriptUtils::intToString(n_args)+"_"+var_name;



		irv.symbol_ref = symbol_ref;
		irv.file = file;
		irv.line = line;
		irv.idxScope=IDX_GLOBAL_SCOPE;

		CScope *base = GET_SCOPE(idxBaseScope);

		base->m_scopeVariable.push_back(irv);

		return &base->m_scopeVariable[base->m_scopeVariable.size()-1];
	}

	tScopeVar * CScope::registerSymbol(const string & file,int line,const string & var_name, int n_params){
		tScopeVar *p_irv=NULL;//idxAstNode=-1;// * irv;


		if((p_irv = existRegisteredSymbol(var_name,n_params))==NULL){ // check whether is local var registered scope ...

			string symbol_ref = "";


			if(n_params>=0){
				symbol_ref="@funp"+CZetScriptUtils::intToString(n_params)+"_"+var_name;
			}
			else{
				symbol_ref="@var_"+var_name;
			}

			tScopeVar irv;
			irv.symbol_ref = symbol_ref;
			irv.file	 = file;
			irv.line 	 = line;
			irv.idxScope=this->idxScope;

			m_scopeVariable.push_back(irv);
			return &m_scopeVariable[m_scopeVariable.size()-1];// irv->idxScopeVar;
		}else{

			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				writeErrorMsg(file.c_str(),line," error var \"%s\" already registered at %s:%i", var_name.c_str(),p_irv->file.c_str(),p_irv->line);
			}else{
				writeErrorMsg(NULL,0," error var \"%s\" already registered as C++", var_name.c_str());
			}
		}
		return NULL;//false;//-1;
	}

	tScopeVar * CScope::existRegisteredSymbolRecursiveDownScope(const string & symbol_ref, int n_params){


		for(unsigned i = 0; i < m_scopeVariable.size(); i++){
			string current_symbol_ref=m_scopeVariable[i].symbol_ref;
			if(n_params==NO_PARAMS_SYMBOL_ONLY){
				current_symbol_ref=CEval::getSymbolNameFromSymbolRef(current_symbol_ref);
			}

			if(current_symbol_ref==symbol_ref){
				return &m_scopeVariable[i];//.idxScopeVar; // ptr scope ?
			}
		}

		int parent =  getIdxParent();
		if(parent != ZS_UNDEFINED_IDX){
			return GET_SCOPE(parent)->existRegisteredSymbolRecursiveDownScope(symbol_ref,n_params);
		}

		return NULL;//false;//-1;

	}

	tScopeVar * CScope::existRegisteredSymbolRecursiveUpScope(const string & symbol_ref, int n_params){
		// only blocks within functions...
		tScopeVar *sv=NULL;

		// for each variable in current scope ...
		for(unsigned i = 0; i < m_scopeVariable.size(); i++){

			string current_symbol_ref=m_scopeVariable[i].symbol_ref;
			if(n_params==NO_PARAMS_SYMBOL_ONLY){
				current_symbol_ref=CEval::getSymbolNameFromSymbolRef(current_symbol_ref);
			}

			if(current_symbol_ref==symbol_ref){
				return &m_scopeVariable[i];//.idxScopeVar; // ptr scope ?
			}
		}

		// ok lets iterate through current scope list
		for(unsigned i = 0; i < m_localScopeList.size(); i++){
			CScope *s=GET_SCOPE(m_localScopeList[i]);
			sv=s->existRegisteredSymbolRecursiveUpScope(symbol_ref,n_params);

			if(sv != NULL) return sv;
		}

		return NULL;//false;//-1;

	}

	tScopeVar * CScope::existRegisteredSymbolRecursive(const string & var_name, int n_params){

		string symbol_ref = "";
		tScopeVar *sv=NULL;

		if(n_params>=0){
			symbol_ref="@funp"+CZetScriptUtils::intToString(n_params)+"_"+var_name;
		}
		else{
			if(n_params==NO_PARAMS_IS_VARIABLE){
				symbol_ref="@var_"+var_name;
			}
			else{ // else only symbol...
				symbol_ref=var_name;
			}
		}

		sv=existRegisteredSymbolRecursiveDownScope(symbol_ref,n_params);
		if(sv!=NULL) return sv;

		return existRegisteredSymbolRecursiveUpScope(symbol_ref,n_params);

	}

	tScopeVar * CScope::existRegisteredSymbol(const string & var_name, int n_params){
		return existRegisteredSymbolRecursive(var_name, n_params);
	}

	tScopeVar * CScope::getInfoRegisteredSymbol(const string & v, int n_params, bool print_msg){
		tScopeVar *irv = existRegisteredSymbol(v,n_params);
		if(irv == NULL && print_msg){
			THROW_RUNTIME_ERROR("%s not exist",v.c_str());
			return NULL;
		}

		return irv;
	}

	//-----------------------------------------------------------------------------------------------------------
	CScope::~CScope(){

	}
}
