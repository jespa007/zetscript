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

#define PTR_FUNCTION_RET_FLOAT_PARAM0(f) ((float (*)())(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM1(f) ((float (*)(zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM2(f) ((float (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM3(f) ((float (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM4(f) ((float (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM5(f) ((float (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM6(f) ((float (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

#define PTR_FUNCTION_RET_BOOL_PARAM0(f) ((bool (*)())(f))
#define PTR_FUNCTION_RET_BOOL_PARAM1(f) ((bool (*)(zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM2(f) ((bool (*)(zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM3(f) ((bool (*)(zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM4(f)	((bool (*)(zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM5(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM6(f) ((bool (*)(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int))(f))

// MEMBER REFERENCES
#define PTR_FUNCTION_MEMBER_VOID_PARAM0(f) (*((std::function<void(zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_VOID_PARAM1(f) (*((std::function<void(zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_VOID_PARAM2(f) (*((std::function<void(zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_VOID_PARAM3(f) (*((std::function<void(zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_VOID_PARAM4(f) (*((std::function<void(zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_VOID_PARAM5(f) (*((std::function<void(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_VOID_PARAM6(f) (*((std::function<void(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))

#define PTR_FUNCTION_MEMBER_RET_PARAM0(f) (*((std::function<zs_int(zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_PARAM1(f) (*((std::function<zs_int(zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_PARAM2(f) (*((std::function<zs_int(zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_PARAM3(f) (*((std::function<zs_int(zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_PARAM4(f) (*((std::function<zs_int(zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_PARAM5(f) (*((std::function<zs_int(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_PARAM6(f) (*((std::function<zs_int(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))

#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM0(f) (*((std::function<float(zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM1(f) (*((std::function<float(zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM2(f) (*((std::function<float(zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM3(f) (*((std::function<float(zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM4(f) (*((std::function<float(zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM5(f) (*((std::function<float(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM6(f) (*((std::function<float(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))

#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM0(f) (*((std::function<bool(zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM1(f) (*((std::function<bool(zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM2(f) (*((std::function<bool(zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM3(f) (*((std::function<bool(zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM4(f) (*((std::function<bool(zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM5(f) (*((std::function<bool(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))
#define PTR_FUNCTION_MEMBER_RET_BOOL_PARAM6(f) (*((std::function<bool(zs_int,zs_int,zs_int,zs_int,zs_int,zs_int,zs_int)> *)f))

namespace zetscript{

