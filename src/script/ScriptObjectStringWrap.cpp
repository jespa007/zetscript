/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int ScriptObjectStringWrap_size(ZetScript *_zs,ScriptObjectString *so){
		ZS_UNUSUED_PARAM(_zs);
		return ((std::string *)so->value)->length();
	}

	bool ScriptObjectStringWrap_contains(ZetScript *_zs,ScriptObjectString *so, std::string *str){
		ZS_UNUSUED_PARAM(_zs);
		return zs_strutils::contains(
				*((std::string *)(so->value))
				,*str
		);
	}

	bool ScriptObjectStringWrap_contains(ZetScript *_zs,ScriptObjectString *so, zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		return strchr(((std::string *)so->value)->c_str(),ch) != NULL;
	}

	void ScriptObjectStringWrap_clear(ZetScript *_zs,ScriptObjectString *so){
		ZS_UNUSUED_PARAM(_zs);
		((std::string *)so->value)->clear();
	}

	ScriptObjectString *ScriptObjectStringWrap_replace(ZetScript *_zs,ScriptObjectString *str_in,std::string *str_old, std::string *str_new){
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(_zs);
		str_out->set(zs_strutils::replace(*((std::string *)str_in->value),*str_old,*str_new));
		return str_out;
	}

	void ScriptObjectStringWrap_eraseAt(ZetScript *_zs,ScriptObjectString *so, zs_int idx){
		ZS_UNUSUED_PARAM(_zs);
		std::string *str=((std::string *)so->value);
		str->erase(idx);
	}

	void ScriptObjectStringWrap_insertAt(ZetScript *_zs,ScriptObjectString *so, zs_int idx,zs_int ch){
		ZS_UNUSUED_PARAM(_zs);
		std::string *str=((std::string *)so->value);
		str->insert(str->begin()+idx,ch);
	}

	ScriptObjectVector * ScriptObjectStringWrap_split(ZetScript *_zs,ScriptObjectString *so,zs_int ch_delim){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ScriptObjectVector *sv=ZS_NEW_OBJECT_VECTOR(_zs);

		auto v=zs_strutils::split(so->toString(),ch_delim);

		for(unsigned i=0; i<v.size(); i++){

			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so_partial=ZS_NEW_OBJECT_STRING(_zs);
			so_partial->set(v[i]);

			// create and share pointer
			if(!vm_create_shared_script_object(vm,so_partial)){
				THROW_RUNTIME_ERRORF("cannot create shared pointer");
			}
			if(!vm_share_script_object(vm,so_partial)){
				THROW_RUNTIME_ERRORF("cannot share pointer");
			}

			stk->value=(zs_int)so_partial;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;
	}

	ScriptObjectVector * ScriptObjectStringWrap_split(ZetScript *_zs,ScriptObjectString *so, std::string * str_token){
		VirtualMachine *vm=_zs->getVirtualMachine();
		ScriptObjectVector *sv=ZS_NEW_OBJECT_VECTOR(_zs);

		auto v=zs_strutils::split(so->toString(),*str_token);

		for(unsigned i=0; i<v.size(); i++){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so_partial=ZS_NEW_OBJECT_STRING(_zs);
			so_partial->set(v[i]);

			// create and share pointer
			if(!vm_create_shared_script_object(vm,so_partial)){
				THROW_RUNTIME_ERRORF("cannot creat shared pointer");
			}
			if(!vm_share_script_object(vm,so_partial)){
				THROW_RUNTIME_ERRORF("cannot share pointer");
			}

			stk->value=(zs_int)so_partial;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;

			// deallocate allocated string
			//delete (std::string *)v[i];

		}

		return sv;
	}

	zs_int ScriptObjectStringWrap_indexOf(ZetScript *_zs,ScriptObjectString *so,zs_int search){
		ZS_UNUSUED_PARAM(_zs);
		std::string *str=((std::string *)so->value);
		std::string pattern="";
		pattern+=(char)+search;
		return zs_strutils::index_of(*str,pattern);
		//str->indexOf
	}

	zs_int ScriptObjectStringWrap_indexOf(ZetScript *_zs,ScriptObjectString *so,std::string *search){
		ZS_UNUSUED_PARAM(_zs);
		std::string *str=((std::string *)so->value);
		return zs_strutils::index_of(*str,*search);
	}

	bool ScriptObjectStringWrap_startsWith(ZetScript *_zs,ScriptObjectString *so,std::string *prefix){
		ZS_UNUSUED_PARAM(_zs);
		std::string *str=((std::string *)so->value);
		return zs_strutils::starts_with(*str,*prefix);
	}

	bool ScriptObjectStringWrap_endsWith(ZetScript *_zs,ScriptObjectString *so,std::string *suffix){
		ZS_UNUSUED_PARAM(_zs);
		std::string *str=((std::string *)so->value);
		return zs_strutils::ends_with(*str,*suffix);
	}

	ScriptObjectString * ScriptObjectStringWrap_substring(ZetScript *_zs,ScriptObjectString *str_in,zs_int start,zs_int end){
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(_zs);
		str_out->set(str_in->toString().substr(start,end));
		return str_out;
	}

	void 							ScriptObjectStringWrap_append(ZetScript *_zs,ScriptObjectString *str_in,ScriptObjectString *str_append){
		ZS_UNUSUED_PARAM(_zs);
		*(std::string *)str_in->value+=*(std::string *)(str_append->value);
	}

	ScriptObjectIteratorString * ScriptObjectStringWrap_iter(ZetScript *_zs,ScriptObjectString *so){
		ZS_UNUSUED_PARAM(_zs);
		return ZS_NEW_OBJECT_ITERATOR_STRING(so);
	}
}
