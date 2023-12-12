/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100


namespace zetscript{

	//------------------------------------------------------------------------------------------------
	ScriptScope::ScriptScope(ScriptEngine * _se,int _idx_script_function, ScriptScope * _scope_parent, uint16_t _properties){
		scope_parent = _scope_parent;
		properties = _properties;
		owner_type=NULL;
		id=_idx_script_function;
		zs=_se;
		offset_instruction_push_scope=ZS_UNDEFINED_IDX;
		scope_factory=_se->getScriptScopesFactory();
		scopes=new Vector<ScriptScope *>;
		symbol_variables=new Vector<Symbol *>;
		symbol_functions=new Vector<Symbol *>;
		symbol_types=new Vector<Symbol *>;

		if(_scope_parent == NULL){ // first node (it should be a type)...
			scope_base = this;
		}else{ // others...
			scope_base = scope_parent->scope_base;
			owner_type=scope_parent->owner_type; // propagate script type

			if(id==ZS_UNDEFINED_IDX){ // May be is a block containing if-else, for, etc --> propagate current script function
				id=scope_parent->id;
			}

			// parent owns this scope
			_scope_parent->scopes->push(this);
		}
	}

	void ScriptScope::setTypeOwner(ScriptType *_owner_type){
		if(scope_parent != NULL){
			ZS_THROW_RUNTIME_ERRORF("Internal error setScriptclass scope_parent should NULL (i.e scope should be root)");
			return;
		}
		owner_type=_owner_type;
	}

	ScriptType * ScriptScope::getOwnerType(){
		return scope_base->owner_type;
	}

	int ScriptScope::getIdxFunction(){
		return id;
	}

	int ScriptScope::numInnerScopesRecursive(ScriptScope *sc, int _n){

		if(sc->scope_parent!=NULL){
			return numInnerScopesRecursive(sc->scope_parent, _n+1);
		}

		return _n;
	}

	void ScriptScope::removeChildrenBlockTypes(){
		for(int i=0; i < this->scopes->length(); i++){
			markBlockScopeAsUnusuedScopesRecursive((ScriptScope *)this->scopes->get(i));
		}

		this->scope_factory->clearUnusuedScopes();
	}

	void ScriptScope::markBlockScopeAsUnusuedScopesRecursive(ScriptScope *_sc){

		if(_sc==NULL){
			return;
		}

		// if scope is block the consecutive are blocks check node
		if(_sc->properties & SCOPE_PROPERTY_IS_SCOPE_BLOCK){
			_sc->properties|=SCOPE_PROPERTY_UNUSUED;

			// check its
			for(int i=0; i < _sc->scopes->length();i++){
				ScriptScope *_child=(ScriptScope *)_sc->scopes->get(i);
				if((_sc->properties & SCOPE_PROPERTY_IS_SCOPE_BLOCK)==0){
					throw std::runtime_error("expected scope as block type");
				}
				markBlockScopeAsUnusuedScopesRecursive(_child);
			}
		}
	}


	int ScriptScope::numInnerScopes(){
		return numInnerScopesRecursive(this,1);
	}

	int ScriptScope::countVariables(bool _recursive){
		if(_recursive){
			return ScriptScope::countVariablesRecursive(this,this->id);
		}

		return this->symbol_variables->length();

	}

	int ScriptScope::countVariablesRecursive(ScriptScope *_sc, int idx_script_function_reference){
		int n_total=0;
		if(_sc->id==idx_script_function_reference){ // only count variables in the scope of the function
			n_total=_sc->symbol_variables->length();

			for(int i=0; i < _sc->scopes->length(); i++){
				n_total+=ScriptScope::countVariablesRecursive((ScriptScope *)_sc->scopes->get(i),idx_script_function_reference);
			}
		}

		return n_total;
	}


