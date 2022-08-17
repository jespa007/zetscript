/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void    			ScriptObjectVectorWrap_push(ZetScript *_zs,ScriptObjectVector *sv,StackElement  * stk){
		ZS_UNUSUED_PARAM(_zs);
		return sv->push(stk);
	}

	void   	ScriptObjectVectorWrap_pop(ZetScript *_zs,ScriptObjectVector *sv){
		ZS_UNUSUED_PARAM(_zs);
		sv->pop();
	}

	zs_int 			ScriptObjectVectorWrap_size(ZetScript *_zs,ScriptObjectVector *sv){
		ZS_UNUSUED_PARAM(_zs);
		return sv->length();
	}

	void 			ScriptObjectVectorWrap_insertAt(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		stk_assign(_zs,new_stk,_stk);

		std::vector<StackElement *> *stk_user_list_elements=sv->getStkUserListElements();
		stk_user_list_elements->insert(
				stk_user_list_elements->begin()+idx
				,new_stk
		);
	}

	void 			ScriptObjectVectorWrap_eraseAt(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx){
		ZS_UNUSUED_PARAM(_zs);
		sv->eraseUserElementAt(idx);
	}

	void 			ScriptObjectVectorWrap_clear(ZetScript *_zs,ScriptObjectVector *sv){
		ZS_UNUSUED_PARAM(_zs);
		sv->eraseAllUserElements();
	}

	ScriptObjectString *		ScriptObjectVectorWrap_join(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx){
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(_zs);
		std::string *ptr_str=(std::string *)so_string->value;
		std::vector<StackElement *> *stk_user_list_elements=sv->getStkUserListElements();

		for(unsigned i=0; i < stk_user_list_elements->size();i++){
			StackElement *stk=(StackElement *)stk_user_list_elements->at(i);
			if(i>0){
				*ptr_str+=((char)idx);
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				ptr_str->append(((ScriptObject *)stk->value)->toString());
			}
			else{
				ptr_str->append(stk_to_str(_zs,stk));
			}
		}

		return so_string;
	}

	ScriptObjectIteratorVector * ScriptObjectVectorWrap_iter(ZetScript *_zs,ScriptObjectVector *so){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_OBJECT_ITERATOR_VECTOR(so);
	}

	bool 							ScriptObjectVectorWrap_contains(ZetScript *_zs,ScriptObjectVector *sv, StackElement *stk_to_compare){
		ZS_UNUSUED_PARAM(_zs);
		bool found=false;
		std::vector<StackElement *> *stk_user_list_elements=sv->getStkUserListElements();

		for(unsigned i=0; i < stk_user_list_elements->size() && found == false;i++){
			StackElement *stk_element=(StackElement *)stk_user_list_elements->at(i);
			switch(stk_to_compare->properties & stk_element->properties){ // match element
			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
			case STK_PROPERTY_ZS_FLOAT:
				found= (stk_element->value == stk_to_compare->value);
				break;
			case STK_PROPERTY_SCRIPT_OBJECT: // compare objects?

				if(
					   ((ScriptObject *)stk_to_compare->value)->idx_script_type ==
					   ((ScriptObject *)stk_element->value)->idx_script_type
					   ){
					// particular case string
					if(((ScriptObject *)stk_to_compare->value)->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING){
						found=((ScriptObject *)stk_to_compare->value)->toString()==((ScriptObject *)stk_element->value)->toString();
					}
				}
				break;
			}

		}
		return found;
	}

	bool 							ScriptObjectVectorWrap_equal(ZetScript *_zs,ScriptObjectVector *so1, ScriptObjectVector *so2){
		ZS_UNUSUED_PARAM(_zs);
		bool equal=true;
		std::vector<StackElement *> *stk_user_list_elements_s1=so1->getStkUserListElements();
		std::vector<StackElement *> *stk_user_list_elements_s2=so2->getStkUserListElements();

		if(stk_user_list_elements_s1->size() != stk_user_list_elements_s2->size()){
			return false;
		}

		for(unsigned i=0; i < stk_user_list_elements_s1->size() && equal == true;i++){
			StackElement *stk_element_s1=(StackElement *)stk_user_list_elements_s1->at(i);
			StackElement *stk_element_s2=(StackElement *)stk_user_list_elements_s2->at(i);

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
						equal=((ScriptObject *)stk_element_s1->value)->toString() == ((ScriptObject *)stk_element_s2->value)->toString();
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
