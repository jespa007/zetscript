/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100


namespace zetscript{

	//------------------------------------------------------------------------------------------------
	Scope::Scope(ZetScript * _zs,int _idx_script_function, Scope * _scope_parent, uint16_t _properties){
		scope_parent = _scope_parent;
		properties = _properties;
		script_type_owner=NULL;
		idx_script_function=_idx_script_function;
		zs=_zs;
		tmp_idx_instruction_push_scope=ZS_IDX_UNDEFINED;
		scope_factory=_zs->getScopeFactory();
		scopes=new zs_vector;
		symbol_variables=new zs_vector;
		symbol_functions=new zs_vector;
		symbol_types=new zs_vector;

		if(_scope_parent == NULL){ // first node (it should be a class)...
			scope_base = this;
		}else{ // others...
			scope_base = scope_parent->scope_base;
			script_type_owner=scope_parent->script_type_owner; // propagate script class

			if(idx_script_function==ZS_IDX_UNDEFINED){ // May be is a block containing if-else, for, etc --> propagate current script function
				idx_script_function=scope_parent->idx_script_function;
			}
		}
	}

	void Scope::setScriptTypeOwner(ScriptType *_script_type_owner){
		if(scope_parent != NULL){
			THROW_RUNTIME_ERRORF("Internal error setScriptclass scope_parent should NULL (i.e scope should be root)");
			return;
		}
		script_type_owner=_script_type_owner;
	}

	ScriptType * Scope::getScriptTypeOwner(){
		return scope_base->script_type_owner;
	}

	int Scope::getIdxScriptFunction(){
		return idx_script_function;
	}

	int Scope::numInnerScopesRecursive(Scope *sc, int n){

		if(sc->scope_parent!=NULL){
			return numInnerScopesRecursive(sc->scope_parent, n+1);
		}

		return n;
	}

	void Scope::removeChildrenBlockTypes(){
		for(int i=0; i < this->scopes->count; i++){
			markBlockScopeAsUnusuedScopesRecursive((Scope *)this->scopes->items[i]);
		}

		this->scope_factory->clearUnusuedScopes();
	}

	void Scope::markBlockScopeAsUnusuedScopesRecursive(Scope *_sc){

		if(_sc==NULL){
			return;
		}

		// if scope is block the consecutive are blocks check node
		if(_sc->properties & SCOPE_PROPERTY_IS_SCOPE_BLOCK){
			_sc->properties|=SCOPE_PROPERTY_UNUSUED;

			// check its
			for(int i=0; i < _sc->scopes->count;i++){
				Scope *_child=(Scope *)_sc->scopes->items[i];
				if((_sc->properties & SCOPE_PROPERTY_IS_SCOPE_BLOCK)==0){
					throw std::runtime_error("expected scope as block type");
				}
				markBlockScopeAsUnusuedScopesRecursive(_child);
			}
		}
	}


	int Scope::numInnerScopes(){
		return numInnerScopesRecursive(this,1);
	}

	int Scope::countVariables(bool _recursive){
		if(_recursive){
			return Scope::countVariablesRecursive(this,this->idx_script_function);
		}

		return this->symbol_variables->count;

	}

	int Scope::countVariablesRecursive(Scope *_sc, int idx_script_function_reference){
		int n_total=0;
		if(_sc->idx_script_function==idx_script_function_reference){ // only count variables in the scope of the function
			n_total=_sc->symbol_variables->count;

			for(int i=0; i < _sc->scopes->count; i++){
				n_total+=Scope::countVariablesRecursive((Scope *)_sc->scopes->items[i],idx_script_function_reference);
			}
		}

		return n_total;
	}


	void						   Scope::markAsUnusued(){

		// because no variables in there it moves all scopes in the parent ...
		if(scope_parent != NULL){
			for(int i=0;i < scopes->count; i++){
				Scope *current_scope=(Scope *)scopes->items[i];
				scope_parent->scopes->push_back((zs_int)current_scope);
				current_scope->scope_parent = scope_parent;
			}

			scopes->clear();
		}

		// mark as unused, late we can remove safely check unusued flag...
		this->properties|=SCOPE_PROPERTY_UNUSUED;
	}

