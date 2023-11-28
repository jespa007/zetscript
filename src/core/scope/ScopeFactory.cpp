/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	//-----------
	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new Vector<Scope *>;
		main_scope=NULL;
		idx_clear_checkpoint=0;  // start from MAIN scope
	}

	void ScopeFactory::init(){
		main_scope=newScope(ZS_IDX_FUNCTION_MAIN,NULL,ZS_SCOPE_PROPERTY_IS_SCOPE_CLASS); // create global scope (scope 0)
		idx_clear_checkpoint=scopes->size();
	}

	Scope *	 ScopeFactory::newScope(int idx_sf,Scope * scope_parent,uint16_t _properties){
		Scope * scope_node = new Scope(this->zs,idx_sf,scope_parent,_properties);
		scopes->append(scope_node);
		return scope_node;
	}

	Vector<Scope *> 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clearUnusuedScopes(){
		int v=(int)(scopes->size()-1);
		while(v>=0){
			Scope *scope=(Scope *)scopes->get(v);

			if(scope!=NULL  // scope can be NULL due it was erased before by the removing parent
					&&
			(scope->properties & ZS_SCOPE_PROPERTY_UNUSUED)){
				// search parent element
				if(scope->scope_parent != NULL){
					// remove child from parent to
					Vector<Scope *> *childs=scope->scope_parent->getScopes();
					for(int i=0; i < childs->size(); i++){
						Scope *child=(Scope *)childs->get(i);
						if(child==scope){
							childs->erase(i);
							break;
						}
					}
				}

				delete scope;
				scopes->erase(v);
			}
			--v;
		}
	}

	void ScopeFactory::clear(){
		int idx_start = idx_clear_checkpoint;

		String global_symbol="";

		for(
			int v=(int)(scopes->size()-1);
			v >= idx_start;
			v--
		){
			Scope * info_scope = (Scope *)scopes->get(v);//(Scope *)scopes->get(v);
			info_scope->properties |= ZS_SCOPE_PROPERTY_UNUSUED;
		}

		clearUnusuedScopes();
	}

	void ScopeFactory::saveState(){
		idx_clear_checkpoint=scopes->size();
	}

	ScopeFactory::~ScopeFactory(){

		// destroy all nodes ...
		for(int i = 0; i < scopes->size(); i++){
			delete (Scope *)scopes->get(i);
		}
		scopes->clear();

		delete scopes;

	}
}
