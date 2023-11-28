/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int StringObjectZs_length(ZetScript *_zs,StringObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return so->str_ptr->length();
	}

	bool StringObjectZs_contains(ZetScript *_zs,StringObject *so, String *str){
		ZS_UNUSUED_PARAM(_zs);
		return String::contains(
				*(so->str_ptr)
				,*str
		);
	}

	bool StringObjectZs_contains(ZetScript *_zs,StringObject *so, zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		return strchr(so->str_ptr->toConstChar(),ch) != NULL;
	}

	void StringObjectZs_clear(ZetScript *_zs,StringObject *so){
		ZS_UNUSUED_PARAM(_zs);
		so->str_ptr->clear();
	}

	StringObject *StringObjectZs_replace(ZetScript *_zs,StringObject *str_in,String *str_old, String *str_new){
		StringObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(String::replace(*(str_in->str_ptr),*str_old,*str_new));
		return str_out;
	}

	void StringObjectZs_eraseAt(ZetScript *_zs,StringObject *so, zs_int idx){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		str->erase(idx);
	}

	void StringObjectZs_insertAt(ZetScript *_zs,StringObject *so, zs_int idx,zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		str->insert(idx,ch);
	}

	void StringObjectZs_insertAt(ZetScript *_zs,StringObject *so, zs_int idx,String * _str){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		str->insert(idx,*_str);
	}

	ArrayObject * StringObjectZs_split(ZetScript *_zs,StringObject *so,zs_int ch_delim){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayObject *sv=ZS_NEW_ARRAY_OBJECT(_zs);

		auto v=String::split(so->toString(),ch_delim);

		for(int i=0; i<v.size(); i++){

			StackElement *stk=sv->newSlot();
			StringObject *so_partial=ZS_NEW_STRING_OBJECT(_zs);
			so_partial->set(v.get(i));

			// create and share pointer
			vm_create_shared_object(vm,so_partial);
			vm_share_object(vm,so_partial);

			stk->value=(zs_int)so_partial;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;
		}

		return sv;
	}

	ArrayObject * StringObjectZs_split(ZetScript *_zs,StringObject *so, String * str_token){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayObject *sv=ZS_NEW_ARRAY_OBJECT(_zs);

		auto v=String::split(so->toString(),*str_token);

		for(int i=0; i<v.size(); i++){
			StackElement *stk=sv->newSlot();
			StringObject *so_partial=ZS_NEW_STRING_OBJECT(_zs);
			so_partial->set(v.get(i));

			// create and share pointer
			vm_create_shared_object(vm,so_partial);
			vm_share_object(vm,so_partial);

			stk->value=(zs_int)so_partial;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;

			// deallocate allocated string
			//delete (String *)v.items[i];

		}

		return sv;
	}

	zs_int StringObjectZs_indexOf(ZetScript *_zs,StringObject *so,zs_int search){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		String pattern="";
		pattern+=(char)+search;
		return String::indexOf(*str,pattern);
		//str->indexOf
	}

	zs_int StringObjectZs_indexOf(ZetScript *_zs,StringObject *so,String *search){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		return String::indexOf(*str,*search);
	}

	bool StringObjectZs_startsWith(ZetScript *_zs,StringObject *so,String *prefix){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		return StringUtils::startsWith(*str,*prefix);
	}

	bool StringObjectZs_endsWith(ZetScript *_zs,StringObject *so,String *suffix){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		return String::endsWith(*str,*suffix);
	}

	void 							StringObjectZs_append(ZetScript *_zs,StringObject *str_in,StringObject *str_append){
		ZS_UNUSUED_PARAM(_zs);
		(str_in->str_ptr)->append(*(str_append->str_ptr));
	}

	void 							StringObjectZs_append(ZetScript *_zs,StringObject *str_in,zs_int _ch){
		ZS_UNUSUED_PARAM(_zs);
		(str_in->str_ptr)->append(_ch);
	}

	StringObject * 	StringObjectZs_toLowerCase(ZetScript *_zs,StringObject *so){
		StringObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(StringUtils::toLower(so->toString()));
		return str_out;
	}

	StringObject * 	StringObjectZs_toUpperCase(ZetScript *_zs,StringObject *so){
		StringObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(String::toUpper(so->toString()));
		return str_out;
	}

	StringObject * 	StringObjectZs_subassignring(ZetScript *_zs,StringObject *_so, zs_int _start_index, zs_int _end_index){
		StringObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(String::substring(_so->toString(),_start_index, _end_index));
		return str_out;
	}

	StringObject * 	StringObjectZs_subassignring(ZetScript *_zs,StringObject *_so, zs_int _start_index){
		return StringObjectZs_subassignring(_zs, _so,  _start_index,-1);
	}

	StringIteratorObject * StringObjectZs_iter(ZetScript *_zs,StringObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_OBJECT_ITERATOR_ASSIGNRING(_zs,so);
	}
}
