/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100


namespace zetscript{

	//------------------------------------------------------------------------------------------------
	Scope::Scope(ZetScript * _zs, Scope * _scope_parent, bool _is_c_node){
		scope_parent = _scope_parent;
		is_c_node = _is_c_node;
		script_class=NULL;
		unusued=false;
		zs=_zs;
		tmp_idx_instruction_push_scope=ZS_IDX_UNDEFINED;
		scope_factory=_zs->getScopeFactory();
		registered_scopes=new zs_vector;
		registered_symbols=new zs_vector;

		if(_scope_parent == NULL){ // first node...
			scope_base = this;
		}else{
			scope_base = scope_parent->scope_base;
		}
	}

	void Scope::setScriptClass(ScriptClass *sc){
		if(scope_parent != NULL){
			THROW_RUNTIME_ERROR("Internal error setScriptclass scope_parent should NULL (i.e scope should be root)");
			return;
		}
		script_class=sc;
	}

	ScriptClass * Scope::getScriptClass(){
		return scope_base->script_class;
	}

	void						   Scope::markAsUnusued(){

		// link parent to its childs
		for(unsigned i=0;i < registered_scopes->count; i++){
			Scope *scope_child=(Scope *)registered_scopes->items[i];
			scope_child->scope_parent=scope_parent;
		}

		// mark as unused, late we can remove safely check unusued flag...
		unusued=true;
	}

	Symbol *  Scope::registerFunctionSymbol(const std::string & file,short line, const std::string & symbol_name, char n_params){
		Symbol *irv = new Symbol();
		irv->name = symbol_name;
		irv->file	 = file;
		irv->line 	 = line;
		irv->scope=  this;
		irv->n_params=n_params;

		registered_symbols->push_back((intptr_t)irv);
		return irv;
	}

	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//
	Symbol * Scope::registerSymbol(const std::string & file,short line, const std::string & symbol_name, char n_params){
		Symbol *p_irv=NULL;//idxAstNode=-1;// * irv;


		if((p_irv = getSymbol(symbol_name,n_params))!=NULL){ // check whether symbol is already registered ...
			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				THROW_SCRIPT_ERROR(file.c_str(),line," error symbol \"%s\" already registered at %s:%i", symbol_name.c_str(),p_irv->file.c_str(),p_irv->line);
			}else{
				THROW_RUNTIME_ERROR(" error symbol \"%s\" already registered as C++", symbol_name.c_str());
			}
		}

		Symbol *irv = new Symbol();
		irv->name = symbol_name;
		irv->file	 = file;
		irv->line 	 = line;
		irv->scope=  this;
		irv->n_params=n_params;

		registered_symbols->push_back((intptr_t)irv);
		return irv;
	}

	Symbol * Scope::getSymbolRecursiveDownScope(const std::string & str_symbol, char n_params){

		for(unsigned i = 0; i < registered_symbols->count; i++){
			Symbol *local_symbol = (Symbol *)registered_symbols->items[i];
			if(local_symbol->name==str_symbol && (n_params == NO_PARAMS_SYMBOL_ONLY?true:local_symbol->n_params == n_params)){
				return local_symbol;//.idxScopeVar; // ptr scope ?
			}
		}

		if(this->scope_parent != NULL){
			return this->scope_parent->getSymbolRecursiveDownScope(str_symbol,n_params);
		}

		return NULL;
	}

	Symbol * Scope::getSymbolRecursiveUpScope(const std::string & str_symbol, char n_params){
		// for each variable in current scope ...
		for(unsigned i = 0; i < registered_symbols->count; i++){
			Symbol *local_symbol=(Symbol *)registered_symbols->items[i];
			if(local_symbol->name==str_symbol && (local_symbol->n_params == n_params || n_params == NO_PARAMS_SYMBOL_ONLY)){
				return local_symbol;
			}
		}

		// ok lets iterate through current scope list
		for(unsigned i = 0; i < registered_scopes->count; i++){
			Scope *s=(Scope *)registered_scopes->items[i];
			Symbol *sv=s->getSymbolRecursiveUpScope(str_symbol,n_params);

			if(sv != NULL) return sv;
		}

		return NULL;//false;//-1;

	}

	Symbol * Scope::getSymbol(const std::string & var_name, char n_params, ScopeDirection scope_direction){
		//return getSymbolRecursive(var_name, n_params);
		Symbol *sv=NULL;


		if(scope_direction==ScopeDirection::SCOPE_DIRECTION_BOTH || scope_direction==ScopeDirection::SCOPE_DIRECTION_DOWN){
			sv=getSymbolRecursiveDownScope(var_name,n_params);
		}

		if(sv == NULL && (scope_direction==ScopeDirection::SCOPE_DIRECTION_BOTH || scope_direction==ScopeDirection::SCOPE_DIRECTION_UP)){
			sv=getSymbolRecursiveUpScope(var_name, n_params);
		}

		return sv;
	}

	//-----------------------------------------------------------------------------------------------------------
	Scope::~Scope(){

		// delete scope found
		/*for(unsigned i = 0; i < registered_scopes->count; i++){
			delete (Scope *)registered_scopes->items[i];
		}*/
		delete registered_scopes;
		registered_scopes=NULL;

		// delete local local_symbols found...
		for(unsigned i = 0; i < registered_symbols->count; i++){
			delete (Symbol *)registered_symbols->items[i];
		}
		delete registered_symbols;
		registered_symbols=NULL;
	}
}
