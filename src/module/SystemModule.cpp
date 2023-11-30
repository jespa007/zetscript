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
		return System::clock();
	}

	void SystemModule_eval(ZetScript *_zs,StringScriptObject *_so_str_eval,DictionaryScriptObject *_oo_param){
		StringScriptObject *so_str_eval=NULL;
		DictionaryScriptObject *oo_param=NULL;
		FunctionParam *function_params=NULL;
		FunctionParam **function_params_ptr=NULL;
		int 				 function_params_len=0;
		String str_param_name;
		ScriptFunction *sf_eval=NULL;
		Vector<StackElement *> stk_params;
		StackElement **stk_params_data=NULL;
		const char *str_start=NULL;
		String str_unescaped_source="";
		VirtualMachine *vm=_zs->getVirtualMachine();
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		int n_ret_args=0;
		uint8_t stk_n_params=0;
		StackElement *stk_start_arg_call=data->vm_stk_current;
		StackElement *stk_start=NULL;
		VM_ScopeFunction *vm_eval_scope_function=data->vm_current_scope_function;


		so_str_eval=_so_str_eval;

		//--------------------------------------
		// 0. setup scope and parameters
		if(_oo_param != NULL){//->properties != 0){

			oo_param=_oo_param;//(DictionaryScriptObject *)stk_oo_param->value;
			function_params_len=oo_param->length();
			if(function_params_len>0){

				function_params=new FunctionParam[function_params_len];
				function_params_ptr=&function_params;

				if(function_params!=NULL){
					int i=0;
					MapString *fields=oo_param->getMapStringFields();

					for(auto it=fields->begin(); !it.end(); it.next()){
						StackElement *stk=((StackElement *)it.value);
						function_params[i]=FunctionParam(it.key);
						stk_params.push(stk);

						if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){
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
		String  name=StringUtils::format("__eval@_%i__",n_eval_function++);
		sf_eval=new	ScriptFunction(
				_zs
				,ZS_FUNCTION_EVAL_IDX
				,SCRIPT_TYPE_ID_CLASS_MAIN
				,-1
				,name
				,function_params_ptr
				,function_params_len
				,SCRIPT_TYPE_ID_INVALID
				,0
				,0
		);

		ScriptScope *main_scope=((((_zs)->getScriptScopesFactory())))->getMainScope();
		sf_eval->scope=(((_zs)->getScriptScopesFactory()))->newScope(sf_eval->id,main_scope,SCOPE_PROPERTY_IS_SCOPE_FUNCTION);

		//--------------------------------------
		// 2. register arg symbols
		// catch parameters...
		if(function_params_len > 0){
			MapString *fields=oo_param->getMapStringFields();

			for(auto it=fields->begin(); !it.end(); it.next()){

				if(sf_eval->registerLocalArgument(
						sf_eval->scope
						,__FILE__
						,__LINE__
						,it.key
				,0)==NULL){
					goto goto_eval_exit;
				}

			}
		}

		str_unescaped_source=StringUtils::unescape(so_str_eval->toString());
		str_start=str_unescaped_source.toConstChar();

		// 3. Call zetscript->eval this function
		try{
			eval_parse_and_compile(
					_zs
					,str_start
					,NULL
					,NULL
					,1
					,sf_eval
			);
		}catch(std::exception & ex){
			vm_set_error(vm,(String("eval error:")+ex.what()).toConstChar());
			goto goto_eval_exit;
		}

		// link unresoved symbols
		_zs->link();

		// check if there's a reset stack at the end and set as end function in order to ensure it gets last value stk ...
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
		stk_n_params=(uint8_t)stk_params.length();
		stk_start=stk_start_arg_call;//vm data->vm_stk_current;
		stk_params_data=stk_params.data();
		for(int i = 0; i < stk_n_params; i++){
			*stk_start++=*stk_params_data[i];
		}

		vm_execute_script_function(
			vm,
			NULL,
			sf_eval,
			stk_start_arg_call);

		// modifug
		if(vm_it_has_error(vm)){
			String error=vm_get_error(vm);
			vm_set_error(vm,StringUtils::format("eval error: %s",error.toConstChar()).toConstChar());
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

		 sf_eval->scope->removeChildrenBlockTypes();
		 sf_eval->scope->markAsUnusued();
		 _zs->getScriptScopesFactory()->clearUnusuedScopes();

		 delete sf_eval;

		 // assign returning vars...
		n_ret_args=data->vm_stk_current-(stk_start_arg_call+stk_n_params);
		stk_start=stk_start_arg_call;

		// overwrite first entered params due are the objects passed before and now are undefined
		for(int i = 0; i < n_ret_args; i++){
			*stk_start++=*(stk_start_arg_call+stk_n_params+i);
		}

		// invert order
		for(int i = 0; i < (n_ret_args>>1); i++){
			StackElement tmp=stk_start_arg_call[n_ret_args-i-1];
			stk_start_arg_call[n_ret_args-i-1]=stk_start_arg_call[i];
			stk_start_arg_call[i]=tmp;
		}

		data->vm_stk_current=stk_start_arg_call+n_ret_args;
	}

	void 	SystemModule_eval(ZetScript *_zs, StringScriptObject *_so_str_eval){
		SystemModule_eval(_zs,_so_str_eval,NULL);
	}

	void SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args){
		StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
		vm_set_error(zs->getVirtualMachine(),str_out->toString().toConstChar());
		delete str_out;

	}

}

