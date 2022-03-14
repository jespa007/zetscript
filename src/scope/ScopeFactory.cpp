/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	//-----------
	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new zs_vector;
		main_scope=NULL;
		idx_clear_checkpoint=0;  // start from MAIN scope
		//idx_clear_global_checkpoint_global_symbol_registered_variables=0;
		//idx_clear_global_checkpoint_global_symbol_registered_functions=0;
		//idx_clear_global_checkpoint_global_symbol_registered_types=0;
	}

	void ScopeFactory::init(){
		main_scope=newScope(IDX_SCRIPT_FUNCTION_MAIN,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS); // create global scope (scope 0)
		idx_clear_checkpoint=scopes->count;
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
		int v=scopes->count-1;
		while(v>=0){
			Scope *scope=(Scope *)scopes->items[v];


			if(scope!=NULL  // scope can be NULL due it was erased before by the removing parent
					&&
			(scope->properties & SCOPE_PROPERTY_UNUSUED)){
				// search parent element
				if(scope->scope_parent != NULL){
					// remove child from parent to
					zs_vector *childs=scope->scope_parent->scopes;
					for(int i=0; i < childs->count; i++){
						Scope *child=(Scope *)childs->items[i];
						if(child==scope){
							childs->erase(i);
							break;
						}
					}
				}

				delete scope;

				scopes->erase(v);
			}else{
				--v;
			}
		}
	}

	void ScopeFactory::clear(){
		int idx_start = idx_clear_checkpoint;

		zs_string global_symbol="";

		for(
			int v=scopes->count-1;
			v >= idx_start;
			v--
		){
			Scope * info_scope = (Scope *)scopes->items[v];//(Scope *)scopes->get(v);
			info_scope->properties |= SCOPE_PROPERTY_UNUSUED;

		}

		clearUnusuedScopes();
	}

	void ScopeFactory::saveState(){
		idx_clear_checkpoint=scopes->count;


		//idx_clear_global_checkpoint_global_symbol_registered_variables=main_scope->symbol_variables->count;
		//idx_clear_global_checkpoint_global_symbol_registered_functions=main_scope->symbol_functions->count;
		//idx_clear_global_checkpoint_global_symbol_registered_types=main_scope->symbol_types->count;

	}

	ScopeFactory::~ScopeFactory(){

		// destroy all nodes ...
		for(int i = 0; i < scopes->count; i++){
			delete (Scope *)scopes->get(i);
		}
		scopes->clear();

		delete scopes;

	}
}
