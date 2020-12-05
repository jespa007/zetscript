#include "zetscript.h"

namespace zetscript{


	ScriptObjectString * StringBuiltIn::formatSf(ZetScript *zs,StackElement *str, StackElement *args){
		std::string first_param=str->toString();
		ScriptObjectVector *sov=NULL;

		if(args->properties & MSK_STK_PROPERTY_PTR_STK){
			args=(StackElement *)args->var_ref;
		}

		if(args->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)args->var_ref;
			if(so->idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR){
				sov=(ScriptObjectVector *)so;
			}
		}

		if(sov != NULL){
			StackElement **stk_it=sov->getUserProperties();
			for(unsigned i=0; i < sov->countUserProperties(); i++){
				first_param=zs_strutils::replace(first_param,zs_strutils::format("{%i}",i),(*stk_it)->toString());
				stk_it++;
			}
		}
		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=new ScriptObjectString(zs);
		str_out->str_value=first_param;//str_in->str_value;
		return str_out;
	}
}
