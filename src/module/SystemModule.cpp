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

	zs_float SystemModule_clock(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return zs_system::clock();
	}

	void SystemModule_makeReadOnly(ZetScript *_zs,StackElement *stk){
		ZS_UNUSUED_PARAM(_zs);
		stk->properties|=STK_PROPERTY_READ_ONLY;
	}

	void SystemModule_eval(ZetScript *zs,StackElement *stk_so_str_eval,StackElement *stk_oo_param){
		StringScriptObject *so_str_eval=NULL;
		ObjectScriptObject *oo_param=NULL;
		ScriptFunctionParam *function_params=NULL;
		ScriptFunctionParam **function_params_ptr=NULL;
		int 				 function_params_len=0;
		zs_string str_param_name;
		ScriptFunction *sf_eval=NULL;
		zs_vector<StackElement *> stk_params;
		const char *str_start=NULL;
		zs_string str_unescaped_source="";
		VirtualMachine *vm=zs->getVirtualMachine();
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		int n_ret_args=0;
		uint8_t stk_n_params=0;
		StackElement *vm_stk_current=vm_get_current_stack_element(vm);
		StackElement *stk_start=NULL;
		VM_ScopeFunction *vm_eval_scope_function=data->vm_current_scope_function;

		// Example of use,
		// System::eval("a+b",{a:1,b:2})
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_so_str_eval) == false){ // expected string to evaluate
			vm_set_error(
					zs->getVirtualMachine()
					,zs_strutils::format("eval error:expected StringScriptObject as first parameter but the typeof is '%s'"
							,data->zs->stackElementToStringTypeOf(stk_so_str_eval).c_str()
					).c_str()
			);
			return;
		}

		so_str_eval=(StringScriptObject *)stk_so_str_eval->value;

		//--------------------------------------
		// 0. setup scope and parameters
		if(stk_oo_param->properties != 0){

			if(STK_IS_SCRIPT_OBJECT_OBJECT(stk_oo_param) == false){
				vm_set_error(
					zs->getVirtualMachine()
					,zs_strutils::format(
						"eval error:expected 'Object' as second parameter but the typeof is '%s'"
						,data->zs->stackElementToStringTypeOf(stk_oo_param).c_str()
					).c_str()
				);
				return;
			}

			oo_param=(ObjectScriptObject *)stk_oo_param->value;
			function_params_len=oo_param->length();
			if(function_params_len>0){

				function_params=new ScriptFunctionParam[function_params_len];//(ScriptFunctionParam *)ZS_MALLOC(sizeof(ScriptFunctionParam)*function_params_len);
				function_params_ptr=&function_params;

				if(function_params!=NULL){
					int i=0;
					zs_map *fields=oo_param->getMapFields();

					for(auto it=fields->begin(); !it.end(); it.next()){
						StackElement *stk=((StackElement *)it.value);
						function_params[i]=ScriptFunctionParam(it.key);
						stk_params.push_back(stk);

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
		zs_string  name_script_function=zs_strutils::format("__eval@_%i__",n_eval_function++);
		sf_eval=new	ScriptFunction(
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
		sf_eval->scope_script_function=(((zs)->getScopeFactory()))->newScope(sf_eval->idx_script_function,main_scope,SCOPE_PROPERTY_IS_SCOPE_FUNCTION);

		//--------------------------------------
		// 2. register arg symbols
		// catch parameters...
		if(function_params_len > 0){
			zs_map *fields=oo_param->getMapFields();

			for(auto it=fields->begin(); !it.end(); it.next()){

				if(sf_eval->registerLocalArgument(
						sf_eval->scope_script_function
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
			eval_parse_and_compile(
					zs
					,str_start
					,NULL
					,NULL
					,1
					,sf_eval
			);
		}catch(std::exception & ex){
			vm_set_error(zs->getVirtualMachine(),(zs_string("eval error:")+ex.what()).c_str());
			goto goto_eval_exit;
		}

		// link unresoved symbols
		zs->link();

		// check if there's a reset stack at the end and set as end function in order to get last value stk ...
		if(sf_eval->instructions_len>2){
			if(sf_eval->instructions[sf_eval->instructions_len-2].byte_code != BYTE_CODE_RET){
				int offset_rst_stack=sf_eval->instructions[sf_eval->instructions_len-2].byte_code == BYTE_CODE_RESET_STACK ? 1:0;
				size_t new_buf_len=sf_eval->instructions_len+2;
				Instruction *new_buf=(Instruction *)ZS_MALLOC(new_buf_len*sizeof(Instruction));
				memcpy(new_buf,sf_eval->instructions,sf_eval->instructions_len*sizeof(Instruction));
				// free old ptr
				free(sf_eval->instructions);

				// assign ret null
				new_buf[new_buf_len-3-offset_rst_stack].byte_code=BYTE_CODE_LOAD_UNDEFINED;
				new_buf[new_buf_len-2-offset_rst_stack].byte_code=BYTE_CODE_RET;

				sf_eval->instructions=new_buf;
				sf_eval->instructions_len=new_buf_len;
			}
		}

		// 4. Call function passing all arg parameter
		// pass data to vm_stk_current
		stk_n_params=(uint8_t)stk_params.size();
		stk_start=vm_stk_current;//vm data->vm_stk_current;
		for(int i = 0; i < stk_n_params; i++){
			*stk_start++=*((StackElement *)stk_params.items[i]);
		}

		vm_execute_script_function(
			zs->getVirtualMachine(),
			NULL,
			sf_eval,
			vm_stk_current);

		// modifug
		if(vm_it_has_error(zs->getVirtualMachine())){
			zs_string error=vm_get_error(zs->getVirtualMachine());
			vm_set_error(zs->getVirtualMachine(),zs_strutils::format("eval error: %s",error.c_str()).c_str());
		}

goto_eval_exit:

		// unref symbols eval instance
		while(data->vm_current_scope_function > vm_eval_scope_function){
			while(
					(ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block > ZS_VM_CURRENT_SCOPE_FUNCTION->first_scope_block)

			){
				vm_pop_scope(vm);
			}

			--data->vm_current_scope_function;
		}

		 sf_eval->scope_script_function->removeChildrenBlockTypes();
		 sf_eval->scope_script_function->markAsUnusued();
		 zs->getScopeFactory()->clearUnusuedScopes();

		 delete sf_eval;


		 // assign returning vars...
		n_ret_args=vm_get_current_stack_element(vm)-vm_stk_current;
		stk_start=vm_stk_current;

		// overwrite first entered params due are the objects passed before and now are undefined
		for(int i = 0; i < n_ret_args-stk_n_params; i++){
			*stk_start++=*(vm_stk_current+stk_n_params+i);
		}

	}

	/*void SystemModule_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args){
		if(*chk_assert == false){
			StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
			vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
			delete str_out;
		}
	}*/

	void SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args){
		StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
		vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
		delete str_out;

	}

}

