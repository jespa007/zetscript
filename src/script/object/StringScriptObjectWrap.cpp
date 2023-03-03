/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int StringScriptObjectWrap_length(ZetScript *_zs,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return so->str_ptr->length();
	}

	bool StringScriptObjectWrap_contains(ZetScript *_zs,StringScriptObject *so, zs_string *str){
		ZS_UNUSUED_PARAM(_zs);
		return zs_strutils::contains(
				*(so->str_ptr)
				,*str
		);
	}

	bool StringScriptObjectWrap_contains(ZetScript *_zs,StringScriptObject *so, zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		return strchr(so->str_ptr->c_str(),ch) != NULL;
	}

	void StringScriptObjectWrap_clear(ZetScript *_zs,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_zs);
		so->str_ptr->clear();
	}

	StringScriptObject *StringScriptObjectWrap_replace(ZetScript *_zs,StringScriptObject *str_in,zs_string *str_old, zs_string *str_new){
		StringScriptObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(zs_strutils::replace(*(str_in->str_ptr),*str_old,*str_new));
		return str_out;
	}

	void StringScriptObjectWrap_eraseAt(ZetScript *_zs,StringScriptObject *so, zs_int idx){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		str->erase(idx);
	}

	void StringScriptObjectWrap_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		str->insert(idx,ch);
	}

	void StringScriptObjectWrap_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_string * _str){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		str->insert(idx,*_str);
	}

	ArrayScriptObject * StringScriptObjectWrap_split(ZetScript *_zs,StringScriptObject *so,zs_int ch_delim){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv=ZS_NEW_ARRAY_OBJECT(_zs);

		auto v=zs_strutils::split(so->toString(),ch_delim);

		for(int i=0; i<v.size(); i++){

			StackElement *stk=sv->newSlot();
			StringScriptObject *so_partial=ZS_NEW_STRING_OBJECT(_zs);
			so_partial->set(v.items[i]);

			// create and share pointer
			if(!vm_create_shared_script_object(vm,so_partial)){
				ZS_THROW_RUNTIME_ERRORF("cannot creat shared pointer");
			}
			if(!vm_share_script_object(vm,so_partial)){
				ZS_THROW_RUNTIME_ERRORF("cannot share pointer");
			}

			stk->value=(zs_int)so_partial;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;
	}

	ArrayScriptObject * StringScriptObjectWrap_split(ZetScript *_zs,StringScriptObject *so, zs_string * str_token){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ArrayScriptObject *sv=ZS_NEW_ARRAY_OBJECT(_zs);

		auto v=zs_strutils::split(so->toString(),*str_token);

		for(int i=0; i<v.size(); i++){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so_partial=ZS_NEW_STRING_OBJECT(_zs);
			so_partial->set(v.items[i]);

			// create and share pointer
			if(!vm_create_shared_script_object(vm,so_partial)){
				ZS_THROW_RUNTIME_ERRORF("cannot creat shared pointer");
			}
			if(!vm_share_script_object(vm,so_partial)){
				ZS_THROW_RUNTIME_ERRORF("cannot share pointer");
			}

			stk->value=(zs_int)so_partial;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;

			// deallocate allocated string
			//delete (zs_string *)v.items[i];

		}

		return sv;
	}

	zs_int StringScriptObjectWrap_indexOf(ZetScript *_zs,StringScriptObject *so,zs_int search){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		zs_string pattern="";
		pattern+=(char)+search;
		return zs_strutils::index_of(*str,pattern);
		//str->indexOf
	}

	zs_int StringScriptObjectWrap_indexOf(ZetScript *_zs,StringScriptObject *so,zs_string *search){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		return zs_strutils::index_of(*str,*search);
	}

	bool StringScriptObjectWrap_startsWith(ZetScript *_zs,StringScriptObject *so,zs_string *prefix){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		return zs_strutils::starts_with(*str,*prefix);
	}

	bool StringScriptObjectWrap_endsWith(ZetScript *_zs,StringScriptObject *so,zs_string *suffix){
		ZS_UNUSUED_PARAM(_zs);
		zs_string *str=so->str_ptr;
		return zs_strutils::ends_with(*str,*suffix);
	}

	void 							StringScriptObjectWrap_append(ZetScript *_zs,StringScriptObject *str_in,StringScriptObject *str_append){
		ZS_UNUSUED_PARAM(_zs);
		(str_in->str_ptr)->append(*(str_append->str_ptr));
	}

	void 							StringScriptObjectWrap_append(ZetScript *_zs,StringScriptObject *str_in,zs_int _ch){
		ZS_UNUSUED_PARAM(_zs);
		(str_in->str_ptr)->append(_ch);
	}

	StringScriptObject * 	StringScriptObjectWrap_toLowerCase(ZetScript *_zs,StringScriptObject *so){
		StringScriptObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(zs_strutils::to_lower(so->toString()));
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectWrap_toUpperCase(ZetScript *_zs,StringScriptObject *so){
		StringScriptObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(zs_strutils::to_upper(so->toString()));
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectWrap_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index, zs_int _end_index){
		StringScriptObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(zs_strutils::substring(_so->toString(),_start_index, _end_index));
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectWrap_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index){
		return StringScriptObjectWrap_substring(_zs, _so,  _start_index,-1);
	}

	StringIteratorScriptObject * StringScriptObjectWrap_iter(ZetScript *_zs,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_OBJECT_ITERATOR_STRING(_zs,so);
	}
}
