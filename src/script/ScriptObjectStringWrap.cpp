/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int ScriptObjectStringWrap_size(ScriptObjectString *so){
		return ((std::string *)so->value)->size();
	}

	bool ScriptObjectStringWrap_contains(ScriptObjectString *so, std::string *str){
		return zs_strutils::contains(
				*((std::string *)(so->value))
				,*str
		);
	}

	bool ScriptObjectStringWrap_contains(ScriptObjectString *so, zs_int ch){
		return strchr(((std::string *)so->value)->c_str(),ch) != NULL;
	}


	void ScriptObjectStringWrap_clear(ScriptObjectString *so){
		((std::string *)so->value)->clear();
	}

	ScriptObjectString *ScriptObjectStringWrap_replace(ScriptObjectString *str_in,std::string *str_old, std::string *str_new){
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(str_in->getZetScript());
		str_out->set(zs_strutils::replace(*((std::string *)str_in->value),*str_old,*str_new));
		return str_out;
	}

	void ScriptObjectStringWrap_eraseAt(ScriptObjectString *so, zs_int idx){
		std::string *str=((std::string *)so->value);
		str->erase(str->begin()+idx);
	}

	void ScriptObjectStringWrap_insertAt(ScriptObjectString *so, zs_int idx,zs_int ch){
		std::string *str=((std::string *)so->value);
		str->insert(str->begin()+idx,ch);
	}

	ScriptObjectVector * ScriptObjectStringWrap_split(ScriptObjectString *so,zs_int ch_delim){
		VirtualMachine *vm=so->getZetScript()->getVirtualMachine();
		ScriptObjectVector *sv=ZS_NEW_OBJECT_VECTOR(so->getZetScript());

		auto v=zs_strutils::split(so->toString(),ch_delim);

		for(auto it=v.begin(); it!=v.end(); it++){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so_partial=ZS_NEW_OBJECT_STRING(so->getZetScript());
			so_partial->set(*it);

			// create and share pointer
			if(!vm_create_shared_pointer(vm,so_partial)){
				THROW_RUNTIME_ERROR("cannot creat shared pointer");
			}
			if(!vm_share_pointer(vm,so_partial)){
				THROW_RUNTIME_ERROR("cannot share pointer");
			}

			stk->value=(zs_int)so_partial;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;
	}

	ScriptObjectVector * ScriptObjectStringWrap_split(ScriptObjectString *so, std::string * str_token){
		VirtualMachine *vm=so->getZetScript()->getVirtualMachine();
		ScriptObjectVector *sv=ZS_NEW_OBJECT_VECTOR(so->getZetScript());

		auto v=zs_strutils::split(so->toString(),*str_token);

		for(auto it=v.begin(); it!=v.end(); it++){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so_partial=ZS_NEW_OBJECT_STRING(so->getZetScript());
			so_partial->set(*it);

			// create and share pointer
			if(!vm_create_shared_pointer(vm,so_partial)){
				THROW_RUNTIME_ERROR("cannot creat shared pointer");
			}
			if(!vm_share_pointer(vm,so_partial)){
				THROW_RUNTIME_ERROR("cannot share pointer");
			}

			stk->value=(zs_int)so_partial;
			stk->properties = STK_PROPERTY_SCRIPT_OBJECT;
		}

		return sv;
	}

	zs_int ScriptObjectStringWrap_indexOf(ScriptObjectString *so,zs_int search){
		std::string *str=((std::string *)so->value);
		std::string pattern="";
		pattern+=(char)+search;
		return zs_strutils::index_of(*str,pattern);
		//str->indexOf
	}

	zs_int ScriptObjectStringWrap_indexOf(ScriptObjectString *so,std::string *search){
		std::string *str=((std::string *)so->value);
		return zs_strutils::index_of(*str,*search);
	}

	bool ScriptObjectStringWrap_startsWith(ScriptObjectString *so,std::string *prefix){
		std::string *str=((std::string *)so->value);
		return zs_strutils::starts_with(*str,*prefix);
	}

	bool ScriptObjectStringWrap_endsWith(ScriptObjectString *so,std::string *suffix){
		std::string *str=((std::string *)so->value);
		return zs_strutils::ends_with(*str,*suffix);
	}

	ScriptObjectString * ScriptObjectStringWrap_substring(ScriptObjectString *str_in,zs_int start,zs_int end){
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(str_in->getZetScript());
		str_out->set(zs_strutils::substring(str_in->toString(),start,end));
		return str_out;
	}

	void 							ScriptObjectStringWrap_append(ScriptObjectString *str_in,ScriptObjectString *str_append){
		*(std::string *)str_in->value+=*(std::string *)(str_append->value);
	}

	ScriptObjectStringIterator * ScriptObjectStringWrap_iter(ScriptObjectString *so){
		return ZS_NEW_OBJECT_STRING_ITERATOR(so);
	}
}
