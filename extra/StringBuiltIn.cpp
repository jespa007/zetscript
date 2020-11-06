#include "zetscript.h"

namespace zetscript{

	void StringBuiltIn::constructorSf(StringBuiltIn *built_in_string,ZetScript *zs){
		built_in_string->zs=zs;
	}

	ScriptObjectString * StringBuiltIn::formatSf(StringBuiltIn *built_in_string,StackElement *str, StackElement *args){
		return built_in_string->format(str, args);
	}

	ScriptObjectString * StringBuiltIn::format(StackElement *str, StackElement *args){
		if(((str->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING) == 0) || str->var_ref == NULL){
			//THROW_EXCEPTION("StringBuiltIn::format expected first parameter as string");
		}

		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=new ScriptObjectString(zs);
		str_out->str_value="dajdjasldas";//str_in->str_value;
		return str_out;
	}
}