	void						   ScriptScope::markAsUnusued(){

		// because no variables in there it moves all scopes in the parent ...
		if(scope_parent != NULL){
			for(int i=0;i < scopes->length(); i++){
				ScriptScope *current_scope=(ScriptScope *)scopes->get(i);
				scope_parent->scopes->push(current_scope);
				current_scope->scope_parent = scope_parent;
			}

			scopes->clear();
		}

		// mark as unused, late we can remove safely check unusued flag...
		this->properties|=SCOPE_PROPERTY_UNUSUED;
	}

	Vector<ScriptScope *>	*ScriptScope::getScopes(){
		return scopes;
	}

	void ScriptScope::checkPreRegisterSymbol(
			const char * file
			,short line
			, const String & symbol_name
			, int8_t n_params
			, uint16_t check_repeated_symbols_direction
	){
		Symbol *p_irv=NULL;

		// check if you register a type...
		// check if symbol collides also with built in type...
		if((check_repeated_symbols_direction & SCRIPT_SCOPE_REGISTER_PROPERTY_NO_CHECK_CLASS_SYMBOLS)==0){
			if(se->getScriptTypesFactory()->getScriptTypeId(symbol_name) != ZS_UNDEFINED_IDX){
				ZS_THROW_EXCEPTION_FILE_LINE(file,line,"Cannot name symbol as '%s' because is a reserved word or script type",symbol_name.toConstChar());
			}
		}

		if((p_irv = getSymbol(symbol_name,n_params,check_repeated_symbols_direction))!=NULL){ // check whether symbol is already registered ...
			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				if(p_irv->file == NULL || *p_irv->file==0){
					ZS_THROW_EXCEPTION_FILE_LINE(file,line," error symbol '%s' already declared", symbol_name.toConstChar());
				}else{
					ZS_THROW_EXCEPTION_FILE_LINE(file,line," error symbol '%s' already declared at '%s:%i'", symbol_name.toConstChar(),Path::getFilename(p_irv->file).toConstChar(),p_irv->line);
				}
			}else{
				ZS_THROW_RUNTIME_ERROR(" error symbol '%s' already registered as C++", symbol_name.toConstChar());
			}
		}
	}

	Symbol * ScriptScope::registerSymbolType(
			const char * _file
			,short _line
			, const String & _symbol_name
			, uint16_t _check_repeated_symbols_direction
	){
		checkPreRegisterSymbol(_file, _line, _symbol_name,  ZS_NO_PARAMS_SYMBOL_ONLY,_check_repeated_symbols_direction);

		Symbol *symbol 		= new Symbol(_symbol_name);
		symbol->idx_position=symbol_types->length();
		symbol->file	 	= _file;
		symbol->line 	 	= _line;
		symbol->scope		=  this;
		symbol->properties |=SYMBOL_PROPERTY_TYPE;

		symbol_types->push(symbol);

		return symbol;

	}

	Symbol * ScriptScope::registerSymbolVariable(
			const char * _file
			,short _line
			, const String & _symbol_name
			,uint16_t _check_repeated_symbols_direction
	){
		checkPreRegisterSymbol(_file, _line, _symbol_name,  ZS_NO_PARAMS_SYMBOL_ONLY,_check_repeated_symbols_direction);

		Symbol *symbol 		= new Symbol(_symbol_name);
		symbol->idx_position=symbol_variables->length();
		symbol->file	 	= _file;
		symbol->line 	 	= _line;
		symbol->scope		=  this;

		symbol_variables->push(symbol);

		return symbol;
	}

	Symbol * ScriptScope::registerSymbolFunction(
			const char * _file
			,short _line
			, const String & _symbol_name
			, int8_t _n_params
			, uint16_t _check_repeated_symbols_direction
	){

		if((_check_repeated_symbols_direction & SCRIPT_SCOPE_REGISTER_PROPERTY_NO_CHECK_REPEATED_SYMBOLS)==0){
			checkPreRegisterSymbol(_file, _line, _symbol_name,  _n_params,_check_repeated_symbols_direction);
		}

		Symbol *symbol 		= new Symbol(_symbol_name);
		symbol->idx_position=symbol_functions->length();
		symbol->file	 	= _file;
		symbol->line 	 	= _line;
		symbol->scope		=  this;
		symbol->n_params	=_n_params;
		symbol->properties |=SYMBOL_PROPERTY_FUNCTION;

		symbol_functions->push(symbol);

		return symbol;
	}


	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//

	Symbol * ScriptScope::getSymbol(const String & _str_symbol, int8_t _n_params, uint16_t _scope_direction){

		// for each variable in current scope ...
		for(int i = 0; i < symbol_types->length(); i++){
			Symbol *sv=(Symbol *)symbol_types->get(i);
			if(
				   ( sv->name == _str_symbol )
			){
				return sv;
			}
		}

		for(int i = 0; i < symbol_variables->length(); i++){
			Symbol *sv=(Symbol *)symbol_variables->get(i);
			if(
				   ( sv->name == _str_symbol )
			){
				return sv;
			}
		}

		for(int i = 0; i < symbol_functions->length(); i++){
			Symbol *sv=(Symbol *)symbol_functions->get(i);
			if(
				   ( sv->name == _str_symbol )
			   &&  ( sv->n_params == _n_params || _n_params==ZS_NO_PARAMS_SYMBOL_ONLY )
			){
				return sv;
			}
		}


		if(_scope_direction&SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_DOWN){
			if(
					   this->scope_parent != NULL			 	 // it says that is the end of scopes
					&& this->scope_parent->getIdxFunction() == id // Only check repeated symbols in the same function scope context.
			){
				//uint16_t avoid_main=scope_direction & SCOPE_DIRECTION_AVOID_MAIN_SCOPE ? SCOPE_DIRECTION_AVOID_MAIN_SCOPE:0;
				return this->scope_parent->getSymbol(_str_symbol,_n_params,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_DOWN);
			}
		}

		if(_scope_direction&SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP){
			for(int i = 0; i < scopes->length(); i++){
				ScriptScope *s=(ScriptScope *)scopes->get(i);

				if(s->getIdxFunction() == id){ // Only check repeated symbols in the same function scope context.
					Symbol *sv=s->getSymbol(_str_symbol,_n_params,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP);

					if(sv != NULL) {
						return sv;
					}
				}
			}
		}

		return NULL;
	}

	bool ScriptScope::unregisterSymbol(Symbol *symbol){
		Symbol *sv=NULL;
		for(int i = 0; i < symbol_functions->length(); i++){
			sv=(Symbol *)symbol_functions->get(i);
			if(
			    sv == symbol
			){
				delete sv;
				symbol_functions->erase(i); // erase symbol scope
				return true;
			}
		}

		for(int i = 0; i < symbol_variables->length(); i++){
			sv=(Symbol *)symbol_variables->get(i);
			if(
			    sv == symbol
			){
				delete sv;
				symbol_variables->erase(i); // erase symbol scope
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------------------
	ScriptScope::~ScriptScope(){

		// delete scope found
		if(scopes!=NULL){
			delete scopes;
			scopes=NULL;
		}

		if(symbol_types != NULL){
			// delete local local_symbols found...
			for(int i = 0; i < symbol_types->length(); i++){
				delete (Symbol *)symbol_types->get(i);
			}

			delete symbol_types;
			symbol_types=NULL;

		}


		if(symbol_functions != NULL){
			for(int i = 0; i < symbol_functions->length(); i++){
				delete (Symbol *)symbol_functions->get(i);
			}

			delete symbol_functions;
			symbol_functions=NULL;
		}

		if(symbol_variables!=NULL){
			for(int i = 0; i < symbol_variables->length(); i++){
				delete (Symbol *)symbol_variables->get(i);
			}
			delete symbol_variables;
			symbol_variables=NULL;
		}
	}
}
