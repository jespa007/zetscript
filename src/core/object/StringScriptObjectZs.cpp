/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int StringScriptObjectZs_length(ZetScript *_zs,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return so->str_ptr->length();
	}

	bool StringScriptObjectZs_contains(ZetScript *_zs,StringScriptObject *so, String *str){
		ZS_UNUSUED_PARAM(_zs);
		return (*so->str_ptr).contains(
				*str
		);
	}

	bool StringScriptObjectZs_contains(ZetScript *_zs,StringScriptObject *so, zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		return strchr(so->str_ptr->toConstChar(),ch) != NULL;
	}

	void StringScriptObjectZs_clear(ZetScript *_zs,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_zs);
		so->str_ptr->clear();
	}

	StringScriptObject *StringScriptObjectZs_replace(ZetScript *_zs,StringScriptObject *str_in,String *str_old, String *str_new){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
		str_out->set((str_in->str_ptr)->replace(*str_old,*str_new));
		return str_out;
	}

	void StringScriptObjectZs_eraseAt(ZetScript *_zs,StringScriptObject *so, zs_int idx){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		str->erase(idx);
	}

	void StringScriptObjectZs_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		str->insert(idx,ch);
	}

	void StringScriptObjectZs_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,String * _str){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		str->insert(idx,*_str);
	}

	ArrayScriptObject * StringScriptObjectZs_split(ZetScript *_zs,StringScriptObject *so,zs_int ch_delim){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv=ZS_NEW_ARRAY_SCRIPT_OBJECT(_zs);

		auto v=so->toString().split(ch_delim);

		for(int i=0; i<v.length(); i++){

			StackElement *stk=sv->newSlot();
			StringScriptObject *so_partial=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
			so_partial->set(v.get(i));

			// create and share pointer
			vm_create_shared_object(vm,so_partial);
			vm_share_object(vm,so_partial);

			stk->value=(zs_int)so_partial;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;
		}

		return sv;
	}

	ArrayScriptObject * StringScriptObjectZs_split(ZetScript *_zs,StringScriptObject *so, String * str_token){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv=ZS_NEW_ARRAY_SCRIPT_OBJECT(_zs);

		auto v=so->toString().split(*str_token);

		for(int i=0; i<v.length(); i++){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so_partial=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
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

	zs_int StringScriptObjectZs_indexOf(ZetScript *_zs,StringScriptObject *so,zs_int search){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		String pattern="";
		pattern+=(char)+search;
		return str->find(pattern, 0);
	}

	zs_int StringScriptObjectZs_indexOf(ZetScript *_zs,StringScriptObject *so,String *search){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		return str->indexOf(*search);
	}

	bool StringScriptObjectZs_startsWith(ZetScript *_zs,StringScriptObject *so,String *prefix){
		ZS_UNUSUED_PARAM(_zs);
		String *str=so->str_ptr;
		return str->startsWith(*prefix);
	}

	bool StringScriptObjectZs_endsWith(ZetScript *_zs,StringScriptObject *so,String *suffix){
		ZS_UNUSUED_PARAM(_zs);
		return so->str_ptr->endsWith(*suffix);
	}

	void 							StringScriptObjectZs_append(ZetScript *_zs,StringScriptObject *str_in,StringScriptObject *str_append){
		ZS_UNUSUED_PARAM(_zs);
		(str_in->str_ptr)->append(*(str_append->str_ptr));
	}

	void 							StringScriptObjectZs_append(ZetScript *_zs,StringScriptObject *str_in,zs_int _ch){
		ZS_UNUSUED_PARAM(_zs);
		(str_in->str_ptr)->append(_ch);
	}

	StringScriptObject * 	StringScriptObjectZs_toLowerCase(ZetScript *_zs,StringScriptObject *so){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
		str_out->set(so->toString().toLowerCase());
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectZs_toUpperCase(ZetScript *_zs,StringScriptObject *so){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
		str_out->set(so->toString().toUpperCase());
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectZs_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index, zs_int _end_index){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_zs);
		str_out->set(_so->toString().substring(_start_index, _end_index));
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectZs_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index){
		return StringScriptObjectZs_substring(_zs, _so,  _start_index,-1);
	}

	StringIteratorScriptObject * StringScriptObjectZs_iter(ZetScript *_zs,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_STRING_ITERATOR_SCRIPT_OBJECT(_zs,so);
	}
}
