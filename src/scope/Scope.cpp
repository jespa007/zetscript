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
		registered_scopes=new std::vector<Scope *>();
		symbol_registered_variables=new std::vector<Symbol *>();
		symbol_registered_functions=new std::vector<Symbol *>();

		if(_scope_parent == NULL){ // first node (it should be a class)...
			scope_base = this;
		}else{ // others...
			scope_base = scope_parent->scope_base;
			script_class=scope_parent->script_class; // propagate script class

			if(idx_script_function==ZS_IDX_UNDEFINED){ // May be is a block containing if-else, for, etc --> propagate current script function
				idx_script_function=scope_parent->idx_script_function;
			}
		}

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
			for(unsigned i=0;i < registered_scopes->size(); i++){
				Scope *current_scope=(Scope *)registered_scopes->at(i);
				scope_parent->registered_scopes->push_back(current_scope);
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
		, char n_params
		){
		Symbol *irv = new Symbol();
		irv->name = symbol_name;
		irv->file	 = file;
		irv->line 	 = line;
		irv->scope=  this;
		irv->n_params=NO_PARAMS_SYMBOL_ONLY;

		if(irv->n_params == NO_PARAMS_SYMBOL_ONLY){
			symbol_registered_variables->push_back(irv);
		}else{
			symbol_registered_functions->push_back(irv);
		}
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
			THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Symbol name '%s' is conflicting with a builtin type or class",symbol_name.c_str());
		}

		if((p_irv = getSymbol(symbol_name,n_params,check_repeated_symbols_direction))!=NULL){ // check whether symbol is already registered ...
			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				if(p_irv->file == NULL || *p_irv->file==0){
					THROW_SCRIPT_ERROR_FILE_LINE(file,line," error symbol '%s' already registered", symbol_name.c_str());
				}else{
					THROW_SCRIPT_ERROR_FILE_LINE(file,line," error symbol '%s' already registered at %s:%i", symbol_name.c_str(),p_irv->file,p_irv->line);
				}
			}else{
				THROW_RUNTIME_ERROR(" error symbol '%s' already registered as C++", symbol_name.c_str());
			}
		}


		return addSymbol(file, line, symbol_name, n_params);
	}

	Symbol * Scope::getSymbol(const std::string & str_symbol, char n_params, uint16_t scope_direction){

		Symbol *sv=NULL;

		// for each variable in current scope ...
		for(unsigned i = 0; i < symbol_registered_variables->size(); i++){
			sv=(Symbol *)symbol_registered_variables->at(i);
			if(
				   ( sv->name == str_symbol )
			){
				return sv;
			}
		}

		for(unsigned i = 0; i < symbol_registered_functions->size(); i++){
			sv=(Symbol *)symbol_registered_variables->at(i);
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
			for(unsigned i = 0; i < registered_scopes->size(); i++){
				Scope *s=(Scope *)registered_scopes->at(i);

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

	bool Scope::unregisterSymbol(Symbol *symbol){
		Symbol *sv=NULL;
		for(unsigned i = 0; i < symbol_registered_functions->size(); i++){
			sv=(Symbol *)symbol_registered_functions->at(i);
			if(
			   ( sv == symbol )
			){
				delete sv;
				symbol_registered_functions->erase(symbol_registered_functions->begin()+i); // erase symbol scope
				return true;
			}
		}

		for(unsigned i = 0; i < symbol_registered_variables->size(); i++){
			sv=(Symbol *)symbol_registered_variables->at(i);
			if(
			   ( sv == symbol )
			){
				delete sv;
				symbol_registered_variables->erase(symbol_registered_functions->begin()+i); // erase symbol scope
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------------------
	Scope::~Scope(){

		// delete scope found
		delete registered_scopes;
		registered_scopes=NULL;

		// delete local local_symbols found...
		for(unsigned i = 0; i < symbol_registered_functions->size(); i++){
			delete (Symbol *)symbol_registered_functions->at(i);
		}

		for(unsigned i = 0; i < symbol_registered_variables->size(); i++){
			delete (Symbol *)symbol_registered_variables->at(i);
		}
		delete symbol_registered_variables;
		delete symbol_registered_functions;
		symbol_registered_functions=NULL;
		symbol_registered_variables=NULL;
	}
}
