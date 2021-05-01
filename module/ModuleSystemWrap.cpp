#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	zs_int ModuleSystemWrap_clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	void ModuleSystemWrap_makeReadOnly(StackElement *stk){
		stk->properties|=STK_PROPERTY_READ_ONLY;
	}

	void ModuleSystemWrap_eval(ZetScript *zs,StackElement *str_eval,StackElement *object_args){
		// Example of use,
		// System::eval("a+b",{a:1,b:2})

		//ScriptObjectString *str_out=ScriptObjectString::formatSf(zs,str,arg_values);
		// 1. Create anonymouse function
		// 2. Eval
		// 3. Call function passing all arg parameter
		printf("not implemented");
		//delete str_out;
	}

	void ModuleSystemWrap_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args){
		if(*chk_assert == false){
			ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
			vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
			delete str_out;
		}
	}
}

