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


	void  writeError(const char *filename, int line, const  char  *string_text, ...);

	//int Scope::n_anonymouse_func=0;


	//------------------------------------------------------------------------------------------------

	Scope::Scope(ZetScript * _zs, short _idx_this, short _idx_parent, bool _is_c_node){//, int _index){
		idx_scope_parent = _idx_parent;
		idx_scope_ptr_current=ZS_IDX_UNDEFINED;
		idx_scope = _idx_this;
		is_c_node = _is_c_node;
		script_class=NULL;
		zs=_zs;
		scope_factory=_zs->getScopeFactory();


		if(_idx_parent == ZS_IDX_UNDEFINED){ // first node...

			idx_scope_base = _idx_this;
			idx_scope_ptr_current=_idx_this;
		}else{
			idx_scope_base = GET_SCOPE(this,_idx_parent)->getIdxScopeBase();
		}
	}

	void Scope::setScriptClass(ScriptClass *sc){
		if(idx_scope_parent != ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("set scriptclass must when scope is root");
			return;
		}

		script_class=sc;
	}

	ScriptClass * Scope::getScriptClass(){
		return GET_SCOPE(this,idx_scope_base)->script_class;
	}

	short Scope::getIdxScopeBase(){
		return idx_scope_base;
	}

	short Scope::getIdxScopeParent(){
		return idx_scope_parent;
	}

	short Scope::getIdxScopePtrCurrent(){
		return GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current;
	}

	Scope * Scope::getScopePtrCurrent(){
		return GET_SCOPE(this,GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current);
	}


	void Scope::resetScopePointer(){
		GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current = idx_scope_base;
	}

	Scope * Scope::pushScope(){

		Scope *new_scope = NEW_SCOPE(this,GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current);//, m_baseScope->incTotalScopes());
		GET_SCOPE(this,GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current)->idx_local_scopes.push_back(new_scope->idx_scope);
		GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current = new_scope->idx_scope;
		return new_scope;
	}

	Scope * Scope::popScope(){

		Scope *current_scope = GET_SCOPE(this,GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current);
		if(current_scope->idx_scope_parent != ZS_IDX_UNDEFINED){

			GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current = current_scope->idx_scope_parent;
			return GET_SCOPE(this,GET_SCOPE(this,idx_scope_base)->idx_scope_ptr_current);
		}

		return NULL;
	}

	std::vector<short> * Scope::getLocalScopeList(){

		return &idx_local_scopes;
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

			scope_symbols.push_back(irv);
			return irv;
		}else{

			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				writeError(file.c_str(),line," error var \"%s\" already registered at %s:%i", var_name.c_str(),p_irv->file.c_str(),p_irv->line);
			}else{
				writeError(NULL,0," error var \"%s\" already registered as C++", var_name.c_str());
			}

			THROW_SCRIPT_ERROR();
		}
		return NULL;
	}

	Symbol * Scope::getSymbolRecursiveDownScope(const std::string & str_symbol, char n_params){


		for(unsigned i = 0; i < scope_symbols.size(); i++){

			if(scope_symbols[i]->name==str_symbol && (scope_symbols[i]->n_params >= 0 && n_params>=0 ?(scope_symbols[i]->n_params == n_params):true)){
					return scope_symbols[i];//.idxScopeVar; // ptr scope ?
			}
		}

		int parent =  getIdxScopeParent();
		if(parent != ZS_IDX_UNDEFINED){
			return GET_SCOPE(this,parent)->getSymbolRecursiveDownScope(str_symbol,n_params);
		}

		return NULL;

	}

	Symbol * Scope::getSymbolRecursiveUpScope(const std::string & str_symbol, char n_params){
		// only blocks within functions...
		Symbol *sv;

		// for each variable in current scope ...
		for(unsigned i = 0; i < scope_symbols.size(); i++){

			if(scope_symbols[i]->name==str_symbol && (scope_symbols[i]->n_params >= 0 && n_params>=0 ?(scope_symbols[i]->n_params == n_params):true)){
				return scope_symbols[i];//.idxScopeVar; // ptr scope ?
			}
		}

		// ok lets iterate through current scope list
		for(unsigned i = 0; i < idx_local_scopes.size(); i++){
			Scope *s=GET_SCOPE(this,idx_local_scopes[i]);
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
		for(unsigned i = 0; i < scope_symbols.size(); i++){
			delete scope_symbols[i];
		}

		scope_symbols.clear();
	}
}
