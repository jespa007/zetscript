/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectString * ScriptObjectStringWrap_format(ZetScript *zs,StackElement *str, StackElement *args){
		std::string first_param=str->toString();
		ScriptObjectVector *sov=NULL;

		if(args->properties & MSK_STK_PROPERTY_PTR_STK){
			args=(StackElement *)args->stk_value;
		}

		if(args->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)args->stk_value;
			if(so->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR){
				sov=(ScriptObjectVector *)so;
			}
		}

		if(sov != NULL){
			for(unsigned i=0; i < sov->length(); i++){
				first_param=zs_strutils::replace(first_param,zs_strutils::format("{%i}",i),sov->getUserElementAt(i)->toString());
			}
		}
		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(zs);
		str_out->set(first_param);//str_in->default_str_value;
		return str_out;
	}

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


	bool ScriptObjectStringWrap_exist(ScriptObjectString *so, zs_int ch){
		return false;
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

			stk->stk_value=so_partial;
			stk->properties = MSK_STK_PROPERTY_SCRIPT_OBJECT;
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

			stk->stk_value=so_partial;
			stk->properties = MSK_STK_PROPERTY_SCRIPT_OBJECT;
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
}
