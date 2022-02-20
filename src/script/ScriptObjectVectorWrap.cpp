/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void    			ScriptObjectVectorWrap_push(ZetScript *_zs,ScriptObjectVector *sv,StackElement  * stk){
		return sv->push(stk);
	}

	void   	ScriptObjectVectorWrap_pop(ZetScript *_zs,ScriptObjectVector *sv){
		sv->pop();
	}

	zs_int 			ScriptObjectVectorWrap_size(ZetScript *_zs,ScriptObjectVector *sv){
		return sv->length();
	}

	void 			ScriptObjectVectorWrap_insertAt(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;
		VirtualMachine *vm=sv->getZetScript()->getVirtualMachine();
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		// update n_refs +1
		if(_stk->properties&STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_param=(ScriptObject *)_stk->value;
			if(so_param->idx_type == IDX_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
				//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
				ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(_zs);
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

	void 			ScriptObjectVectorWrap_eraseAt(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx){
		sv->eraseUserElementAt(idx);
	}

	void 			ScriptObjectVectorWrap_clear(ZetScript *_zs,ScriptObjectVector *sv){
		sv->eraseAllUserElements();
	}

	ScriptObjectString *		ScriptObjectVectorWrap_join(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx){
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(sv->getZetScript());
		zs_string *ptr_str=(zs_string *)so_string->value;
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		for(int i=0; i < stk_user_list_elements->count;i++){
			StackElement *stk=(StackElement *)stk_user_list_elements->items[i];
			if(i>0){
				ptr_str->append((char)idx);
			}
			ptr_str->append(stk_to_str(_zs,stk));
		}

		return so_string;
	}

	ScriptObjectIteratorVector * ScriptObjectVectorWrap_iter(ZetScript *_zs,ScriptObjectVector *so){
		return ZS_NEW_OBJECT_ITERATOR_VECTOR(so);
	}

	bool 							ScriptObjectVectorWrap_contains(ZetScript *_zs,ScriptObjectVector *sv, StackElement *stk_to_compare){
		bool found=false;
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		for(int i=0; i < stk_user_list_elements->count && found == false;i++){
			StackElement *stk_element=(StackElement *)stk_user_list_elements->items[i];
			switch(stk_to_compare->properties & stk_element->properties){ // match element
			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
			case STK_PROPERTY_ZS_FLOAT:
				found= (stk_element->value == stk_to_compare->value);
				break;
			case STK_PROPERTY_SCRIPT_OBJECT: // compare objects?

				if(
					   ((ScriptObject *)stk_to_compare->value)->idx_type ==
					   ((ScriptObject *)stk_element->value)->idx_type
					   ){
					// particular case string
					if(((ScriptObject *)stk_to_compare->value)->idx_type == IDX_TYPE_SCRIPT_OBJECT_STRING){
						found=stk_to_str(_zs,stk_to_compare)==stk_to_str(_zs,stk_element);
					}
				}
				break;
			}

		}
		return found;
	}

	bool 							ScriptObjectVectorWrap_equal(ZetScript *_zs,ScriptObjectVector *so1, ScriptObjectVector *so2){
		bool equal=true;
		zs_vector *stk_user_list_elements_s1=so1->getStkUserListElements();
		zs_vector *stk_user_list_elements_s2=so2->getStkUserListElements();

		if(stk_user_list_elements_s1->count != stk_user_list_elements_s2->count){
			return false;
		}

		for(int i=0; i < stk_user_list_elements_s1->count && equal == true;i++){
			StackElement *stk_element_s1=(StackElement *)stk_user_list_elements_s1->items[i];
			StackElement *stk_element_s2=(StackElement *)stk_user_list_elements_s2->items[i];

			// compare primitives (inclusive strings) other objects as the same pointer
			switch(stk_element_s1->properties | stk_element_s2->properties){
				case STK_PROPERTY_ZS_INT:
				case STK_PROPERTY_ZS_FLOAT:
				case STK_PROPERTY_BOOL:
					equal=stk_element_s1->value==stk_element_s2->value;
					break;
				case (STK_PROPERTY_ZS_INT|STK_PROPERTY_ZS_FLOAT):
					if((stk_element_s1->properties & STK_PROPERTY_ZS_INT) && (stk_element_s2->properties & STK_PROPERTY_ZS_FLOAT)){
						equal=stk_element_s1->value == *((float *)&stk_element_s2->value);
					}else{
						equal=*((float *)&stk_element_s1->value)==stk_element_s2->value;
					}
					break;
				case STK_PROPERTY_SCRIPT_OBJECT:
					if(STK_IS_SCRIPT_OBJECT_STRING(stk_element_s1) && STK_IS_SCRIPT_OBJECT_STRING(stk_element_s2)){
						equal=stk_to_str(_zs,stk_element_s1) == stk_to_str(_zs,stk_element_s2);
					}else{
						equal=stk_element_s1->value==stk_element_s2->value;
					}
					break;
				default:
					equal=false;
					break;
			}

		}
		return equal;
	}
}
