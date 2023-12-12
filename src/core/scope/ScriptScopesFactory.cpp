/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	//-----------
	ScriptScopesFactory::ScriptScopesFactory(ScriptEngine *zs){
		this->zs=zs;
		this->scopes = new Vector<ScriptScope *>;
		main_scope=NULL;
		idx_clear_checkpoint=0;  // start from MAIN scope
	}

	void ScriptScopesFactory::init(){
		main_scope=newScope(ZS_IDX_FUNCTION_MAIN,NULL,SCOPE_PROPERTY_IS_SCOPE_CLASS); // create global scope (scope 0)
		idx_clear_checkpoint=scopes->length();
	}

	ScriptScope *	 ScriptScopesFactory::newScope(int idx_sf,ScriptScope * scope_parent,uint16_t _properties){
		ScriptScope * scope_node = new ScriptScope(this->zs,idx_sf,scope_parent,_properties);
		scopes->push(scope_node);
		return scope_node;
	}

	Vector<ScriptScope *> 	*		ScriptScopesFactory::getScopes(){
		return scopes;
	}

	void ScriptScopesFactory::clearUnusuedScopes(){
		int v=(int)(scopes->length()-1);
		while(v>=0){
			ScriptScope *scope=(ScriptScope *)scopes->get(v);

			if(scope!=NULL  // scope can be NULL due it was erased before by the removing parent
					&&
			(scope->properties & SCOPE_PROPERTY_UNUSUED)){
				// search parent element
				if(scope->scope_parent != NULL){
					// remove child from parent to
					Vector<ScriptScope *> *childs=scope->scope_parent->getScopes();
					for(int i=0; i < childs->length(); i++){
						ScriptScope *child=(ScriptScope *)childs->get(i);
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

	void ScriptScopesFactory::clear(){
		int idx_start = idx_clear_checkpoint;

		String global_symbol="";

		for(
			int v=(int)(scopes->length()-1);
			v >= idx_start;
			v--
		){
			ScriptScope * info_scope = (ScriptScope *)scopes->get(v);//(ScriptScope *)scopes->get(v);
			info_scope->properties |= SCOPE_PROPERTY_UNUSUED;
		}

		clearUnusuedScopes();
	}

	void ScriptScopesFactory::saveState(){
		idx_clear_checkpoint=scopes->length();
	}

	ScriptScopesFactory::~ScriptScopesFactory(){

		// destroy all nodes ...
		for(int i = 0; i < scopes->length(); i++){
			delete (ScriptScope *)scopes->get(i);
		}
		scopes->clear();

		delete scopes;

	}
}
