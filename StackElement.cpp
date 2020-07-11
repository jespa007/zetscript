#include "ZetScript.h"

namespace zetscript{

	const char * StackElement::toString(){
		StackElement *stk=this;
		const char * result="undefined";
		if(STK_VALUE_IS_INT(stk))
			result= "int";
		else if(STK_VALUE_IS_FLOAT(stk))
			result= "float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= "bool";
		else if(STK_VALUE_IS_STRING(stk))
			result= "std::string";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "function";

		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			if(this->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
				stk=(StackElement *)stk->var_ref;
			}
			result=((ScriptVar *)stk->var_ref)->getClassName().c_str();
		}

		return result;
	}


	/*inline	std::string convertStackElementVarTypeToStr(StackElement stk_v){
			if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){
				return zs_rtti::demangle(typeid(int).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				return zs_rtti::demangle(typeid(float).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
				return zs_rtti::demangle(typeid(bool).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
				return zs_rtti::demangle(typeid(std::string).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

				ScriptClass *c = GET_SCRIPT_CLASS(this,((ScriptVar *)(stk_v.var_ref))->idx_class);

				if(c!=NULL){
					return zs_rtti::demangle(c->str_class_ptr_type);
				}
			}
			return "unknow";
		}*/

}
