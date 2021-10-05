/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	void    			ScriptObjectVectorWrap_push(ScriptObjectVector *sv,StackElement  * stk){
		return sv->push(stk);
	}

	void   	ScriptObjectVectorWrap_pop(ScriptObjectVector *sv){
		sv->pop();
	}

	zs_int 			ScriptObjectVectorWrap_size(ScriptObjectVector *sv){
		return sv->length();
	}

	void 			ScriptObjectVectorWrap_insertAt(ScriptObjectVector *sv, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;
		ZetScript *zs=sv->getZetScript();
		VirtualMachine *vm=sv->getZetScript()->getVirtualMachine();
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		// update n_refs +1
		if(_stk->properties&STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_param=(ScriptObject *)_stk->value;
			if(so_param->idx_script_class == IDX_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
				//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
				ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(zs);
				if(!vm_create_shared_pointer(vm,sc)){
					return;
				}
				sc->set(so_param->toString());
				so_param=sc;
				new_stk->value=(zs_int)so_param; // update pointer
			}

			vm_share_pointer(vm,so_param);

		}

		stk_user_list_elements->insert(idx,(zs_int)new_stk);
	}

	void 			ScriptObjectVectorWrap_eraseAt(ScriptObjectVector *sv, zs_int idx){
		sv->eraseUserElementAt(idx);
	}

	void 			ScriptObjectVectorWrap_clear(ScriptObjectVector *sv){
		sv->eraseAllUserElements();
	}

	ScriptObjectString *		ScriptObjectVectorWrap_join(ScriptObjectVector *sv, zs_int idx){
		ScriptObjectString *so_ref=NULL;
		//zs_string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(sv->getZetScript());
		zs_string *ptr_str=(zs_string *)so_string->value;
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();
		ZetScript *zs=sv->getZetScript();

		for(unsigned i=0; i < stk_user_list_elements->count;i++){
			StackElement *stk=(StackElement *)stk_user_list_elements->items[i];
			if(i>0){
				ptr_str->append((char)idx);
			}
			ptr_str->append(stk_to_str(zs,stk));
		}

		return so_string;
	}

	ScriptObjectVectorIterator * ScriptObjectVectorWrap_iter(ScriptObjectVector *so){
		return ZS_NEW_OBJECT_VECTOR_ITERATOR(so);
	}

	bool 							ScriptObjectVectorWrap_contains(ScriptObjectVector *sv, StackElement *stk_to_compare){
		bool found=false;
		ZetScript *zs=sv->getZetScript();
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		for(unsigned i=0; i < stk_user_list_elements->count && found == false;i++){
			StackElement *stk_element=(StackElement *)stk_user_list_elements->items[i];
			switch(stk_to_compare->properties & stk_element->properties){ // match element
			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
			case STK_PROPERTY_ZS_FLOAT:
				found= (stk_element->value == stk_to_compare->value);
				break;
			case STK_PROPERTY_SCRIPT_OBJECT: // compare objects?

				if(
					   ((ScriptObject *)stk_to_compare->value)->idx_script_class ==
					   ((ScriptObject *)stk_element->value)->idx_script_class
					   ){
					// particular case string
					if(((ScriptObject *)stk_to_compare->value)->idx_script_class == IDX_TYPE_SCRIPT_OBJECT_STRING){
						found=stk_to_str(zs,stk_to_compare)==stk_to_str(zs,stk_element);
					}
				}
				break;
			}

		}
		return found;
	}
}