	void Scope::checkPreRegisterSymbol(const char * file,short line, const zs_string & symbol_name, char n_params, uint16_t check_repeated_symbols_direction){
		Symbol *p_irv=NULL;

		// check if you register a class...
		// check if symbol collides also with built in type...
		if((check_repeated_symbols_direction & REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS)==0){
			if(zs->getScriptTypeFactory()->getIdxScriptType(symbol_name) != ZS_IDX_UNDEFINED){
				THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot name symbol as '%s' because is a reserved builtin-type or defined class",symbol_name.c_str());
			}
		}

		if((p_irv = getSymbol(symbol_name,n_params,check_repeated_symbols_direction))!=NULL){ // check whether symbol is already registered ...
			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				if(p_irv->file == NULL || *p_irv->file==0){
					THROW_SCRIPT_ERROR_FILE_LINE(file,line," error symbol '%s' already registered", symbol_name.c_str());
				}else{
					THROW_SCRIPT_ERROR_FILE_LINE(file,line," error symbol '%s' already registered at '%s:%i'", symbol_name.c_str(),p_irv->file,p_irv->line);
				}
			}else{
				THROW_RUNTIME_ERROR(" error symbol '%s' already registered as C++", symbol_name.c_str());
			}
		}
	}

	Symbol * Scope::registerSymbolScriptType(const char * _file,short _line, const zs_string & _symbol_name, uint16_t _check_repeated_symbols_direction){

		checkPreRegisterSymbol(_file, _line, _symbol_name,  NO_PARAMS_SYMBOL_ONLY,_check_repeated_symbols_direction);

		Symbol *symbol 		= new Symbol();
		symbol->idx_position=symbol_types->count;
		symbol->name 		= _symbol_name;
		symbol->file	 	= _file;
		symbol->line 	 	= _line;
		symbol->scope		=  this;
		symbol->properties |=SYMBOL_PROPERTY_TYPE;

		symbol_types->push_back((zs_int)symbol);

		return symbol;

	}

	Symbol * Scope::registerSymbolVariable(const char * _file,short _line, const zs_string & _symbol_name ,uint16_t _check_repeated_symbols_direction){

		checkPreRegisterSymbol(_file, _line, _symbol_name,  NO_PARAMS_SYMBOL_ONLY,_check_repeated_symbols_direction);

		Symbol *symbol 		= new Symbol();
		symbol->idx_position=symbol_variables->count;
		symbol->name 		= _symbol_name;
		symbol->file	 	= _file;
		symbol->line 	 	= _line;
		symbol->scope		=  this;

		symbol_variables->push_back((zs_int)symbol);

		return symbol;
	}

	Symbol * Scope::registerSymbolFunction(const char * _file,short _line, const zs_string & _symbol_name, char _n_params, uint16_t _check_repeated_symbols_direction){
		if((_check_repeated_symbols_direction & REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS)==0){
			checkPreRegisterSymbol(_file, _line, _symbol_name,  _n_params,_check_repeated_symbols_direction);
		}

		Symbol *symbol 		= new Symbol();
		symbol->idx_position=symbol_functions->count;
		symbol->name 		= _symbol_name;
		symbol->file	 	= _file;
		symbol->line 	 	= _line;
		symbol->scope		=  this;
		symbol->n_params	=_n_params;
		symbol->properties |=SYMBOL_PROPERTY_FUNCTION;

		symbol_functions->push_back((zs_int)symbol);

		return symbol;
	}


	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//

	Symbol * Scope::getSymbol(const zs_string & str_symbol, char n_params, uint16_t scope_direction){

		// for each variable in current scope ...
		for(int i = 0; i < symbol_types->count; i++){
			Symbol *sv=(Symbol *)symbol_types->items[i];
			if(
				   ( sv->name == str_symbol )
			){
				return sv;
			}
		}

		for(int i = 0; i < symbol_variables->count; i++){
			Symbol *sv=(Symbol *)symbol_variables->items[i];
			if(
				   ( sv->name == str_symbol )
			){
				return sv;
			}
		}

		for(int i = 0; i < symbol_functions->count; i++){
			Symbol *sv=(Symbol *)symbol_functions->items[i];
			if(
				   ( sv->name == str_symbol )
			   &&  ( sv->n_params == n_params || n_params==NO_PARAMS_SYMBOL_ONLY )
			){
				return sv;
			}
		}


		if(scope_direction&REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN){
			if(
					   this->scope_parent != NULL			 	 // it says that is the end of scopes
					&& this->scope_parent->getIdxScriptFunction() == idx_script_function // Only check repeated symbols in the same function scope context.
			){
				//uint16_t avoid_main=scope_direction & SCOPE_DIRECTION_AVOID_MAIN_SCOPE ? SCOPE_DIRECTION_AVOID_MAIN_SCOPE:0;
				return this->scope_parent->getSymbol(str_symbol,n_params,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
			}
		}

		if(scope_direction&REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP){
			for(int i = 0; i < scopes->count; i++){
				Scope *s=(Scope *)scopes->items[i];

				if(s->getIdxScriptFunction() == idx_script_function){ // Only check repeated symbols in the same function scope context.
					Symbol *sv=s->getSymbol(str_symbol,n_params,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP);

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
		for(int i = 0; i < symbol_functions->count; i++){
			sv=(Symbol *)symbol_functions->items[i];
			if(
			   ( sv == symbol )
			){
				delete sv;
				symbol_functions->erase(i); // erase symbol scope
				return true;
			}
		}

		for(int i = 0; i < symbol_variables->count; i++){
			sv=(Symbol *)symbol_variables->items[i];
			if(
			   ( sv == symbol )
			){
				delete sv;
				symbol_variables->erase(i); // erase symbol scope
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------------------
	Scope::~Scope(){

		// delete scope found
		delete scopes;
		scopes=NULL;

		// delete local local_symbols found...
		for(int i = 0; i < symbol_types->count; i++){
			delete (Symbol *)symbol_types->items[i];
		}


		for(int i = 0; i < symbol_functions->count; i++){
			delete (Symbol *)symbol_functions->items[i];
		}

		for(int i = 0; i < symbol_variables->count; i++){
			delete (Symbol *)symbol_variables->items[i];
		}

		delete symbol_variables;
		delete symbol_functions;
		delete symbol_types;

		symbol_functions=NULL;
		symbol_variables=NULL;
		symbol_types=NULL;
	}
}
