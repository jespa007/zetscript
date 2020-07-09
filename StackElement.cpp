
#include "ZetScript.h"


namespace zetgine{

	std::string StackElement::VarTypeToStr(){
		if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){
			return zs_rtti::Demangle(typeid(int).name());
		}else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
			return zs_rtti::Demangle(typeid(float).name());
		}else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
			return zs_rtti::Demangle(typeid(bool).name());
		}else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
			return zs_rtti::Demangle(typeid(std::string).name());
		}else if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

			ScriptClass *c = GET_SCRIPT_CLASS(((ScriptVar *)(stk_v.var_ref))->idx_class);

			if(c!=NULL){
				return zs_rtti::Demangle(c->str_class_ptr_type);
			}
		}
		return "unknow";
	}
}
