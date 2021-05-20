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

	void ModuleSystemWrap_eval(ZetScript *zs,StackElement *stk_so_str_eval,StackElement *stk_oo_param){
		ScriptFunction *sf_main=zs->getScriptFunctionFactory()->getScriptFunction(IDX_SCRIPT_FUNCTION_MAIN);
		ScriptObjectString *so_str_eval=NULL;
		ScriptObjectObject *oo_param=NULL;
		std::vector<ScriptFunctionArg> function_args;
		std::string str_param_name;
		ScriptFunction *sf;
		std::vector<StackElement> stk_params;
		StackElement stk_ret=k_stk_undefined;
		const char *str_start=NULL;
		std::string str_unescaped_source="";

		// Example of use,
		// System::eval("a+b",{a:1,b:2})
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_so_str_eval) == false){ // expected string to evaluate
			THROW_RUNTIME_ERROR("eval:expected string as first parameter but it was '%s'",stk_so_str_eval->toString().c_str());
		}

		so_str_eval=(ScriptObjectString *)stk_so_str_eval->value;

		if(stk_oo_param->properties != 0){ // parameters were passed
			int n_param=0;
			if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_oo_param) == false){
				THROW_RUNTIME_ERROR("eval:expected params as second parameter but it was '%'",stk_oo_param->toString().c_str());
			}

			oo_param=(ScriptObjectObject *)stk_oo_param->value;

			// catch parameters...
			for(auto it=oo_param->begin(); !it.end(); it.next()){
				function_args.push_back(ScriptFunctionArg(it.getKey()));
				stk_params.push_back(*((StackElement *)it.getValue()));
				n_param++;
			}
		}

		// 1. Create lambda function that configures and call with entered parameters like this
		//    function(a,b){a+b}(1,2);
		Symbol *symbol_sf=sf_main->registerLocalFunction(
				zs->getScopeFactory()->getMainScope()
				,""
				, -1
				, eval_anonymous_function_name()
				, function_args
		);

		sf=(ScriptFunction *)symbol_sf->ref_ptr;
		str_unescaped_source=zs_strutils::unescape(stk_so_str_eval->toString());
		str_start=str_unescaped_source.c_str();
		// 2. Call zetscript->eval this function
		try{
			eval_parse_and_compile(zs,sf,str_start);
		}catch(std::exception & ex){
			vm_set_error(zs->getVirtualMachine(),std::string("eval error:")+ex.what());
			return;
		}

		// check if there's a reset stack at the end and set as end function in order to get last value stk ...
		if(sf->instructions_len>2){
			if(sf->instructions[sf->instructions_len-2].byte_code == BYTE_CODE_RESET_STACK){
				sf->instructions[sf->instructions_len-2].byte_code =BYTE_CODE_END_FUNCTION;
			}
		}

		// 3. Call function passing all arg parameter
		stk_ret=vm_execute(
			zs->getVirtualMachine()
			 ,NULL
			 ,sf
			 ,stk_params.data()
			 ,stk_params.size()
		);

		vm_push_stack_element(zs->getVirtualMachine(),stk_ret);

	}

	void ModuleSystemWrap_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args){
		if(*chk_assert == false){
			ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
			vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
			delete str_out;
		}
	}
}

