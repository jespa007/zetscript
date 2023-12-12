/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void    			ArrayScriptObjectZs_push(ScriptEngine *_se,ArrayScriptObject *sv,StackElement  * stk){
		ZS_UNUSUED_PARAM(_se);
		return sv->pushStackElement(stk);
	}

	void   	ArrayScriptObjectZs_pop(ScriptEngine *_se,ArrayScriptObject *_this){
		ZS_UNUSUED_PARAM(_se);
		_this->pop();
	}

	zs_int 			ArrayScriptObjectZs_length(ScriptEngine *_se,ArrayScriptObject *_this){
		ZS_UNUSUED_PARAM(_se);
		return _this->length();
	}

	void 			ArrayScriptObjectZs_insertAt(ScriptEngine *_se,ArrayScriptObject *_this, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		_se->stackElementAssign(new_stk,_stk);

		Vector<StackElement *> *stk_user_list_elements=_this->getStkListElements();
		stk_user_list_elements->insert(idx,new_stk);
	}

	void 			ArrayScriptObjectZs_eraseAt(ScriptEngine *_se,ArrayScriptObject *_this, zs_int idx){
		ZS_UNUSUED_PARAM(_se);
		_this->eraseElementAt(idx);
	}

	void 			ArrayScriptObjectZs_clear(ScriptEngine *_se,ArrayScriptObject *_this){
		ZS_UNUSUED_PARAM(_se);
		_this->eraseAllElements();
	}

	StringScriptObject *		ArrayScriptObjectZs_join(ScriptEngine *_se,ArrayScriptObject *_this, zs_int idx){
		StringScriptObject *so_string = ZS_NEW_STRING_SCRIPT_OBJECT(_se);
		String *ptr_str=so_string->str_ptr;
		Vector<StackElement *> *stk_user_list_elements=_this->getStkListElements();

		for(int i=0; i < stk_user_list_elements->length();i++){
			StackElement *stk=(StackElement *)stk_user_list_elements->get(i);
			if(i>0){
				ptr_str->append((char)idx);
			}

			if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				ptr_str->append(((ScriptObject *)stk->value)->toString());
			}
			else{
				ptr_str->append(_se->stackElementToString(stk));
			}
		}

		return so_string;
	}

	ArrayIteratorScriptObject * ArrayScriptObjectZs_iter(ScriptEngine *_se,ArrayScriptObject *so){
		ZS_UNUSUED_PARAM(_se);
		return ZS_NEW_ARRAY_ITERATOR_SCRIPT_OBJECT(_se,so);
	}

	ArrayScriptObject 			*	ArrayScriptObjectZs_concat(ScriptEngine *_se,ArrayScriptObject *_so1, ArrayScriptObject *_so2){

		return ArrayScriptObject::concat(_se,_so1, _so2);
	}

	void 							ArrayScriptObjectZs_extend(ScriptEngine *_se,ArrayScriptObject *_this, ArrayScriptObject *_sv_extend){
		ZS_UNUSUED_PARAM(_se);
		_this->concat(_sv_extend);
	}

	bool 							ArrayScriptObjectZs_contains(ScriptEngine *_se,ArrayScriptObject *sv, StackElement *stk_to_compare){
		ZS_UNUSUED_PARAM(_se);
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
					   ((ScriptObject *)stk_to_compare->value)->script_type_id ==
					   ((ScriptObject *)stk_element->value)->script_type_id
					   ){
					// particular case string
					if(((ScriptObject *)stk_to_compare->value)->script_type_id == SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT){
						found=((ScriptObject *)stk_to_compare->value)->toString()==((ScriptObject *)stk_element->value)->toString();
					}
				}
				break;
			}

		}
		return found;
	}

	bool 							ArrayScriptObjectZs_equal(ScriptEngine *_se,ArrayScriptObject *so1, ArrayScriptObject *so2){
		ZS_UNUSUED_PARAM(_se);
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
					if(STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(stk_element_s1) && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(stk_element_s2)){
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
