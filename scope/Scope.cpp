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
			script_class=scope_parent->script_class; // propagate script class
		}

		n_registered_symbols_as_variables=0;
		is_scope_function=false;
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

		// because no variables in there...
		if(scope_parent != NULL){
			for(unsigned i=0;i < registered_scopes->count; i++){
				Scope *current_scope=(Scope *)registered_scopes->items[i];
				scope_parent->registered_scopes->push_back((zs_int)current_scope);
				current_scope->scope_parent = scope_parent;
			}

			registered_scopes->clear();
		}

		// mark as unused, late we can remove safely check unusued flag...
		unusued=true;
	}

	Symbol *  Scope::addSymbol(
		const std::string & file
		,short line
		, const std::string & symbol_name
		, char n_params){
		Symbol *irv = new Symbol();
		irv->name = symbol_name;
		irv->file	 = file;
		irv->line 	 = line;
		irv->scope=  this;
		irv->n_params=n_params;

		if(irv->n_params == NO_PARAMS_SYMBOL_ONLY){
			n_registered_symbols_as_variables++;
		}

		registered_symbols->push_back((zs_int)irv);
		return irv;
	}

	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//
	Symbol * Scope::registerSymbol(const std::string & file,short line, const std::string & symbol_name, char n_params, ScopeDirection check_repeated_symbols_direction){
		Symbol *p_irv=NULL;//idxAstNode=-1;// * irv;

		if((p_irv = getSymbol(symbol_name,n_params,check_repeated_symbols_direction))!=NULL){ // check whether symbol is already registered ...
			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				THROW_SCRIPT_ERROR(file.c_str(),line," error symbol \"%s\" already registered at %s:%i", symbol_name.c_str(),p_irv->file.c_str(),p_irv->line);
			}else{
				THROW_RUNTIME_ERROR(" error symbol \"%s\" already registered as C++", symbol_name.c_str());
			}
		}

		return addSymbol(file, line, symbol_name, n_params);
	}

	Symbol * Scope::getSymbol(const std::string & str_symbol, char n_params, ScopeDirection scope_direction_repeated_symbols){

		Symbol *sv=NULL;

		// for each variable in current scope ...
		for(unsigned i = 0; i < registered_symbols->count; i++){
			sv=(Symbol *)registered_symbols->items[i];
			if(
				   ( sv->name == str_symbol )
			   &&  ( sv->n_params == n_params || n_params == NO_PARAMS_SYMBOL_ONLY )
			){
				return sv;
			}
		}

		if(scope_direction_repeated_symbols==ScopeDirection::SCOPE_DIRECTION_BOTH || scope_direction_repeated_symbols==ScopeDirection::SCOPE_DIRECTION_DOWN){
			if(
					this->scope_parent != NULL			 	 // it says that is the end of scopes
					&& this->scope_parent != MAIN_SCOPE(this) // avoid find symbols to global scope. If not found in local it will try link global on eval_pop_function
			){
				return this->scope_parent->getSymbol(str_symbol,n_params,ScopeDirection::SCOPE_DIRECTION_DOWN);
			}
		}

		if(scope_direction_repeated_symbols==ScopeDirection::SCOPE_DIRECTION_BOTH || scope_direction_repeated_symbols==ScopeDirection::SCOPE_DIRECTION_UP){
			for(unsigned i = 0; i < registered_scopes->count; i++){
				Scope *s=(Scope *)registered_scopes->items[i];

				if(s->is_scope_function == false){ // avoid local scope functions ...
					Symbol *sv=s->getSymbol(str_symbol,n_params,ScopeDirection::SCOPE_DIRECTION_UP);

					if(sv != NULL) {
						return sv;
					}
				}
			}
		}

		return NULL;
	}

	unsigned int Scope::numRegisteredSymbolsAsVariable(){
		unsigned int n_registered_symbols_as_variables=0;
		for(unsigned i=0; i < this->registered_symbols->count; i++){
			Symbol *symbol = (Symbol *)this->registered_symbols->items[i];

		}

		return n_registered_symbols_as_variables;

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
