#include "zetscript.h"

namespace zetscript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new zs_vector;
		main_scope=newScope(IDX_SCRIPT_FUNCTION_MAIN,NULL,false); // create global scope (scope 0)
		idx_clear_checkpoint=1;  // start from MAIN scope
		idx_clear_global_checkpoint_global_registered_symbols=0;
		idx_clear_global_checkpoint_global_scopes=0;

	}




	Scope *	 ScopeFactory::newScope(int idx_sf,Scope * scope_parent,bool is_c_node){
		Scope * scope_node = new Scope(this->zs,idx_sf,scope_parent,is_c_node);
		scopes->push_back((zs_int)scope_node);
		return scope_node;
	}

	zs_vector 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clear(){
		int idx_start = idx_clear_checkpoint;

		int idx_start_global_scopes=idx_clear_global_checkpoint_global_scopes;


		std::string global_symbol="";

		for(
			int v=scopes->count-1;
			v > idx_start; // avoid main scope
			v--
		){
			Scope * info_scope = (Scope *)scopes->get(v);
			delete info_scope;
			scopes->pop_back();
		}

		// erase all sub scopes in the main scope
		for (
				int v = main_scope->registered_scopes->count-1;
				v > idx_start_global_scopes ;
				v--) {
			//delete (Scope *)main_scope->registered_scopes->items[v];
			main_scope->registered_scopes->pop_back();
		}


		//clearGlobalSymbols();
	}

	void ScopeFactory::saveState(){
		Scope *main_scope= (Scope *)this->scopes->items[IDX_SCRIPT_SCOPE_MAIN];
		idx_clear_checkpoint=scopes->count-1;


		idx_clear_global_checkpoint_global_registered_symbols=main_scope->registered_symbols->count-1;
		idx_clear_global_checkpoint_global_scopes=main_scope->registered_scopes->count-1;

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
