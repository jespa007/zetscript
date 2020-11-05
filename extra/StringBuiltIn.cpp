#include "zetscript.h"

namespace zetscript{

	ScriptObjectString * StringBuiltIn::format(StackElement *str, StackElement *args){
		if(((str->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) == 0) || str->var_ref == NULL){
			THROW_EXCEPTION("StringBuiltIn::format expected first parameter as string");
		}

		ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=new ScriptObjectString(str_in->getZetScript());
		str_out->str_value=str_in->str_value;
		return str_out;
	}
}
