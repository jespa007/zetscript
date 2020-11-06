#include "zetscript.h"

namespace zetscript{

	void SystemBuiltIn::constructorSf(SystemBuiltIn *system_builtin,ZetScript *_zs, StringBuiltIn *_string_builtin){
		system_builtin->zs=_zs;
		system_builtin->string_builtin = _string_builtin;

		/*ScriptObject *obj=NULL;
		if(stk_string_format->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
			if((obj=(ScriptObject *)stk_string_format->var_ref) != NULL){
				ScriptClass * c_scriptclass_info = obj->getNativeClass();
				if(c_scriptclass_info->str_class_ptr_type == typeid(StringBuiltIn *).name()){
					system_builtin->string_builtin = (StringBuiltIn *)obj->getNativeObject();
				}
			}
		}*/

	}

	void SystemBuiltIn::printlnSf(SystemBuiltIn *system_builtin,StackElement *str, StackElement *args){
		system_builtin->println(str,args);
	}

	void SystemBuiltIn::printSf(SystemBuiltIn *system_builtin,StackElement *str, StackElement *args){
		system_builtin->print(str,args);
	}

	void SystemBuiltIn::evalSf(SystemBuiltIn *system_builtin,StackElement *str, StackElement *args){
		system_builtin->eval(str,args);
	}


	zs_int SystemBuiltIn::clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	void SystemBuiltIn::println(StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(string_builtin,str,args);
		printf("%s\n",str_out->str_value.c_str());
		delete str_out;
	}


	void SystemBuiltIn::print(StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(string_builtin,str,args);
		printf("%s",str_out->str_value.c_str());
		delete str_out;
	}

	void SystemBuiltIn::makeReadOnly(StackElement *stk){
		stk->properties|=MSK_STACK_ELEMENT_PROPERTY_READ_ONLY;
	}


	void SystemBuiltIn::eval(StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(string_builtin,str,args);
		printf("not implemented");
		delete str_out;
	}
}

