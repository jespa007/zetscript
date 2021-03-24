// STATIC REFERENCES
#define PTR_FUNCTION_VOID_PARAM0(f) ((void (*)())(f))
#define PTR_FUNCTION_VOID_PARAM1(f) ((void (*)(zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM2(f) ((void (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM3(f) ((void (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM4(f) ((void (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM5(f) ((void (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_VOID_PARAM6(f) ((void (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_PARAM0(f) ((zs_int (*)())(f))
#define PTR_FUNCTION_RET_PARAM1(f) ((zs_int (*)(zs_int))(f))
#define PTR_FUNCTION_RET_PARAM2(f) ((zs_int (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM3(f) ((zs_int (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM4(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM5(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_PARAM6(f) ((zs_int (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_FLOAT_PARAM0(f) ((zs_float (*)())(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM1(f) ((zs_float (*)(zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM2(f) ((zs_float (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM3(f) ((zs_float (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM4(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM5(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM6(f) ((zs_float (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_BOOL_PARAM0(f) ((bool (*)())(f))
#define PTR_FUNCTION_RET_BOOL_PARAM1(f) ((bool (*)(zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM2(f) ((bool (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM3(f) ((bool (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM4(f)	((bool (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM5(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM6(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

namespace zetscript{

	void  vm_call_function_native(
			VirtualMachine *vm,
			ScriptObject  * this_object,
			const ScriptFunction *calling_function,
			StackElement *stk_arg_calling_function,
			unsigned char n_args,
			Instruction *instruction

	){

		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		zs_int  fun_ptr = calling_function->ref_native_function_ptr;

		if((calling_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){
			VM_SET_USER_ERROR(vm,"Internal error: Function not native");
			return;
		}

		/*if((calling_function->symbol.properties &  SYMBOL_PROPERTY_MEMBER_FUNCTION)){ // is a function member  ...
			if(this_object!= NULL){
				StackElement *stk_prop_fun=NULL;
				if((stk_prop_fun = this_object->getBuiltinElementAt(calling_function->symbol.idx_position))==NULL){
					return;
				}
				fun_ptr=((ScriptFunction *)stk_prop_fun->stk_value)->ref_native_function_ptr; // var ref holds function ptr
			}else{
				VM_SET_USER_ERROR(vm,"Internal error: expected object for function member");
				return;
			}
		}*/


		zs_int converted_param[MAX_NATIVE_FUNCTION_ARGS];
		zs_int result=0;
		StackElement *stk_arg_current;
		data->current_call_c_function = calling_function;
		//bool static_ref=calling_function->symbol.properties&SYMBOL_PROPERTY_STATIC;
		int this_param=0;
		zs_int param_this_object=0;
		int idx_arg_start=0;



		// special case that this is passed in static ref function
		/*if(static_ref == false){ // Is member function set as lambda function and save param_this_object for passing as first parameter in the PTR_FUNCTION_MEMBER_XXXX

			if(this_object==NULL){
				VM_ERROR_AND_RET("Internal error: Cannot set parameter as this object due this object is NULL");
			}

			if(this_object->idx_script_class>=IDX_BUILTIN_TYPE_MAX){
				param_this_object=(zs_int)this_object->getNativeObject(); // pass c object
			}else{ // pass script var
				param_this_object=(zs_int)this_object; // pass built-in zetscript object (vector/object/etc)
			}
		}else{*/

			// special case that this is passed in static ref function
			if(		(calling_function->symbol.properties&SYMBOL_PROPERTY_MEMBER_FUNCTION)
			//		&& instruction->byte_code==BYTE_CODE_CALL_CONSTRUCTOR
					&& this_object!=NULL
			){
				idx_arg_start = 1;
				n_args++;
				converted_param[0]=(zs_int)this_object->getNativeObject();
			}

		//}

		if(n_args>MAX_NATIVE_FUNCTION_ARGS){
			VM_ERROR_AND_RET("Max run-time args! (Max:%i Provided:%i)",MAX_NATIVE_FUNCTION_ARGS,n_args);
		}

		if((calling_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF) {
			VM_ERROR_AND_RET("Function is not registered as C");
		}

		if(fun_ptr==0){
			VM_ERROR_AND_RET("Null function");
		}

		if((char)calling_function->params->count != (n_args-this_param)){
			VM_ERROR_AND_RET("C argument VS script argument doestn't match sizes");
		}

		if(calling_function->params->count > MAX_NATIVE_FUNCTION_ARGS){
			VM_ERROR_AND_RET("Reached max param for C function (Current: %i Max Allowed: %i)",calling_function->params->count,MAX_NATIVE_FUNCTION_ARGS);
		}

		// convert parameters script to c...
		for(unsigned char  i = idx_arg_start; i < n_args;i++){

			stk_arg_current=&stk_arg_calling_function[i-idx_arg_start];
			ScriptFunctionArg *function_param=(ScriptFunctionArg *)calling_function->params->items[i];

			if(!data->zs->convertStackElementToVar(
					stk_arg_current
					,function_param->idx_type
					,(zs_int *)&converted_param[i]
					,data->vm_error_str
			)){
				VM_ERROR_AND_RET("Function \"%s\", param %i: %s",
					calling_function->symbol.name.c_str(),
					i,
					data->vm_error_str.c_str()
				);
			}
		}

		if(calling_function->idx_return_type == IDX_BUILTIN_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			case 0:
				PTR_FUNCTION_VOID_PARAM0(fun_ptr)();
				break;
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
			}

		}else if(calling_function->idx_return_type==IDX_BUILTIN_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
			switch(n_args){
			case 0:
				result=PTR_FUNCTION_RET_BOOL_PARAM0(fun_ptr)();
				break;
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
			}
		}else if(calling_function->idx_return_type==IDX_BUILTIN_TYPE_FLOAT_C){ // we must do a float cast in order to get float return.
			zs_float aux_flt=0;
			switch(n_args){
			case 0:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM0(fun_ptr)();
				break;
			case 1:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM1(fun_ptr)(
					converted_param[0]
				);
				break;
			case 2:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM2(fun_ptr)(
					converted_param[0]
					,converted_param[1]
				);
				break;
			case 3:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM3(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
				);
				break;
			case 4:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM4(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
				);
				break;
			case 5:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM5(fun_ptr)(
					converted_param[0]
					,converted_param[1]
					,converted_param[2]
					,converted_param[3]
					,converted_param[4]
				);
				break;
			case 6:
				aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM6(fun_ptr)(
					converted_param[0]
					 ,converted_param[1]
					 ,converted_param[2]
					 ,converted_param[3]
					 ,converted_param[4]
					,converted_param[5]
				);
				break;
			}

			ZS_FLOAT_COPY(&result, &aux_flt);
		}else{ // generic pointer or int

			switch(n_args){
			case 0:
				result=PTR_FUNCTION_RET_PARAM0(fun_ptr)();
				break;
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
			}
		}

		*data->stk_vm_current++=data->zs->convertVarToStackElement(result,calling_function->idx_return_type);
	}
}
