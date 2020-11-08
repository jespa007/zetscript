#include "zetscript.h"

namespace zetscript{

	zs_int SystemBuiltIn::clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	void SystemBuiltIn::printlnSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(zs,str,args);
		printf("%s\n",str_out->str_value.c_str());
		delete str_out;
	}


	void SystemBuiltIn::printSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(zs,str,args);
		printf("%s",str_out->str_value.c_str());
		delete str_out;
	}

	void SystemBuiltIn::makeReadOnly(StackElement *stk){
		stk->properties|=MSK_STACK_ELEMENT_PROPERTY_READ_ONLY;
	}


	void SystemBuiltIn::evalSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(zs,str,args);
		printf("not implemented");
		delete str_out;
	}
}