	StackElement  VirtualMachine::callFunctionNative(
			const ScriptFunction *calling_function,
			StackElement *stk_arg_calling_function,
			unsigned char n_args,
			Instruction *instruction,
			ScriptObject  * this_object
			){

		zs_int  fun_ptr = calling_function->ref_native_function_ptr;

		if((calling_function->symbol.properties &  SYMBOL_PROPERTY_C_STATIC_REF) == 0){ // is function member  ...
			if(this_object!= NULL){
				StackElement *stk_prop_fun = this_object->getProperty(calling_function->symbol.idx_position);
				fun_ptr=((ScriptFunction *)stk_prop_fun->var_ref)->ref_native_function_ptr; // var ref holds function ptr
			}else{
				THROW_RUNTIME_ERROR("Internal error: expected object for function member");
			}
		}

		zs_int converted_param[MAX_NATIVE_FUNCTION_ARGS];
		float 	 float_converted_param[MAX_NATIVE_FUNCTION_ARGS];
		zs_int result=0;
		StackElement *stk_arg_current;
		current_call_c_function = calling_function;
		bool static_ref=calling_function->symbol.properties&SYMBOL_PROPERTY_C_STATIC_REF;
		int this_param=0;
		zs_int param_this_object=0;

		if(static_ref==false){
			this_param=1;

			if(this_object==NULL){
				THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Internal error: Cannot set parameter as this object due this object is NULL");
			}

			if(this_object->idx_class>=IDX_BUILTIN_TYPE_MAX){
				param_this_object=(zs_int)this_object->getNativeObject(); // pass c object
			}else{ // pass script var
				param_this_object=(zs_int)this_object; // pass c object
			}
		}

		if(n_args>MAX_NATIVE_FUNCTION_ARGS){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Max run-time args! (Max:%i Provided:%i)",MAX_NATIVE_FUNCTION_ARGS,n_args);
		}

		if((calling_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF) {
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Function is not registered as C");
		}

		if(fun_ptr==0){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Null function");
		}

		if((char)calling_function->params->count != (n_args-this_param)){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"C argument VS scrip argument doestn't match sizes");
		}

		if(calling_function->params->count > MAX_NATIVE_FUNCTION_ARGS){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Reached max param for C function (Current: %i Max Allowed: %i)",calling_function->params->count,MAX_NATIVE_FUNCTION_ARGS);
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			stk_arg_current=&stk_arg_calling_function[i];
			FunctionParam *function_param=(FunctionParam *)calling_function->params->items[i];

			if(!zs->convertStackElementToVar(
					stk_arg_current
					,function_param->idx_type
					,(zs_int *)&converted_param[i]
					,error_str
			)){
				THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction),"Function \"%s\", param %i: %s. Native function \"%s\" that was found for first time it has different argument types now.",
																calling_function->symbol.name.c_str(),
																i,
																error_str.c_str(),
																calling_function->symbol.name.c_str()
																);
			}

			if(function_param->idx_type == IDX_BUILTIN_TYPE_FLOAT_PTR_C){
				float *ptr=&float_converted_param[i];
				*ptr = *((float *)&converted_param[i]);
				converted_param[i]=(zs_int)ptr;

			}

		}

		ZS_PRINT_DEBUG("pre_call %i",n_args);

		if(calling_function->idx_return_type == IDX_BUILTIN_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			case 0:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM0(fun_ptr)();
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM0(fun_ptr)(
						param_this_object
					);
				}
				break;
			case 1:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM1(fun_ptr)(
						param_this_object
						,converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM2(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM3(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM4(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM5(fun_ptr)(
						 converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM5(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					PTR_FUNCTION_MEMBER_VOID_PARAM6(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
						,converted_param[5]
					);
				}
				break;
			}

		}else if(calling_function->idx_return_type==IDX_BUILTIN_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM0(fun_ptr)();
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM0(fun_ptr)(
						param_this_object
					);
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM1(fun_ptr)(
						param_this_object
						,converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM2(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM3(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM4(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM5(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					result=PTR_FUNCTION_MEMBER_RET_BOOL_PARAM6(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
						,converted_param[5]
					);
				}
				break;
			}
		}else if(calling_function->idx_return_type==IDX_BUILTIN_TYPE_FLOAT_C){ // we must do a float cast in order to get float return.
			float aux_flt;
			switch(n_args){
			case 0:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM0(fun_ptr)();
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM0(fun_ptr)(
						param_this_object
					);
				}
				break;
			case 1:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM1(fun_ptr)(
						param_this_object
						,converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM2(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM3(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM4(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM5(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					aux_flt=PTR_FUNCTION_MEMBER_RET_FLOAT_PARAM6(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
						,converted_param[5]
					);
				}
				break;
			}

			memcpy(&result, &aux_flt, sizeof(float));
		}else{ // generic pointer or int

			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM0(fun_ptr)();
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM0(fun_ptr)(
						param_this_object
					);
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM1(fun_ptr)(
						param_this_object
						,converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM2(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM3(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM4(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM5(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					result=PTR_FUNCTION_MEMBER_RET_PARAM6(fun_ptr)(
						param_this_object
						,converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
						,converted_param[5]
					);
				}
				break;
			}
		}

		return this->zs->convertVarToStackElement(result,calling_function->idx_return_type);
	}
}
