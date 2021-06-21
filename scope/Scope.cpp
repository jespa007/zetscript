/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100


namespace zetscript{

	//------------------------------------------------------------------------------------------------
	Scope::Scope(ZetScript * _zs,int idx_sf, Scope * _scope_parent, bool _is_c_node){
		scope_parent = _scope_parent;
		is_c_node = _is_c_node;
		script_class=NULL;
		idx_script_function=idx_sf;
		unusued=false;
		zs=_zs;
		tmp_idx_instruction_push_scope=ZS_IDX_UNDEFINED;
		scope_factory=_zs->getScopeFactory();
		registered_scopes=new zs_vector;
		registered_symbols=new zs_vector;

		if(_scope_parent == NULL){ // first node (it should be a class)...
			scope_base = this;
		}else{ // others...
			scope_base = scope_parent->scope_base;
			script_class=scope_parent->script_class; // propagate script class

			if(idx_script_function==ZS_IDX_UNDEFINED){ // May be is a block containing if-else, for, etc --> propagate current script function
				idx_script_function=scope_parent->idx_script_function;
			}
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

	int Scope::getIdxScriptFunction(){
		return idx_script_function;
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
		const char * file
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
	Symbol * Scope::registerSymbol(const char * file,short line, const std::string & symbol_name, char n_params, uint16_t check_repeated_symbols_direction){
		Symbol *p_irv=NULL;//idxAstNode=-1;// * irv;

		// check if you register a class...
		// check if symbol collides also with built in type...
		if(zs->getScriptClassFactory()->getBuiltinTypeOrClass(symbol_name) != ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR(file,line,"Symbol name '%s' is conflicting with a builtin type or class",symbol_name.c_str());
		}

		if((p_irv = getSymbol(symbol_name,n_params,check_repeated_symbols_direction))!=NULL){ // check whether symbol is already registered ...
			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				THROW_SCRIPT_ERROR(file,line," error symbol \"%s\" already registered at %s:%i", symbol_name.c_str(),p_irv->file,p_irv->line);
			}else{
				THROW_RUNTIME_ERROR(" error symbol \"%s\" already registered as C++", symbol_name.c_str());
			}
		}


		return addSymbol(file, line, symbol_name, n_params);
	}

	Symbol * Scope::getSymbol(const std::string & str_symbol, char n_params, uint16_t scope_direction){

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

		if(scope_direction&SCOPE_DIRECTION_DOWN){
			if(
					   this->scope_parent != NULL			 	 // it says that is the end of scopes
					&& this->scope_parent->getIdxScriptFunction() == idx_script_function // Only check repeated symbols in the same function scope context.
			){
				//uint16_t avoid_main=scope_direction & SCOPE_DIRECTION_AVOID_MAIN_SCOPE ? SCOPE_DIRECTION_AVOID_MAIN_SCOPE:0;
				return this->scope_parent->getSymbol(str_symbol,n_params,SCOPE_DIRECTION_DOWN);
			}
		}

		if(scope_direction&SCOPE_DIRECTION_UP){
			for(unsigned i = 0; i < registered_scopes->count; i++){
				Scope *s=(Scope *)registered_scopes->items[i];

				if(s->getIdxScriptFunction() == idx_script_function){ // Only check repeated symbols in the same function scope context.
					Symbol *sv=s->getSymbol(str_symbol,n_params,SCOPE_DIRECTION_UP);

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
