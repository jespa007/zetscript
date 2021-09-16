#include "zetscript.h"

namespace zetscript{


	//-----------
	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new zs_vector;
		main_scope=newScope(IDX_SCRIPT_FUNCTION_MAIN,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS); // create global scope (scope 0)
		idx_clear_checkpoint=1;  // start from MAIN scope
		idx_clear_global_checkpoint_global_symbol_registered_variables=0;
		idx_clear_global_checkpoint_global_symbol_registered_functions=0;
		idx_clear_global_checkpoint_global_symbol_registered_classes=0;
		idx_clear_global_checkpoint_global_scopes=0;
	}

	Scope *	 ScopeFactory::newScope(int idx_sf,Scope * scope_parent,uint16_t _properties){
		Scope * scope_node = new Scope(this->zs,idx_sf,scope_parent,_properties);
		scopes->push_back((zs_int)scope_node);
		return scope_node;
	}

	zs_vector 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clearUnusuedScopes(){
		int v=0;
		while(v<scopes->count){
			Scope *scope=(Scope *)scopes->items[v];
			if(scope->properties & SCOPE_PROPERTY_UNUSUED){
				scopes->erase(v);
			}else{
				++v;
			}
		}

	}

	void ScopeFactory::clear(){
		int idx_start = idx_clear_checkpoint;

		int idx_start_global_scopes=idx_clear_global_checkpoint_global_scopes;


		zs_string global_symbol="";

		for(
			int v=scopes->count-1;
			v > idx_start; // avoid main scope
			v--
		){
			Scope * info_scope = (Scope *)scopes->pop_back();//(Scope *)scopes->get(v);
			delete info_scope;
		}

		// erase all local scopes in the main scope
		for (
				int v = main_scope->scopes->count-1;
				v > idx_start_global_scopes ;
				v--) {
			// Only pop-back, don't delete because they were deleted on previous loop
			main_scope->scopes->pop_back();

		}
		//clearGlobalSymbols();
	}

	void ScopeFactory::saveState(){
		Scope *main_scope= (Scope *)this->scopes->items[IDX_SCRIPT_SCOPE_MAIN];
		idx_clear_checkpoint=scopes->count-1;


		idx_clear_global_checkpoint_global_symbol_registered_variables=main_scope->symbol_variables->count-1;
		idx_clear_global_checkpoint_global_symbol_registered_functions=main_scope->symbol_functions->count-1;

		idx_clear_global_checkpoint_global_scopes=main_scope->scopes->count-1;

	}
/*
	void ScopeFactory::clearGlobalSymbols(int _idx_start){
		int idx_start_global_registered_symbols=_idx_start==ZS_IDX_UNDEFINED?idx_clear_global_checkpoint_global_registered_symbols:_idx_start;
		Scope *main_scope= (Scope *)this->scopes->items[IDX_SCRIPT_SCOPE_MAIN];
		VirtualMachine *vm=this->zs->getVirtualMachine();
		StackElement *vm_stk_element=&vm_get_stack_elements(vm)[main_scope->registered_symbols->count-1];

		// remove all symbols/scopes registered main scope till idx_start
		for (
				int v = main_scope->registered_symbols->count-1;
				v > idx_start_global_registered_symbols;
				v--) {
			//delete (Symbol *)main_scope->registered_symbols->items[v];
			//main_scope->registered_symbols->pop_back();
			Symbol *symbol=(Symbol *)main_scope->registered_symbols->items[v];
			//global_symbol=symbol->name;
			ScriptObjectObject *var = NULL;


			if(vm_stk_element->properties & STK_PROPERTY_SCRIPT_OBJECT){
				var =((ScriptObjectObject *)(vm_stk_element->value));
				if(var){
					if(var->shared_pointer != NULL){
						if(!vm_unref_shared_script_object(vm,var,IDX_CALL_STACK_MAIN)){
							THROW_RUNTIME_ERROR("error clearing variables: %s",vm_get_error(vm));
						}
					}
				}
			}
			delete symbol;
			main_scope->registered_symbols->pop_back();
			*vm_stk_element--=k_stk_undefined;
		}


	}
*/

	ScopeFactory::~ScopeFactory(){

		//clearGlobalSymbols(0);

		// destroy all nodes ...
		for(unsigned i = 0; i < scopes->count; i++){
			delete (Scope *)scopes->get(i);
		}
		scopes->clear();

		delete scopes;

	}
};
