#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	static int n_eval_function=0;

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
		Scope *main_scope=zs->getScopeFactory()->getMainScope();
		std::string str_unescaped_source="";
		VirtualMachine *vm=zs->getVirtualMachine();
		Symbol *symbol_sf=NULL;

		// Example of use,
		// System::eval("a+b",{a:1,b:2})
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_so_str_eval) == false){ // expected string to evaluate
			vm_set_error(zs->getVirtualMachine(),zs_strutils::format("eval error:expected ScriptObjectString as first parameter but the typeof is '%s'",stk_so_str_eval->toString().c_str()));
			return;
		}

		so_str_eval=(ScriptObjectString *)stk_so_str_eval->value;

		if(stk_oo_param->properties != 0){ // parameters were passed
			if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_oo_param) == false){
				vm_set_error(zs->getVirtualMachine(),zs_strutils::format("eval error:expected ScriptObjectObject as second parameter but the typeof is '%'",stk_oo_param->toString().c_str()));
				return;
			}

			oo_param=(ScriptObjectObject *)stk_oo_param->value;

			// catch parameters...
			for(auto it=oo_param->begin(); !it.end(); it.next()){
				function_args.push_back(ScriptFunctionArg(it.getKey()));
				stk_params.push_back(*((StackElement *)it.getValue()));
			}
		}

		// 1. Create lambda function that configures and call with entered parameters like this
		//    function(a,b){a+b}(1,2);
		/*Symbol *symbol_sf=sf_main->registerLocalFunction(
				main_scope
				,""
				, -1
				, eval_anonymous_function_name()
				, function_args
		);*/
		symbol_sf=new Symbol(
				);

		symbol_sf->name=zs_strutils::format("eval@",n_eval_function++);

		 sf=new	ScriptFunction(
				zs
				,IDX_SCRIPT_CLASS_MAIN
				,ZS_IDX_EVAL_FUNCTION
				,function_args
				,ZS_IDX_UNDEFINED
				,symbol_sf
				,0
		);


		str_unescaped_source=zs_strutils::unescape(so_str_eval->toString());
		str_start=str_unescaped_source.c_str();
		// 2. Call zetscript->eval this function

		try{
			eval_parse_and_compile(zs,str_start,NULL,1,sf,&function_args);
		}catch(std::exception & ex){
			delete sf;
			delete symbol_sf;
			vm_set_error(zs->getVirtualMachine(),std::string("eval error:")+ex.what());
			return;
		}

		// check if there's a reset stack at the end and set as end function in order to get last value stk ...
		if(sf->instructions_len>2){
			if(sf->instructions[sf->instructions_len-2].byte_code == BYTE_CODE_RESET_STACK){
				sf->instructions[sf->instructions_len-2].byte_code =BYTE_CODE_RET;
			}
		}

		// 3. Call function passing all arg parameter

		// add data stk_vm_current
		uint8_t stk_start_size=(uint8_t)stk_params.size();
		StackElement *stk_vm_current=vm_get_current_stack_element(vm);
		StackElement *stk_start=stk_vm_current;//vm data->stk_vm_current;
		for(unsigned i = 0; i < stk_start_size; i++){
			*stk_start++=stk_params[i];
		}

		vm_call_function_script(
			zs->getVirtualMachine(),
			NULL,
			sf,
			stk_vm_current,
			stk_start_size);

		// modifug
		if(vm_it_has_error(zs->getVirtualMachine())){
			std::string error=vm_get_error(zs->getVirtualMachine());
			vm_set_error(zs->getVirtualMachine(),zs_strutils::format("eval error %s",error.c_str()));
		}

		//vm_push_stack_element(zs->getVirtualMachine(),stk_ret);

		 delete sf;
		 delete symbol_sf;

	}

	void ModuleSystemWrap_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args){
		if(*chk_assert == false){
			ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
			vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
			delete str_out;
		}
	}

	void ModuleSystemWrap_error(ZetScript *zs, StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
		delete str_out;

	}
}

