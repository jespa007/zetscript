/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	static int n_eval_function=0;

	zs_int SystemModule_clock(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		//return std::clock()*(1000.0f/CLOCKS_PER_SEC);
		//return std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void SystemModule_makeReadOnly(ZetScript *_zs,StackElement *stk){
		ZS_UNUSUED_PARAM(_zs);
		stk->properties|=STK_PROPERTY_READ_ONLY;
	}

	void SystemModule_eval(ZetScript *zs,StackElement *stk_so_str_eval,StackElement *stk_oo_param){
		ScriptObjectString *so_str_eval=NULL;
		ScriptObjectObject *oo_param=NULL;
		ScriptFunctionParam *function_params=NULL;
		ScriptFunctionParam **function_params_ptr=NULL;
		int 				 function_params_len=0;
		zs_string str_param_name;
		ScriptFunction *sf;
		zs_vector stk_params;
		StackElement stk_ret=k_stk_undefined;
		const char *str_start=NULL;
		zs_string str_unescaped_source="";
		VirtualMachine *vm=zs->getVirtualMachine();
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		Symbol *symbol_sf=NULL;
		int n_ret_args=0;
		uint8_t stk_n_params=0;
		StackElement *stk_vm_current=NULL;
		StackElement *stk_start=NULL;

		// Example of use,
		// System::eval("a+b",{a:1,b:2})
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_so_str_eval) == false){ // expected string to evaluate
			vm_set_error(
					zs->getVirtualMachine()
					,zs_strutils::format("eval error:expected ScriptObjectString as first parameter but the typeof is '%s'"
							,stk_to_typeof_str(data->zs,stk_so_str_eval).c_str())
			);
			return;
		}

		so_str_eval=(ScriptObjectString *)stk_so_str_eval->value;

		//--------------------------------------
		// 0. setup scope and parameters
		if(stk_oo_param->properties != 0){

			if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_oo_param) == false){
				vm_set_error(
						zs->getVirtualMachine()
						,zs_strutils::format("eval error:expected ScriptObjectObject as second parameter but the typeof is '%'"
								,stk_to_typeof_str(data->zs,stk_oo_param).c_str())
				);
				return;
			}

			oo_param=(ScriptObjectObject *)stk_oo_param->value;
			function_params_len=oo_param->length();
			if(function_params_len>0){

				function_params=new ScriptFunctionParam[function_params_len];//(ScriptFunctionParam *)ZS_MALLOC(sizeof(ScriptFunctionParam)*function_params_len);
				function_params_ptr=&function_params;

				if(function_params!=NULL){
					int i=0;
					zs_map *map=oo_param->getMapUserProperties();

					for(auto it=map->begin(); !it.end(); it.next()){
						StackElement *stk=((StackElement *)it.value);
						function_params[i]=ScriptFunctionParam(it.key);
						stk_params.push_back((zs_int)stk);

						if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
							// inc number of ref as standard in pass object args
							((ScriptObject *)stk->value)->shared_pointer->data.n_shares++;
						}

						i++;
					}
				}
			}
		}

		//--------------------------------------
		// 1. Create lambda function that configures and call with entered parameters like this
		//    function(a,b){a+b}(1,2);
		zs_string  name_script_function=zs_strutils::format("eval@",n_eval_function++);
		sf=new	ScriptFunction(
				zs
				,IDX_ZS_SCRIPT_FUNCTION_EVAL
				,IDX_TYPE_CLASS_MAIN
				,-1
				,name_script_function
				,function_params_ptr
				,function_params_len
				,ZS_IDX_UNDEFINED
				,0
				,0
		);

		Scope *main_scope=((((zs)->getScopeFactory())))->getMainScope();
		sf->scope_script_function=(((zs)->getScopeFactory()))->newScope(sf->idx_script_function,main_scope,SCOPE_PROPERTY_IS_SCOPE_FUNCTION);
		main_scope->scopes->push_back((zs_int)sf->scope_script_function);

		//--------------------------------------
		// 2. register arg symbols
		// catch parameters...
		if(function_params_len > 0){
			zs_map *map=oo_param->getMapUserProperties();

			for(auto it=map->begin(); !it.end(); it.next()){

				if(sf->registerLocalArgument(
						sf->scope_script_function
						,__FILE__
						,__LINE__
						,it.key
				,0)==NULL){
					goto goto_eval_exit;
				}

			}
		}

		str_unescaped_source=zs_strutils::unescape(so_str_eval->toString());
		str_start=str_unescaped_source.c_str();

		// 3. Call zetscript->eval this function
		try{
			eval_parse_and_compile(zs,str_start,NULL,1,sf/*,function_params,function_params_len*/);
		}catch(std::exception & ex){
			vm_set_error(zs->getVirtualMachine(),zs_string("eval error:")+ex.what());
			goto goto_eval_exit;
		}

		// link unresoved symbols
		zs->link();

		// check if there's a reset stack at the end and set as end function in order to get last value stk ...
		if(sf->instructions_len>2){
			if(sf->instructions[sf->instructions_len-2].byte_code != BYTE_CODE_RET){
				int offset_rst_stack=sf->instructions[sf->instructions_len-2].byte_code == BYTE_CODE_RESET_STACK ? 1:0;
				size_t new_buf_len=sf->instructions_len+2;
				Instruction *new_buf=(Instruction *)ZS_MALLOC(new_buf_len*sizeof(Instruction));
				memcpy(new_buf,sf->instructions,sf->instructions_len*sizeof(Instruction));
				// free old ptr
				free(sf->instructions);

				// assign ret null
				new_buf[new_buf_len-3-offset_rst_stack].byte_code=BYTE_CODE_LOAD_UNDEFINED;
				new_buf[new_buf_len-2-offset_rst_stack].byte_code=BYTE_CODE_RET;

				sf->instructions=new_buf;
				sf->instructions_len=new_buf_len;
			}
		}

		// 4. Call function passing all arg parameter
		// pass data to stk_vm_current
		stk_n_params=(uint8_t)stk_params.count;
		stk_vm_current=vm_get_current_stack_element(vm);
		stk_start=stk_vm_current;//vm data->stk_vm_current;
		for(unsigned i = 0; i < stk_n_params; i++){
			*stk_start++=*((StackElement *)stk_params.items[i]);
		}

		vm_call_function_script(
			zs->getVirtualMachine(),
			NULL,
			sf,
			stk_vm_current);

		// modifug
		if(vm_it_has_error(zs->getVirtualMachine())){
			zs_string error=vm_get_error(zs->getVirtualMachine());
			vm_set_error(zs->getVirtualMachine(),zs_strutils::format("eval error %s",error.c_str()));
		}

		n_ret_args=vm_get_current_stack_element(vm)-stk_vm_current;
		stk_start=stk_vm_current;

		// overwrite first entered params due are the objects passed before and now are undefined
		for(int i = 0; i < n_ret_args-stk_n_params; i++){
			*stk_start++=*(stk_vm_current+stk_n_params+i);
		}

		// avoid pass params
		data->stk_vm_current-=stk_n_params;
goto_eval_exit:

		 delete sf;
		 delete symbol_sf;

		 sf->scope_script_function->removeChildrenBlockTypes();
		 sf->scope_script_function->markAsUnusued();
		 zs->getScopeFactory()->clearUnusuedScopes();
	}

	/*void SystemModule_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args){
		if(*chk_assert == false){
			ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
			vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
			delete str_out;
		}
	}*/

	void SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
		delete str_out;

	}

}

