/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int ScriptObjectStringWrap_size(ScriptObjectString *so){
		return ((zs_string *)so->value)->length();
	}

	bool ScriptObjectStringWrap_contains(ScriptObjectString *so, zs_string *str){
		return zs_strutils::contains(
				*((zs_string *)(so->value))
				,*str
		);
	}

	bool ScriptObjectStringWrap_contains(ScriptObjectString *so, zs_int ch){
		return strchr(((zs_string *)so->value)->c_str(),ch) != NULL;
	}


	void ScriptObjectStringWrap_clear(ScriptObjectString *so){
		((zs_string *)so->value)->clear();
	}

	ScriptObjectString *ScriptObjectStringWrap_replace(ScriptObjectString *str_in,zs_string *str_old, zs_string *str_new){
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(str_in->getZetScript());
		str_out->set(zs_strutils::replace(*((zs_string *)str_in->value),*str_old,*str_new));
		return str_out;
	}

	void ScriptObjectStringWrap_eraseAt(ScriptObjectString *so, zs_int idx){
		zs_string *str=((zs_string *)so->value);
		str->erase(idx);
	}

	void ScriptObjectStringWrap_insertAt(ScriptObjectString *so, zs_int idx,zs_int ch){
		zs_string *str=((zs_string *)so->value);
		str->insert(idx,ch);
	}

	ScriptObjectVector * ScriptObjectStringWrap_split(ScriptObjectString *so,zs_int ch_delim){
		VirtualMachine *vm=so->getZetScript()->getVirtualMachine();
		ScriptObjectVector *sv=ZS_NEW_OBJECT_VECTOR(so->getZetScript());

		auto v=zs_strutils::split(so->toString(),ch_delim);

		for(unsigned i=0; i<v.count; i++){

			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so_partial=ZS_NEW_OBJECT_STRING(so->getZetScript());
			so_partial->set(*(zs_string *)v.items[i]);

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

	ScriptObjectVector * ScriptObjectStringWrap_split(ScriptObjectString *so, zs_string * str_token){
		VirtualMachine *vm=so->getZetScript()->getVirtualMachine();
		ScriptObjectVector *sv=ZS_NEW_OBJECT_VECTOR(so->getZetScript());

		auto v=zs_strutils::split(so->toString(),*str_token);

		for(unsigned i=0; i<v.count; i++){
			StackElement *stk=sv->pushNewUserSlot();
			ScriptObjectString *so_partial=ZS_NEW_OBJECT_STRING(so->getZetScript());
			so_partial->set(*((zs_string *)v.items[i]));

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
		zs_string *str=((zs_string *)so->value);
		zs_string pattern="";
		pattern+=(char)+search;
		return zs_strutils::index_of(*str,pattern);
		//str->indexOf
	}

	zs_int ScriptObjectStringWrap_indexOf(ScriptObjectString *so,zs_string *search){
		zs_string *str=((zs_string *)so->value);
		return zs_strutils::index_of(*str,*search);
	}

	bool ScriptObjectStringWrap_startsWith(ScriptObjectString *so,zs_string *prefix){
		zs_string *str=((zs_string *)so->value);
		return zs_strutils::starts_with(*str,*prefix);
	}

	bool ScriptObjectStringWrap_endsWith(ScriptObjectString *so,zs_string *suffix){
		zs_string *str=((zs_string *)so->value);
		return zs_strutils::ends_with(*str,*suffix);
	}

	ScriptObjectString * ScriptObjectStringWrap_substring(ScriptObjectString *str_in,zs_int start,zs_int end){
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(str_in->getZetScript());
		str_out->set(str_in->toString().substr(start,end));
		return str_out;
	}

	void 							ScriptObjectStringWrap_append(ScriptObjectString *str_in,ScriptObjectString *str_append){
		*(zs_string *)str_in->value+=*(zs_string *)(str_append->value);
	}

	ScriptObjectStringIterator * ScriptObjectStringWrap_iter(ScriptObjectString *so){
		return ZS_NEW_OBJECT_STRING_ITERATOR(so);
	}
}
