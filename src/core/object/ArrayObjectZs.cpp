/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void    			ArrayObjectZs_push(ZetScript *_zs,ArrayObject *sv,StackElement  * stk){
		ZS_UNUSUED_PARAM(_zs);
		return sv->pushStackElement(stk);
	}

	void   	ArrayObjectZs_pop(ZetScript *_zs,ArrayObject *_this){
		ZS_UNUSUED_PARAM(_zs);
		_this->pop();
	}

	zs_int 			ArrayObjectZs_length(ZetScript *_zs,ArrayObject *_this){
		ZS_UNUSUED_PARAM(_zs);
		return _this->length();
	}

	void 			ArrayObjectZs_insertAt(ZetScript *_zs,ArrayObject *_this, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		_zs->stackElementAssign(new_stk,_stk);

		Vector<StackElement *> *stk_user_list_elements=_this->getStkListElements();
		stk_user_list_elements->insert(idx,new_stk);
	}

	void 			ArrayObjectZs_eraseAt(ZetScript *_zs,ArrayObject *_this, zs_int idx){
		ZS_UNUSUED_PARAM(_zs);
		_this->eraseElementAt(idx);
	}

	void 			ArrayObjectZs_clear(ZetScript *_zs,ArrayObject *_this){
		ZS_UNUSUED_PARAM(_zs);
		_this->eraseAllElements();
	}

	StringObject *		ArrayObjectZs_join(ZetScript *_zs,ArrayObject *_this, zs_int idx){
		StringObject *so_string = ZS_NEW_STRING_OBJECT(_zs);
		String *ptr_str=so_string->str_ptr;
		Vector<StackElement *> *stk_user_list_elements=_this->getStkListElements();

		for(int i=0; i < stk_user_list_elements->length();i++){
			StackElement *stk=(StackElement *)stk_user_list_elements->get(i);
			if(i>0){
				ptr_str->append((char)idx);
			}

			if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				ptr_str->append(((Object *)stk->value)->toString());
			}
			else{
				ptr_str->append(_zs->stackElementToString(stk));
			}
		}

		return so_string;
	}

	ArrayIteratorObject * ArrayObjectZs_iter(ZetScript *_zs,ArrayObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_OBJECT_ITERATOR_ARRAY(_zs,so);
	}

	ArrayObject 			*	ArrayObjectZs_concat(ZetScript *_zs,ArrayObject *_so1, ArrayObject *_so2){

		return ArrayObject::concat(_zs,_so1, _so2);
	}

	void 							ArrayObjectZs_extend(ZetScript *_zs,ArrayObject *_this, ArrayObject *_sv_extend){
		ZS_UNUSUED_PARAM(_zs);
		_this->concat(_sv_extend);
	}

	bool 							ArrayObjectZs_contains(ZetScript *_zs,ArrayObject *sv, StackElement *stk_to_compare){
		ZS_UNUSUED_PARAM(_zs);
		bool found=false;
		Vector<StackElement *> *stk_user_list_elements=sv->getStkListElements();

		for(int i=0; i < stk_user_list_elements->length() && found == false;i++){
			StackElement *stk_element=(StackElement *)stk_user_list_elements->get(i);
			switch(stk_to_compare->properties & stk_element->properties){ // match element
			case STACK_ELEMENT_PROPERTY_BOOL:
			case STACK_ELEMENT_PROPERTY_INT:
			case STACK_ELEMENT_PROPERTY_FLOAT:
				found= (stk_element->value == stk_to_compare->value);
				break;
			case STACK_ELEMENT_PROPERTY_OBJECT: // compare objects?

				if(
					   ((Object *)stk_to_compare->value)->type_id ==
					   ((Object *)stk_element->value)->type_id
					   ){
					// particular case string
					if(((Object *)stk_to_compare->value)->type_id == TYPE_ID_OBJECT_STRING){
						found=((Object *)stk_to_compare->value)->toString()==((Object *)stk_element->value)->toString();
					}
				}
				break;
			}

		}
		return found;
	}

	bool 							ArrayObjectZs_equal(ZetScript *_zs,ArrayObject *so1, ArrayObject *so2){
		ZS_UNUSUED_PARAM(_zs);
		bool equal=true;
		Vector<StackElement *> *stk_user_list_elements_s1=so1->getStkListElements();
		Vector<StackElement *> *stk_user_list_elements_s2=so2->getStkListElements();

		if(stk_user_list_elements_s1->length() != stk_user_list_elements_s2->length()){
			return false;
		}

		for(int i=0; i < stk_user_list_elements_s1->length() && equal == true;i++){
			StackElement *stk_element_s1=(StackElement *)stk_user_list_elements_s1->get(i);
			StackElement *stk_element_s2=(StackElement *)stk_user_list_elements_s2->get(i);

			// compare primitives (inclusive strings) other objects as the same pointer
			switch(stk_element_s1->properties | stk_element_s2->properties){
				case STACK_ELEMENT_PROPERTY_INT:
				case STACK_ELEMENT_PROPERTY_FLOAT:
				case STACK_ELEMENT_PROPERTY_BOOL:
					equal=stk_element_s1->value==stk_element_s2->value;
					break;
				case (STACK_ELEMENT_PROPERTY_INT|STACK_ELEMENT_PROPERTY_FLOAT):
					if((stk_element_s1->properties & STACK_ELEMENT_PROPERTY_INT) && (stk_element_s2->properties & STACK_ELEMENT_PROPERTY_FLOAT)){
						equal=stk_element_s1->value == ZS_READ_INTPTR_FLOAT(stk_element_s2->value);
					}else{
						equal=ZS_READ_INTPTR_FLOAT(stk_element_s1->value)==stk_element_s2->value;
					}
					break;
				case STACK_ELEMENT_PROPERTY_OBJECT:
					if(STACK_ELEMENT_IS_STRING_OBJECT(stk_element_s1) && STACK_ELEMENT_IS_STRING_OBJECT(stk_element_s2)){
						equal=((Object *)stk_element_s1->value)->toString() == ((Object *)stk_element_s2->value)->toString();
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
