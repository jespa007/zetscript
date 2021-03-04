#include "zetscript.h"

namespace zetscript{


	ScriptObjectString * StringBuiltIn::formatSf(ZetScript *zs,StackElement *str, StackElement *args){
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
				first_param=zs_strutils::replace(first_param,zs_strutils::format("{%i}",i),sov->getElementAt(i)->toString());
			}
		}
		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(zs);
		str_out->set(first_param);//str_in->str_value;
		return str_out;
	}
}
