namespace zetscript{

	const char * indexInstructionVarTypeToStr(StackElement *ptr_info_ale){
		const char * result="undefined";
		if(STK_VALUE_IS_INT(ptr_info_ale->properties))
			result= "int";
		else if(STK_VALUE_IS_FLOAT(ptr_info_ale->properties))
			result= "float";
		else if(STK_VALUE_IS_BOOLEAN(ptr_info_ale->properties))
			result= "bool";
		else if(STK_VALUE_IS_STRING(ptr_info_ale->properties))
			result= "std::string";
		else if(STK_VALUE_IS_FUNCTION(ptr_info_ale->properties))
			result= "function";

		else if(STK_VALUE_IS_SCRIPT_VAR(ptr_info_ale->properties)){
			StackElement *var=ptr_info_ale;
			if(ptr_info_ale->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
				var=(StackElement *)var->var_ref;
			}
			result=((ScriptVar *)var->var_ref)->getClassName().c_str();
		}

		return result;
	}


	inline	std::string StackElementVarTypeToStr(StackElement stk_v){
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
		}

}
