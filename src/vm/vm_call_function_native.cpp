/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define PTR_FUNCTION_VOID_PARAM1(f) ((void (*)(zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM2(f) ((void (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM3(f) ((void (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM4(f) ((void (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM5(f) ((void (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM6(f) ((void (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM7(f) ((void (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_PARAM1(f) ((zs_int (*)(zs_int))(f))
#define PTR_FUNCTION_RET_PARAM2(f) ((zs_int (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM3(f) ((zs_int (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM4(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM5(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM6(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM7(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_FLOAT_PARAM1(f) ((zs_float (*)(zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM2(f) ((zs_float (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM3(f) ((zs_float (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM4(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM5(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM6(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM7(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_BOOL_PARAM1(f) ((bool (*)(zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM2(f) ((bool (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM3(f) ((bool (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM4(f)	((bool (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM5(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM6(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM7(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_STRING_PARAM1(f) ((zs_string (*)(zs_int))(f))
#define PTR_FUNCTION_RET_STRING_PARAM2(f) ((zs_string (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_STRING_PARAM3(f) ((zs_string (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_STRING_PARAM4(f) ((zs_string (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_STRING_PARAM5(f) ((zs_string (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_STRING_PARAM6(f) ((zs_string (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_STRING_PARAM7(f) ((zs_string (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

namespace zetscript{

	void  vm_call_function_native(
			VirtualMachine *vm,
			ScriptObject  * this_object,
			const ScriptFunction *c_function,
			const ScriptFunction *calling_function,
			Instruction *instruction,
			StackElement *stk_arg_c_function,
			unsigned char _n_args
	){

		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		zs_int  fun_ptr = c_function->ref_native_function_ptr;
		zs_string str_aux;

		if((c_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0){
			VM_SET_USER_ERRORF(vm,"Internal error: Function not native");
			return;
		}

		int idx_return_type=c_function->idx_return_type;
		zs_int converted_param[MAX_NATIVE_FUNCTION_ARGS];
		zs_int result=0;
		StackElement *stk_arg_current;
		data->current_call_c_function = c_function;
		//int this_param=0;
		int idx_arg_start=1;
		unsigned char n_args=_n_args+1;


		// First param is zetscript instance reference always
		converted_param[0]=(zs_int)data->zs;


		// special case that this is passed in static ref function
		if(this_object!=NULL){
			if(
				(c_function->properties&FUNCTION_PROPERTY_MEMBER_FUNCTION)
			){
				idx_arg_start++;
				n_args++;
				converted_param[1]=(zs_int)this_object->getNativeObject();
			}else if(this_object->idx_type != IDX_TYPE_CLASS_MAIN){
				VM_ERROR_AND_RET("Function '%s' is binded as STATIC at but it was acceded as member. You have to use STATIC access (i.e '%s::%s')"
						,c_function->function_name.c_str()
						,this_object->getScriptType()->script_type_name.c_str()
						,c_function->function_name.c_str()
						,c_function->function_name.c_str()
						);
			}
		}

		if(n_args>MAX_NATIVE_FUNCTION_ARGS){
			VM_ERROR_AND_RET("Max run-time args! (Max:%i Provided:%i)",MAX_NATIVE_FUNCTION_ARGS,n_args);
		}

		if((c_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF) != FUNCTION_PROPERTY_C_OBJECT_REF) {
			VM_ERROR_AND_RETF("Function is not registered as C");
		}

		if(fun_ptr==0){
			VM_ERROR_AND_RETF("Null function");
		}

		if((char)c_function->params_len != (n_args)){
			VM_ERROR_AND_RET("Native function '%s' expects %i arguments but it passed %i arguments"
					,c_function->function_name.c_str()
					,c_function->params_len
					,n_args);
		}

		if(c_function->params_len > MAX_NATIVE_FUNCTION_ARGS){
			VM_ERROR_AND_RET("Reached max param for C function (Current: %i Max Allowed: %i)",c_function->params_len,MAX_NATIVE_FUNCTION_ARGS);
		}

		// convert parameters script to c...
		if(stk_arg_c_function!=NULL){
			for(unsigned char  i = idx_arg_start; i < n_args;i++){

				stk_arg_current=&stk_arg_c_function[i-idx_arg_start];

				if(!data->zs->convertStackElementToVar(
						stk_arg_current
						,c_function->params[i].idx_type
						,(zs_int *)&converted_param[i]
						,data->vm_error_str
				)){
					VM_ERROR_AND_RET("Function '%s', param %i: %s",
						c_function->function_name.c_str(),
						i,
						data->vm_error_str.c_str()
					);
				}
			}
		}

		if(c_function->idx_return_type == IDX_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			case 1:
				PTR_FUNCTION_VOID_PARAM1(fun_ptr)(
					converted_param[0]
				);
				break;
			case 2:
				PTR_FUNCTION_VOID_PARAM2(fun_ptr)(
					converted_param[0]
					,converted_param[1]
				);
				break;
			case 3:
				PTR_FUNCTION_VOID_PARAM3(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
				);
				break;
			case 4:
				PTR_FUNCTION_VOID_PARAM4(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
				);
				break;
			case 5:
				PTR_FUNCTION_VOID_PARAM5(fun_ptr)(
					 converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
					,converted_param[4]
				);
				break;
			case 6:
				PTR_FUNCTION_VOID_PARAM6(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					 ,converted_param[5]
				);
				break;
			case 7:
				PTR_FUNCTION_VOID_PARAM7(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					 ,converted_param[5]
					,converted_param[6]
				);
				break;
			}

		}else if(c_function->idx_return_type==IDX_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
			switch(n_args){
			case 1:
				result=PTR_FUNCTION_RET_BOOL_PARAM1(fun_ptr)(
					converted_param[0]
				);
				break;
			case 2:
				result=PTR_FUNCTION_RET_BOOL_PARAM2(fun_ptr)(
					converted_param[0]
					,converted_param[1]
				);
				break;
			case 3:
				result=PTR_FUNCTION_RET_BOOL_PARAM3(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
				);
				break;
			case 4:
				result=PTR_FUNCTION_RET_BOOL_PARAM4(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
				);
				break;
			case 5:
				result=PTR_FUNCTION_RET_BOOL_PARAM5(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
					,converted_param[4]
				);
				break;
			case 6:
				result=PTR_FUNCTION_RET_BOOL_PARAM6(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					 ,converted_param[5]
				);
				break;
			case 7:
				result=PTR_FUNCTION_RET_BOOL_PARAM7(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					 ,converted_param[5]
					,converted_param[6]
				);
				break;

			}
		}else if(c_function->idx_return_type==IDX_TYPE_ZS_FLOAT_C){ // we must do a float cast in order to get float return.
			zs_float float_aux=0;
			switch(n_args){
			case 1:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM1(fun_ptr)(
					converted_param[0]
				);
				break;
			case 2:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM2(fun_ptr)(
					converted_param[0]
					,converted_param[1]
				);
				break;
			case 3:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM3(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
				);
				break;
			case 4:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM4(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
				);
				break;
			case 5:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM5(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
					,converted_param[4]
				);
				break;
			case 6:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM6(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					,converted_param[5]
				);
				break;
			case 7:
				float_aux=PTR_FUNCTION_RET_FLOAT_PARAM7(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					,converted_param[5]
					,converted_param[6]
				);
				break;
			}

			ZS_FLOAT_COPY(&result,&float_aux);

		}else if(c_function->idx_return_type==IDX_TYPE_STRING_C){ // we must do a float cast in order to get float return.

			switch(n_args){
			case 1:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM1(fun_ptr)(
					converted_param[0]
				);
				break;
			case 2:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM2(fun_ptr)(
					converted_param[0]
					,converted_param[1]
				);
				break;
			case 3:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM3(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
				);
				break;
			case 4:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM4(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
				);
				break;
			case 5:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM5(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
					,converted_param[4]
				);
				break;
			case 6:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM6(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					,converted_param[5]
				);
				break;

			case 7:
				str_aux=PTR_FUNCTION_RET_STRING_PARAM7(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					,converted_param[5]
					,converted_param[6]
				);
				break;
			}

			result=(zs_int)&str_aux;
			idx_return_type=IDX_TYPE_STRING_PTR_C;

		}else{ // generic pointer or int

			switch(n_args){
			case 1:
				result=PTR_FUNCTION_RET_PARAM1(fun_ptr)(
					converted_param[0]
				);
				break;
			case 2:
				result=PTR_FUNCTION_RET_PARAM2(fun_ptr)(
					converted_param[0]
					,converted_param[1]
				);
				break;
			case 3:
				result=PTR_FUNCTION_RET_PARAM3(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
				);
				break;
			case 4:
				result=PTR_FUNCTION_RET_PARAM4(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
				);
				break;
			case 5:
				result=PTR_FUNCTION_RET_PARAM5(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
					,converted_param[4]
				);
				break;
			case 6:
				result=PTR_FUNCTION_RET_PARAM6(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					 ,converted_param[5]
				);
				break;
			case 7:
				result=PTR_FUNCTION_RET_PARAM7(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					 ,converted_param[5]
					,converted_param[6]
				);
				break;
			}
		}

		*data->stk_vm_current++=data->zs->convertVarToStackElement(result,idx_return_type);
	}
}
